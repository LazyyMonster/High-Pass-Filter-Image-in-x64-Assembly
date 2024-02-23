namespace DLLmanager {

	using namespace System::Runtime::InteropServices; //required to import dll's

	public ref class NativeMethods
	{
	public:

		//Import the DLL
		[DllImport(".\\CppDLL.dll", CallingConvention = CallingConvention::StdCall)]
			static void CppHighPassFilter(
				unsigned char* bmpDataScan0,
				unsigned char* bmpOriginal,
				int imageSizeInBytes,
				int width,
				int height
			);

		[DllImport(".\\AsmDLL.dll", CallingConvention = CallingConvention::StdCall)]
			static void ASMHighPassFilter(
				unsigned char* bmpDataScan0,
				unsigned char* bmpOriginal,
				int imageSizeInBytes,
				int width,
				int height
			);
	};
}