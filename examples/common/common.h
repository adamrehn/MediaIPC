#ifndef _EXAMPLE_COMMON_CODE
#define _EXAMPLE_COMMON_CODE

#include "../../source/public/ControlBlock.h"
#include <iostream>
#include <fstream>
#include <string>
#include <stdint.h>

//Prints a control block to an output stream
void printControlBlock(const MediaIPC::ControlBlock& cb, std::ostream& stream);

//Represents a named pipe
class NamedPipe
{
	public:
		NamedPipe() : filename(""), valid(false) {}
		NamedPipe(const std::string& name);
		~NamedPipe();
		
		//Retrieves the full filesystem path for the named pipe
		std::string path() const;
		
		//Determines if the pipe is valid
		bool isValid() const;
		
		//Attempts to open the write end of the pipe (will block until the read end is opened)
		bool open();
		
		//Closes the write end of the pipe
		void close();
		
		//Writes data to the pipe
		void write(const void* data, uint64_t length);
		
		//Named pipes cannot be copied, only moved
		
		NamedPipe(const NamedPipe& other) = delete;
		NamedPipe& operator=(const NamedPipe& other) = delete;
		
		NamedPipe(NamedPipe&& other);
		NamedPipe& operator=(NamedPipe&& other);
		
	private:
		std::string filename;
		bool valid;
		
		#ifdef _WIN32
			//Under Windows we maintain a native handle to the pipe
			void* handle;
		#else
			//Under POSIX-based systems we wrap the pipe in an output stream
			std::ofstream stream;
		#endif
		
		void moveFrom(NamedPipe&& other);
};

#endif
