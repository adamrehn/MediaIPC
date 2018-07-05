#ifndef _MEDIA_IPC_MEDIA_BASE
#define _MEDIA_IPC_MEDIA_BASE

#include <memory>

namespace MediaIPC {

class ControlBlock;
class MemoryWrapper;
class MutexWrapper;
class RingBuffer;
typedef std::unique_ptr<MemoryWrapper> MemoryWrapperPtr;
typedef std::unique_ptr<MutexWrapper> MutexWrapperPtr;

class MediaBase
{
	protected:
		
		//Producer status named mutex
		MutexWrapperPtr statusMutex;
		
		//Control block shared memory
		MemoryWrapperPtr controlBlockMemory;
		
		//Video shared memory and named mutexes
		MemoryWrapperPtr videoFrontBuffer;
		MemoryWrapperPtr videoBackBuffer;
		MutexWrapperPtr videoMutex;
		MutexWrapperPtr frontBufferMutex;
		MutexWrapperPtr backBufferMutex;
		
		//Audio shared memory and named mutex
		MemoryWrapperPtr audioBuffer;
		MutexWrapperPtr audioMutex;
		
		//Ring buffer interface for the audio shared memory
		std::unique_ptr<RingBuffer> ringBuffer;
		
		//Control block pointer (points to the control block shared memory)
		ControlBlock* controlBlock;
};

} //End MediaIPC

#endif
