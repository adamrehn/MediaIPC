#ifndef _MEDIA_IPC_CONSUMER_DELEGATE
#define _MEDIA_IPC_CONSUMER_DELEGATE

#include "ControlBlock.h"
#include <functional>

namespace MediaIPC {

//Abstract base class that consumer delegates must implement
class ConsumerDelegate
{
	public:
		virtual ~ConsumerDelegate();
		
		//Called when the control block is received from the producer
		virtual void controlBlockReceived(const ControlBlock& cb) = 0;
		
		//Called on the video thread when a new video frame has been sampled
		virtual void videoFrameReceived(const uint8_t* buffer, uint64_t length) = 0;
		
		//Called on the audio thread when a new buffer of audio samples have been sampled
		virtual void audioSamplesReceived(const uint8_t* buffer, uint64_t length) = 0;
};

//Consumer delegate implementation for wrapping std::function instances
class FunctionConsumerDelegate : public ConsumerDelegate
{
	public:
		typedef std::function<void(const ControlBlock&)> ControlBlockCallback;
		typedef std::function<void(const uint8_t*, uint64_t)> DataCallback;
		
		FunctionConsumerDelegate();
		
		void setControlBlockHandler(ControlBlockCallback cbHandler);
		void setVideoHandler(DataCallback videoHandler);
		void setAudioHandler(DataCallback audioHandler);
		
		void controlBlockReceived(const ControlBlock& cb);
		void videoFrameReceived(const uint8_t* buffer, uint64_t length);
		void audioSamplesReceived(const uint8_t* buffer, uint64_t length);
	
	private:
		ControlBlockCallback cbHandler;
		DataCallback videoHandler;
		DataCallback audioHandler;
};

} //End MediaIPC

#endif
