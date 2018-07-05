#ifndef _MEDIA_IPC_OBJECT_NAMES
#define _MEDIA_IPC_OBJECT_NAMES

#include <string>
using std::string;

namespace MediaIPC {

class ObjectNames
{
	public:
		
		//Resolves the fully-qualified object names for the supplied prefix
		ObjectNames(const string& prefix);
		
		//Producer status shared memory and named mutex
		string statusMemory;
		string statusMutex;
		
		//Control block shared memory and named mutex
		string controlBlockMemory;
		string controlBlockMutex;
		
		//Video shared memory and named mutexes
		string videoFrontBuffer;
		string videoBackBuffer;
		string videoMutex;
		string frontBufferMutex;
		string backBufferMutex;
		
		//Audio shared memory and named mutex
		string audioBuffer;
		string audioMutex;
		//Something here to coordinate ring buffer?
};

} //End MediaIPC

#endif
