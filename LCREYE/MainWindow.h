#pragma once

#include "ComboboxItem.h"
#include "VFrameReader.h"
#include <vector>
#include <string>
#include <iostream>
#include <codecvt>
#include <list>

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
	private: System::Windows::Forms::PictureBox^ CaptureView;

	private: System::Windows::Forms::Button^ RecordBtn;

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;
	private: System::ComponentModel::BackgroundWorker^ WindowCaptureWorker;

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
			this->CaptureView = (gcnew System::Windows::Forms::PictureBox());
			this->RecordBtn = (gcnew System::Windows::Forms::Button());
			this->WindowCaptureWorker = (gcnew System::ComponentModel::BackgroundWorker());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->CaptureView))->BeginInit();
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
			this->ConsoleBox->Location = System::Drawing::Point(1151, 31);
			this->ConsoleBox->Name = L"ConsoleBox";
			this->ConsoleBox->ReadOnly = true;
			this->ConsoleBox->Size = System::Drawing::Size(261, 636);
			this->ConsoleBox->TabIndex = 6;
			this->ConsoleBox->Text = L"";
			// 
			// WindowSelection
			// 
			this->WindowSelection->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Right));
			this->WindowSelection->FormattingEnabled = true;
			this->WindowSelection->Location = System::Drawing::Point(1151, 4);
			this->WindowSelection->Name = L"WindowSelection";
			this->WindowSelection->Size = System::Drawing::Size(261, 21);
			this->WindowSelection->TabIndex = 3;
			this->WindowSelection->SelectionChangeCommitted += gcnew System::EventHandler(this, &MainWindow::WindowSelection_SelectionChangeCommitted);
			this->WindowSelection->Click += gcnew System::EventHandler(this, &MainWindow::WindowSelection_Click);
			// 
			// CaptureView
			// 
			this->CaptureView->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->CaptureView->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->CaptureView->Location = System::Drawing::Point(2, 4);
			this->CaptureView->Name = L"CaptureView";
			this->CaptureView->Size = System::Drawing::Size(1143, 710);
			this->CaptureView->SizeMode = System::Windows::Forms::PictureBoxSizeMode::StretchImage;
			this->CaptureView->TabIndex = 7;
			this->CaptureView->TabStop = false;
			// 
			// RecordBtn
			// 
			this->RecordBtn->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Right));
			this->RecordBtn->BackColor = System::Drawing::SystemColors::Control;
			this->RecordBtn->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 11.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->RecordBtn->ForeColor = System::Drawing::SystemColors::Desktop;
			this->RecordBtn->Location = System::Drawing::Point(1151, 673);
			this->RecordBtn->Name = L"RecordBtn";
			this->RecordBtn->Size = System::Drawing::Size(261, 41);
			this->RecordBtn->TabIndex = 8;
			this->RecordBtn->Text = L"R  e  c  o  r  d";
			this->RecordBtn->UseVisualStyleBackColor = false;
			// 
			// WindowCaptureWorker
			// 
			this->WindowCaptureWorker->DoWork += gcnew System::ComponentModel::DoWorkEventHandler(this, &MainWindow::WindowCaptureWorker_DoWork);
			this->WindowCaptureWorker->RunWorkerCompleted += gcnew System::ComponentModel::RunWorkerCompletedEventHandler(this, &MainWindow::WindowCaptureWorker_RunWorkerCompleted);
			// 
			// MainWindow
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->BackColor = System::Drawing::SystemColors::Desktop;
			this->ClientSize = System::Drawing::Size(1414, 717);
			this->Controls->Add(this->RecordBtn);
			this->Controls->Add(this->CaptureView);
			this->Controls->Add(this->WindowSelection);
			this->Controls->Add(this->ConsoleBox);
			this->ForeColor = System::Drawing::SystemColors::Control;
			this->Name = L"MainWindow";
			this->Text = L"LCREYE [ALPHA]";
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->CaptureView))->EndInit();
			this->ResumeLayout(false);

		}
#pragma endregion
private: System::Void WindowCaptureWorker_DoWork(System::Object^ sender, System::ComponentModel::DoWorkEventArgs^ e);
private: System::Void WindowCaptureWorker_RunWorkerCompleted(System::Object^ sender, System::ComponentModel::RunWorkerCompletedEventArgs^ e);
private: System::Void GetAppNames();
private: System::Void WindowSelection_Click(System::Object^ sender, System::EventArgs^ e);
private: System::Void WindowSelection_SelectionChangeCommitted(System::Object^ sender, System::EventArgs^ e);
};
}
