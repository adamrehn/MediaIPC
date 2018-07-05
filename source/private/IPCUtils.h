#ifndef _MEDIA_IPC_IPC_UTILS
#define _MEDIA_IPC_IPC_UTILS

#define BOOST_DATE_TIME_NO_LIB
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
namespace ipc = boost::interprocess;

#include <stdint.h>
#include <memory>
#include <string>
using std::string;
using std::unique_ptr;

namespace MediaIPC {

typedef ipc::scoped_lock<ipc::named_mutex> MutexLock;

//Performs automatic cleanup for a shared memory object
class MemoryCleanup
{
	public:
		MemoryCleanup(const string& name = "");
		~MemoryCleanup();
		
		MemoryCleanup(const MemoryCleanup& other) = delete;
		MemoryCleanup& operator=(const MemoryCleanup& other) = delete;
		MemoryCleanup(MemoryCleanup&& other) = default;
		MemoryCleanup& operator=(MemoryCleanup&& other) = default;
		
	private:
		string memoryName;
};

//Performs automatic cleanup for a named mutex
class MutexCleanup
{
	public:
		MutexCleanup(const string& name = "");
		~MutexCleanup();
		
		MutexCleanup(const MutexCleanup& other) = delete;
		MutexCleanup& operator=(const MutexCleanup& other) = delete;
		MutexCleanup(MutexCleanup&& other) = default;
		MutexCleanup& operator=(MutexCleanup&& other) = default;
		
	private:
		string mutexName;
};

//Wrapper for a shared memory object with associated mapped region and optional cleanup
class MemoryWrapper
{
	public:
		MemoryWrapper() : cleanup("") {}
		~MemoryWrapper();
		
		MemoryWrapper(const MemoryWrapper& other) = delete;
		MemoryWrapper& operator=(const MemoryWrapper& other) = delete;
		MemoryWrapper(MemoryWrapper&& other) = default;
		MemoryWrapper& operator=(MemoryWrapper&& other) = default;
		
		void map(ipc::mode_t mode);
		
		unique_ptr<ipc::shared_memory_object> memory;
		unique_ptr<ipc::mapped_region> mapped;
		MemoryCleanup cleanup;
};

//Wrapper for a named mutex with optional cleanup
class MutexWrapper
{
	public:
		MutexWrapper() : cleanup("") {}
		~MutexWrapper();
		
		MutexWrapper(const MutexWrapper& other) = delete;
		MutexWrapper& operator=(const MutexWrapper& other) = delete;
		MutexWrapper(MutexWrapper&& other) = default;
		MutexWrapper& operator=(MutexWrapper&& other) = default;
		
		unique_ptr<ipc::named_mutex> mutex;
		MutexCleanup cleanup;
};

class IPCUtils
{
	public:
		
		//Creates a shared memory object
		static MemoryWrapper createSharedMemory(const string& name, uint64_t size, ipc::mode_t mode);
		
		//Waits until the specified shared memory object exists and then retrieves it
		static MemoryWrapper getMemoryOnceExists(const string& name, ipc::mode_t mode);
		
		//Creates or opens the specified named mutex
		static MutexWrapper getNamedMutex(const string& name, bool cleanup = false);
		
		//Fills the contents of a shared memory region
		static void fillMemory(ipc::mapped_region& region, uint8_t value);
};

} //End MediaIPC

#endif
