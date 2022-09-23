#include "MainWindow.h"
#include <Windows.h>
using namespace System;
using namespace System::Windows::Forms;

[STAThread]
int main(array<String^>^ args) { 
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(0);
	LCREYE::MainWindow mw;
	Application::Run(% mw);
}