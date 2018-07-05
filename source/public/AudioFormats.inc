#ifndef AUDIO_FORMAT
#define AUDIO_FORMAT(name, bytes, description)
#endif

AUDIO_FORMAT(PCM_S8,    sizeof(int8_t),   "PCM signed 8-bit")
AUDIO_FORMAT(PCM_U8,    sizeof(uint8_t),  "PCM unsigned 8-bit")
AUDIO_FORMAT(PCM_S16BE, sizeof(int16_t),  "PCM signed 16-bit big-endian")
AUDIO_FORMAT(PCM_S16LE, sizeof(int16_t),  "PCM signed 16-bit little-endian")
AUDIO_FORMAT(PCM_U16BE, sizeof(uint16_t), "PCM unsigned 16-bit big-endian")
AUDIO_FORMAT(PCM_U16LE, sizeof(uint16_t), "PCM unsigned 16-bit little-endian")
AUDIO_FORMAT(PCM_S24BE, sizeof(3),        "PCM signed 24-bit big-endian")
AUDIO_FORMAT(PCM_S24LE, sizeof(3),        "PCM signed 24-bit little-endian")
AUDIO_FORMAT(PCM_U24BE, sizeof(3),        "PCM unsigned 24-bit big-endian")
AUDIO_FORMAT(PCM_U24LE, sizeof(3),        "PCM unsigned 24-bit little-endian")
AUDIO_FORMAT(PCM_S32BE, sizeof(int32_t),  "PCM signed 32-bit big-endian")
AUDIO_FORMAT(PCM_S32LE, sizeof(int32_t),  "PCM signed 32-bit little-endian")
AUDIO_FORMAT(PCM_U32BE, sizeof(uint32_t), "PCM unsigned 32-bit big-endian")
AUDIO_FORMAT(PCM_U32LE, sizeof(uint32_t), "PCM unsigned 32-bit little-endian")
AUDIO_FORMAT(PCM_F32BE, sizeof(float),    "PCM 32-bit floating-point big-endian")
AUDIO_FORMAT(PCM_F32LE, sizeof(float),    "PCM 32-bit floating-point little-endian")
AUDIO_FORMAT(PCM_F64BE, sizeof(double),   "PCM 64-bit floating-point big-endian")
AUDIO_FORMAT(PCM_F64LE, sizeof(double),   "PCM 64-bit floating-point little-endian")

#undef AUDIO_FORMAT
