#pragma once

namespace LCREYE {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Summary for MainWindow
	/// </summary>
	public ref class MainWindow : public System::Windows::Forms::Form
	{
	public:
		MainWindow(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~MainWindow()
		{
			if (components)
			{
				delete components;
			}
		}

	protected:



	private: System::Windows::Forms::RichTextBox^ Console;
	private: System::Windows::Forms::ComboBox^ WindowSelection;
	private: System::Windows::Forms::PictureBox^ CapView;
	private: System::Windows::Forms::Button^ RecordBtn;





	protected:

	protected:






	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->Console = (gcnew System::Windows::Forms::RichTextBox());
			this->WindowSelection = (gcnew System::Windows::Forms::ComboBox());
			this->CapView = (gcnew System::Windows::Forms::PictureBox());
			this->RecordBtn = (gcnew System::Windows::Forms::Button());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->CapView))->BeginInit();
			this->SuspendLayout();
			// 
			// Console
			// 
			this->Console->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Right));
			this->Console->BackColor = System::Drawing::SystemColors::Desktop;
			this->Console->Cursor = System::Windows::Forms::Cursors::Default;
			this->Console->Font = (gcnew System::Drawing::Font(L"Consolas", 11.5F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(2)));
			this->Console->ForeColor = System::Drawing::Color::Lime;
			this->Console->Location = System::Drawing::Point(1151, 31);
			this->Console->Name = L"Console";
			this->Console->ReadOnly = true;
			this->Console->Size = System::Drawing::Size(261, 636);
			this->Console->TabIndex = 6;
			this->Console->Text = L"Test\nTEST\nT E S T";
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
			// 
			// CapView
			// 
			this->CapView->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->CapView->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->CapView->Location = System::Drawing::Point(2, 4);
			this->CapView->Name = L"CapView";
			this->CapView->Size = System::Drawing::Size(1143, 710);
			this->CapView->TabIndex = 7;
			this->CapView->TabStop = false;
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
			// MainWindow
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->BackColor = System::Drawing::SystemColors::Desktop;
			this->ClientSize = System::Drawing::Size(1414, 717);
			this->Controls->Add(this->RecordBtn);
			this->Controls->Add(this->CapView);
			this->Controls->Add(this->WindowSelection);
			this->Controls->Add(this->Console);
			this->ForeColor = System::Drawing::SystemColors::Control;
			this->Name = L"MainWindow";
			this->Text = L"LCREYE [ALPHA]";
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->CapView))->EndInit();
			this->ResumeLayout(false);

		}
#pragma endregion
};
}
