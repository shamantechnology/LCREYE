#include "MainWindow.h"

/// <summary>
/// Callback for enumerating the titles of open apps
/// </summary>
BOOL CALLBACK EnumCallback(HWND hwnd, LPARAM lParam) {
    const DWORD TITLE_SIZE = 1024;
    WCHAR windowTitle[TITLE_SIZE];

    GetWindowTextW(hwnd, windowTitle, TITLE_SIZE);

    int length = ::GetWindowTextLength(hwnd);
    std::wstring title(&windowTitle[0]);
    if (!IsWindowVisible(hwnd) || length == 0 || title == L"Program Manager") {
        return TRUE;
    }

    // Retrieve the pointer passed into this callback, and re-'type' it.
    // The only way for a C API to pass arbitrary data is by means of a void*.
    std::vector<std::wstring>& titles =
        *reinterpret_cast<std::vector<std::wstring>*>(lParam);
    titles.push_back(title);

    return TRUE;
}

/// <summary>
/// Constructor
/// </summary>
LCREYE::MainWindow::MainWindow() {
    LCREYE::MainWindow::InitializeComponent();

    // setup frame capture
    this->vreader = gcnew LCREYE::VFrameReader();

    // setup app names selection
    LCREYE::MainWindow::GetAppNames();
}

/// <summary>
/// Deconstructor
/// </summary>
LCREYE::MainWindow::~MainWindow() {
    if (components)
    {
        delete components;
    }

    // clean up frame reader
    this->vreader = nullptr;
}

// <summary>
/// Get all open application names
/// </summary>
System::Void LCREYE::MainWindow::GetAppNames() {
    // get list of HWNDs that are open
    std::vector<std::wstring> titles;
    EnumWindows(EnumCallback, reinterpret_cast<LPARAM>(&titles));

    // loop through titles
    for (const auto& title : titles) {
        // getting the hwnd per title
        // HWND wHandle;
        // wHandle = FindWindow(NULL, title.c_str());
        // std::wcout << L"Title: " << title << L" HWND: " << wHandle << std::endl;

        // add titles to combobox
        System::String^ sTitle = gcnew System::String(title.c_str());
        LCREYE::ComboboxItem^ cbi = gcnew LCREYE::ComboboxItem(sTitle, 0);
        this->WindowSelection->Items->Add(cbi);
    }
}

/// <summary>
/// Worker to live capture window from desktop
/// </summary>
System::Void LCREYE::MainWindow::WindowCaptureWorker_DoWork(System::Object^ sender, System::ComponentModel::DoWorkEventArgs^ e) {
    // check worker status
    Debug::WriteLine("DoWork Start");
    
    this->vreader->DoWork(e);
}

/// <summary>
/// Worker to live capture window from desktop
/// </summary>
System::Void LCREYE::MainWindow::WindowCaptureWorker_RunWorkerCompleted(System::Object^ sender, System::ComponentModel::RunWorkerCompletedEventArgs^ e)
{
   // run new worker with new selected app name
    Debug::WriteLine("Starting after old worker stopped");
    this->vreader->isCanceled = false;
    this->vreader->bgWorker->RunWorkerAsync(this->vreader->appName);
}

/// <summary>
/// When combo box is clicked refresh app name list
/// </summary>
System::Void LCREYE::MainWindow::WindowSelection_Click(System::Object^ sender, System::EventArgs^ e) {
    // refresh app name list
    this->WindowSelection->Items->Clear();
    LCREYE::MainWindow::GetAppNames();
}

/// <summary>
/// Selection change for Window focus
/// </summary>
System::Void LCREYE::MainWindow::WindowSelection_SelectionChangeCommitted(System::Object^ sender, System::EventArgs^ e) {
    ComboBox^ senderCB = (ComboBox^)sender;
    
    // set app title
    this->vreader->appName = senderCB->SelectedItem->ToString();

    this->ConsoleBox->Text += "Capturing from \"" + this->vreader->appName + "\"\n";

    // set capture window
    this->vreader->capView = this->CaptureView;

    // set background worker
    this->WindowCaptureWorker->WorkerReportsProgress = false;
    this->WindowCaptureWorker->WorkerSupportsCancellation = true;
    this->vreader->bgWorker = this->WindowCaptureWorker;

    // run worker for capture
    // stop previous capture if there is one
    Debug::WriteLine("Is busy?");
    Debug::Write(this->vreader->bgWorker->IsBusy);
    if (!this->vreader->bgWorker->IsBusy) {
        this->vreader->bgWorker->RunWorkerAsync(this->vreader->appName);
    } else {
        if (!this->vreader->isCanceled) {
            Debug::WriteLine("\nStopping worker to start a new capture\n");

            // close and start new process
            this->vreader->bgWorker->CancelAsync();
            this->vreader->CancelWork();
        }
    }
}