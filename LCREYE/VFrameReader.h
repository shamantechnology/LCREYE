#pragma once

#include "dxgi.h"
#include <vcclr.h>
#include <opencv2/opencv.hpp>

namespace LCREYE {
	public ref class VFrameReader {
	public:
		// public variables
		int cFrameWidth;
		int cFrameHeight;
		bool isCanceled = false;
		System::String^ appName = nullptr;
		System::Windows::Forms::PictureBox^ capView = nullptr;
		System::ComponentModel::BackgroundWorker^ bgWorker = nullptr;

		// public functions
		VFrameReader();

		// get frame with GDI
		System::Drawing::Image^ GetFrame(HWND);

		// get words from an image
		System::Void GetSpatialWords(void);

		// background worker CancelWork
		System::Void CancelWork(void);

		// background worker DoWork
		System::Void DoWork(System::ComponentModel::DoWorkEventArgs^);

	protected:
		// public protected functions
		~VFrameReader();

	};
}