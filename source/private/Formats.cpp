#include "../public/Formats.h"

namespace MediaIPC {

uint8_t FormatDetails::bytesPerSample(AudioFormat format)
{
	switch (format)
	{
		#define AUDIO_FORMAT(name, bytes, description) case AudioFormat::name: return bytes;
		#include "../public/AudioFormats.inc"
		
		case AudioFormat::None:
		default:
			return 0;
	}
}

uint8_t FormatDetails::bytesPerPixel(VideoFormat format)
{
	switch (format)
	{
		#define VIDEO_FORMAT(name, bytes, description) case VideoFormat::name: return bytes;
		#include "../public/VideoFormats.inc"
		
		case VideoFormat::None:
		default:
			return 0;
	}
}

std::string FormatDetails::description(AudioFormat format)
{
	switch (format)
	{
		#define AUDIO_FORMAT(name, bytes, description) case AudioFormat::name: return description;
		#include "../public/AudioFormats.inc"
		
		case AudioFormat::None:
		default:
			return "No audio";
	}
}

std::string FormatDetails::description(VideoFormat format)
{
	switch (format)
	{
		#define VIDEO_FORMAT(name, bytes, description) case VideoFormat::name: return description;
		#include "../public/VideoFormats.inc"
		
		case VideoFormat::None:
		default:
			return "No video";
	}
}

} //End MediaIPC
