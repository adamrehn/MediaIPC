#ifndef _MEDIA_IPC_MEDIA_CONSUMER
#define _MEDIA_IPC_MEDIA_CONSUMER

#include "ConsumerDelegate.h"
#include "ControlBlock.h"
#include "MediaBase.h"
#include <string>

namespace MediaIPC {

class MediaConsumer : public MediaBase
{
	public:
		MediaConsumer(const std::string& prefix, std::unique_ptr<ConsumerDelegate>&& delegate);
		~MediaConsumer();
		
		//MediaConsumer objects cannot be copied, only moved
		MediaConsumer(const MediaConsumer& other) = delete;
		MediaConsumer& operator=(const MediaConsumer& other) = delete;
		MediaConsumer(MediaConsumer&& other) = default;
		MediaConsumer& operator=(MediaConsumer&& other) = default;
		
	private:
		
		//Determines if the producer is still streaming data
		bool streamIsActive();
		
		//The video sampling loop
		void videoLoop();
		
		//The audio sampling loop
		void audioLoop();
		
		std::unique_ptr<ConsumerDelegate> delegate;
};

} //End MediaIPC

#endif
