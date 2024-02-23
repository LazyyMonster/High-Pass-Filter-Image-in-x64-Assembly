
namespace Histogram {

	using namespace System::Runtime::InteropServices; //required to import dll's
    using namespace System::Windows::Forms::DataVisualization::Charting;
    using namespace System::Drawing;
    using namespace System::Collections;
	public ref class NativeMethods
	{/*
        void CreateColorChannelHistogram(Bitmap^ bmp)
        {
            // Create histograms for each color channel
            array<int>^ redHistogram = gcnew array<int>(256);
            array<int>^ greenHistogram = gcnew array<int>(256);
            array<int>^ blueHistogram = gcnew array<int>(256);

            for (int x = 0; x < bmp->Width; x++)
            {
                for (int y = 0; y < bmp->Height; y++)
                {
                    Color pixelColor = bmp->GetPixel(x, y);

                    // Increment the count for each color channel
                    redHistogram[pixelColor.R]++;
                    greenHistogram[pixelColor.G]++;
                    blueHistogram[pixelColor.B]++;
                }
            }

            // Create and display the histograms
            CreateHistogram(redHistogram, "Red");
            CreateHistogram(greenHistogram, "Green");
            CreateHistogram(blueHistogram, "Blue");
        }

        void CreateHistogram(array<int>^ histogram, String^ channelName)
        {
            // Create a new chart
            Chart^ chart = gcnew Chart();
            chart->Width = 800;
            chart->Height = 600;

            // Create a new chart area
            ChartArea^ chartArea = gcnew ChartArea();
            chart->ChartAreas->Add(chartArea);

            // Create a new series using the histogram data
            Series^ series = gcnew Series();
            series->ChartType = SeriesChartType::Column;
            for (int i = 0; i < histogram->Length; i++)
            {
                if (histogram[i] > 0)
                {
                    series->Points->AddXY(i, histogram[i]);
                }
            }

            // Add the series to the chart
            chart->Series->Add(series);

            // Set the title of the chart
            chart->Titles->Add(channelName + " Histogram");

            // Add the chart to the form
            this->Controls->Add(chart);
        }*/


	};
}