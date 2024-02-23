#pragma once
#pragma unmanaged
extern "C" void ASMHighPassFilter(
	unsigned char* bmpDataScan0,
	unsigned char* bmpOriginal,
	int imageSizeInBytes
);