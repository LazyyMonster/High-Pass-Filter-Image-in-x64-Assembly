#include "NativeMethods.h"

namespace CppCLRWinFormsProject {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::Drawing::Imaging;
	using namespace System::Diagnostics;
	using namespace System::Runtime::InteropServices;
	using namespace System::Threading;
	using namespace System::Collections::Generic;
	using namespace System::Windows::Forms::DataVisualization::Charting;
	using namespace System::Drawing;
	using namespace DLLmanager;

	public ref class Form1 : public System::Windows::Forms::Form {
	
	public:
		NativeMethods^ nativeMethods = gcnew NativeMethods();
		int processorCount = Environment::ProcessorCount;
		String^ processorCountStr = processorCount.ToString();
		Bitmap^ bmpFront;
		Bitmap^ bmpFrontAsm;
		Bitmap^ bmpFrontCpp;
		unsigned char* bmpOriginal;
		int imageSizeInBytes = -1;
		Rectangle imageRectangle;
		BitmapData^ bmpData;

		void ClearOriginalImage()
		{
			if (bmpOriginal != nullptr)
			{
				delete[] bmpOriginal;
			}
		}

		void SaveOriginalImage(Bitmap^ bmp)
		{
			ClearOriginalImage();
			imageSizeInBytes = bmp->Width * bmp->Height * 3;

			bmpOriginal = new unsigned char[imageSizeInBytes];

			imageRectangle.Width = bmp->Width;
			imageRectangle.Height = bmp->Height;

			bmpData = bmp->LockBits(imageRectangle, System::Drawing::Imaging::ImageLockMode::ReadOnly,
				PixelFormat::Format24bppRgb);

			unsigned char* p = (unsigned char*)bmpData->Scan0.ToPointer();

			for (int i = 0; i < imageSizeInBytes; i++)
			{
				bmpOriginal[i] = *p++;
			}
			bmp->UnlockBits(bmpData);
		}

		// Methods to start the imported methods in separate threads
		void CppHighPassFilterWrapper(Object^ obj)
		{
			Tuple<Barrier^, Tuple<int, int, int>^>^ data = safe_cast<Tuple<Barrier^, Tuple<int, int, int>^>^>(obj);
			Barrier^ barrier = data->Item1;
			Tuple<int, int, int>^ params = data->Item2;

			int start = params->Item1;
			int end = params->Item2;
			int height = params->Item3;

			barrier->SignalAndWait();

			nativeMethods->CppHighPassFilter((unsigned char*)bmpData->Scan0.ToPointer() + start, bmpOriginal + start, end - start, imageRectangle.Width, height);
		}

		void ASMHighPassFilterWrapper(Object^ obj)
		{
			
			Tuple<Barrier^, Tuple<int, int, int>^>^ data = safe_cast<Tuple<Barrier^, Tuple<int, int, int>^>^>(obj);
			Barrier^ barrier = data->Item1;
			Tuple<int, int, int>^ params = data->Item2;

			int start = params->Item1;
			int end = params->Item2;
			int height = params->Item3;
			
			barrier->SignalAndWait();

			nativeMethods->ASMHighPassFilter((unsigned char*)bmpData->Scan0.ToPointer() + start, bmpOriginal + start, end - start, 3 * (imageRectangle.Width), height);
		}

		Form1(void)
		{
			InitializeComponent();

			btnRun->Enabled = false;
			btnHistogram->Enabled = false;
			rbAsm->Checked = true;
			
			lbThreadsNmb->Text = processorCountStr;
			trkThreads->Value = processorCount;
		}

	protected:
		~Form1()
		{
			if (components)
			{
				delete components;
			}
		}

	
	private: System::Windows::Forms::PictureBox^ pbOld;
	private: System::Windows::Forms::PictureBox^ pbNew;
	private: System::Windows::Forms::RadioButton^ rbAsm;
	private: System::Windows::Forms::RadioButton^ rbCpp;
	private: System::Windows::Forms::Button^ btnRun;
	private: System::Windows::Forms::Button^ btnChoose;
	private: System::Windows::Forms::Button^ btnClear;
	private: System::Windows::Forms::Button^ btnHistogram;
	private: System::Windows::Forms::Button^ btnSaveImg;
	private: System::Windows::Forms::SaveFileDialog^ dlgSave;
	private: System::Windows::Forms::TrackBar^ trkThreads;
	private: System::Windows::Forms::Label^ lbThreadsTxt;
	private: System::Windows::Forms::Label^ lbThreadsNmb;
	private: System::Windows::Forms::Label^ lblASMTime;
	private: System::Windows::Forms::Label^ lblCppTime;
	private: System::Windows::Forms::OpenFileDialog^ dlgOpen;

	private: System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->pbOld = (gcnew System::Windows::Forms::PictureBox());
			this->pbNew = (gcnew System::Windows::Forms::PictureBox());
			this->rbAsm = (gcnew System::Windows::Forms::RadioButton());
			this->rbCpp = (gcnew System::Windows::Forms::RadioButton());
			this->btnRun = (gcnew System::Windows::Forms::Button());
			this->btnChoose = (gcnew System::Windows::Forms::Button());
			this->btnClear = (gcnew System::Windows::Forms::Button());
			this->trkThreads = (gcnew System::Windows::Forms::TrackBar());
			this->lbThreadsTxt = (gcnew System::Windows::Forms::Label());
			this->lbThreadsNmb = (gcnew System::Windows::Forms::Label());
			this->dlgOpen = (gcnew System::Windows::Forms::OpenFileDialog());
			this->lblCppTime = (gcnew System::Windows::Forms::Label());
			this->lblASMTime = (gcnew System::Windows::Forms::Label());
			this->btnHistogram = (gcnew System::Windows::Forms::Button());
			this->btnSaveImg = (gcnew System::Windows::Forms::Button());
			this->dlgSave = (gcnew System::Windows::Forms::SaveFileDialog());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pbOld))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pbNew))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->trkThreads))->BeginInit();
			this->SuspendLayout();
			// 
			// pbOld
			// 
			this->pbOld->Location = System::Drawing::Point(383, 12);
			this->pbOld->Name = L"pbOld";
			this->pbOld->Size = System::Drawing::Size(446, 240);
			this->pbOld->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			this->pbOld->TabIndex = 0;
			this->pbOld->TabStop = false;
			// 
			// pbNew
			// 
			this->pbNew->Location = System::Drawing::Point(383, 259);
			this->pbNew->Name = L"pbNew";
			this->pbNew->Size = System::Drawing::Size(446, 240);
			this->pbNew->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			this->pbNew->TabIndex = 1;
			this->pbNew->TabStop = false;
			// 
			// rbAsm
			// 
			this->rbAsm->AutoSize = true;
			this->rbAsm->Location = System::Drawing::Point(318, 386);
			this->rbAsm->Name = L"rbAsm";
			this->rbAsm->Size = System::Drawing::Size(45, 17);
			this->rbAsm->TabIndex = 2;
			this->rbAsm->TabStop = true;
			this->rbAsm->Text = L"Asm";
			this->rbAsm->UseVisualStyleBackColor = true;
			this->rbAsm->CheckedChanged += gcnew System::EventHandler(this, &Form1::rbAsm_CheckedChanged);
			// 
			// rbCpp
			// 
			this->rbCpp->AutoSize = true;
			this->rbCpp->Location = System::Drawing::Point(261, 386);
			this->rbCpp->Name = L"rbCpp";
			this->rbCpp->Size = System::Drawing::Size(44, 17);
			this->rbCpp->TabIndex = 3;
			this->rbCpp->TabStop = true;
			this->rbCpp->Text = L"C++";
			this->rbCpp->UseVisualStyleBackColor = true;
			this->rbCpp->CheckedChanged += gcnew System::EventHandler(this, &Form1::rbCpp_CheckedChanged);
			// 
			// btnRun
			// 
			this->btnRun->AutoSize = true;
			this->btnRun->Location = System::Drawing::Point(277, 339);
			this->btnRun->Name = L"btnRun";
			this->btnRun->Size = System::Drawing::Size(75, 23);
			this->btnRun->TabIndex = 4;
			this->btnRun->Text = L"Run";
			this->btnRun->UseVisualStyleBackColor = true;
			this->btnRun->Click += gcnew System::EventHandler(this, &Form1::btnRun_Click);
			// 
			// btnChoose
			// 
			this->btnChoose->AutoSize = true;
			this->btnChoose->Location = System::Drawing::Point(25, 339);
			this->btnChoose->Name = L"btnChoose";
			this->btnChoose->Size = System::Drawing::Size(88, 23);
			this->btnChoose->TabIndex = 5;
			this->btnChoose->Text = L"Choose picture";
			this->btnChoose->UseVisualStyleBackColor = true;
			this->btnChoose->Click += gcnew System::EventHandler(this, &Form1::btnChoose_Click);
			// 
			// btnClear
			// 
			this->btnClear->AutoSize = true;
			this->btnClear->Location = System::Drawing::Point(155, 339);
			this->btnClear->Name = L"btnClear";
			this->btnClear->Size = System::Drawing::Size(75, 23);
			this->btnClear->TabIndex = 6;
			this->btnClear->Text = L"Clear";
			this->btnClear->UseVisualStyleBackColor = true;
			this->btnClear->Click += gcnew System::EventHandler(this, &Form1::btnClear_Click);
			// 
			// trkThreads
			// 
			this->trkThreads->Location = System::Drawing::Point(25, 421);
			this->trkThreads->Maximum = 64;
			this->trkThreads->Minimum = 1;
			this->trkThreads->Name = L"trkThreads";
			this->trkThreads->Size = System::Drawing::Size(214, 45);
			this->trkThreads->TabIndex = 7;
			this->trkThreads->TickFrequency = 8;
			this->trkThreads->Value = 1;
			this->trkThreads->Scroll += gcnew System::EventHandler(this, &Form1::trkThreads_Scroll);
			// 
			// lbThreadsTxt
			// 
			this->lbThreadsTxt->AutoSize = true;
			this->lbThreadsTxt->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(238)));
			this->lbThreadsTxt->Location = System::Drawing::Point(257, 421);
			this->lbThreadsTxt->Name = L"lbThreadsTxt";
			this->lbThreadsTxt->Size = System::Drawing::Size(71, 20);
			this->lbThreadsTxt->TabIndex = 8;
			this->lbThreadsTxt->Text = L"Threads:";
			// 
			// lbThreadsNmb
			// 
			this->lbThreadsNmb->AutoSize = true;
			this->lbThreadsNmb->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(238)));
			this->lbThreadsNmb->Location = System::Drawing::Point(334, 421);
			this->lbThreadsNmb->Name = L"lbThreadsNmb";
			this->lbThreadsNmb->Size = System::Drawing::Size(18, 20);
			this->lbThreadsNmb->TabIndex = 9;
			this->lbThreadsNmb->Text = L"1";
			// 
			// dlgOpen
			// 
			this->dlgOpen->FileName = L"openFileDialog1";
			this->dlgOpen->Filter = L"Jpeg|*.jpg|Bitmap|*.bmp|All files|*.*";
			// 
			// lblCppTime
			// 
			this->lblCppTime->AutoSize = true;
			this->lblCppTime->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(238)));
			this->lblCppTime->Location = System::Drawing::Point(36, 464);
			this->lblCppTime->Name = L"lblCppTime";
			this->lblCppTime->Size = System::Drawing::Size(142, 20);
			this->lblCppTime->TabIndex = 10;
			this->lblCppTime->Text = L"C++  in uSeconds: ";
			// 
			// lblASMTime
			// 
			this->lblASMTime->AutoSize = true;
			this->lblASMTime->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(238)));
			this->lblASMTime->Location = System::Drawing::Point(36, 484);
			this->lblASMTime->Name = L"lblASMTime";
			this->lblASMTime->Size = System::Drawing::Size(141, 20);
			this->lblASMTime->TabIndex = 11;
			this->lblASMTime->Text = L"Asm in uSeconds: ";
			// 
			// btnHistogram
			// 
			this->btnHistogram->Location = System::Drawing::Point(25, 380);
			this->btnHistogram->Name = L"btnHistogram";
			this->btnHistogram->Size = System::Drawing::Size(105, 23);
			this->btnHistogram->TabIndex = 12;
			this->btnHistogram->Text = L"Create histograms";
			this->btnHistogram->UseVisualStyleBackColor = true;
			this->btnHistogram->Click += gcnew System::EventHandler(this, &Form1::btnHistogram_Click);
			// 
			// btnSaveImg
			// 
			this->btnSaveImg->Location = System::Drawing::Point(277, 464);
			this->btnSaveImg->Name = L"btnSaveImg";
			this->btnSaveImg->Size = System::Drawing::Size(75, 23);
			this->btnSaveImg->TabIndex = 13;
			this->btnSaveImg->Text = L"Save image";
			this->btnSaveImg->UseVisualStyleBackColor = true;
			this->btnSaveImg->Click += gcnew System::EventHandler(this, &Form1::btnSaveImg_Click);
			// 
			// dlgSave
			// 
			this->dlgSave->CreatePrompt = true;
			this->dlgSave->FileName = L"tak";
			this->dlgSave->Filter = L"Jpeg|*.jpg|Bitmap|*.bmp|All files|*.*";
			// 
			// Form1
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(1520, 506);
			this->Controls->Add(this->btnSaveImg);
			this->Controls->Add(this->btnHistogram);
			this->Controls->Add(this->lblASMTime);
			this->Controls->Add(this->lblCppTime);
			this->Controls->Add(this->lbThreadsNmb);
			this->Controls->Add(this->lbThreadsTxt);
			this->Controls->Add(this->trkThreads);
			this->Controls->Add(this->btnClear);
			this->Controls->Add(this->btnChoose);
			this->Controls->Add(this->btnRun);
			this->Controls->Add(this->rbCpp);
			this->Controls->Add(this->rbAsm);
			this->Controls->Add(this->pbNew);
			this->Controls->Add(this->pbOld);
			this->Name = L"Form1";
			this->Text = L"Picture Processing";
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pbOld))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pbNew))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->trkThreads))->EndInit();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private: System::Void btnChoose_Click(System::Object^ sender, System::EventArgs^ e) {
		if (dlgOpen->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
			try {
				bmpFront = (Bitmap^)Image::FromFile(dlgOpen->FileName);

				if (bmpFront->PixelFormat == System::Drawing::Imaging::PixelFormat::Format24bppRgb)
				{
					SaveOriginalImage(bmpFront);
					RemoveAllCharts();
					pbOld->Image = bmpFront;
					pbNew->Image = nullptr;
					lbThreadsNmb->Text = processorCountStr;
					trkThreads->Value = processorCount;
					btnRun->Enabled = true;
				}
				else
				{
					MessageBox::Show("Incorect format of the image!");
					bmpFront = nullptr;
				}
			}
			catch (Exception^ ex) {
				MessageBox::Show(ex->Message);
			}
		}
	}

	private: System::Void btnSaveImg_Click(System::Object^ sender, System::EventArgs^ e) {
		if (dlgSave->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
			try {
				this->pbNew->Image->Save(dlgSave->FileName);
			}
			catch (Exception^ ex) {
				MessageBox::Show(ex->Message);
			}
		}
	}

	private: System::Void trkThreads_Scroll(System::Object^ sender, System::EventArgs^ e) 
	{
		lbThreadsNmb->Text = "" + trkThreads->Value;
	}

	private: System::Void btnClear_Click(System::Object^ sender, System::EventArgs^ e) 
	{
		btnRun->Enabled = false;
		pbOld->Image = nullptr;
		pbNew->Image = nullptr;
		RemoveAllCharts();
		lblASMTime->Text = "Asm in uSeconds:  ";
		lblCppTime->Text = "C++  in uSeconds:  ";
	}

	private: System::Void btnRun_Click(System::Object^ sender, System::EventArgs^ e) 
	{
		pbNew->Image = nullptr;
		RemoveAllCharts();

		bmpFrontAsm = bmpFront->Clone(Rectangle(0, 0, bmpFront->Width, bmpFront->Height), bmpFront->PixelFormat);
		bmpFrontCpp = bmpFront->Clone(Rectangle(0, 0, bmpFront->Width, bmpFront->Height), bmpFront->PixelFormat);

		Stopwatch^ timePerParse = gcnew Stopwatch();
		long ticksThisTime = 0;
		timePerParse->Start();
		
		if (rbAsm->Checked) {
			bmpData = bmpFrontAsm->LockBits(imageRectangle, System::Drawing::Imaging::ImageLockMode::WriteOnly,
				PixelFormat::Format24bppRgb);
			int threadNumbers = trkThreads->Value;
			int height = imageRectangle.Height;
			int width = imageRectangle.Width;

			int bytesPerLine = width * 3;
			int linesPerThread = height / threadNumbers;
			int bytesPerThread = bytesPerLine * linesPerThread;

			List<Thread^>^ threads = gcnew List<Thread^>();
			List<Tuple<int, int, int>^>^ threadData = gcnew List<Tuple<int, int, int>^>();
			Barrier^ barrier = gcnew Barrier(threadNumbers);

			for (int i = 0; i < threadNumbers; ++i)
			{
				int start = i * bytesPerThread;
				int end = (i == threadNumbers - 1) ? imageSizeInBytes : start + bytesPerThread;
				int height = (end - start) / bytesPerLine;

				Thread^ thread = gcnew Thread(gcnew ParameterizedThreadStart(this, &Form1::ASMHighPassFilterWrapper));
				thread->Name = "Thread " + i;
				threads->Add(thread);
				threadData->Add(gcnew Tuple<int, int, int>(start, end, height));
			}

			for (int i = 0; i < threads->Count; ++i)
			{
				threads[i]->Start(gcnew Tuple<Barrier^, Tuple<int, int, int>^>(barrier, threadData[i]));
			}

			for each (Thread ^ thread in threads)
			{
				thread->Join();
			}

			bmpFrontAsm->UnlockBits(bmpData);
			pbNew->Image = bmpFrontAsm;
		}
		else {
			bmpData = bmpFrontCpp->LockBits(imageRectangle, System::Drawing::Imaging::ImageLockMode::WriteOnly,
				PixelFormat::Format24bppRgb);
			int threadNumbers = trkThreads->Value;
			int height = imageRectangle.Height;
			int width = imageRectangle.Width;

			int bytesPerLine = width * 3;
			int linesPerThread = height / threadNumbers;
			int bytesPerThread = bytesPerLine * linesPerThread;

			List<Thread^>^ threads = gcnew List<Thread^>();
			List<Tuple<int, int, int>^>^ threadData = gcnew List<Tuple<int, int, int>^>();
			Barrier^ barrier = gcnew Barrier(threadNumbers);

			for (int i = 0; i < threadNumbers; ++i)
			{
				int start = i * bytesPerThread;
				int end = (i == threadNumbers - 1) ? imageSizeInBytes : start + bytesPerThread;
				int height = (end - start) / bytesPerLine;

				Thread^ thread = gcnew Thread(gcnew ParameterizedThreadStart(this, &Form1::CppHighPassFilterWrapper));
				thread->Name = "Thread " + i;
				threads->Add(thread);
				threadData->Add(gcnew Tuple<int, int, int>(start, end, height));
			}

			for (int i = 0; i < threads->Count; ++i)
			{
				threads[i]->Start(gcnew Tuple<Barrier^, Tuple<int, int, int>^>(barrier, threadData[i]));
			}

			for each (Thread ^ thread in threads)
			{
				thread->Join();
			}

			bmpFrontCpp->UnlockBits(bmpData);
			pbNew->Image = bmpFrontCpp;
		}
			
		timePerParse->Stop();
		ticksThisTime = timePerParse->ElapsedTicks;
		double microsecondsThisTime = (double)ticksThisTime / Stopwatch::Frequency * 1000000;
		//double millisecondsThisTime =  timePerParse->ElapsedMilliseconds;

		btnHistogram->Enabled = true;
		
		if (rbAsm->Checked) {
			lblASMTime->Text = "Asm in uSeconds:  " + microsecondsThisTime;
		}
		else {
			lblCppTime->Text = "C++  in uSeconds:  " + microsecondsThisTime;
		}
		delete timePerParse;
	}

	private: System::Void btnHistogram_Click(System::Object^ sender, System::EventArgs^ e) {

		RemoveAllCharts();
		if (rbAsm->Checked) {
			CreateColorChannelHistogram(bmpFront, Point(850, 25), 200, 200);
			CreateColorChannelHistogram(bmpFrontAsm, Point(850, 275), 200, 200);
		}
		else {
			CreateColorChannelHistogram(bmpFront, Point(850, 25), 200, 200);
			CreateColorChannelHistogram(bmpFrontCpp, Point(850, 275), 200, 200);
		}
	}

	private: System::Void rbCpp_CheckedChanged(System::Object^ sender, System::EventArgs^ e) {
		btnHistogram->Enabled = false;
	}

	private: System::Void rbAsm_CheckedChanged(System::Object^ sender, System::EventArgs^ e) {
		btnHistogram->Enabled = false;
	}

	   void CreateColorChannelHistogram(Bitmap^ bmp, Point location, int width, int height)
	   {
		   array<int>^ redHistogram = gcnew array<int>(256);
		   array<int>^ greenHistogram = gcnew array<int>(256);
		   array<int>^ blueHistogram = gcnew array<int>(256);

		   for (int x = 0; x < bmp->Width; x++)
		   {
			   for (int y = 0; y < bmp->Height; y++)
			   {
				   Color pixelColor = bmp->GetPixel(x, y);

				   redHistogram[pixelColor.R]++;
				   greenHistogram[pixelColor.G]++;
				   blueHistogram[pixelColor.B]++;
			   }
		   }
		   CreateHistogram(redHistogram, "Red", Point(location.X, location.Y), width, height);
		   CreateHistogram(greenHistogram, "Green", Point(location.X + width, location.Y), width, height);
		   CreateHistogram(blueHistogram, "Blue", Point(location.X + 2 * width, location.Y), width, height);
	   }

	   void CreateHistogram(array<int>^ histogram, String^ channelName, Point location, int width, int height)
	   {
		   Chart^ chart = gcnew Chart();
		   chart->Width = width;
		   chart->Height = height;

		   chart->Location = location;
		   ChartArea^ chartArea = gcnew ChartArea();
		   chart->ChartAreas->Add(chartArea);

		   Series^ series = gcnew Series();
		   series->ChartType = SeriesChartType::Column;
		   for (int i = 0; i < histogram->Length; i++)
		   {
			   if (histogram[i] > 0)
			   {
				   series->Points->AddXY(i, histogram[i]);
			   }
		   }	
		   chart->Series->Add(series);
		   chart->Titles->Add(channelName + " Histogram");
		   this->Controls->Add(chart);
	   }

	   void RemoveAllCharts()
	   {
		   btnHistogram->Enabled = false;
		   for (int i = this->Controls->Count - 1; i >= 0; i--)
		   {
			   Chart^ chart = dynamic_cast<Chart^>(this->Controls[i]);
			   if (chart != nullptr)
			   {
				   this->Controls->RemoveAt(i);
			   }
		   }
	   }
	};
}
