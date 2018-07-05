#include "ObjectNames.h"

namespace MediaIPC {

ObjectNames::ObjectNames(const string& prefix)
{
	this->statusMemory = prefix + "ProducerStatusSharedMemory";
	this->statusMutex = prefix + "ProducerStatusNamedMutex";
	this->controlBlockMemory = prefix + "ControlBlockSharedMemory";
	this->controlBlockMutex = prefix + "ControlBlockNamedMutex";
	this->videoFrontBuffer = prefix + "VideoFrontBufferSharedMemory";
	this->videoBackBuffer = prefix + "VideoBackBufferSharedMemory";
	this->videoMutex = prefix + "VideoNamedMutex";
	this->frontBufferMutex = prefix + "VideoFrontBufferNamedMutex";
	this->backBufferMutex = prefix + "VideoBackBufferNamedMutex";
	this->audioBuffer = prefix + "AudioBufferSharedMemory";
	this->audioMutex = prefix + "AudioNamedMutex";
}

} //End MediaIPC
