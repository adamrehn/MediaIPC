#include "../public/MediaProducer.h"
#include "IPCUtils.h"
#include "MemoryUtils.h"
#include "ObjectNames.h"
#include "RingBuffer.h"
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <utility>

namespace MediaIPC {

namespace
{
	MemoryWrapperPtr producerMemory(const std::string& name, uint64_t size) {
		return MemoryUtils::toPointer(IPCUtils::createSharedMemory(name, size, ipc::read_write));
	}
	
	MutexWrapperPtr producerMutex(const std::string& name) {
		return MemoryUtils::toPointer(IPCUtils::getNamedMutex(name, true));
	}
}

MediaProducer::MediaProducer(const std::string& prefix, const ControlBlock& cb)
{
	//Resolve the names of our shared memory objects and mutexes
	ObjectNames names(prefix);
	
	//Create our named mutexes
	this->statusMutex = producerMutex(names.statusMutex);
	this->videoMutex = producerMutex(names.videoMutex);
	this->frontBufferMutex = producerMutex(names.frontBufferMutex);
	this->backBufferMutex = producerMutex(names.backBufferMutex);
	this->audioMutex = producerMutex(names.audioMutex);
	
	//Lock the status mutex while we create our shared memory objects
	//(This ensures the consumer can't acquire any locks before we've populated our initial values)
	{
		MutexLock lock(*this->statusMutex->mutex);
		
		//Create the shared memory for the control block and bind our pointer to it
		this->controlBlockMemory = producerMemory(names.controlBlockMemory, sizeof(ControlBlock));
		this->controlBlock = (ControlBlock*)(this->controlBlockMemory->mapped->get_address());
		
		//Populate the initial control block data
		std::memcpy(this->controlBlock, &cb, sizeof(ControlBlock));
		this->controlBlock->active = true;
		this->controlBlock->lastBuffer = VideoBuffer::FrontBuffer;
		this->controlBlock->ringHead = 0;
		
		//Create the shared memory for the video data (ensuring the size is non-zero)
		uint64_t videoBufsize = std::max((uint64_t)1, this->controlBlock->calculateVideoBufsize());
		this->videoFrontBuffer = producerMemory(names.videoFrontBuffer, videoBufsize);
		this->videoBackBuffer = producerMemory(names.videoBackBuffer, videoBufsize);
		
		//Zero-out the video buffers
		IPCUtils::fillMemory(*this->videoFrontBuffer->mapped, 0);
		IPCUtils::fillMemory(*this->videoBackBuffer->mapped, 0);
		
		//Create the shared memory for the audio data (ensuring the size is non-zero)
		uint64_t audioBufsize = std::max((uint64_t)1, this->controlBlock->calculateAudioBufsize());
		this->audioBuffer = producerMemory(names.audioBuffer, audioBufsize);
		
		//Zero-out the audio buffer
		IPCUtils::fillMemory(*this->audioBuffer->mapped, 0);
		
		//Wrap our ring buffer interface around the audio buffer
		this->ringBuffer.reset(new RingBuffer(
			(uint8_t*)(this->audioBuffer->mapped->get_address()),
			this->audioBuffer->mapped->get_size(),
			&(this->controlBlock->ringHead
		)));
	}
}

MediaProducer::~MediaProducer() {
	this->stop();
}

void MediaProducer::submitVideoFrame(void* buffer, uint64_t length)
{
	//Determine which buffer to use
	VideoBuffer bufToUse = VideoBuffer::FrontBuffer;
	{
		MutexLock lock(*this->videoMutex->mutex);
		bufToUse = (this->controlBlock->lastBuffer == VideoBuffer::FrontBuffer) ? VideoBuffer::BackBuffer : VideoBuffer::FrontBuffer;
	}
	
	//Write to the selected buffer
	{
		auto& mutex = (bufToUse == VideoBuffer::FrontBuffer) ? this->frontBufferMutex : this->backBufferMutex;
		auto& dest = (bufToUse == VideoBuffer::FrontBuffer) ? this->videoFrontBuffer : this->videoBackBuffer;
		
		MutexLock lock(*mutex->mutex);
		std::memcpy(dest->mapped->get_address(), buffer, std::min(length, (uint64_t)(dest->mapped->get_size())));
	}
	
	//Update the "last buffer" flag
	{
		MutexLock lock(*this->videoMutex->mutex);
		this->controlBlock->lastBuffer = bufToUse;
	}
}

void MediaProducer::submitAudioSamples(void* buffer, uint64_t length)
{
	MutexLock lock(*this->audioMutex->mutex);
	this->ringBuffer->write(buffer, length);
}

void MediaProducer::stop()
{
	//Set our status flag to inactive
	{
		MutexLock lock(*this->statusMutex->mutex);
		this->controlBlock->active = false;
	}
}

} //End MediaIPC
