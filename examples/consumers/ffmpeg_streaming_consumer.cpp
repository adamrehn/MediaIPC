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

//Represents the audio and video settings for a streaming profile
class StreamingProfile
{
	public:
		StreamingProfile(const string& vcodec, const string& vflags, const string& acodec, const string& aflags)
		{
			this->vcodec = vcodec;
			this->vflags = vflags;
			this->acodec = acodec;
			this->aflags = aflags;
		}
		
		string vcodec;
		string vflags;
		string acodec;
		string aflags;
};

int main (int argc, char* argv[])
{
	//Our video format name mappings for ffmpeg
	std::map<MediaIPC::VideoFormat, string> videoFormats =
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
	
	//Our audio format name mappings for ffmpeg
	std::map<MediaIPC::AudioFormat, string> audioFormats =
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
	
	//Our streaming profiles
	std::map<string, StreamingProfile> profiles =
	{
		{"h264", StreamingProfile("libx264", "-tune zerolatency", "aac", "")},
		{"vp8",  StreamingProfile("libvpx", "-deadline realtime", "libopus", "-ac 2")}
	};
	
	try
	{
		//Check that the user supplied all of the required arguments
		if (argc < 5)
		{
			//Build our usage string
			stringstream usage;
			usage << "Usage:\n  ffmpeg_streaming_consumer PREFIX PROFILE VIDEO_DEST AUDIO_DEST\n\n";
			usage << "  (Where VIDEO_DEST and AUDIO_DEST are RTP streaming endpoints)\n\n";
			usage << "Supported profiles:\n";
			for (auto pair : profiles) {
				usage << "  " << pair.first << "\n";
			}
			usage << "\nExample:\n  ffmpeg_streaming_consumer TestPrefix vp8 127.0.0.1:5002 127.0.0.1:5004\n";
			
			//Display usage and exit
			throw std::runtime_error("all required arguments must be specified.\n\n" + usage.str());
		}
		
		//Store the arguments and verify that the specified profile is valid
		string prefix = argv[1];
		string profileName = argv[2];
		string videoDest = argv[3];
		string audioDest = argv[4];
		if (profiles.count(profileName) == 0) {
			throw std::runtime_error("unknown profile \"" + profileName + "\"");
		}
		
		//Retrieve the audio and video settings for the specified profile
		StreamingProfile profile = profiles.at(profileName);
		
		//Create the audio and video threads that will run our ffmpeg child processes
		std::thread videoThread;
		std::thread audioThread;
		
		//Create the named pipes
		NamedPipe videoPipe("videoPipe");
		NamedPipe audioPipe("audioPipe");
		
		//Create our consumer delegate
		std::unique_ptr<MediaIPC::FunctionConsumerDelegate> delegate( new MediaIPC::FunctionConsumerDelegate() );
		
		//Bind our callback for when the control block data is received
		delegate->setControlBlockHandler([&videoThread, &audioThread, &videoPipe, &audioPipe, videoFormats, audioFormats, videoDest, audioDest, profile](const MediaIPC::ControlBlock& cb)
		{
			//Print the control block contents
			cout << "Received Control Block:" << endl << endl;
			printControlBlock(cb, cout);
			
			//Build the command to run the ffmpeg video child process
			stringstream videoCommand;
			videoCommand << "ffmpeg";
			videoCommand << " -f rawvideo";
			videoCommand << " -pixel_format " << videoFormats.at(cb.videoFormat);
			videoCommand << " -video_size " << cb.width << "x" << cb.height;
			videoCommand << " -framerate " << cb.frameRate;
			videoCommand << " -i " << videoPipe.path();
			videoCommand << " -an";
			videoCommand << " -pix_fmt yuv420p";
			videoCommand << " -c:v " << profile.vcodec << " " << profile.vflags;
			videoCommand << " -f rtp rtp://" << videoDest;
			
			//Build the command to run the ffmpeg audio child process
			stringstream audioCommand;
			audioCommand << "ffmpeg";
			audioCommand << " -f " << audioFormats.at(cb.audioFormat);
			audioCommand << " -ac " << cb.channels;
			audioCommand << " -ar " << cb.sampleRate;
			audioCommand << " -i " << audioPipe.path();
			audioCommand << " -vn";
			audioCommand << " -c:a " << profile.acodec << " " << profile.aflags;
			audioCommand << " -f rtp rtp://" << audioDest;
			
			//If we are receiving video, start the ffmpeg child process for transcoding video
			if (cb.videoFormat != MediaIPC::VideoFormat::None)
			{
				videoThread = std::thread([&videoCommand]() {
					system(videoCommand.str().c_str());
				});
			}
			
			//If we are receiving audio, start the ffmpeg child process for transcoding audio
			if (cb.audioFormat != MediaIPC::AudioFormat::None)
			{
				audioThread = std::thread([&audioCommand]() {
					system(audioCommand.str().c_str());
				});
			}
			
			//Wait for the ffmpeg processes to start and open their ends of the pipes
			if (cb.videoFormat != MediaIPC::VideoFormat::None) { videoPipe.open(); }
			if (cb.audioFormat != MediaIPC::AudioFormat::None) { audioPipe.open(); }
			
			//After this point the MediaConsumer will loop until the stream ends
			cout << "Receiving stream data from producer process..." << endl;
		});
		
		//Bind our callback for when video data is received
		delegate->setVideoHandler([&videoPipe](const uint8_t* buffer, uint64_t length) {
			videoPipe.write(buffer, length);
		});
		
		//Bind our callback for when audio data is received
		delegate->setAudioHandler([&audioPipe](const uint8_t* buffer, uint64_t length) {
			audioPipe.write(buffer, length);
		});
		
		//Consume data until the stream completes
		cout << "Awaiting control block from producer process..." << endl << endl;
		MediaIPC::MediaConsumer consumer(prefix, std::move(delegate));
		
		//Close our pipes
		cout << "Stream complete." << endl;
		cout << "Producer has stopped sending data, performing shutdown..." << endl;
		cout << "Closing and destroying pipes..." << endl;
		videoPipe.close();
		audioPipe.close();
		
		//Wait for the ffmpeg threads to complete
		videoThread.join();
		audioThread.join();
	}
	catch (std::runtime_error& e) {
		cout << "Error: " << e.what() << endl;
	}
	
	return 0;
}
