#include "common.h"
#include "../../source/public/Formats.h"

#ifdef _WIN32
	#include <windows.h>
#else
	#include <sys/stat.h>
	#include <unistd.h>
#endif

void printControlBlock(const MediaIPC::ControlBlock& cb, std::ostream& stream)
{
	//Print the video details
	stream << "videoFormat = " << MediaIPC::FormatDetails::description(cb.videoFormat) << std::endl;
	if (cb.videoFormat != MediaIPC::VideoFormat::None)
	{
		stream << "width = " << cb.width << std::endl;
		stream << "height = " << cb.height << std::endl;
		stream << "bytesPerPixel = " << (uint32_t)MediaIPC::FormatDetails::bytesPerPixel(cb.videoFormat) << std::endl;
		stream << "frameRate = " << cb.frameRate << std::endl;
	}
	
	//Print the audio details
	stream << "audioFormat = " << MediaIPC::FormatDetails::description(cb.audioFormat) << std::endl;
	if (cb.audioFormat != MediaIPC::AudioFormat::None)
	{
		stream << "channels = " << cb.channels << std::endl;
		stream << "sampleRate = " << cb.sampleRate << std::endl;
		stream << "samplesPerBuffer = " << cb.samplesPerBuffer << std::endl;
		stream << "bytesPerSample = " << (uint32_t)MediaIPC::FormatDetails::bytesPerSample(cb.audioFormat) << std::endl;
	}
	
	stream << std::endl;
}

NamedPipe::NamedPipe(const std::string& name)
{
	//Generate the full path for the pipe
	#ifdef _WIN32
		this->filename = "\\\\.\\pipe\\" + name;
	#else
		this->filename = "/tmp/" + name;
	#endif
	
	//Attempt to create the pipe
	#ifdef _WIN32
		DWORD dwOpenMode = PIPE_ACCESS_OUTBOUND | FILE_FLAG_FIRST_PIPE_INSTANCE;
		DWORD dwPipeMode = PIPE_TYPE_BYTE | PIPE_WAIT | PIPE_REJECT_REMOTE_CLIENTS;
		this->handle = CreateNamedPipe(this->filename.c_str(), dwOpenMode, dwPipeMode, 1, 4096, 4096, 0, nullptr);
		this->valid = (this->handle != INVALID_HANDLE_VALUE);
	#else
		this->valid = (mkfifo(this->filename.c_str(), 0666) == 0);
	#endif
}

NamedPipe::~NamedPipe()
{
	if (this->valid == true)
	{
		//Remove the pipe from the filesystem under POSIX-based systems
		#ifndef _WIN32
			unlink(this->filename.c_str());
		#endif
	}
}

std::string NamedPipe::path() const {
	return this->filename;
}

bool NamedPipe::isValid() const {
	return this->valid;
}

bool NamedPipe::open()
{
	#ifdef _WIN32
		return (ConnectNamedPipe(this->handle, nullptr) ? true : (GetLastError() == ERROR_PIPE_CONNECTED)); 
	#else
		this->stream.open(this->filename, std::ios::binary);
		return this->stream.is_open();
	#endif
}

void NamedPipe::close()
{
	#ifdef _WIN32
		CloseHandle(this->handle);
	#else
		this->stream.close();
	#endif
}

void NamedPipe::write(const void* data, uint64_t length)
{
	#ifdef _WIN32
		DWORD bytesWritten;
		WriteFile(this->handle, data, length, &bytesWritten, nullptr);
	#else
		this->stream.write((const char*)data, length);
	#endif
}

NamedPipe::NamedPipe(NamedPipe&& other) {
	this->moveFrom(std::move(other));
}

NamedPipe& NamedPipe::operator=(NamedPipe&& other)
{
	this->moveFrom(std::move(other));
	return *this;
}

void NamedPipe::moveFrom(NamedPipe&& other)
{
	this->filename = std::move(other.filename);
	this->valid = other.valid;
	other.valid = false;
}
