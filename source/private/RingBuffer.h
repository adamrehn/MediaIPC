#ifndef _MEDIA_IPC_RING_BUFFER
#define _MEDIA_IPC_RING_BUFFER

#include <stdint.h>

namespace MediaIPC {

class RingBuffer
{
	public:
		RingBuffer(uint8_t* buffer, uint32_t size, uint32_t* head);
		
		void read(void* destination, uint32_t bytesToRead);
		void write(void* source, uint32_t bytesToWrite);
		
	private:
		uint8_t* buffer;
		uint32_t size;
		uint32_t* head;
};

} //End MediaIPC

#endif
