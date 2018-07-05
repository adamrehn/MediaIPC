#ifndef _MEDIA_IPC_MEMORY_UTILS
#define _MEDIA_IPC_MEMORY_UTILS

#include <memory>
#include <utility>

namespace MediaIPC {

class MemoryUtils
{
	public:
		
		//Converts an r-value reference to a move-constructible type into a std::unique_ptr
		template <typename T>
		static std::unique_ptr<T> toPointer(T&& ref) {
			return std::unique_ptr<T>( new T(std::move(ref)) );
		}
};

} //End MediaIPC

#endif
