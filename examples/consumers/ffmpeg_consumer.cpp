#include <boost/asio/io_service.hpp>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>
using std::cout;
using std::endl;
using std::string;
using std::stringstream;
using std::vector;

//When building your own consumers, this will be #include <MediaIPC/MediaConsumer.h>
#include "../../source/public/MediaConsumer.h"
#include "../common/common.h"

//Represents a thread that runs an event loop
class EventLoopThread
{
	public:
		EventLoopThread()
		{
			this->thread = std::thread([this]()
			{
				boost::asio::io_service::work work(this->loop);
				this->loop.run();
			});
		}
		
		void stop()
		{
			//Stop the event loop
			this->loop.post([this](){
				this->loop.stop();
			});
			
			//Wait for the thread to complete
			this->thread.join();
		}
		
		boost::asio::io_service loop;
		std::thread thread;
};

int main (int argc, char* argv[])
{
	//Our video format name mappings for ffmpeg
	std::map<MediaIPC::VideoFormat, std::string> videoFormats =
	{
		{MediaIPC::VideoFormat::GRAY8,    "gray"},
		{MediaIPC::VideoFormat::GRAY16BE, "gray16be"},
		{MediaIPC::VideoFormat::GRAY16LE, "gray16le"},
		{MediaIPC::VideoFormat::RGB,      "rgb24"},
		{MediaIPC::VideoFormat::BGR,      "bgr24"},
		{MediaIPC::VideoFormat::RGBA,     "rgba"},
		{MediaIPC::VideoFormat::BGRA,     "bgra"},
		{MediaIPC::VideoFormat::ARGB,     "argb"},
		{MediaIPC::VideoFormat::ABGR,     "abgr"},
		{MediaIPC::VideoFormat::None,     "none"}
	};
	
	//Our video format name mappings for ffmpeg
	std::map<MediaIPC::AudioFormat, std::string> audioFormats =
	{
		{MediaIPC::AudioFormat::PCM_S8,    "s8"},
		{MediaIPC::AudioFormat::PCM_U8,    "u8"},
		{MediaIPC::AudioFormat::PCM_S16BE, "s16be"},
		{MediaIPC::AudioFormat::PCM_S16LE, "s16le"},
		{MediaIPC::AudioFormat::PCM_U16BE, "u16be"},
		{MediaIPC::AudioFormat::PCM_U16LE, "u16le"},
		{MediaIPC::AudioFormat::PCM_S24BE, "s24be"},
		{MediaIPC::AudioFormat::PCM_S24LE, "s24le"},
		{MediaIPC::AudioFormat::PCM_U24BE, "u24be"},
		{MediaIPC::AudioFormat::PCM_U24LE, "u24le"},
		{MediaIPC::AudioFormat::PCM_S32BE, "s32be"},
		{MediaIPC::AudioFormat::PCM_S32LE, "s32le"},
		{MediaIPC::AudioFormat::PCM_U32BE, "u32be"},
		{MediaIPC::AudioFormat::PCM_U32LE, "u32le"},
		{MediaIPC::AudioFormat::PCM_F32BE, "f32be"},
		{MediaIPC::AudioFormat::PCM_F32LE, "f32le"},
		{MediaIPC::AudioFormat::PCM_F64BE, "f64be"},
		{MediaIPC::AudioFormat::PCM_F64LE, "f64le"},
		{MediaIPC::AudioFormat::None,      "none"}
	};
	
	try
	{
		//If the user supplied a prefix string, use it instead of our default
		std::string prefix = ((argc > 1) ? argv[1] : "TestPrefix");
		
		//If the user supplied additional ffmpeg command-line arguments, use them
		std::string ffmpegExtraArgs = " -y out.mp4";
		if (argc > 2)
		{
			ffmpegExtraArgs = "";
			for (int i = 2; i < argc; ++i) {
				ffmpegExtraArgs += string(" \"") + string(argv[i]) + string("\"");
			}
		}
		
		//Create the audio and video threads with their event loops
		EventLoopThread videoThread;
		EventLoopThread audioThread;
		
		//Create the named pipes
		NamedPipe videoPipe("videoPipe");
		NamedPipe audioPipe("audioPipe");
		
		//Open the video pipe
		videoThread.loop.post([&videoPipe](){
			videoPipe.open();
		});
		
		//Open the audio pipe
		audioThread.loop.post([&audioPipe](){
			audioPipe.open();
		});
		
		//The thread that will run our ffmpeg child process
		std::thread ffmpegThread;
		
		//Create our consumer delegate
		std::unique_ptr<MediaIPC::FunctionConsumerDelegate> delegate( new MediaIPC::FunctionConsumerDelegate() );
		
		//Bind our callback for when the control block data is received
		delegate->setControlBlockHandler([&ffmpegThread, &videoPipe, &audioPipe, videoFormats, audioFormats, ffmpegExtraArgs](const MediaIPC::ControlBlock& cb)
		{
			//Print the control block contents
			cout << "Received Control Block:" << endl << endl;
			printControlBlock(cb, cout);
			
			//Build the command to run our ffmpeg child process
			stringstream command;
			command << "ffmpeg";
			command << " -f rawvideo";
			command << " -pixel_format " << videoFormats.at(cb.videoFormat);
			command << " -video_size " << cb.width << "x" << cb.height;
			command << " -framerate " << cb.frameRate;
			command << " -re";
			command << " -thread_queue_size 512";
			command << " -i " << videoPipe.path();
			command << " -f " << audioFormats.at(cb.audioFormat);
			command << " -ac " << cb.channels;
			command << " -ar " << cb.sampleRate;
			command << " -thread_queue_size 512";
			command << " -i " << audioPipe.path();
			command << " -map 0:0 -map 1:0";
			command << ffmpegExtraArgs;
			string commandStr = command.str();
			
			//Print the ffmpeg command string
			cout << commandStr << endl << endl;
			
			//Start our ffmpeg child process
			ffmpegThread = std::thread([commandStr]() {
				system(commandStr.c_str());
			});
			
			//After this point the MediaConsumer will loop until the stream ends
			cout << "Receiving stream data from producer process..." << endl;
		});
		
		//Bind our callback for when video data is received
		delegate->setVideoHandler([&videoPipe, &videoThread](const uint8_t* buffer, uint64_t length)
		{
			//Create a temporary buffer to hold a copy of the received data
			vector<uint8_t> bufferCopy(buffer, buffer + length);
			
			//Write the data to the named pipe on the video pipe thread
			videoThread.loop.post([&videoPipe, bufferCopy, length]() {
				videoPipe.write((const char*)bufferCopy.data(), length);
			});
		});
		
		//Bind our callback for when audio data is received
		delegate->setAudioHandler([&audioPipe, &audioThread](const uint8_t* buffer, uint64_t length)
		{
			//Create a temporary buffer to hold a copy of the received data
			vector<uint8_t> bufferCopy(buffer, buffer + length);
			
			//Write the data to the named pipe on the audio pipe thread
			audioThread.loop.post([&audioPipe, bufferCopy, length]() {
				audioPipe.write((const char*)bufferCopy.data(), length);
			});
		});
		
		//Consume data until the stream completes
		cout << "Awaiting control block from producer process..." << endl << endl;
		MediaIPC::MediaConsumer consumer(prefix, std::move(delegate));
		
		//Flush any remaining audio and video data
		cout << "Stream complete." << endl;
		cout << "Producer has stopped sending data, performing shutdown..." << endl;
		videoThread.stop();
		audioThread.stop();
		
		//Close our pipes
		cout << "Closing and destroying pipes..." << endl;
		videoPipe.close();
		audioPipe.close();
		
		//Wait for the ffmpeg thread to complete
		ffmpegThread.join();
	}
	catch (std::runtime_error& e) {
		cout << "Error: " << e.what() << endl;
	}
	
	return 0;
}
