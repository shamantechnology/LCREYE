#pragma once

#include <dxgi.h>
#include <vcclr.h>
#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <cmath>
#include <msclr\marshal_cppstd.h>

namespace LCREYE {
	public ref class VFrameReader {
	public:
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
		bool faceCascadeLoaded = FALSE; // faces opencv interface loaded check
		System::String^ faceCascadeXML = "C:\\opencv\\build\\etc\\haarcascades\\haarcascade_frontalface_alt2.xml";
		
		VFrameReader();

		// background workers
		System::Void CancelWork(void);

		// captures per app
		System::Void DoWorkApp(System::ComponentModel::DoWorkEventArgs^);

		// captures per monitor
		System::Void DoWorkMonitor(System::ComponentModel::DoWorkEventArgs^);

	protected:
		~VFrameReader();
	
	private:
		// get frame with GDI
		System::Drawing::Image^ GetFrame(HWND);
		System::Drawing::Image^ GetFrameMonitor(HDC);

		// convert bitmap to mat
		cv::Mat Bitmap2Mat(System::Drawing::Bitmap^);

		// convert mat to bitmap
		System::Drawing::Bitmap^ Mat2Bitmap(cv::Mat);

		// capture rectangle in image via cv Mat
		cv::Mat DetectRectangles(cv::Mat&);

		// Load face haar xml
		cv::CascadeClassifier LoadFaceCascadeXML();
	
		// Detect face in cv Mat
		cv::Mat DetectFaces(cv::Mat&, cv::CascadeClassifier);
	};

	

}