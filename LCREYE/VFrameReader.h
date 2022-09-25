#pragma once

#include <dxgi.h>
#include <vcclr.h>
#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <cmath>

namespace LCREYE {
	public ref class VFrameReader {
	public:
		// public variables
		int cFrameWidth;
		int cFrameHeight;
		bool isCanceled = false;
		System::String^ appName = nullptr;
		HDC selectedMonitor;
		int selectedMonitorNumber = 0;
		int selectedMonitorWidth;
		int selectedMonitorHeight;
		//System::Windows::Forms::PictureBox^ capView = nullptr;
		System::ComponentModel::BackgroundWorker^ bgWorker = nullptr;
		System::Windows::Forms::RichTextBox^ consoleBox = nullptr;

		// public functions
		VFrameReader();

		// get frame with GDI
		System::Drawing::Image^ GetFrame(HWND);
		System::Drawing::Image^ GetFrameMonitor(HDC);

		// background worker CancelWork
		System::Void CancelWork(void);

		// background worker DoWork

		// captures per app
		System::Void DoWorkApp(System::ComponentModel::DoWorkEventArgs^);

		// captures per monitor
		System::Void DoWorkMonitor(System::ComponentModel::DoWorkEventArgs^ e);

		// capture rectangle in image via openCV
		System::Void DetectRectangles(System::Drawing::Image^ cFrame);

	protected:
		// public protected functions
		~VFrameReader();
	
	private:
		// convert bitmap to mat
		cv::Mat Bitmap2Mat(System::Drawing::Bitmap^);

		// convert mat to bitmap
		System::Drawing::Bitmap^ Mat2Bitmap(cv::Mat);
	};

	

}