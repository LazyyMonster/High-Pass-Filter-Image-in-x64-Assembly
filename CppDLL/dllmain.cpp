// Image filtering procedure with high - pass filter using 3x3 mask
//
// 2023 / 2024
// Author: Karol Skiba
// Silesian University of Technology
//
//Description:
//	This procedure calculates a new value of each color channel of 24-bit pixel from a given array and stores it in an empty array.
//	It processes the corners and edges seperately to calculate edge pixel values.

#include "pch.h"
#pragma unmanaged

extern "C" __declspec(dllexport) void CppHighPassFilter(
	unsigned char* bmpDataScan0,
	unsigned char* bmpOriginal,
	int imageSizeInBytes,
	int width,
	int height)
{
	int kernel[3][3] = {
		{-1, -1, -1},
		{-1, 9, -1},
		{-1, -1, -1}
	};

	for (int y = 1; y < height - 1; y++)
	{
		for (int x = 1; x < width - 1; x++)
		{
			for (int c = 0; c < 3; c++) 
			{
				float sum = 0;
				for (int ky = -1; ky <= 1; ky++)
				{
					for (int kx = -1; kx <= 1; kx++)
					{
						int pos = ((y + ky) * width + (x + kx)) * 3 + c;
						sum += bmpOriginal[pos] * kernel[ky + 1][kx + 1];
					}
				}

				int pos = (y * width + x) * 3 + c;
				if (sum < 0) {
					bmpDataScan0[pos] = 0;
				}
				else if (sum > 255) {
					bmpDataScan0[pos] = 255;
				}
				else {
					bmpDataScan0[pos] = sum;
				}
			}
		}
	}
}

BOOL APIENTRY DllMain(HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

