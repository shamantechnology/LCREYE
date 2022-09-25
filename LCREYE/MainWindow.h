#pragma once

#include "ComboboxItem.h"
#include "VFrameReader.h"
#include <string>
#include <iostream>
#include <codecvt>
#include <list>
#include <tuple>

namespace LCREYE {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::Diagnostics;

	/// <summary>
	/// Summary for MainWindow
	/// </summary>
	public ref class MainWindow : public System::Windows::Forms::Form
	{
	public: MainWindow(void);
	protected: ~MainWindow();
	private: System::Windows::Forms::RichTextBox^ ConsoleBox;
	protected:


	private: System::Windows::Forms::ComboBox^ WindowSelection;




	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;
	private: System::ComponentModel::BackgroundWorker^ WindowCaptureWorker;
	private: System::Windows::Forms::RichTextBox^ foundObjectsList;


		// vframe member
		LCREYE::VFrameReader^ vreader;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->ConsoleBox = (gcnew System::Windows::Forms::RichTextBox());
			this->WindowSelection = (gcnew System::Windows::Forms::ComboBox());
			this->WindowCaptureWorker = (gcnew System::ComponentModel::BackgroundWorker());
			this->foundObjectsList = (gcnew System::Windows::Forms::RichTextBox());
			this->SuspendLayout();
			// 
			// ConsoleBox
			// 
			this->ConsoleBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Right));
			this->ConsoleBox->BackColor = System::Drawing::SystemColors::Desktop;
			this->ConsoleBox->Cursor = System::Windows::Forms::Cursors::Default;
			this->ConsoleBox->Font = (gcnew System::Drawing::Font(L"Consolas", 11.5F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(2)));
			this->ConsoleBox->ForeColor = System::Drawing::Color::Lime;
			this->ConsoleBox->Location = System::Drawing::Point(1, 31);
			this->ConsoleBox->Name = L"ConsoleBox";
			this->ConsoleBox->ReadOnly = true;
			this->ConsoleBox->Size = System::Drawing::Size(261, 105);
			this->ConsoleBox->TabIndex = 6;
			this->ConsoleBox->Text = L"";
			// 
			// WindowSelection
			// 
			this->WindowSelection->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Right));
			this->WindowSelection->FormattingEnabled = true;
			this->WindowSelection->Location = System::Drawing::Point(1, 4);
			this->WindowSelection->Name = L"WindowSelection";
			this->WindowSelection->Size = System::Drawing::Size(261, 21);
			this->WindowSelection->TabIndex = 3;
			this->WindowSelection->SelectionChangeCommitted += gcnew System::EventHandler(this, &MainWindow::WindowSelection_SelectionChangeCommitted);
			this->WindowSelection->Click += gcnew System::EventHandler(this, &MainWindow::WindowSelection_Click);
			// 
			// WindowCaptureWorker
			// 
			this->WindowCaptureWorker->DoWork += gcnew System::ComponentModel::DoWorkEventHandler(this, &MainWindow::WindowCaptureWorker_DoWork);
			this->WindowCaptureWorker->RunWorkerCompleted += gcnew System::ComponentModel::RunWorkerCompletedEventHandler(this, &MainWindow::WindowCaptureWorker_RunWorkerCompleted);
			// 
			// foundObjectsList
			// 
			this->foundObjectsList->BackColor = System::Drawing::SystemColors::InfoText;
			this->foundObjectsList->Font = (gcnew System::Drawing::Font(L"Segoe UI", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->foundObjectsList->ForeColor = System::Drawing::SystemColors::Menu;
			this->foundObjectsList->Location = System::Drawing::Point(1, 142);
			this->foundObjectsList->Name = L"foundObjectsList";
			this->foundObjectsList->Size = System::Drawing::Size(261, 563);
			this->foundObjectsList->TabIndex = 7;
			this->foundObjectsList->Text = L"Object 1\nObject 2";
			// 
			// MainWindow
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->BackColor = System::Drawing::SystemColors::Desktop;
			this->ClientSize = System::Drawing::Size(264, 717);
			this->Controls->Add(this->foundObjectsList);
			this->Controls->Add(this->WindowSelection);
			this->Controls->Add(this->ConsoleBox);
			this->ForeColor = System::Drawing::SystemColors::Control;
			this->Name = L"MainWindow";
			this->Text = L"LCREYE [ALPHA]";
			this->ResumeLayout(false);

		}
#pragma endregion
private: System::Void WindowCaptureWorker_DoWork(System::Object^, System::ComponentModel::DoWorkEventArgs^);
private: System::Void WindowCaptureWorker_RunWorkerCompleted(System::Object^, System::ComponentModel::RunWorkerCompletedEventArgs^);
private: System::Void GetAppNames();
private: std::vector<HDC> GetAllMonitors();
private: std::vector<std::pair<int, int>> GetAllMonitorsSizes();
private: System::Void AddMonitorSelection(std::vector<HDC>);
private: System::Void WindowSelection_Click(System::Object^, System::EventArgs^);
private: System::Void WindowSelection_SelectionChangeCommitted(System::Object^, System::EventArgs^);

};
}
