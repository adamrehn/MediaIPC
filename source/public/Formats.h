#ifndef _MEDIA_IPC_FORMATS
#define _MEDIA_IPC_FORMATS

#include <stdint.h>
#include <string>

namespace MediaIPC {

enum class AudioFormat : uint8_t
{
	#define AUDIO_FORMAT(name, bytes, description) name,
	#include "AudioFormats.inc"
	
	Unknown = 255
};

enum class VideoFormat : uint8_t
{
	#define VIDEO_FORMAT(name, bytes, description) name,
	#include "VideoFormats.inc"
	
	Unknown = 255
};

class FormatDetails
{
	public:
		static uint8_t bytesPerSample(AudioFormat format);
		static uint8_t bytesPerPixel(VideoFormat format);
		
		static std::string description(AudioFormat format);
		static std::string description(VideoFormat format);
};

} //End MediaIPC

#endif
