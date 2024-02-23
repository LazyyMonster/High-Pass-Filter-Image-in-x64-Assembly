


public ref class Cpp
{
public:
	void CppHighPassFilter(
		unsigned char* bmpDataScan0,
		unsigned char* bmpOriginal,
		int imageSizeInBytes,
		int width,
		int height)
	{
		int kernel[3][3] = {
			{0, -1, 0},
			{-1, 5, -1},
			{0, -1, 0}
		};

		for (int y = 1; y < height - 1; y++)
		{
			for (int x = 1; x < width - 1; x++)
			{
				for (int c = 0; c < 3; c++) // loop over color channels
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
					if (sum < 0) bmpDataScan0[pos] = 0;
					else if (sum > 255) bmpDataScan0[pos] = 255;
					else bmpDataScan0[pos] = sum;
				}
			}
		}
	}

};