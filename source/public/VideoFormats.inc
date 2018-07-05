#ifndef VIDEO_FORMAT
#define VIDEO_FORMAT(name, bytes, description)
#endif

VIDEO_FORMAT(GRAY8,    1, "Grayscale 8-bit")
VIDEO_FORMAT(GRAY16BE, 2, "Grayscale 16-bit big-endian")
VIDEO_FORMAT(GRAY16LE, 2, "Grayscale 16-bit little-endian")
VIDEO_FORMAT(RGB,      3, "Packed RGB 8:8:8")
VIDEO_FORMAT(BGR,      3, "Packed RGB 8:8:8")
VIDEO_FORMAT(RGBA,     4, "Packed RGBA 8:8:8:8")
VIDEO_FORMAT(BGRA,     4, "Packed BGRA 8:8:8:8")
VIDEO_FORMAT(ARGB,     4, "Packed ARGB 8:8:8:8")
VIDEO_FORMAT(ABGR,     4, "Packed ABGR 8:8:8:8")

#undef VIDEO_FORMAT
