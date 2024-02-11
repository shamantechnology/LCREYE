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
/// Callback for getting the monitor hdcs
/// </summar>
BOOL CALLBACK MonitorEnumProcCallback(_In_ HMONITOR hMonitor, _In_ HDC DevC, _In_  LPRECT lprcMonitor, _In_  LPARAM dwData) {
    MONITORINFOEX mInfo;
    mInfo.cbSize = sizeof(MONITORINFOEX);

    BOOL gmCheck = GetMonitorInfo(hMonitor, &mInfo);
    if (gmCheck) {
        HDC monitorDC = CreateDC(NULL, mInfo.szDevice, NULL, NULL);
        std::vector<HDC>& monitors = *reinterpret_cast<std::vector<HDC>*>(dwData);
        monitors.push_back(monitorDC);
    }
    return TRUE;
}

/// <summary>
/// Callback for getting the monitor display coordinates
/// </summar>
BOOL CALLBACK MonitorXYEnumProcCallback(_In_ HMONITOR hMonitor, _In_ HDC DevC, _In_  LPRECT lprcMonitor, _In_  LPARAM dwData) {
    MONITORINFOEX mInfo;
    mInfo.cbSize = sizeof(MONITORINFOEX);

    BOOL gmCheck = GetMonitorInfo(hMonitor, &mInfo);
    if (gmCheck) {
        HDC monitorDC = CreateDC(NULL, mInfo.szDevice, NULL, NULL);
        std::vector<std::pair<int, int>>& monitorsXY = *reinterpret_cast<std::vector<std::pair<int, int>>*>(dwData);
        int monitorWidth = mInfo.rcMonitor.right;// - mInfo.rcMonitor.left;
        int monitorHeight = mInfo.rcMonitor.bottom;// - mInfo.rcMonitor.top;

        monitorsXY.push_back(std::pair<int, int>(monitorWidth, monitorHeight));
    }
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
    //LCREYE::MainWindow::GetAppNames();

    // due to C++/cli design I can't have this be a managed class thing...
    std::vector<HDC> monitors = this->GetAllMonitors();
    
    monitors.clear();
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

/// <summary>
/// Add monitors to drop down selection
/// </summary>
System::Void LCREYE::MainWindow::AddMonitorSelection(std::vector<HDC> monitors) {
    // loop through monitors and add to selection
    int monitor_count = 1;
    for (int i = 0; i < monitors.size(); i++) {
        System::String^ mTitle = gcnew System::String("Monitor #" + monitor_count.ToString());
        LCREYE::ComboboxItem^ cbi = gcnew LCREYE::ComboboxItem(mTitle, 0);
        this->WindowSelection->Items->Add(cbi);
        monitor_count += 1;
    }
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
/// Get all monitors
/// </summary>
std::vector<HDC> LCREYE::MainWindow::GetAllMonitors() {
    std::vector<HDC> monitors;
    EnumDisplayMonitors(NULL, NULL, MonitorEnumProcCallback, reinterpret_cast<LPARAM>(&monitors));
  
    System::Diagnostics::Debug::WriteLine("\n Monitors found: ");
    System::Diagnostics::Debug::Write(monitors.size());

    this->AddMonitorSelection(monitors);

    return monitors;
}

std::vector<std::pair<int, int>> LCREYE::MainWindow::GetAllMonitorsSizes() {
    std::vector<std::pair<int, int>> monitorsXY;
    HDC monitorHDC = GetDC(NULL);

    // get sizes
    EnumDisplayMonitors(monitorHDC, NULL, MonitorEnumProcCallback, reinterpret_cast<LPARAM>(&monitorsXY));

    return monitorsXY;
}

/// <summary>
/// Worker to live capture window from desktop
/// </summary>
System::Void LCREYE::MainWindow::WindowCaptureWorker_DoWork(System::Object^ sender, System::ComponentModel::DoWorkEventArgs^ e) {
    // check worker status
    Debug::WriteLine("DoWork Start");
    
    this->vreader->DoWorkMonitor(e);
}

/// <summary>
/// Worker to live capture window from desktop
/// </summary>
System::Void LCREYE::MainWindow::WindowCaptureWorker_RunWorkerCompleted(System::Object^ sender, System::ComponentModel::RunWorkerCompletedEventArgs^ e)
{
   // run new worker with new selected app name
    Debug::WriteLine("Starting after old worker stopped");
    this->vreader->isCanceled = false;
    this->vreader->bgWorker->RunWorkerAsync();
}

/// <summary>
/// When combo box is clicked refresh app name list
/// </summary>
System::Void LCREYE::MainWindow::WindowSelection_Click(System::Object^ sender, System::EventArgs^ e) {
    // refresh app name list
    this->WindowSelection->Items->Clear();
    // LCREYE::MainWindow::GetAppNames();

    std::vector<HDC> monitors = this->GetAllMonitors();

    monitors.clear();
}

/// <summary>
/// Selection change for Window focus
/// </summary>
System::Void LCREYE::MainWindow::WindowSelection_SelectionChangeCommitted(System::Object^ sender, System::EventArgs^ e) {
    ComboBox^ senderCB = (ComboBox^)sender;
    
    // set monitor (use to be app title)
    System::String^ senderMonitor = senderCB->SelectedItem->ToString();
    Debug::WriteLine("Selected " + senderMonitor + "\n");
    senderMonitor = senderMonitor->Replace("Monitor #", "");
    Debug::WriteLine("Replaced Selected " + senderMonitor + "\n");

    // have to get list of monitors again since vecotr is a managed class
    std::vector<HDC> monitors = this->GetAllMonitors();

    System::Diagnostics::Debug::WriteLine("\n Monitors found in Selected: ");
    System::Diagnostics::Debug::Write(monitors.size());

    int monNum; // windows monitor number
    int monNumDisplay = 0; // monitor number to show to user
    if (!int::TryParse(senderMonitor, monNum)) {
        monNum = 0;
    }

    if (monNum > 0) {
        monNumDisplay = monNum;
        monNum = monNum - 1;
    }

    this->vreader->selectedMonitorNumber = monNum;
    this->vreader->selectedMonitor = monitors[monNum];

    this->ConsoleBox->Text += "Capturing from \"monitor #" + monNumDisplay.ToString() + "\"\n";

    // set capture window
    //this->vreader->capView = this->CaptureView;

    // set background worker
    this->WindowCaptureWorker->WorkerReportsProgress = false;
    this->WindowCaptureWorker->WorkerSupportsCancellation = true;
    this->vreader->bgWorker = this->WindowCaptureWorker;

    // run worker for capture
    // stop previous capture if there is one
    Debug::WriteLine("Is busy?");
    Debug::Write(this->vreader->bgWorker->IsBusy);
    if (!this->vreader->bgWorker->IsBusy) {
        this->vreader->bgWorker->RunWorkerAsync();
    } else {
        if (!this->vreader->isCanceled) {
            Debug::WriteLine("\nStopping worker to start a new capture\n");

            // close and start new process
            this->vreader->bgWorker->CancelAsync();
            this->vreader->CancelWork();
        }
    }

    monitors.clear();
}