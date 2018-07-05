#include "RingBuffer.h"

#include <algorithm>
#include <cstring>

namespace MediaIPC {

RingBuffer::RingBuffer(uint8_t* buffer, uint32_t size, uint32_t* head)
{
	this->buffer = buffer;
	this->size = size;
	this->head = head;
}

void RingBuffer::read(void* destination, uint32_t bytesToRead)
{
	uint32_t currOffset = *this->head;
	uint8_t* dest = (uint8_t*)destination;
	while (bytesToRead > 0)
	{
		uint32_t readCount = std::min(bytesToRead, this->size - currOffset);
		std::memcpy(dest, this->buffer + currOffset, readCount);
		
		currOffset = (currOffset + readCount) % this->size;
		bytesToRead -= readCount;
		dest += readCount;
	}
}

void RingBuffer::write(void* source, uint32_t bytesToWrite)
{
	uint8_t* src = (uint8_t*)source;
	while (bytesToWrite > 0)
	{
		uint32_t writeCount = std::min(bytesToWrite, this->size - *this->head);
		std::memcpy(this->buffer + *this->head, src, writeCount);
		
		*this->head = (*this->head + writeCount) % this->size;
		bytesToWrite -= writeCount;
		src += writeCount;
	}
}

} //End MediaIPC
