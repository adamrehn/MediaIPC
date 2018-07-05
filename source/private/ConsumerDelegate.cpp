#include "../public/ConsumerDelegate.h"

namespace MediaIPC {

ConsumerDelegate::~ConsumerDelegate() {}

FunctionConsumerDelegate::FunctionConsumerDelegate()
{
	this->setControlBlockHandler( [](const ControlBlock&){} );
	this->setVideoHandler( [](const uint8_t*, uint64_t){} );
	this->setAudioHandler( [](const uint8_t*, uint64_t){} );
}

void FunctionConsumerDelegate::setControlBlockHandler(ControlBlockCallback cbHandler) {
	this->cbHandler = cbHandler;
}

void FunctionConsumerDelegate::setVideoHandler(DataCallback videoHandler) {
	this->videoHandler = videoHandler;
}

void FunctionConsumerDelegate::setAudioHandler(DataCallback audioHandler) {
	this->audioHandler = audioHandler;
}

void FunctionConsumerDelegate::controlBlockReceived(const ControlBlock& cb) {
	this->cbHandler(cb);
}

void FunctionConsumerDelegate::videoFrameReceived(const uint8_t* buffer, uint64_t length) {
	this->videoHandler(buffer, length);
}

void FunctionConsumerDelegate::audioSamplesReceived(const uint8_t* buffer, uint64_t length) {
	this->audioHandler(buffer, length);
}

} //End MediaIPC
