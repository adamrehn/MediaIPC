#ifndef _MEDIA_IPC_MEDIA_PRODUCER
#define _MEDIA_IPC_MEDIA_PRODUCER

#include "ControlBlock.h"
#include "MediaBase.h"
#include <string>

namespace MediaIPC {

class MediaProducer : public MediaBase
{
	public:
		MediaProducer(const std::string& prefix, const ControlBlock& cb);
		~MediaProducer();
		
		//MediaProducer objects cannot be copied, only moved
		MediaProducer(const MediaProducer& other) = delete;
		MediaProducer& operator=(const MediaProducer& other) = delete;
		MediaProducer(MediaProducer&& other) = default;
		MediaProducer& operator=(MediaProducer&& other) = default;
		
		void submitVideoFrame(void* buffer, uint64_t length);
		void submitAudioSamples(void* buffer, uint64_t length);
		void stop();
};

} //End MediaIPC

#endif
