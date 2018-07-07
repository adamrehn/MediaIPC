#include <cstdlib>
#include <fstream>
#include <iostream>
#include <stdexcept>
using std::cout;
using std::endl;
using std::ofstream;

//When building your own consumers, this will be #include <MediaIPC/MediaConsumer.h>
#include "../../source/public/MediaConsumer.h"

//Prints a control block to an output stream
void printControlBlock(const MediaIPC::ControlBlock& cb, std::ostream& stream)
{
	//Print the video details
	stream << "videoFormat = " << MediaIPC::FormatDetails::description(cb.videoFormat) << endl;
	if (cb.videoFormat != MediaIPC::VideoFormat::None)
	{
		stream << "width = " << cb.width << endl;
		stream << "height = " << cb.height << endl;
		stream << "bytesPerPixel = " << (uint32_t)MediaIPC::FormatDetails::bytesPerPixel(cb.videoFormat) << endl;
		stream << "frameRate = " << cb.frameRate << endl;
	}
	
	//Print the audio details
	stream << "audioFormat = " << MediaIPC::FormatDetails::description(cb.audioFormat) << endl;
	if (cb.audioFormat != MediaIPC::AudioFormat::None)
	{
		stream << "channels = " << cb.channels << endl;
		stream << "sampleRate = " << cb.sampleRate << endl;
		stream << "samplesPerBuffer = " << cb.samplesPerBuffer << endl;
		stream << "bytesPerSample = " << (uint32_t)MediaIPC::FormatDetails::bytesPerSample(cb.audioFormat) << endl;
	}
	
	stream << endl;
}

int main (int argc, char* argv[])
{
	try
	{
		//Create our consumer delegate
		std::unique_ptr<MediaIPC::FunctionConsumerDelegate> delegate( new MediaIPC::FunctionConsumerDelegate() );
		
		//Bind our callback for when the control block data is received
		delegate->setControlBlockHandler([](const MediaIPC::ControlBlock& cb)
		{
			//Print the control block contents
			cout << "Received Control Block:" << endl << endl;
			printControlBlock(cb, cout);
			
			//Write the control block data to file
			ofstream cbFile("controlblock.txt");
			if (cbFile.is_open())
			{
				printControlBlock(cb, cbFile);
				cbFile.close();
			}
			
			//After this point the MediaConsumer will loop until the stream ends
			cout << "Receiving stream data from producer process..." << endl;
		});
		
		//Bind our callback for when video data is received (will be called on the video thread)
		ofstream videoFile("video.raw", std::ios::binary);
		delegate->setVideoHandler([&videoFile](const uint8_t* buffer, uint64_t length)
		{
			//cout << "Video frame received!" << endl;
			videoFile.write((const char*)buffer, length);
		});
		
		//Bind our callback for when audio data is received (will be called on the audio thread)
		ofstream audioFile("audio.raw", std::ios::binary);
		delegate->setAudioHandler([&audioFile](const uint8_t* buffer, uint64_t length)
		{
			//cout << "Audio samples received!" << endl;
			audioFile.write((const char*)buffer, length);
		});
		
		//Consume data until the stream completes
		cout << "Awaiting control block from producer process..." << endl << endl;
		MediaIPC::MediaConsumer consumer("TestPrefix-", std::move(delegate));
		videoFile.close();
		audioFile.close();
		cout << "Stream complete." << endl;
	}
	catch (std::runtime_error& e) {
		cout << "Error: " << e.what() << endl;
	}
	
	return 0;
}
