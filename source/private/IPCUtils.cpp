#include "IPCUtils.h"
#include <cstring>
#include <thread>
#include <utility>

namespace MediaIPC {

MemoryCleanup::MemoryCleanup(const string& name)
{
	this->memoryName = name;
	if (this->memoryName.empty() == false) {
		ipc::shared_memory_object::remove(this->memoryName.c_str());
	}
}

MemoryCleanup::~MemoryCleanup()
{
	if (this->memoryName.empty() == false) {
		ipc::shared_memory_object::remove(this->memoryName.c_str());
	}
}

MutexCleanup::MutexCleanup(const string& name)
{
	this->mutexName = name;
	if (this->mutexName.empty() == false) {
		ipc::named_mutex::remove(this->mutexName.c_str());
	}
}

MutexCleanup::~MutexCleanup()
{
	if (this->mutexName.empty() == false) {
		ipc::named_mutex::remove(this->mutexName.c_str());
	}
}

MemoryWrapper::~MemoryWrapper()
{
	//Make sure we release our object references prior to any cleanup
	this->mapped.reset();
	this->memory.reset();
}

MutexWrapper::~MutexWrapper()
{
	//Make sure we release our object reference prior to any cleanup
	this->mutex.reset();
}

void MemoryWrapper::map(ipc::mode_t mode) {
	this->mapped.reset(new ipc::mapped_region(*this->memory, mode));
}

MemoryWrapper IPCUtils::createSharedMemory(const string& name, uint64_t size, ipc::mode_t mode)
{
	MemoryWrapper wrapper;
	wrapper.cleanup = MemoryCleanup(name);
	wrapper.memory.reset( new ipc::shared_memory_object(ipc::create_only, name.c_str(), mode) );
	wrapper.memory->truncate(size);
	wrapper.map(mode);
	return wrapper;
}

MemoryWrapper IPCUtils::getMemoryOnceExists(const string& name, ipc::mode_t mode)
{
	unique_ptr<ipc::shared_memory_object> memory;
	while (memory.get() == nullptr)
	{
		try {
			memory.reset(new ipc::shared_memory_object(ipc::open_only, name.c_str(), mode));
		}
		catch (...)
		{
			//Shared memory does not exist yet
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}
	}
	
	MemoryWrapper wrapper;
	wrapper.memory = std::move(memory);
	wrapper.map(mode);
	return wrapper;
}

MutexWrapper IPCUtils::getNamedMutex(const string& name, bool cleanup)
{
	MutexWrapper wrapper;
	wrapper.cleanup = MutexCleanup( (cleanup == true ? name : "") );
	wrapper.mutex.reset( new ipc::named_mutex(ipc::open_or_create, name.c_str()) );
	return wrapper;
}

void IPCUtils::fillMemory(ipc::mapped_region& region, uint8_t value) {
	std::memset(region.get_address(), value, region.get_size());
}

} //End MediaIPC
