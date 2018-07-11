# MediaIPC: an IPC-based media transfer library

MediaIPC provides a convenient and efficient mechanism for in-memory transfer of raw, uncompressed audio and video data. The library provides a simple interface that allows producer and consumer processes to exchange data without needing to concern themselves with managing shared memory resources and synchronisation primitives. MediaIPC is designed to be lightweight and portable, making it suitable for being embedded in wide variety of media and 3D applications.

Example use cases:

- Capturing audio and/or video streams from 3D applications performing offscreen rendering on a headless device (such as a Raspberry Pi)
- Providing bridging code for applications that procedurally generate raw audio and/or video data but lack the functionality to encode it
- Injecting debug hooks to intercept raw audio and/or video data as it flows through a processing pipeline


## Contents

- [Requirements](#requirements)
- [Usage](#usage)
- [License](#license)


## Requirements

The following components are required in order to build libMediaIPC from source:

- A modern, C++11-compliant compiler
- [CMake](https://cmake.org/) 3.8 or newer
- [Boost](https://www.boost.org/) 1.64 or newer (only the headers are needed to build libMediaIPC itself, but additional examples will be built if Boost.System is available)

Only a C++11-compliant compiler is needed to build applications that link against libMediaIPC. The Boost headers are only used inside the private translation units of the library, which means that **client applications do not have a dependency on Boost.**


## Usage

The [examples directory](./examples) contains example code for both producer processes and consumer processes. The high-level flow for a one-to-one transfer scenario (one producer process and one consumer process) is as follows:

- The producer process populates a [control block](./source/public/ControlBlock.h) structure with the details of the audio and video data (resolution, framerate, channel count, encoding, etc.)
- The producer process creates all of the shared memory and synchronisation primitives that will be used for communication, and places the control block data in its shared memory buffers.
- The consumer process awaits the creation of the shared resources and then reads the control block data.
- Once the consumer process has received the control block, it automatically begins sampling the audio and video data buffers at regular intervals.
- Whenever new data is available, the producer places the data in its shared memory buffers, ready to be sampled by the consumer process.
- To end the data transfer, the producer process sets a completion flag in its shared memory buffers, which is then detected by the consumer process.

The flow for a one-to-many scenario (one producer process and multiple consumer processes) follows the same pattern, except that consumer processes may join in at any time (once the shared resources are created and the control block data is in place, new consumer processes will begin sampling immediately.) Note however that access to the shared memory buffers is protected by synchronisation primitives and that large numbers of consumer processes all sampling the data of one producer process concurrently may result in a degradation of transfer performance.


## License

Copyright &copy; 2018, Adam Rehn. Licensed under the MIT License, see the file [LICENSE](./LICENSE) for details.
