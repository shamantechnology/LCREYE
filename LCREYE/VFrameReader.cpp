#include "VFrameReader.h"
using namespace System;
using namespace System::Drawing;
using namespace System::Diagnostics;

///<summary>
/// Constructor
/// <summary>
LCREYE::VFrameReader::VFrameReader() {
    // default 1920x1080
    this->cFrameWidth = 1920;
    this->cFrameHeight = 1080;


}

/// <summary>
/// Deconstructor
/// </summary>
LCREYE::VFrameReader::~VFrameReader() {
    //this->capView = nullptr;
    this->bgWorker = nullptr;
}

/// <summary>
/// Convert CV mat to Bitmap
/// </summary>
Bitmap^ LCREYE::VFrameReader::Mat2Bitmap(cv::Mat cvImage) {
    //array<Byte>^ matImageData;
    //cv::cvtColor(cvImage, newImage, cv::COLOR_BGR2BGRA);
    cv::Size cvImageSize = cvImage.size();

    Bitmap^ matBitmap = gcnew Bitmap(cvImageSize.width, cvImageSize.height, cvImage.step1(), Imaging::PixelFormat::Format32bppRgb, (IntPtr)cvImage.data);

    //System::IO::MemoryStream^ matStream = gcnew System::IO::MemoryStream();
    //matBitmap->Save(matStream, Imaging::ImageFormat::Jpeg);
    //matImageData = matStream->ToArray();
    //delete matStream;

    //System::IO::MemoryStream^ matStream2 = gcnew System::IO::MemoryStream(matImageData);
    
    //delete matBitmap;
    //return (Bitmap^)Bitmap::FromStream(matStream2);

   
    
    //HBITMAP hBit = CreateBitmap(cvImage.cols,cvImage.rows,1,32,cvImage.data);

    //Bitmap^ outBitmap = Bitmap::FromHbitmap((IntPtr)hBit);

    //return outBitmap;
    
    return matBitmap;
}

/// <summary>
/// Convert CV Bitmap to mat with locking and in color
/// change the CV_8UC3 to CV_8UC1 for black and white
/// </summary>
cv::Mat LCREYE::VFrameReader::Bitmap2Mat(System::Drawing::Bitmap^ bmImage) {
    System::Drawing::Rectangle bmRect = System::Drawing::Rectangle(0, 0, bmImage->Width, bmImage->Height);

    System::Drawing::Imaging::BitmapData^ bmData = bmImage->LockBits(bmRect, System::Drawing::Imaging::ImageLockMode::ReadWrite, bmImage->PixelFormat);
    
    cv::Mat cvMat;

    try {
        void* data = bmData->Scan0.ToPointer();
        cvMat = cv::Mat(cv::Size(bmImage->Width, bmImage->Height), CV_8UC4, data, std::abs(bmData->Stride));
    }
    finally { bmImage->UnlockBits(bmData); }

    return cvMat;
}

///<summary>
///GetFrame gets the current desktop or window frame and display it in
/// output window CaptureView along with later analysis in OpenCV
/// This doesn't bring the window to the top and will need to be done by user
/// GDI based
///</summary>
Image^ LCREYE::VFrameReader::GetFrame(HWND cApp) {
    SetProcessDPIAware();

    Image^ vFrameImage;

    // use whole desktop to capture window to get over non windows api gui issue
    HDC cFrame = GetDC(HWND_DESKTOP);

    RECT fRect;
    GetWindowRect(cApp, &fRect);
    int frWidth = fRect.right - fRect.left;
    int frHeight = fRect.bottom - fRect.top;

    HDC cFrameMem = CreateCompatibleDC(cFrame);

    // create a bitmap to send to CaptureView
    HBITMAP cFrameHBitmap = CreateCompatibleBitmap(cFrame, frWidth, frHeight);
    HBITMAP cFrameCopy = (HBITMAP)SelectObject(cFrameMem, cFrameHBitmap);
    BitBlt(cFrameMem, 0, 0, frWidth, frHeight, cFrame, fRect.left, fRect.top, SRCCOPY | CAPTUREBLT);
    cFrameHBitmap = (HBITMAP)SelectObject(cFrameMem, cFrameCopy);

    // convert HBITMAP to BITMAP
    if (cFrameHBitmap != nullptr) {
        Bitmap^ vFrameBitmap = Bitmap::FromHbitmap((IntPtr)cFrameHBitmap);
       
        // clean up capture
        DeleteObject(cFrameHBitmap);
        DeleteObject(cFrameCopy);
        ReleaseDC(HWND_DESKTOP, cFrame);
        ReleaseDC(HWND_DESKTOP, cFrameMem);
        DeleteDC(cFrameMem);

        vFrameImage = (Image^)vFrameBitmap;
    }

    return vFrameImage;

}

///<summary>
///GetFrameMonitor like GetFrame but for monitors
/// GDI based
///</summary>
Image^ LCREYE::VFrameReader::GetFrameMonitor(HDC cHDC) {
    SetProcessDPIAware();

    // set window to top 
    // SetForegroundWindow(capWindow);
    // Sleep(250); // sleep to wait for the window to move
    Image^ vFrameImage;

    // use whole desktop to capture window to get over non windows api gui issue
    HDC cFrameMem = CreateCompatibleDC(cHDC);

    this->selectedMonitorWidth = GetSystemMetrics(SM_CXSCREEN);
    this->selectedMonitorHeight = GetSystemMetrics(SM_CYSCREEN);

    // create a bitmap to send to CaptureView
    HBITMAP cFrameHBitmap = CreateCompatibleBitmap(cHDC, this->selectedMonitorWidth, this->selectedMonitorHeight);
    HBITMAP cFrameCopy = (HBITMAP)SelectObject(cFrameMem, cFrameHBitmap);
    BitBlt(cFrameMem, 0, 0, this->selectedMonitorWidth, this->selectedMonitorHeight, cHDC, 0, 0, SRCCOPY);
    cFrameHBitmap = (HBITMAP)SelectObject(cFrameMem, cFrameCopy);

    //restore the foreground
    // SetForegroundWindow(GetConsoleWindow());

    // convert HBITMAP to BITMAP
    if (cFrameHBitmap != nullptr) {
        Bitmap^ vFrameBitmap = Bitmap::FromHbitmap((IntPtr)cFrameHBitmap);

        // clean up capture
        DeleteObject(cFrameHBitmap);
        DeleteObject(cFrameCopy);
        //ReleaseDC(HWND_DESKTOP, cHDC);
        ReleaseDC(HWND_DESKTOP, cFrameMem);
        DeleteDC(cFrameMem);

        vFrameImage = (Image^)vFrameBitmap;
    }

    return vFrameImage;

}

///<summary>
/// CancelWork, Cancels work of frame grabbing done by WindowCaptureWorker in MainWindow
///</summary>
System::Void LCREYE::VFrameReader::CancelWork(void) {
    this->isCanceled = true;
}

///<summary>
/// DoWorkApp, runs frame capture for WindowCaptureWorker and selected app
///</summary>
System::Void LCREYE::VFrameReader::DoWorkApp(System::ComponentModel::DoWorkEventArgs^ e) {
    // get app name from argument
    this->appName = safe_cast<String^>(e->Argument);

    // get window info
    pin_ptr<const wchar_t> lcTitle = PtrToStringChars(this->appName);
    HWND cAppHWND = FindWindow(NULL, lcTitle);

    if (cAppHWND != nullptr) {
        Debug::WriteLine("\n");
        Debug::Write(this->appName);
        Debug::WriteLine("\n");

        // output image operations
        cv::Mat cfMat, bwMat, blurMat, thresholdOut;

        // capture live
        while (!this->isCanceled) {
            Image^ cFrame = this->GetFrame(cAppHWND);
            ReleaseDC(NULL, this->selectedMonitor);

            if (cFrame != nullptr) {
                Debug::WriteLine("Grabbing frame from GDI");
                Debug::WriteLine("Size " + cFrame->Size.ToString());
                //this->DetectRectangles(cFrame);
                //this->capView->Image = cFrame;

                // use cv image viewer as using ImageBox is not working right
                
                // setup image for cv::Mat conversion
                Bitmap^ bmImage = gcnew Bitmap(cFrame);
                cv::Mat cfMat = LCREYE::VFrameReader::Bitmap2Mat(bmImage);

                // turn image b&w
                cv::cvtColor(cfMat, bwMat, cv::COLOR_BGR2GRAY);
                
                // do cleaning up or blur of image
                // testing which works
                //cv::pyrMeanShiftFiltering(cfMat, blurMat, 11, 21);
                cv::blur(bwMat, blurMat, cv::Size(3, 3));

                
                //cv::cvtColor(cfMat, bwMat, cv::COLOR_BGR2GRAY);

                // setup threshold
                cv::threshold(blurMat, thresholdOut, 0, 255, cv::THRESH_BINARY);

                // find contours
                std::vector<std::vector<cv::Point>> contours;
                std::vector<cv::Vec4i> hierarchy;
                cv::findContours(thresholdOut, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);

                // setup hulls and find convexHull
                //std::vector<std::vector<cv::Point>> hull(contours.size());
                std::vector<std::vector<cv::Point>> polygon(contours.size());
                std::vector<cv::Rect> boundRect(contours.size());
                for (int i = 0; i < contours.size(); i++) {
                    double fArcLength = cv::arcLength(contours[i], TRUE);
                    //cv::convexHull(cv::Mat(contours[i]), hull[i], FALSE);
                    //cv::approxPolyDP(hull[i], polygon[i], 0.015 * fArcLength, TRUE);
                    cv::approxPolyDP(cv::Mat(contours[i]), polygon[i], 0.015 * fArcLength, TRUE);
                    cv::drawContours(cfMat, polygon, i, cv::Scalar(0, 0, 255));
                    if (polygon[i].size() == 4) {
                        boundRect[i] = cv::boundingRect(polygon[i]);
                    }
                }

                // 
                for (int i = 0; i < boundRect.size(); i++) {
                    // -1 blocks screen up to chat part of screen
                    cv::rectangle(cfMat, boundRect[i].tl(), boundRect[i].br(), cv::Scalar(255, 0, 0), 2);
                }

                // convert back to image and display
                //Bitmap^ cvBitmap = gcnew Bitmap(LCREYE::VFrameReader::Mat2Bitmap(cfMat));

                //this->capView->Image = (Image^)cvBitmap;
                cv::namedWindow("Img Analysis...", cv::WINDOW_NORMAL);
                //cv::resize(cfMat, cfMat, cv::Size(1024, 768));
                cv::imshow("Img Analysis...", cfMat);
                cv::waitKey(1);
                
            }

            bwMat.release();
            cfMat.release();

            Debug::WriteLine("\nCancellationPending?");
            Debug::WriteLine(this->bgWorker->CancellationPending);
            if (this->bgWorker->CancellationPending) {
                e->Cancel = true;
                break;
            }

        }
    }
}


///<summary>
/// DoWorkMonitor, runs frame capture for WindowCaptureWorker and selected monitor
///</summary>
System::Void LCREYE::VFrameReader::DoWorkMonitor(System::ComponentModel::DoWorkEventArgs^ e) {
    // capture live
    Debug::WriteLine("DoWorkMonitor starts for monitor # " + (this->selectedMonitorNumber + 1).ToString());

    // face detect vars
    cv::CascadeClassifier faceCascade;
    faceCascade.load("C:\\opencv\\build\\etc\\haarcascades\\haarcascade_frontalface_alt2.xml");

    while (!this->isCanceled) {
        Image^ cFrame = this->GetFrameMonitor(this->selectedMonitor);

        // output image operations
        cv::Mat cfMat, bwMat, blurMat, thresholdOut, canMat;

        if (cFrame != nullptr) {

            Debug::WriteLine("Grabbing Monitor #" + (this->selectedMonitorNumber+1).ToString() + " frame");
            Debug::WriteLine("Size " + cFrame->Size.ToString());

            // use cv image viewer as using ImageBox is not working right

            // setup image for cv::Mat conversion
            Bitmap^ bmImage = gcnew Bitmap(cFrame);
            cv::Mat cfMat = LCREYE::VFrameReader::Bitmap2Mat(bmImage);

            
            // turn image b&w
            cv::cvtColor(cfMat, bwMat, cv::COLOR_BGR2GRAY);

            cv::Mat ccfMat = cfMat.clone();

            cv::Canny(bwMat, canMat, 0, 150, 3);
            //cv::imshow("canMat", canMat);

            // do cleaning up or blur of image
            // testing which works
            //cv::pyrMeanShiftFiltering(cfMat, blurMat, 11, 21);
            //cv::blur(bwMat, blurMat, cv::Size(3, 3));
            //cv::blur(canMat, blurMat, cv::Size(3, 3));
            //cv::imshow("blurMat", blurMat);
            //cv::threshold(bwMat, thresholdOut, 0, 255, cv::THRESH_BINARY_INV | cv::THRESH_OTSU);
            //cv::imshow("thresholdOut", thresholdOut);

            //cv::GaussianBlur(bwMat, blurMat, cv::Size(3, 3), 0, 0);
            //cv::imshow("blurMat", blurMat);

            //cv::Canny(blurMat, canMat, 0, 0);

            std::vector<std::vector<cv::Point>> contours;
            std::vector<cv::Vec4i> hierarchy;
            //cv::findContours(canMat, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
            cv::findContours(canMat, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);

           // cv::imshow("canMat", canMat);

            std::vector<std::vector<cv::Point>> polygon(contours.size());
            std::vector<cv::Rect> boundRect(contours.size());
            std::vector<cv::Rect> conBoundRect(contours.size());

            for (int i = 0; i < contours.size(); i++) {
                //double fArcLength = cv::arcLength(contours[i], TRUE);
                //cv::convexHull(cv::Mat(contours[i]), hull[i], FALSE);
                //cv::approxPolyDP(hull[i], polygon[i], 0.015 * fArcLength, TRUE);
                
                //cv::drawContours(cfMat, contours, i, cv::Scalar(0, 0, 255), 1, 8, hierarchy, 0);
                
                //cv::approxPolyDP(cv::Mat(contours[i]), polygon[i], fArcLength*0.5, TRUE);

                //if (polygon[i].size() >= 4) {
                //    boundRect[i] = cv::boundingRect(polygon[i]);
                //}

                if (contours[i].size() >= 4 && contours[i].size() <= 8) {
                    //conBoundRect[i] = cv::boundingRect(contours[i]);
                    cv::drawContours(cfMat, contours, i, cv::Scalar(255, 0, 0), 2, 8, hierarchy, 0);
                }
                
            }

            cv::namedWindow("Contours", cv::WINDOW_NORMAL);
            cv::imshow("Contours", cfMat);

 
            /*for (int i = 0; i < boundRect.size(); i++) {
                // -1 blocks screen up to chat part of screen
                cv::rectangle(cfMat, boundRect[i].tl(), boundRect[i].br(), cv::Scalar(255, 0, 0), 1);
            }

            for (int i = 0; i < boundRect.size(); i++) {
                // -1 blocks screen up to chat part of screen
                cv::rectangle(cfMat, conBoundRect[i].tl(), conBoundRect[i].br(), cv::Scalar(0, 0, 255), 1);
            }*/

            // convert back to image and display
            //Bitmap^ cvBitmap = gcnew Bitmap(LCREYE::VFrameReader::Mat2Bitmap(cfMat));

            // -- face detection -- //
            std::vector<cv::Rect> facesRects;
            
            faceCascade.detectMultiScale(ccfMat, facesRects, 1.1, 3, 0, cv::Size(20, 20));
            for (int i = 0; i < facesRects.size(); i++) {
                cv::rectangle(ccfMat, facesRects[i], cv::Scalar(255, 255, 0), 2, 1, 0);
            }

            cv::namedWindow("Faces", cv::WINDOW_NORMAL);
            cv::imshow("Faces", ccfMat);

            //this->capView->Image = (Image^)cvBitmap;
            //cv::namedWindow("Img Analysis...", cv::WINDOW_NORMAL);
            //cv::resize(cfMat, cfMat, cv::Size(1024, 768));
            
            cv::waitKey(1);
        }

        bwMat.release();
        cfMat.release();

        Debug::WriteLine("\nCancellationPending?");
        Debug::WriteLine(this->bgWorker->CancellationPending);
        if (this->bgWorker->CancellationPending) {
            e->Cancel = true;
            break;
        }
    }
}


///<summary>
/// Detect rectangle in image
///</summary>
System::Void LCREYE::VFrameReader::DetectRectangles(Image^ cFrame)
{
    
    // setup image for cv::Mat conversion
    Bitmap^ bmImage = gcnew Bitmap(cFrame);
    cv::Mat cfMat = LCREYE::VFrameReader::Bitmap2Mat(bmImage);
    
    // output image operations
    cv::Mat bwMat;

    
    
    // -- doing it with convexhulls --
    
    // setup mat to find contours
    cv::Mat blurMat, thresholdOut;

    

    // do cleaning up or blur of image
    // testing which works
    cv::pyrMeanShiftFiltering(cfMat, blurMat, 11, 21);
    //cv::blur(bwMat, blurMat, cv::Size(3, 3));

    // turn image b&w
    cv::cvtColor(blurMat, bwMat, cv::COLOR_BGR2GRAY);

    // setup threshold
    cv::threshold(bwMat, thresholdOut, 0, 255, cv::THRESH_BINARY_INV + cv::THRESH_OTSU);

    // find contours
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(thresholdOut, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0));

    // setup hulls and find convexHull
    //std::vector<std::vector<cv::Point>> hull(contours.size());
    std::vector<std::vector<cv::Point>> polygon(contours.size());
    std::vector<cv::Rect> boundRect(contours.size());
    for (int i = 0; i < contours.size(); i++) {
        double fArcLength = cv::arcLength(contours[i], TRUE);
        //cv::convexHull(cv::Mat(contours[i]), hull[i], FALSE);
        //cv::approxPolyDP(hull[i], polygon[i], 0.015 * fArcLength, TRUE);
        cv::approxPolyDP(cv::Mat(contours[i]), polygon[i], 0.015 * fArcLength, TRUE);
        if (polygon[i].size() == 4) {
            boundRect[i] = cv::boundingRect(polygon[i]);
        }
    }

    // 
    for (int i = 0; i < boundRect.size(); i++) {
        // -1 blocks screen up to chat part of screen
        cv::rectangle(cfMat, boundRect[i].tl(), boundRect[i].br(), cv::Scalar(255, 0, 0), -1);
    }

    // convert back to image and display
    Bitmap^ cvBitmap = gcnew Bitmap(LCREYE::VFrameReader::Mat2Bitmap(cfMat));

    //this->capView->Image = (Image^)cvBitmap;

    bwMat.release();
    cfMat.release();

    // -- doing it with houglines --

    /*
    
    cv::Mat cvMat, cvdMat;

    // detect edges
    cv::Canny(bwMat, cvMat, 50, 200, 3);

    // copy results to image
    cvtColor(cvMat, cvdMat, cv::COLOR_GRAY2BGR);

    // hough line transform
    std::vector<cv::Vec4i> lines;
    cv::HoughLinesP(cvMat, lines, 1, CV_PI / 180, 150, 0, 0);

    // rectangle detection
    if (lines.size() > 3) {
        //this->consoleBox->Text += "Lines >3 found: " + lines.size().ToString();
        for (size_t i = 0; i < lines.size(); i++) {
            cv::Vec4i lineVec = lines[i];
            cv::line(
                cvdMat,
                cv::Point(lineVec[0], lineVec[1]),
                cv::Point(lineVec[2], lineVec[3]),
                cv::Scalar(255, 0, 255),
                3,
                cv::LINE_AA
            );
        }
    }

     // convert back to image and display
    Bitmap^ cvBitmap = LCREYE::VFrameReader::Mat2Bitmap(cvdMat);

    Debug::WriteLine("cvBitmap Size\n");
    Debug::Write(cvBitmap->Size);
    this->capView->Image = (Image^)cvBitmap;

    bwMat.release();
    cvMat.release();
    cvdMat.release();
    */
    

   
}