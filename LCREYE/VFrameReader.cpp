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

    Bitmap^ matBitmap = gcnew Bitmap(cvImageSize.width, cvImageSize.height, (int)cvImage.step1(), Imaging::PixelFormat::Format32bppRgb, (IntPtr)cvImage.data);

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

    // could not find a way to get the specific screen resolution so had to use the main monitor resolution
    // solutions online using HMONITORINFO did not give correct information
    // still looking into a better solution
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
/// displays the captures in CV imshow windows
///</summary>
System::Void LCREYE::VFrameReader::DoWorkApp(System::ComponentModel::DoWorkEventArgs^ e) {
    // get app name from argument
    this->appName = safe_cast<String^>(e->Argument);

    // get window info
    pin_ptr<const wchar_t> lcTitle = PtrToStringChars(this->appName);
    HWND cAppHWND = FindWindow(NULL, lcTitle);

    // load faces
    // make this an option later with a config manager
    cv::CascadeClassifier faceXML = this->LoadFaceCascadeXML();

   

    if (cAppHWND != nullptr) {
        Debug::WriteLine("\n");
        Debug::Write(this->appName);
        Debug::WriteLine("\n");

        // output image operations
        cv::Mat cfMat, rectMat, faceMat, lineMat;

        // capture live
        while (!this->isCanceled) {
            Image^ cFrame = this->GetFrame(cAppHWND);
            ReleaseDC(NULL, this->selectedMonitor);

            if (cFrame != nullptr) {
                Debug::WriteLine("Grabbing frame from app " + this->appName);
                Debug::WriteLine("Size " + cFrame->Size.ToString());
                // setup image for cv::Mat conversion
                Bitmap^ bmImage = gcnew Bitmap(cFrame);
                cv::Mat cfMat = LCREYE::VFrameReader::Bitmap2Mat(bmImage);

                // do contour/object match
                //rectMat = this->DetectRectangles(cfMat);
                //cv::namedWindow("Rectangles", cv::WINDOW_NORMAL);
                //cv::imshow("Rectangles", rectMat);

                // do a face match

                if (this->faceCascadeLoaded) {
                    faceMat = this->DetectFaces(cfMat, faceXML);
                    cv::namedWindow("Faces", cv::WINDOW_NORMAL);
                    cv::imshow("Faces", faceMat);
                }

                lineMat = this->DetectLines(cfMat);
                cv::namedWindow("Lines", cv::WINDOW_NORMAL);
                cv::imshow("Lines", lineMat);

                cv::waitKey(1);
                
            }

            cfMat.release();
            rectMat.release();
            faceMat.release();
            lineMat.release();

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
/// displays the captures in CV imshow windows
///</summary>
System::Void LCREYE::VFrameReader::DoWorkMonitor(System::ComponentModel::DoWorkEventArgs^ e) {
    // capture live
    Debug::WriteLine("DoWorkMonitor starts for monitor # " + (this->selectedMonitorNumber + 1).ToString());

    // load faces
    // make this an option later with a config manager
    //cv::CascadeClassifier faceXML = this->LoadFaceCascadeXML();
    
    // setup dnn model path
    // need to clean this up to use a setting files for the model paths instead of hardcoding
    std::string ynModelPath;
    std::string yoloModelPath;

    PWSTR appData;

    // YN model path
    if (SHGetKnownFolderPath(FOLDERID_RoamingAppData, KF_FLAG_DEFAULT, NULL, &appData) == S_OK) {
        char rawYNModelPath[MAX_PATH];
        wcstombs(rawYNModelPath, appData, MAX_PATH);
        std::string srYNModelPath(rawYNModelPath);

        ynModelPath = srYNModelPath + "\\AlphaThirdEye\\face_detection_yunet_2022mar.onnx";

        //delete srYNModelPath;
        //delete[] rawYNModelPath;
    }

    // SFace model path
    /*if (SHGetKnownFolderPath(FOLDERID_RoamingAppData, KF_FLAG_DEFAULT, NULL, &appData) == S_OK) {
        char rawSFModelPath[MAX_PATH];
        wcstombs(rawSFModelPath, appData, MAX_PATH);
        String^ srSFModelPath = gcnew String(rawSFModelPath);

        this->sfModelPath = srSFModelPath + "\\AlphaThirdEye\\face_recognition_sface_2021dec.onnx";

        delete srSFModelPath;
        //delete[] rawSFModelPath;
    }*/

    // yolov8x.onnx model path
    if (SHGetKnownFolderPath(FOLDERID_RoamingAppData, KF_FLAG_DEFAULT, NULL, &appData) == S_OK) {
        char rawYNModelPath[MAX_PATH];
        wcstombs(rawYNModelPath, appData, MAX_PATH);
        std::string srYNModelPath(rawYNModelPath);

        yoloModelPath = srYNModelPath + "\\AlphaThirdEye\\yolov8x.onnx";

        //delete srYNModelPath;
        //delete[] rawYNModelPath;
    }


    //delete appData;

    while (!this->isCanceled) {
        Image^ cFrame = this->GetFrameMonitor(this->selectedMonitor);

        // output image operations
        cv::Mat cfMat, rectMat, faceMat, lineMat, comboMat, yoloMat;
        cv::dnn::Net yModel;

        if (cFrame != nullptr) {

            Debug::WriteLine("Grabbing Monitor #" + (this->selectedMonitorNumber+1).ToString() + " frame");
            Debug::WriteLine("Size " + cFrame->Size.ToString());

            // setup image for cv::Mat conversion
            Bitmap^ bmImage = gcnew Bitmap(cFrame);
            cv::Mat cfMat = LCREYE::VFrameReader::Bitmap2Mat(bmImage);

            // do contour/object match
            //rectMat = this->DetectRectangles(cfMat.clone());
            //Debug::WriteLine("rectMat channels: ");
            //Debug::Write(rectMat.channels());
            //Debug::WriteLine("\n");
            //cv::namedWindow("Rectangles", cv::WINDOW_NORMAL);
            //cv::imshow("Rectangles", rectMat);

            // brings back 1 or 3 channels while others are 2 channel
            //lineMat = this->DetectLines(cfMat.clone());
            //Debug::WriteLine("lineMat channels: ");
            //Debug::Write(lineMat.channels());
            //Debug::WriteLine("\n");
            //cv::namedWindow("Lines", cv::WINDOW_NORMAL);
            //cv::imshow("Lines", lineMat);

            // do a face match
            faceMat = this->DetectFacesYunet(cfMat.clone(), ynModelPath);
            Debug::WriteLine("faceMat channels: ");
            Debug::Write(faceMat.channels());
            Debug::WriteLine("\n");
            cv::namedWindow("Faces", cv::WINDOW_NORMAL);
            cv::imshow("Faces", faceMat);

            // do yolo object detection
            yModel = this->LoadYOLO(yoloModelPath);
            yoloMat = this->ProcessYOLO(cfMat.clone(), yModel);
            cv::namedWindow("Yolo", cv::WINDOW_NORMAL);
            cv::imshow("Yolo", yoloMat);

            /*if (this->faceCascadeLoaded) {
                faceMat = this->DetectFaces(cfMat.clone(), faceXML);
                Debug::WriteLine("faceMat channels: ");
                Debug::Write(faceMat.channels());
                Debug::WriteLine("\n");
                cv::namedWindow("Faces", cv::WINDOW_NORMAL);
                cv::imshow("Faces", faceMat);
            }*/

            //std::vector<cv::Mat> comboMatVect = {
            //    rectMat,
                //lineMat,
                //faceMat
            //};
            //cv::hconcat(comboMatVect, comboMat);
            //comboMat = rectMat + lineMat + faceMat;

            //cv::namedWindow("Combo", cv::WINDOW_NORMAL);
            //cv::imshow("Combo", comboMat);
            
            cv::waitKey(1);
        }

        cfMat.release();
        rectMat.release();
        faceMat.release();
        lineMat.release();

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
cv::Mat LCREYE::VFrameReader::DetectRectangles(cv::Mat& cfMat)
{
    // output image operations
    cv::Mat bwMat, blurMat, thresholdOut, canMat;
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    
    
    // turn image b&w
    cv::cvtColor(cfMat, bwMat, cv::COLOR_BGR2GRAY);

    //cv::Canny(bwMat, canMat, 0, 150, 3);
    cv::medianBlur(bwMat, bwMat, 9);
    cv::adaptiveThreshold(bwMat, canMat, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY, 9, 2);
    
    // noise removal attempts
    //cv::dilate(canMat, canMat, cv::Mat(), cv::Point(-1, -1)); // hole removal
    cv::Mat matKernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
    cv::morphologyEx(canMat, canMat, cv::MORPH_OPEN, matKernel);

    cv::findContours(canMat, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);

    //cv::blur(bwMat, blurMat, cv::Size(3, 3));
    //cv::findContours(blurMat, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);
    //cv::findContours(bwMat, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);
   
    // setup vectors from contour info
    std::vector<std::vector<cv::Point>> polygon(contours.size());
    std::vector<cv::Rect> boundRect(contours.size());

    // find from contours with PolyDP and ArcLength
    // try making arcLength variable user can change
    for (int i = 0; i < contours.size(); i++) {
        double fArcLength = cv::arcLength(contours[i], TRUE);

        // arclength multiple tuning: 0.015, 0.02, 0.01
        //cv::approxPolyDP(cv::Mat(contours[i]), polygon[i], 0.015 * fArcLength, TRUE);
        //cv::approxPolyDP(cv::Mat(contours[i]), polygon[i], 0.002 * fArcLength, TRUE);
        cv::approxPolyDP(cv::Mat(contours[i]), polygon[i], 0.01 * fArcLength, TRUE);
        // if (polygon[i].size() == 4) {
        if (polygon[i].size() == 4 && fabs(cv::contourArea(polygon[i])) > 1000 && cv::isContourConvex(polygon[i])) {
             boundRect[i] = cv::boundingRect(polygon[i]);
        }
    }

    for (int i = 0; i < boundRect.size(); i++) {
        // -1 blocks screen up to chat part of screen
        cv::rectangle(cfMat, boundRect[i].tl(), boundRect[i].br(), cv::Scalar(0, 0, 255), 2);
    }

    return cfMat;
}

///<summary>
/// Find lines in image
///</summary>
cv::Mat LCREYE::VFrameReader::DetectLines(cv::Mat& cfMat) {
    // turn image b&w
    cv::Mat bwMat, linesMat;
    cv::cvtColor(cfMat, bwMat, cv::COLOR_BGR2GRAY);
    cv::Ptr<cv::LineSegmentDetector> lineDetector;
    
    lineDetector = cv::createLineSegmentDetector();

    lineDetector->detect(bwMat, linesMat);
    cv::cvtColor(cfMat, cfMat, cv::COLOR_BGRA2BGR);
    lineDetector->drawSegments(cfMat, linesMat);
    cv::cvtColor(cfMat, cfMat, cv::COLOR_BGR2BGRA);

    return cfMat;
}

///<summary>
/// Load faces HAAR information - must be ran before detectfaces
///</summar>
cv::CascadeClassifier LCREYE::VFrameReader::LoadFaceCascadeXML() 
{
    // face detect vars
    cv::CascadeClassifier faceCascade;

    try {
        faceCascade.load("C:\\opencv\\build\\etc\\haarcascades\\haarcascade_frontalface_alt2.xml");
    }
    catch (...) {
        this->consoleBox->Text += "\n! face casecade xml could not be loaded !";
    }
    finally {
        this->faceCascadeLoaded = TRUE;
    }
    
    return faceCascade;
}

///<summary>
/// Detect faces in image
///</summary>
cv::Mat LCREYE::VFrameReader::DetectFaces(cv::Mat& cfMat, cv::CascadeClassifier faceCascade)
{

    if (this->faceCascadeLoaded) {
        // rects of faces captured
        std::vector<cv::Rect> facesRects;

        faceCascade.detectMultiScale(cfMat, facesRects, 1.1, 3, 0, cv::Size(20, 20));
        for (int i = 0; i < facesRects.size(); i++) {
            cv::rectangle(cfMat, facesRects[i], cv::Scalar(255, 255, 0), 2, 1, 0);
        }
    }
    
    return cfMat;
}

///<summary>
/// Detect faces in image using deep nn and YUNET
///</summary>
cv::Mat LCREYE::VFrameReader::DetectFacesYunet(cv::Mat& cfMat, std::string ynPath)
{
    cv::Ptr<cv::FaceDetectorYN> ynDetector = cv::FaceDetectorYN::create(
        ynPath,
        "",
        cfMat.size(),
        this->scoreThreshold,
        this->nmsThreshold,
        this->topK
    );

    cv::Mat faceMat;
    cv::cvtColor(cfMat, cfMat, cv::COLOR_BGRA2BGR);
    ynDetector->detect(cfMat, faceMat);
    if (faceMat.rows >= 1) {
        // faces found
        for (int i = 0; i < faceMat.rows; i++) {
            cv::Rect2i faceRecCoords = cv::Rect2i(
                int(faceMat.at<float>(i, 0)),
                int(faceMat.at<float>(i, 1)),
                int(faceMat.at<float>(i, 2)),
                int(faceMat.at<float>(i, 3))
            );

            cv::Scalar faceRecColor = cv::Scalar(255, 0, 255); // yellow
            cv::rectangle(cfMat, faceRecCoords, faceRecColor, 2);
        }
    }

    return cfMat;
}

// Detect objects using YOLO
// return a matrix that can be turn into a rectangle with label around the object
cv::dnn::Net LCREYE::VFrameReader::LoadYOLO(std::string yoloPath) {
    cv::dnn::Net yoloModel = cv::dnn::readNet(yoloPath);
    return yoloModel;
}

// Process information from YOLO model detection
// using image input
cv::Mat LCREYE::VFrameReader::ProcessYOLO(cv::Mat& cfMat, cv::dnn::Net& yoloModel) {

    // convert image mat to blob
    cv::Mat imgBlob;
    cv::dnn::blobFromImage(
        cfMat,
        imgBlob,
        1. / 255,
        cfMat.size(),
        cv::Scalar(),
        true,
        false
    );

    // set blob to model input
    yoloModel.setInput(imgBlob);
    // forward prop/analyze image
    std::vector<cv::Mat> outs;
    yoloModel.forward(outs, yoloModel.getUnconnectedOutLayersNames());

    // get layers
    static std::vector<int> outLayers = yoloModel.getUnconnectedOutLayers();
    static std::string outLayerType = yoloModel.getLayer(outLayers[0])->type;

    // process outputs by layer
    std::vector<int> classIds;
    std::vector<float> confidences;
    std::vector<cv::Rect> boxes;

    if (outLayerType == "DetectionOutput")
    {
        // Network produces output blob with a shape 1x1xNx7 where N is a number of
        // detections and an every detection is a vector of values
        // [batchId, classId, confidence, left, top, right, bottom]
        CV_Assert(outs.size() > 0);
        for (size_t k = 0; k < outs.size(); k++)
        {
            float* data = (float*)outs[k].data;
            for (size_t i = 0; i < outs[k].total(); i += 7)
            {
                float confidence = data[i + 2];
                if (confidence > this->yoloConfidenceThreshold)
                {
                    int left = (int)data[i + 3];
                    int top = (int)data[i + 4];
                    int right = (int)data[i + 5];
                    int bottom = (int)data[i + 6];
                    int width = right - left + 1;
                    int height = bottom - top + 1;
                    if (width <= 2 || height <= 2)
                    {
                        left = (int)(data[i + 3] * cfMat.cols);
                        top = (int)(data[i + 4] * cfMat.rows);
                        right = (int)(data[i + 5] * cfMat.cols);
                        bottom = (int)(data[i + 6] * cfMat.rows);
                        width = right - left + 1;
                        height = bottom - top + 1;
                    }
                    classIds.push_back((int)(data[i + 1]) - 1);  // Skip 0th background class id.
                    boxes.push_back(cv::Rect(left, top, width, height));
                    confidences.push_back(confidence);
                }
            }
        }
    }
    else if (outLayerType == "Region")
    {
        for (size_t i = 0; i < outs.size(); ++i)
        {
            // Network produces output blob with a shape NxC where N is a number of
            // detected objects and C is a number of classes + 4 where the first 4
            // numbers are [center_x, center_y, width, height]
            float* data = (float*)outs[i].data;
            for (int j = 0; j < outs[i].rows; ++j, data += outs[i].cols)
            {
                cv::Mat scores = outs[i].row(j).colRange(5, outs[i].cols);
                cv::Point classIdPoint;
                double confidence;
                cv::minMaxLoc(scores, 0, &confidence, 0, &classIdPoint);
                if (confidence > this->yoloConfidenceThreshold)
                {
                    int centerX = (int)(data[0] * cfMat.cols);
                    int centerY = (int)(data[1] * cfMat.rows);
                    int width = (int)(data[2] * cfMat.cols);
                    int height = (int)(data[3] * cfMat.rows);
                    int left = centerX - width / 2;
                    int top = centerY - height / 2;

                    classIds.push_back(classIdPoint.x);
                    confidences.push_back((float)confidence);
                    boxes.push_back(cv::Rect(left, top, width, height));
                }
            }
        }
    }
    else
        CV_Error(cv::Error::StsNotImplemented, "Unknown output layer type: " + outLayerType);
    
    // process further with non-maxium suppression / NMS
    if (outLayers.size() > 1 || outLayerType == "Region")
    {
        std::map<int, std::vector<size_t> > class2indices;
        for (size_t i = 0; i < classIds.size(); i++)
        {
            if (confidences[i] >= this->yoloConfidenceThreshold)
            {
                class2indices[classIds[i]].push_back(i);
            }
        }
        std::vector<cv::Rect> nmsBoxes;
        std::vector<float> nmsConfidences;
        std::vector<int> nmsClassIds;
        for (std::map<int, std::vector<size_t> >::iterator it = class2indices.begin(); it != class2indices.end(); ++it)
        {
            std::vector<cv::Rect> localBoxes;
            std::vector<float> localConfidences;
            std::vector<size_t> classIndices = it->second;
            for (size_t i = 0; i < classIndices.size(); i++)
            {
                localBoxes.push_back(boxes[classIndices[i]]);
                localConfidences.push_back(confidences[classIndices[i]]);
            }
            std::vector<int> nmsIndices;
            cv::dnn::NMSBoxes(localBoxes, localConfidences, this->yoloConfidenceThreshold, this->yoloNMSThreshold, nmsIndices);
            for (size_t i = 0; i < nmsIndices.size(); i++)
            {
                size_t idx = nmsIndices[i];
                nmsBoxes.push_back(localBoxes[idx]);
                nmsConfidences.push_back(localConfidences[idx]);
                nmsClassIds.push_back(it->first);
            }
        }
        boxes = nmsBoxes;
        classIds = nmsClassIds;
        confidences = nmsConfidences;
    }

    for (size_t idx = 0; idx < boxes.size(); ++idx)
    {
        cv::Rect box = boxes[idx];

        // draw box into rectangle
        int boxLeft = box.x;
        int boxTop = box.y;
        int boxRight = box.x + box.width;
        int boxBottom = box.y + box.height;
        
        cv::rectangle(cfMat, cv::Point(boxLeft, boxTop), cv::Point(boxRight, boxBottom), cv::Scalar(0, 255, 0));
        std::string label = cv::format("%.2f", confidences[idx]);
        
        int baseLine;
        cv::Size labelSize = cv::getTextSize(label, cv::FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseLine);

        boxTop = cv::max(boxTop, labelSize.height);
        cv::rectangle(
            cfMat,
            cv::Point(boxLeft, boxTop - labelSize.height),
            cv::Point(boxLeft + labelSize.width, boxTop + baseLine),
            cv::Scalar::all(255),
            cv::FILLED
        );
        cv::putText(
            cfMat, 
            label, 
            cv::Point(boxLeft, boxTop), 
            cv::FONT_HERSHEY_SIMPLEX, 
            0.5, 
            cv::Scalar()
        );
    }

    return cfMat;
}























