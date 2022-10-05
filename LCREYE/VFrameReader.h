#pragma once

#include <dxgi.h>
#include <vcclr.h>
#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <cmath>
#include <msclr\marshal_cppstd.h>
#include <shlobj_core.h>
#include <shlwapi.h>
#include <string>


namespace LCREYE {
	public ref class VFrameReader {
	public:
		int cFrameWidth;
		int cFrameHeight;
		bool isCanceled = false;
		System::String^ appName;
		HDC selectedMonitor;
		int selectedMonitorNumber = 0;
		int selectedMonitorWidth;
		int selectedMonitorHeight;
		bool faceCascadeLoaded = FALSE; // faces opencv interface loaded check
		

		//System::Windows::Forms::PictureBox^ capView = nullptr;
		System::ComponentModel::BackgroundWorker^ bgWorker;
		System::Windows::Forms::RichTextBox^ consoleBox;
		
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
		float scoreThreshold = (float)0.9; // Filter out faces of score < score_threshold
		float nmsThreshold = (float)0.3; // Suppress bounding boxes of iou >= nms_threshold
		float scale = (float)1.0; // Scale factor used to resize input video frames
		int topK = 5000; // Keep top_k bounding boxes before NMS
		double cosineSimilarThreshold = (double)0.363;
		double l2NorSimilarThreshold = (double)1.128;

		// get frame with GDI
		System::Drawing::Image^ GetFrame(HWND);
		System::Drawing::Image^ GetFrameMonitor(HDC);

		// convert bitmap to mat
		cv::Mat Bitmap2Mat(System::Drawing::Bitmap^);

		// convert mat to bitmap
		System::Drawing::Bitmap^ Mat2Bitmap(cv::Mat);

		// capture rectangle in image via cv Mat
		cv::Mat DetectRectangles(cv::Mat&);

		// capture line in image via cv Mat
		cv::Mat DetectLines(cv::Mat&);

		// Load face haar xml
		cv::CascadeClassifier LoadFaceCascadeXML();
	
		// Detect face in cv Mat
		cv::Mat DetectFaces(cv::Mat&, cv::CascadeClassifier);

		// Detect face with dnn yunet
		cv::Mat DetectFacesYunet(cv::Mat&, std::string);
	};

	

}