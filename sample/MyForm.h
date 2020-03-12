#pragma once//インクルードガード：ソースコードのinclude処理が重複して発生することを防ぐ

#include"stdafx.h"

#include<stdlib.h>
#include <string.h>
#include <msclr/marshal.h>
#include <iostream>
#include<vector>
#include<fstream>
#include<opencv2/core.hpp>// opencv346 win32

#include<opencv2/opencv.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include <opencv/highgui.h>
#include <conio.h>
#include <string>

#include <windows.h>
#include "cvwin.hpp"

#include "pch.h"
#include <librealsense2/rsutil.h>
#include <librealsense2/rs.hpp>//*****
//#include"cv-helpers.hpp"//*****

#include<opencv2/imgcodecs.hpp>
#include<opencv2/stitching.hpp>

#include <opencv2/xfeatures2d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include<opencv2/calib3d/calib3d.hpp>

#include <typeinfo.h>
#include <cmath>
#include<math.h>

#include<Eigen/Core>
#include<Eigen/Dense>
#include <opencv2/core/eigen.hpp>
#include <typeinfo.h>



void rsExposure3();
void rsGetPointCloud();
void rsCalib(int n);


void nirExposurekai();
void coordiTransRs2NirAuto2();
void coordiTransRs2NirPanTilt2();
void getNirPanTilt();
double cordiTransBagFile();
void removeToFar(std::string n);




void estimateWater(std::string num);
void createHistgram();
cv::Mat kMeans(cv::Mat src_img);
cv::Mat createDiffImg(cv::Mat grayRs, cv::Mat nir, std::string date, std::string num, int limen);


void testFileStorage();
void calicTest();
void nitika();
void blockNoise();


namespace sample {
	
	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
    using namespace System::Text;
    using namespace System::Configuration;
    using namespace msclr::interop;

    #include "resource.h"
    #include "..\BitranCCDlib.h"
    #define TREEVIEW_CONTINUE   TreeView->Nodes[0]
    #define TREEVIEW_RECORD     TreeView->Nodes[0]->Nodes[0]
    #define TREEVIEW_CONVERT    TreeView->Nodes[1]
    #define TREEVIEW_FANSTOP    TreeView->Nodes[2]
    #define TREEVIEW_CSVMODE    TreeView->Nodes[3]

	/// <summary>
	/// Summary for MyForm
	/// </summary>
	public ref class MyForm : public System::Windows::Forms::Form
	{
	/*
	 * 自分で定義したメンバ変数
 	 */
	private:
		bool startCalibration = false;

	public:
		MyForm(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//

 			//
            // If setting of application is not necessary, you can delete the following
			//
            array<String^>^split;
            String^ str = ConfigurationManager::AppSettings["Location"];
            if (str)
            {
                StartPosition=FormStartPosition::Manual;
                split=str->Split(',');
                this->Location=Point(Convert::ToInt32(split[0]), Convert::ToInt32(split[1]));
            }
            str=ConfigurationManager::AppSettings["Size"];
            if (str)
            {
                split=str->Split(',');
                this->Width=Convert::ToInt32(split[0]);
                this->Height=Convert::ToInt32(split[1]);
            }

            str=ConfigurationManager::AppSettings["Power"];
            if (str)
                CoolerPower->Text=str;
    
            str=ConfigurationManager::AppSettings["Temp"];
            if (str)
                CoolerTemp->Text=str;
    
            str=ConfigurationManager::AppSettings["Time"];
            if (str)
                ShutterTime->Text=str;
    
            str=ConfigurationManager::AppSettings["Count"];
            if (str)
                CountBox->Text=str;

            str=ConfigurationManager::AppSettings["Continuous"];
            if (str)
                TREEVIEW_CONTINUE->Checked=Convert::ToBoolean(str);

			str = ConfigurationManager::AppSettings["Continuous2"];//**************************
			if (str)
				TREEVIEW_CONTINUE->Checked = Convert::ToBoolean(str);

            str=ConfigurationManager::AppSettings["Zoom"];
            if (str)
                ZoomTrackBar->Value=Convert::ToInt32(str);

            str=ConfigurationManager::AppSettings["Convert1"];
            if (str)
                ConvertRadio1->Checked=Convert::ToBoolean(str);
            str=ConfigurationManager::AppSettings["Convert2"];
            if (str)
                ConvertRadio2->Checked=Convert::ToBoolean(str);
            str=ConfigurationManager::AppSettings["Convert3"];
            if (str)
                ConvertRadio3->Checked=Convert::ToBoolean(str);
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~MyForm()
		{
            // When you want to store all application set points, you must add you
            System::Configuration::Configuration ^config = 
                    ConfigurationManager::OpenExeConfiguration(ConfigurationUserLevel::None);

            config->AppSettings->Settings->Remove("Location");
            config->AppSettings->Settings->Add("Location", String::Format("{0:D},{1:D}", Location.X, Location.Y));
            config->AppSettings->Settings->Remove("Size");
            config->AppSettings->Settings->Add("Size", String::Format("{0:D},{1:D}", Width, Height));
            config->AppSettings->Settings->Remove("Power");
            config->AppSettings->Settings->Add("Power", CoolerPower->Value.ToString());
            config->AppSettings->Settings->Remove("Temp");
            config->AppSettings->Settings->Add("Temp", CoolerTemp->Text);
            config->AppSettings->Settings->Remove("Time");
            config->AppSettings->Settings->Add("Time", ShutterTime->Text);
            config->AppSettings->Settings->Remove("Count");
            config->AppSettings->Settings->Add("Count", CountBox->Text);
            config->AppSettings->Settings->Remove("Continuous");
            config->AppSettings->Settings->Add("Continuous", TREEVIEW_CONTINUE->Checked.ToString());

			config->AppSettings->Settings->Remove("Continuous2");
			config->AppSettings->Settings->Add("Continuous2", TREEVIEW_CONTINUE->Checked.ToString());//***********************

            config->AppSettings->Settings->Remove("Zoom");
            config->AppSettings->Settings->Add("Zoom", ZoomTrackBar->Value.ToString());
            config->AppSettings->Settings->Remove("Convert1");
            config->AppSettings->Settings->Add("Convert1", ConvertRadio1->Checked.ToString());
            config->AppSettings->Settings->Remove("Convert2");
            config->AppSettings->Settings->Add("Convert2", ConvertRadio2->Checked.ToString());
            config->AppSettings->Settings->Remove("Convert3");
            config->AppSettings->Settings->Add("Convert3", ConvertRadio3->Checked.ToString());
            config->Save(ConfigurationSaveMode::Modified);
            ConfigurationManager::RefreshSection("appSettings");

            if (pImageData)
                delete[] pImageData;

			if (components)
			{
				delete components;
			}
		}

#pragma region Windows Form Designer generated code
    protected: 
    private: System::Windows::Forms::PictureBox^  PictureImage;
    private: System::Windows::Forms::Timer^  CoolerTimer;
    private: System::Windows::Forms::Button^  CameraConnect;
    private: System::Windows::Forms::SaveFileDialog^  SaveFileDialog;
    private: System::Windows::Forms::ToolTip^  CoolerPowerToolTip;
    private: System::Windows::Forms::StatusBar^  StatusBar;
    private: System::Windows::Forms::TabControl^  CoolerTab;
    private: System::Windows::Forms::TabPage^  tabPage1;
    private: System::Windows::Forms::Button^  PowerButton;
    private: System::Windows::Forms::ProgressBar^  PowerProgress;
    private: System::Windows::Forms::NumericUpDown^  CoolerPower;
    private: System::Windows::Forms::TabPage^  tabPage2;
    private: System::Windows::Forms::Label^  PowerLabel;
    private: System::Windows::Forms::Button^  CoolerButton;
    private: System::Windows::Forms::DomainUpDown^  CoolerTemp;
    private: System::Windows::Forms::Label^  BodyTemp;
    private: System::Windows::Forms::Label^  CcdTemp;
    private: System::Windows::Forms::Label^  PowerSupply;
    private: System::Windows::Forms::TextBox^  CameraStatus;
    private: System::Windows::Forms::ComboBox^  CameraCombo;
    private: System::Windows::Forms::ErrorProvider^  errorProvider;
    private: System::Windows::Forms::ToolTip^  toolTip;
    private: System::Windows::Forms::Button^  CameraDisconnect;
    private: System::Windows::Forms::GroupBox^  groupBox1;
    private: System::Windows::Forms::RadioButton^  ConvertRadio3;
    private: System::Windows::Forms::RadioButton^  ConvertRadio2;
    private: System::Windows::Forms::RadioButton^  ConvertRadio1;
    private: System::Windows::Forms::Button^  StartButton;
	private: System::Windows::Forms::ComboBox^  CountBox;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::Button^  CalibrateButton;

    //private: System::Windows::Forms::ComboBox^  CountBox;
    //private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::Button^  StopButton;

    private: System::Windows::Forms::TreeView^  TreeView;
	//private: System::Windows::Forms::NumericUpDown^  ShutterTime;
	System::Windows::Forms::NumericUpDown^  ShutterTime;
	private: System::Windows::Forms::Label^  label3;
    private: System::Windows::Forms::StatusBarPanel^  statusBarInfo;
    private: System::Windows::Forms::StatusBarPanel^  statusBarStamp;
    private: System::Windows::Forms::StatusBarPanel^  statusBarBack;
    private: System::Windows::Forms::StatusBarPanel^  statusBarCounter;
    private: System::Windows::Forms::StatusBarPanel^  statusBarPlay;
    private: System::Windows::Forms::StatusBarPanel^  statusBarFirst;
    private: System::Windows::Forms::StatusBarPanel^  statusBarLast;
    private: System::Windows::Forms::NumericUpDown^  DelayUpDown;
    private: System::Windows::Forms::Label^  DelayLabel;
    private: System::Windows::Forms::TrackBar^  ZoomTrackBar;
    private: System::Windows::Forms::Label^  label4;
    private: System::Windows::Forms::Label^  CountLabel;
    private: System::Windows::Forms::Label^  label6;
    private: System::Windows::Forms::Label^  DelayMs;
	private: System::Diagnostics::Process^  CsvProcess;
	private: System::Windows::Forms::Button^  button1;
	private: System::Windows::Forms::PictureBox^  pictureImage2;


	private: System::Windows::Forms::Button^  button4;




	private: System::Windows::Forms::Button^  button9;
private: System::Windows::Forms::Button^  button10;
private: System::Windows::Forms::Button^  button11;
private: System::Windows::Forms::ComboBox^  comboBox1;
private: System::Windows::Forms::Button^  button12;
private: System::Windows::Forms::GroupBox^  groupBox3;
private: System::Windows::Forms::GroupBox^  groupBox2;
private: System::Windows::Forms::Button^  button2;
private: System::Windows::Forms::ComboBox^  comboBox2;
private: System::Windows::Forms::Button^  button3;

		 //private: System::Windows::Forms::Button^  calibreat;

    private: System::ComponentModel::IContainer^  components;
#pragma endregion

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>


#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(MyForm::typeid));
			System::Windows::Forms::TreeNode^  treeNode1 = (gcnew System::Windows::Forms::TreeNode(L"Record"));
			System::Windows::Forms::TreeNode^  treeNode2 = (gcnew System::Windows::Forms::TreeNode(L"Continuous2", gcnew cli::array< System::Windows::Forms::TreeNode^  >(1) { treeNode1 }));
			System::Windows::Forms::TreeNode^  treeNode3 = (gcnew System::Windows::Forms::TreeNode(L"Convert Mode"));
			System::Windows::Forms::TreeNode^  treeNode4 = (gcnew System::Windows::Forms::TreeNode(L"FAN-OFF"));
			System::Windows::Forms::TreeNode^  treeNode5 = (gcnew System::Windows::Forms::TreeNode(L"CSV(Excel)"));
			this->PictureImage = (gcnew System::Windows::Forms::PictureBox());
			this->CoolerTimer = (gcnew System::Windows::Forms::Timer(this->components));
			this->CameraConnect = (gcnew System::Windows::Forms::Button());
			this->SaveFileDialog = (gcnew System::Windows::Forms::SaveFileDialog());
			this->CoolerPowerToolTip = (gcnew System::Windows::Forms::ToolTip(this->components));
			this->ShutterTime = (gcnew System::Windows::Forms::NumericUpDown());
			this->button11 = (gcnew System::Windows::Forms::Button());
			this->button9 = (gcnew System::Windows::Forms::Button());
			this->button4 = (gcnew System::Windows::Forms::Button());
			this->button10 = (gcnew System::Windows::Forms::Button());
			this->StatusBar = (gcnew System::Windows::Forms::StatusBar());
			this->statusBarInfo = (gcnew System::Windows::Forms::StatusBarPanel());
			this->statusBarStamp = (gcnew System::Windows::Forms::StatusBarPanel());
			this->statusBarFirst = (gcnew System::Windows::Forms::StatusBarPanel());
			this->statusBarBack = (gcnew System::Windows::Forms::StatusBarPanel());
			this->statusBarCounter = (gcnew System::Windows::Forms::StatusBarPanel());
			this->statusBarPlay = (gcnew System::Windows::Forms::StatusBarPanel());
			this->statusBarLast = (gcnew System::Windows::Forms::StatusBarPanel());
			this->CoolerTab = (gcnew System::Windows::Forms::TabControl());
			this->tabPage1 = (gcnew System::Windows::Forms::TabPage());
			this->PowerButton = (gcnew System::Windows::Forms::Button());
			this->PowerProgress = (gcnew System::Windows::Forms::ProgressBar());
			this->CoolerPower = (gcnew System::Windows::Forms::NumericUpDown());
			this->tabPage2 = (gcnew System::Windows::Forms::TabPage());
			this->PowerLabel = (gcnew System::Windows::Forms::Label());
			this->CoolerButton = (gcnew System::Windows::Forms::Button());
			this->CoolerTemp = (gcnew System::Windows::Forms::DomainUpDown());
			this->BodyTemp = (gcnew System::Windows::Forms::Label());
			this->CcdTemp = (gcnew System::Windows::Forms::Label());
			this->PowerSupply = (gcnew System::Windows::Forms::Label());
			this->CameraStatus = (gcnew System::Windows::Forms::TextBox());
			this->CameraCombo = (gcnew System::Windows::Forms::ComboBox());
			this->errorProvider = (gcnew System::Windows::Forms::ErrorProvider(this->components));
			this->toolTip = (gcnew System::Windows::Forms::ToolTip(this->components));
			this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
			this->button1 = (gcnew System::Windows::Forms::Button());
			this->ZoomTrackBar = (gcnew System::Windows::Forms::TrackBar());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->DelayUpDown = (gcnew System::Windows::Forms::NumericUpDown());
			this->CountLabel = (gcnew System::Windows::Forms::Label());
			this->DelayMs = (gcnew System::Windows::Forms::Label());
			this->DelayLabel = (gcnew System::Windows::Forms::Label());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->ConvertRadio3 = (gcnew System::Windows::Forms::RadioButton());
			this->ConvertRadio2 = (gcnew System::Windows::Forms::RadioButton());
			this->ConvertRadio1 = (gcnew System::Windows::Forms::RadioButton());
			this->CountBox = (gcnew System::Windows::Forms::ComboBox());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->TreeView = (gcnew System::Windows::Forms::TreeView());
			this->button12 = (gcnew System::Windows::Forms::Button());
			this->comboBox1 = (gcnew System::Windows::Forms::ComboBox());
			this->StopButton = (gcnew System::Windows::Forms::Button());
			this->StartButton = (gcnew System::Windows::Forms::Button());
			this->CalibrateButton = (gcnew System::Windows::Forms::Button());
			this->CameraDisconnect = (gcnew System::Windows::Forms::Button());
			this->label6 = (gcnew System::Windows::Forms::Label());
			this->CsvProcess = (gcnew System::Diagnostics::Process());
			this->pictureImage2 = (gcnew System::Windows::Forms::PictureBox());
			this->groupBox2 = (gcnew System::Windows::Forms::GroupBox());
			this->groupBox3 = (gcnew System::Windows::Forms::GroupBox());
			this->comboBox2 = (gcnew System::Windows::Forms::ComboBox());
			this->button2 = (gcnew System::Windows::Forms::Button());
			this->button3 = (gcnew System::Windows::Forms::Button());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->PictureImage))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->ShutterTime))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->statusBarInfo))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->statusBarStamp))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->statusBarFirst))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->statusBarBack))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->statusBarCounter))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->statusBarPlay))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->statusBarLast))->BeginInit();
			this->CoolerTab->SuspendLayout();
			this->tabPage1->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->CoolerPower))->BeginInit();
			this->tabPage2->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->errorProvider))->BeginInit();
			this->groupBox1->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->ZoomTrackBar))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->DelayUpDown))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureImage2))->BeginInit();
			this->groupBox2->SuspendLayout();
			this->groupBox3->SuspendLayout();
			this->SuspendLayout();
			// 
			// PictureImage
			// 
			this->PictureImage->Cursor = System::Windows::Forms::Cursors::UpArrow;
			this->PictureImage->Enabled = false;
			this->PictureImage->Location = System::Drawing::Point(846, 15);
			this->PictureImage->Margin = System::Windows::Forms::Padding(3, 2, 3, 2);
			this->PictureImage->Name = L"PictureImage";
			this->PictureImage->Size = System::Drawing::Size(295, 274);
			this->PictureImage->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			this->PictureImage->TabIndex = 21;
			this->PictureImage->TabStop = false;
			this->PictureImage->Click += gcnew System::EventHandler(this, &MyForm::PictureImage_Click);
			// 
			// CoolerTimer
			// 
			this->CoolerTimer->Tick += gcnew System::EventHandler(this, &MyForm::CoolerTimer_Tick);
			// 
			// CameraConnect
			// 
			this->CameraConnect->Location = System::Drawing::Point(703, 40);
			this->CameraConnect->Margin = System::Windows::Forms::Padding(3, 2, 3, 2);
			this->CameraConnect->Name = L"CameraConnect";
			this->CameraConnect->Size = System::Drawing::Size(113, 24);
			this->CameraConnect->TabIndex = 19;
			this->CameraConnect->Text = L"Connect";
			this->CameraConnect->UseVisualStyleBackColor = true;
			this->CameraConnect->Click += gcnew System::EventHandler(this, &MyForm::CameraConnect_Click);
			// 
			// SaveFileDialog
			// 
			this->SaveFileDialog->Filter = L"Binary|*.bin|RAW|*.raw|TIFF|*.tif|CSV|*.csv";
			// 
			// ShutterTime
			// 
			this->ShutterTime->Location = System::Drawing::Point(17, 38);
			this->ShutterTime->Margin = System::Windows::Forms::Padding(3, 2, 3, 2);
			this->ShutterTime->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 1000, 0, 0, 0 });
			this->ShutterTime->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 4, 0, 0, 0 });
			this->ShutterTime->Name = L"ShutterTime";
			this->ShutterTime->Size = System::Drawing::Size(81, 22);
			this->ShutterTime->TabIndex = 25;
			this->ShutterTime->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			this->CoolerPowerToolTip->SetToolTip(this->ShutterTime, L"こっち見んなｗ");
			this->ShutterTime->UpDownAlign = System::Windows::Forms::LeftRightAlignment::Left;
			this->ShutterTime->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) { 100, 0, 0, 0 });
			this->ShutterTime->ValueChanged += gcnew System::EventHandler(this, &MyForm::ShutterTime_ValueChanged);
			// 
			// button11
			// 
			this->button11->Enabled = false;
			this->button11->Location = System::Drawing::Point(16, 63);
			this->button11->Name = L"button11";
			this->button11->Size = System::Drawing::Size(116, 23);
			this->button11->TabIndex = 45;
			this->button11->Text = L"getNirPnaTilt";
			this->CoolerPowerToolTip->SetToolTip(this->button11, L"座標変換せずパンチルト雲台使って撮影");
			this->button11->UseVisualStyleBackColor = true;
			this->button11->Click += gcnew System::EventHandler(this, &MyForm::button11_Click);
			// 
			// button9
			// 
			this->button9->Enabled = false;
			this->button9->Location = System::Drawing::Point(6, 34);
			this->button9->Name = L"button9";
			this->button9->Size = System::Drawing::Size(139, 23);
			this->button9->TabIndex = 43;
			this->button9->Text = L"convCoordiRs2Nir";
			this->CoolerPowerToolTip->SetToolTip(this->button9, L"ちゃんと1フレームのpcで座標変換");
			this->button9->UseVisualStyleBackColor = true;
			this->button9->Click += gcnew System::EventHandler(this, &MyForm::button9_Click);
			// 
			// button4
			// 
			this->button4->Location = System::Drawing::Point(149, 105);
			this->button4->Name = L"button4";
			this->button4->Size = System::Drawing::Size(100, 24);
			this->button4->TabIndex = 38;
			this->button4->Text = L"test_function";
			this->CoolerPowerToolTip->SetToolTip(this->button4, L"テスト用のボタン");
			this->button4->UseVisualStyleBackColor = true;
			this->button4->Click += gcnew System::EventHandler(this, &MyForm::button4_Click);
			// 
			// button10
			// 
			this->button10->Location = System::Drawing::Point(359, 14);
			this->button10->Name = L"button10";
			this->button10->Size = System::Drawing::Size(135, 43);
			this->button10->TabIndex = 44;
			this->button10->Text = L"３次元距離情報のぶっ飛んだ値を消す";
			this->CoolerPowerToolTip->SetToolTip(this->button10, L"csvを読み込む");
			this->button10->UseVisualStyleBackColor = true;
			this->button10->Click += gcnew System::EventHandler(this, &MyForm::button10_Click);
			// 
			// StatusBar
			// 
			this->StatusBar->Location = System::Drawing::Point(0, 869);
			this->StatusBar->Margin = System::Windows::Forms::Padding(3, 2, 3, 2);
			this->StatusBar->Name = L"StatusBar";
			this->StatusBar->Panels->AddRange(gcnew cli::array< System::Windows::Forms::StatusBarPanel^  >(7) {
				this->statusBarInfo, this->statusBarStamp,
					this->statusBarFirst, this->statusBarBack, this->statusBarCounter, this->statusBarPlay, this->statusBarLast
			});
			this->StatusBar->ShowPanels = true;
			this->StatusBar->Size = System::Drawing::Size(1153, 21);
			this->StatusBar->TabIndex = 22;
			this->StatusBar->PanelClick += gcnew System::Windows::Forms::StatusBarPanelClickEventHandler(this, &MyForm::StatusBar_PanelClick);
			// 
			// statusBarInfo
			// 
			this->statusBarInfo->AutoSize = System::Windows::Forms::StatusBarPanelAutoSize::Spring;
			this->statusBarInfo->MinWidth = 30;
			this->statusBarInfo->Name = L"statusBarInfo";
			this->statusBarInfo->Width = 702;
			// 
			// statusBarStamp
			// 
			this->statusBarStamp->Name = L"statusBarStamp";
			this->statusBarStamp->Width = 200;
			// 
			// statusBarFirst
			// 
			this->statusBarFirst->Icon = (cli::safe_cast<System::Drawing::Icon^>(resources->GetObject(L"statusBarFirst.Icon")));
			this->statusBarFirst->Name = L"statusBarFirst";
			this->statusBarFirst->Width = 20;
			// 
			// statusBarBack
			// 
			this->statusBarBack->Icon = (cli::safe_cast<System::Drawing::Icon^>(resources->GetObject(L"statusBarBack.Icon")));
			this->statusBarBack->Name = L"statusBarBack";
			this->statusBarBack->Width = 20;
			// 
			// statusBarCounter
			// 
			this->statusBarCounter->Alignment = System::Windows::Forms::HorizontalAlignment::Center;
			this->statusBarCounter->Name = L"statusBarCounter";
			this->statusBarCounter->Width = 150;
			// 
			// statusBarPlay
			// 
			this->statusBarPlay->Icon = (cli::safe_cast<System::Drawing::Icon^>(resources->GetObject(L"statusBarPlay.Icon")));
			this->statusBarPlay->Name = L"statusBarPlay";
			this->statusBarPlay->Width = 20;
			// 
			// statusBarLast
			// 
			this->statusBarLast->Icon = (cli::safe_cast<System::Drawing::Icon^>(resources->GetObject(L"statusBarLast.Icon")));
			this->statusBarLast->Name = L"statusBarLast";
			this->statusBarLast->Width = 20;
			// 
			// CoolerTab
			// 
			this->CoolerTab->Controls->Add(this->tabPage1);
			this->CoolerTab->Controls->Add(this->tabPage2);
			this->CoolerTab->Location = System::Drawing::Point(15, 15);
			this->CoolerTab->Margin = System::Windows::Forms::Padding(3, 2, 3, 2);
			this->CoolerTab->Name = L"CoolerTab";
			this->CoolerTab->SelectedIndex = 1;
			this->CoolerTab->Size = System::Drawing::Size(251, 145);
			this->CoolerTab->TabIndex = 13;
			// 
			// tabPage1
			// 
			this->tabPage1->Controls->Add(this->PowerButton);
			this->tabPage1->Controls->Add(this->PowerProgress);
			this->tabPage1->Controls->Add(this->CoolerPower);
			this->tabPage1->Location = System::Drawing::Point(4, 25);
			this->tabPage1->Margin = System::Windows::Forms::Padding(3, 2, 3, 2);
			this->tabPage1->Name = L"tabPage1";
			this->tabPage1->Padding = System::Windows::Forms::Padding(3, 2, 3, 2);
			this->tabPage1->Size = System::Drawing::Size(243, 116);
			this->tabPage1->TabIndex = 0;
			this->tabPage1->Text = L"Cooler Power";
			this->tabPage1->UseVisualStyleBackColor = true;
			// 
			// PowerButton
			// 
			this->PowerButton->Enabled = false;
			this->PowerButton->Location = System::Drawing::Point(124, 30);
			this->PowerButton->Margin = System::Windows::Forms::Padding(3, 2, 3, 2);
			this->PowerButton->Name = L"PowerButton";
			this->PowerButton->Size = System::Drawing::Size(88, 25);
			this->PowerButton->TabIndex = 2;
			this->PowerButton->Text = L"Apply";
			this->PowerButton->UseVisualStyleBackColor = true;
			this->PowerButton->Click += gcnew System::EventHandler(this, &MyForm::PowerButton_Click);
			// 
			// PowerProgress
			// 
			this->PowerProgress->Location = System::Drawing::Point(27, 78);
			this->PowerProgress->Margin = System::Windows::Forms::Padding(3, 2, 3, 2);
			this->PowerProgress->Maximum = 255;
			this->PowerProgress->Name = L"PowerProgress";
			this->PowerProgress->Size = System::Drawing::Size(185, 24);
			this->PowerProgress->TabIndex = 1;
			// 
			// CoolerPower
			// 
			this->CoolerPower->Location = System::Drawing::Point(27, 31);
			this->CoolerPower->Margin = System::Windows::Forms::Padding(3, 2, 3, 2);
			this->CoolerPower->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 255, 0, 0, 0 });
			this->CoolerPower->Name = L"CoolerPower";
			this->CoolerPower->Size = System::Drawing::Size(83, 22);
			this->CoolerPower->TabIndex = 0;
			this->CoolerPower->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) { 150, 0, 0, 0 });
			// 
			// tabPage2
			// 
			this->tabPage2->Controls->Add(this->PowerLabel);
			this->tabPage2->Controls->Add(this->CoolerButton);
			this->tabPage2->Controls->Add(this->CoolerTemp);
			this->tabPage2->Location = System::Drawing::Point(4, 25);
			this->tabPage2->Margin = System::Windows::Forms::Padding(3, 2, 3, 2);
			this->tabPage2->Name = L"tabPage2";
			this->tabPage2->Padding = System::Windows::Forms::Padding(3, 2, 3, 2);
			this->tabPage2->Size = System::Drawing::Size(243, 116);
			this->tabPage2->TabIndex = 1;
			this->tabPage2->Text = L"Cooler Control";
			this->tabPage2->UseVisualStyleBackColor = true;
			// 
			// PowerLabel
			// 
			this->PowerLabel->AutoSize = true;
			this->PowerLabel->Location = System::Drawing::Point(51, 92);
			this->PowerLabel->Name = L"PowerLabel";
			this->PowerLabel->Size = System::Drawing::Size(137, 15);
			this->PowerLabel->TabIndex = 3;
			this->PowerLabel->Text = L"Current power value";
			this->PowerLabel->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// CoolerButton
			// 
			this->CoolerButton->Enabled = false;
			this->CoolerButton->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 8.861538F, static_cast<System::Drawing::FontStyle>((System::Drawing::FontStyle::Bold | System::Drawing::FontStyle::Italic)),
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(128)));
			this->CoolerButton->Location = System::Drawing::Point(19, 41);
			this->CoolerButton->Margin = System::Windows::Forms::Padding(3, 2, 3, 2);
			this->CoolerButton->Name = L"CoolerButton";
			this->CoolerButton->Size = System::Drawing::Size(201, 49);
			this->CoolerButton->TabIndex = 2;
			this->CoolerButton->Text = L"Temperature Control";
			this->CoolerButton->UseVisualStyleBackColor = true;
			this->CoolerButton->Click += gcnew System::EventHandler(this, &MyForm::CoolerButton_Click);
			// 
			// CoolerTemp
			// 
			this->CoolerTemp->Items->Add(L"10 C");
			this->CoolerTemp->Items->Add(L"5 C");
			this->CoolerTemp->Items->Add(L"0 C");
			this->CoolerTemp->Items->Add(L"-5 C");
			this->CoolerTemp->Items->Add(L"-10 C");
			this->CoolerTemp->Items->Add(L"-20 C");
			this->CoolerTemp->Location = System::Drawing::Point(68, 14);
			this->CoolerTemp->Margin = System::Windows::Forms::Padding(3, 2, 3, 2);
			this->CoolerTemp->Name = L"CoolerTemp";
			this->CoolerTemp->Size = System::Drawing::Size(103, 22);
			this->CoolerTemp->TabIndex = 0;
			this->CoolerTemp->Text = L"-10 C";
			// 
			// BodyTemp
			// 
			this->BodyTemp->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->BodyTemp->ForeColor = System::Drawing::SystemColors::HotTrack;
			this->BodyTemp->Location = System::Drawing::Point(287, 80);
			this->BodyTemp->Name = L"BodyTemp";
			this->BodyTemp->Size = System::Drawing::Size(112, 31);
			this->BodyTemp->TabIndex = 15;
			this->BodyTemp->Text = L"External";
			this->BodyTemp->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// CcdTemp
			// 
			this->CcdTemp->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->CcdTemp->ForeColor = System::Drawing::SystemColors::HotTrack;
			this->CcdTemp->Location = System::Drawing::Point(287, 36);
			this->CcdTemp->Name = L"CcdTemp";
			this->CcdTemp->Size = System::Drawing::Size(112, 31);
			this->CcdTemp->TabIndex = 14;
			this->CcdTemp->Text = L"Camera";
			this->CcdTemp->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// PowerSupply
			// 
			this->PowerSupply->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(157)), static_cast<System::Int32>(static_cast<System::Byte>(172)),
				static_cast<System::Int32>(static_cast<System::Byte>(189)));
			this->PowerSupply->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->PowerSupply->Location = System::Drawing::Point(287, 124);
			this->PowerSupply->Name = L"PowerSupply";
			this->PowerSupply->Size = System::Drawing::Size(112, 31);
			this->PowerSupply->TabIndex = 16;
			this->PowerSupply->Text = L"Voltage";
			this->PowerSupply->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// CameraStatus
			// 
			this->CameraStatus->Location = System::Drawing::Point(417, 36);
			this->CameraStatus->Margin = System::Windows::Forms::Padding(3, 2, 3, 2);
			this->CameraStatus->Multiline = true;
			this->CameraStatus->Name = L"CameraStatus";
			this->CameraStatus->ReadOnly = true;
			this->CameraStatus->Size = System::Drawing::Size(271, 118);
			this->CameraStatus->TabIndex = 17;
			this->CameraStatus->Text = L"Unconnected";
			// 
			// CameraCombo
			// 
			this->CameraCombo->BackColor = System::Drawing::Color::RosyBrown;
			this->CameraCombo->FormattingEnabled = true;
			this->CameraCombo->Items->AddRange(gcnew cli::array< System::Object^  >(1) { L"BK-51IGA" });
			this->CameraCombo->Location = System::Drawing::Point(703, 11);
			this->CameraCombo->Margin = System::Windows::Forms::Padding(3, 2, 3, 2);
			this->CameraCombo->Name = L"CameraCombo";
			this->CameraCombo->Size = System::Drawing::Size(113, 23);
			this->CameraCombo->TabIndex = 18;
			this->CameraCombo->Text = L"BK-51IGA";
			this->CameraCombo->SelectedIndexChanged += gcnew System::EventHandler(this, &MyForm::CameraCombo_SelectedIndexChanged);
			// 
			// errorProvider
			// 
			this->errorProvider->ContainerControl = this;
			// 
			// groupBox1
			// 
			this->groupBox1->Controls->Add(this->button1);
			this->groupBox1->Controls->Add(this->button4);
			this->groupBox1->Controls->Add(this->ZoomTrackBar);
			this->groupBox1->Controls->Add(this->label4);
			this->groupBox1->Controls->Add(this->DelayUpDown);
			this->groupBox1->Controls->Add(this->CountLabel);
			this->groupBox1->Controls->Add(this->DelayMs);
			this->groupBox1->Controls->Add(this->DelayLabel);
			this->groupBox1->Controls->Add(this->label3);
			this->groupBox1->Controls->Add(this->ShutterTime);
			this->groupBox1->Controls->Add(this->ConvertRadio3);
			this->groupBox1->Controls->Add(this->ConvertRadio2);
			this->groupBox1->Controls->Add(this->ConvertRadio1);
			this->groupBox1->Controls->Add(this->CountBox);
			this->groupBox1->Controls->Add(this->label1);
			this->groupBox1->Controls->Add(this->TreeView);
			this->groupBox1->Location = System::Drawing::Point(17, 178);
			this->groupBox1->Margin = System::Windows::Forms::Padding(3, 2, 3, 2);
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Padding = System::Windows::Forms::Padding(3, 2, 3, 2);
			this->groupBox1->Size = System::Drawing::Size(808, 141);
			this->groupBox1->TabIndex = 23;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = L"settings";
			// 
			// button1
			// 
			this->button1->Location = System::Drawing::Point(23, 106);
			this->button1->Name = L"button1";
			this->button1->Size = System::Drawing::Size(75, 23);
			this->button1->TabIndex = 29;
			this->button1->Text = L"usb_cam";
			this->button1->UseVisualStyleBackColor = true;
			this->button1->Click += gcnew System::EventHandler(this, &MyForm::button1_Click);
			// 
			// ZoomTrackBar
			// 
			this->ZoomTrackBar->LargeChange = 50;
			this->ZoomTrackBar->Location = System::Drawing::Point(156, 34);
			this->ZoomTrackBar->Margin = System::Windows::Forms::Padding(3, 2, 3, 2);
			this->ZoomTrackBar->Maximum = 500;
			this->ZoomTrackBar->Minimum = 100;
			this->ZoomTrackBar->Name = L"ZoomTrackBar";
			this->ZoomTrackBar->Size = System::Drawing::Size(109, 56);
			this->ZoomTrackBar->SmallChange = 50;
			this->ZoomTrackBar->TabIndex = 35;
			this->ZoomTrackBar->TickFrequency = 100;
			this->ZoomTrackBar->TickStyle = System::Windows::Forms::TickStyle::TopLeft;
			this->ZoomTrackBar->Value = 200;
			this->ZoomTrackBar->ValueChanged += gcnew System::EventHandler(this, &MyForm::Zoom_Changed);
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->Location = System::Drawing::Point(184, 17);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(42, 15);
			this->label4->TabIndex = 34;
			this->label4->Text = L"Zoom";
			// 
			// DelayUpDown
			// 
			this->DelayUpDown->BackColor = System::Drawing::SystemColors::Info;
			this->DelayUpDown->Location = System::Drawing::Point(669, 89);
			this->DelayUpDown->Margin = System::Windows::Forms::Padding(3, 2, 3, 2);
			this->DelayUpDown->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 10, 0, 0, 0 });
			this->DelayUpDown->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 1, 0, 0, 0 });
			this->DelayUpDown->Name = L"DelayUpDown";
			this->DelayUpDown->Size = System::Drawing::Size(67, 22);
			this->DelayUpDown->TabIndex = 33;
			this->DelayUpDown->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) { 3, 0, 0, 0 });
			this->DelayUpDown->Visible = false;
			// 
			// CountLabel
			// 
			this->CountLabel->AutoSize = true;
			this->CountLabel->BackColor = System::Drawing::SystemColors::Info;
			this->CountLabel->Location = System::Drawing::Point(481, 80);
			this->CountLabel->Name = L"CountLabel";
			this->CountLabel->Size = System::Drawing::Size(155, 15);
			this->CountLabel->TabIndex = 32;
			this->CountLabel->Text = L"The number of records";
			this->CountLabel->Visible = false;
			// 
			// DelayMs
			// 
			this->DelayMs->AutoSize = true;
			this->DelayMs->BackColor = System::Drawing::SystemColors::Info;
			this->DelayMs->Location = System::Drawing::Point(754, 65);
			this->DelayMs->Name = L"DelayMs";
			this->DelayMs->Size = System::Drawing::Size(25, 15);
			this->DelayMs->TabIndex = 32;
			this->DelayMs->Text = L"ms";
			this->DelayMs->Visible = false;
			// 
			// DelayLabel
			// 
			this->DelayLabel->AutoSize = true;
			this->DelayLabel->BackColor = System::Drawing::SystemColors::Info;
			this->DelayLabel->Location = System::Drawing::Point(480, 58);
			this->DelayLabel->Name = L"DelayLabel";
			this->DelayLabel->Size = System::Drawing::Size(156, 15);
			this->DelayLabel->TabIndex = 32;
			this->DelayLabel->Text = L"Time to delay exposure";
			this->DelayLabel->Visible = false;
			// 
			// label3
			// 
			this->label3->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 8.059701F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(128)));
			this->label3->Location = System::Drawing::Point(99, 40);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(33, 21);
			this->label3->TabIndex = 29;
			this->label3->Text = L"ms";
			this->label3->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// ConvertRadio3
			// 
			this->ConvertRadio3->AutoSize = true;
			this->ConvertRadio3->BackColor = System::Drawing::SystemColors::Info;
			this->ConvertRadio3->Location = System::Drawing::Point(483, 101);
			this->ConvertRadio3->Margin = System::Windows::Forms::Padding(3, 2, 3, 2);
			this->ConvertRadio3->Name = L"ConvertRadio3";
			this->ConvertRadio3->Size = System::Drawing::Size(152, 19);
			this->ConvertRadio3->TabIndex = 25;
			this->ConvertRadio3->Text = L"Quarter of the light";
			this->ConvertRadio3->UseVisualStyleBackColor = false;
			this->ConvertRadio3->Visible = false;
			// 
			// ConvertRadio2
			// 
			this->ConvertRadio2->AutoSize = true;
			this->ConvertRadio2->BackColor = System::Drawing::SystemColors::Info;
			this->ConvertRadio2->Location = System::Drawing::Point(669, 39);
			this->ConvertRadio2->Margin = System::Windows::Forms::Padding(3, 2, 3, 2);
			this->ConvertRadio2->Name = L"ConvertRadio2";
			this->ConvertRadio2->Size = System::Drawing::Size(101, 19);
			this->ConvertRadio2->TabIndex = 24;
			this->ConvertRadio2->Text = L"Logarithmic";
			this->ConvertRadio2->UseVisualStyleBackColor = false;
			this->ConvertRadio2->Visible = false;
			// 
			// ConvertRadio1
			// 
			this->ConvertRadio1->AutoSize = true;
			this->ConvertRadio1->BackColor = System::Drawing::SystemColors::Info;
			this->ConvertRadio1->Checked = true;
			this->ConvertRadio1->Location = System::Drawing::Point(483, 40);
			this->ConvertRadio1->Margin = System::Windows::Forms::Padding(3, 2, 3, 2);
			this->ConvertRadio1->Name = L"ConvertRadio1";
			this->ConvertRadio1->Size = System::Drawing::Size(83, 19);
			this->ConvertRadio1->TabIndex = 23;
			this->ConvertRadio1->TabStop = true;
			this->ConvertRadio1->Text = L"Min-Max";
			this->ConvertRadio1->UseVisualStyleBackColor = false;
			this->ConvertRadio1->Visible = false;
			// 
			// CountBox
			// 
			this->CountBox->BackColor = System::Drawing::SystemColors::Info;
			this->CountBox->Items->AddRange(gcnew cli::array< System::Object^  >(5) { L"10", L"100", L"300", L"500", L"1024" });
			this->CountBox->Location = System::Drawing::Point(669, 62);
			this->CountBox->Margin = System::Windows::Forms::Padding(3, 2, 3, 2);
			this->CountBox->Name = L"CountBox";
			this->CountBox->Size = System::Drawing::Size(83, 23);
			this->CountBox->TabIndex = 7;
			this->CountBox->Text = L"100";
			this->CountBox->Visible = false;
			// 
			// label1
			// 
			this->label1->Location = System::Drawing::Point(14, 17);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(105, 20);
			this->label1->TabIndex = 0;
			this->label1->Text = L"Exposure Time";
			// 
			// TreeView
			// 
			this->TreeView->BackColor = System::Drawing::SystemColors::Info;
			this->TreeView->CheckBoxes = true;
			this->TreeView->HotTracking = true;
			this->TreeView->Location = System::Drawing::Point(292, 29);
			this->TreeView->Margin = System::Windows::Forms::Padding(3, 2, 3, 2);
			this->TreeView->Name = L"TreeView";
			treeNode1->Name = L"RecordMode";
			treeNode1->Text = L"Record";
			treeNode1->ToolTipText = L"Set the number of sheets to record it";
			treeNode2->Name = L"ContinueMode2";
			treeNode2->Text = L"Continuous2";
			treeNode2->ToolTipText = L"Use it if you want to display consecutive images";
			treeNode3->Name = L"Convert";
			treeNode3->Text = L"Convert Mode";
			treeNode3->ToolTipText = L"When you do not check it, the camera converts an image automatically";
			treeNode4->Name = L"FanMode";
			treeNode4->Text = L"FAN-OFF";
			treeNode4->ToolTipText = L"When you check here, exposure begins after a fan stopped for five seconds";
			treeNode5->Name = L"ExcelMode";
			treeNode5->Text = L"CSV(Excel)";
			treeNode5->ToolTipText = L"I can start excel if csv is connected with excel";
			this->TreeView->Nodes->AddRange(gcnew cli::array< System::Windows::Forms::TreeNode^  >(4) {
				treeNode2, treeNode3, treeNode4,
					treeNode5
			});
			this->TreeView->ShowNodeToolTips = true;
			this->TreeView->Size = System::Drawing::Size(507, 100);
			this->TreeView->TabIndex = 22;
			this->TreeView->AfterCheck += gcnew System::Windows::Forms::TreeViewEventHandler(this, &MyForm::TreeCheck);
			this->TreeView->AfterSelect += gcnew System::Windows::Forms::TreeViewEventHandler(this, &MyForm::TreeSelect);
			// 
			// button12
			// 
			this->button12->Location = System::Drawing::Point(167, 27);
			this->button12->Name = L"button12";
			this->button12->Size = System::Drawing::Size(121, 23);
			this->button12->TabIndex = 47;
			this->button12->Text = L"start RS";
			this->button12->UseVisualStyleBackColor = true;
			this->button12->Click += gcnew System::EventHandler(this, &MyForm::button12_Click);
			// 
			// comboBox1
			// 
			this->comboBox1->FormattingEnabled = true;
			this->comboBox1->Items->AddRange(gcnew cli::array< System::Object^  >(3) { L"expose_realsense", L"calib_realsense", L"get_point_cloud" });
			this->comboBox1->Location = System::Drawing::Point(6, 27);
			this->comboBox1->Name = L"comboBox1";
			this->comboBox1->Size = System::Drawing::Size(155, 23);
			this->comboBox1->TabIndex = 46;
			this->comboBox1->Text = L"select RS mode";
			this->comboBox1->SelectedIndexChanged += gcnew System::EventHandler(this, &MyForm::comboBox1_SelectedIndexChanged);
			// 
			// StopButton
			// 
			this->StopButton->Enabled = false;
			this->StopButton->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->StopButton->Location = System::Drawing::Point(705, 153);
			this->StopButton->Margin = System::Windows::Forms::Padding(3, 2, 3, 2);
			this->StopButton->Name = L"StopButton";
			this->StopButton->Size = System::Drawing::Size(111, 26);
			this->StopButton->TabIndex = 5;
			this->StopButton->Text = L"Stop";
			this->StopButton->Click += gcnew System::EventHandler(this, &MyForm::StopButton_Click);
			// 
			// StartButton
			// 
			this->StartButton->Enabled = false;
			this->StartButton->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->StartButton->Location = System::Drawing::Point(703, 96);
			this->StartButton->Margin = System::Windows::Forms::Padding(3, 2, 3, 2);
			this->StartButton->Name = L"StartButton";
			this->StartButton->Size = System::Drawing::Size(113, 21);
			this->StartButton->TabIndex = 4;
			this->StartButton->Text = L"nirExpose";
			this->StartButton->Click += gcnew System::EventHandler(this, &MyForm::StartButton_Click);
			// 
			// CalibrateButton
			// 
			this->CalibrateButton->Enabled = false;
			this->CalibrateButton->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->CalibrateButton->Location = System::Drawing::Point(705, 121);
			this->CalibrateButton->Name = L"CalibrateButton";
			this->CalibrateButton->Size = System::Drawing::Size(111, 30);
			this->CalibrateButton->TabIndex = 29;
			this->CalibrateButton->Text = L"calibration";
			this->CalibrateButton->UseVisualStyleBackColor = true;
			this->CalibrateButton->Click += gcnew System::EventHandler(this, &MyForm::ContinueMode2);
			// 
			// CameraDisconnect
			// 
			this->CameraDisconnect->Cursor = System::Windows::Forms::Cursors::Default;
			this->CameraDisconnect->Enabled = false;
			this->CameraDisconnect->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->CameraDisconnect->Location = System::Drawing::Point(703, 68);
			this->CameraDisconnect->Margin = System::Windows::Forms::Padding(3, 2, 3, 2);
			this->CameraDisconnect->Name = L"CameraDisconnect";
			this->CameraDisconnect->Size = System::Drawing::Size(113, 24);
			this->CameraDisconnect->TabIndex = 24;
			this->CameraDisconnect->Text = L"Disconnect";
			this->CameraDisconnect->Click += gcnew System::EventHandler(this, &MyForm::CameraDisconnect_Click);
			// 
			// label6
			// 
			this->label6->AutoSize = true;
			this->label6->Location = System::Drawing::Point(1049, 86);
			this->label6->Margin = System::Windows::Forms::Padding(4, 0, 4, 0);
			this->label6->Name = L"label6";
			this->label6->Size = System::Drawing::Size(0, 15);
			this->label6->TabIndex = 28;
			// 
			// CsvProcess
			// 
			this->CsvProcess->StartInfo->Domain = L"";
			this->CsvProcess->StartInfo->LoadUserProfile = false;
			this->CsvProcess->StartInfo->Password = nullptr;
			this->CsvProcess->StartInfo->StandardErrorEncoding = nullptr;
			this->CsvProcess->StartInfo->StandardOutputEncoding = nullptr;
			this->CsvProcess->StartInfo->UserName = L"";
			this->CsvProcess->SynchronizingObject = this;
			// 
			// pictureImage2
			// 
			this->pictureImage2->Cursor = System::Windows::Forms::Cursors::UpArrow;
			this->pictureImage2->Enabled = false;
			this->pictureImage2->Location = System::Drawing::Point(846, 305);
			this->pictureImage2->Margin = System::Windows::Forms::Padding(3, 2, 3, 2);
			this->pictureImage2->Name = L"pictureImage2";
			this->pictureImage2->Size = System::Drawing::Size(295, 257);
			this->pictureImage2->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			this->pictureImage2->TabIndex = 21;
			this->pictureImage2->TabStop = false;
			this->pictureImage2->Click += gcnew System::EventHandler(this, &MyForm::pictureBox1_Click);
			// 
			// groupBox2
			// 
			this->groupBox2->Controls->Add(this->button12);
			this->groupBox2->Controls->Add(this->comboBox1);
			this->groupBox2->Location = System::Drawing::Point(19, 334);
			this->groupBox2->Name = L"groupBox2";
			this->groupBox2->Size = System::Drawing::Size(288, 65);
			this->groupBox2->TabIndex = 30;
			this->groupBox2->TabStop = false;
			this->groupBox2->Text = L"realsense";
			// 
			// groupBox3
			// 
			this->groupBox3->Controls->Add(this->button3);
			this->groupBox3->Controls->Add(this->comboBox2);
			this->groupBox3->Controls->Add(this->button2);
			this->groupBox3->Controls->Add(this->button9);
			this->groupBox3->Controls->Add(this->button10);
			this->groupBox3->Controls->Add(this->button11);
			this->groupBox3->Location = System::Drawing::Point(322, 334);
			this->groupBox3->Name = L"groupBox3";
			this->groupBox3->Size = System::Drawing::Size(503, 123);
			this->groupBox3->TabIndex = 31;
			this->groupBox3->TabStop = false;
			this->groupBox3->Text = L"coordinate transform realsense to nir";
			this->groupBox3->Enter += gcnew System::EventHandler(this, &MyForm::groupBox3_Enter);
			// 
			// comboBox2
			// 
			this->comboBox2->FormattingEnabled = true;
			this->comboBox2->Items->AddRange(gcnew cli::array< System::Object^  >(6) { L"1", L"2", L"3", L"4", L"5", L"getHist" });
			this->comboBox2->Location = System::Drawing::Point(210, 28);
			this->comboBox2->Name = L"comboBox2";
			this->comboBox2->Size = System::Drawing::Size(121, 23);
			this->comboBox2->TabIndex = 47;
			this->comboBox2->Text = L"data number";
			this->comboBox2->SelectedIndexChanged += gcnew System::EventHandler(this, &MyForm::comboBox2_SelectedIndexChanged);
			// 
			// button2
			// 
			this->button2->Location = System::Drawing::Point(359, 63);
			this->button2->Name = L"button2";
			this->button2->Size = System::Drawing::Size(129, 23);
			this->button2->TabIndex = 46;
			this->button2->Text = L"estimateWater";
			this->button2->UseVisualStyleBackColor = true;
			this->button2->Click += gcnew System::EventHandler(this, &MyForm::button2_Click_1);
			// 
			// button3
			// 
			this->button3->Location = System::Drawing::Point(359, 93);
			this->button3->Name = L"button3";
			this->button3->Size = System::Drawing::Size(129, 23);
			this->button3->TabIndex = 48;
			this->button3->Text = L"convCordiBagFile";
			this->button3->UseVisualStyleBackColor = true;
			this->button3->Click += gcnew System::EventHandler(this, &MyForm::button3_Click_1);
			// 
			// MyForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(8, 15);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(1153, 890);
			this->Controls->Add(this->groupBox3);
			this->Controls->Add(this->groupBox2);
			this->Controls->Add(this->pictureImage2);
			this->Controls->Add(this->label6);
			this->Controls->Add(this->CameraDisconnect);
			this->Controls->Add(this->groupBox1);
			this->Controls->Add(this->PictureImage);
			this->Controls->Add(this->CameraConnect);
			this->Controls->Add(this->StatusBar);
			this->Controls->Add(this->CoolerTab);
			this->Controls->Add(this->BodyTemp);
			this->Controls->Add(this->CcdTemp);
			this->Controls->Add(this->PowerSupply);
			this->Controls->Add(this->CameraStatus);
			this->Controls->Add(this->CameraCombo);
			this->Controls->Add(this->CalibrateButton);
			this->Controls->Add(this->StartButton);
			this->Controls->Add(this->StopButton);
			this->Icon = (cli::safe_cast<System::Drawing::Icon^>(resources->GetObject(L"$this.Icon")));
			this->Margin = System::Windows::Forms::Padding(3, 2, 3, 2);
			this->Name = L"MyForm";
			this->Text = L"VC++ 2012 sample";
			this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &MyForm::SampleClose);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->PictureImage))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->ShutterTime))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->statusBarInfo))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->statusBarStamp))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->statusBarFirst))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->statusBarBack))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->statusBarCounter))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->statusBarPlay))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->statusBarLast))->EndInit();
			this->CoolerTab->ResumeLayout(false);
			this->tabPage1->ResumeLayout(false);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->CoolerPower))->EndInit();
			this->tabPage2->ResumeLayout(false);
			this->tabPage2->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->errorProvider))->EndInit();
			this->groupBox1->ResumeLayout(false);
			this->groupBox1->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->ZoomTrackBar))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->DelayUpDown))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureImage2))->EndInit();
			this->groupBox2->ResumeLayout(false);
			this->groupBox3->ResumeLayout(false);
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion

        // The definition of common use data
        static unsigned short* pImageData = NULL;
        static bool StartFlag, TimerBusy = false;
        static int ImageNumber, ImageCount = 0;

        private: String^ GetResourcesString(int id)
        {
            switch (id)
            {
                case IDS_Celsius:
                    return " C";
                case IDS_Failure:
                    return "Failure";
                case IDS_TempFormat:
                    return "{0:F1}C";
                case IDS_VoltFormat:
                    return "{0:F1}V";
                case IDS_PowerFormat:
                    return "Cooler Power {0:D}";
                case IDS_Stop:
                    return "Stop";
                case IDS_FanWait:
                    return "The camera stops a fan";
                case IDS_MsFormat:
                    return "{0:D}ms";
                case IDS_FpsFormat:
                    return "Average {0:F1}fps";
                case IDS_ExposeError:
                    return "Failure of the exposure";
                case IDS_Message:
                    return "The camera prepares for exposure";
                case IDS_AcuisError:
                    return "Failure of the image acquisition";
				case IDS_CounterFormat:
					return "{0:D} / {1:D}";
                case IDS_Complete:
                    return "Exposure was completed";
            }
                            
            return "";
        }
        

        /////////////////////////////////////////////////////////////////////////////
        // This function starts a camera and communication
        private: System::Void CameraConnect_Click(System::Object^  sender, System::EventArgs^  e)
        {
			
			StartFlag = false;
            CameraConnect->Enabled = false;

            int ImageWidth, ImageHeight;
            int result = BitranCCDlibCreate(NULL, &ImageWidth, &ImageHeight);
            if (result == 1)
            {   // Get the information of the camera
                char str1[256], str2[256];
                int lng1 = BitranCCDlibCameraInfo(0, str1, sizeof(str1));//CCDやカメラの情報を得る：0→センサ情報、1→カメラ情報
                int lng2 = BitranCCDlibCameraInfo(1, str2, sizeof(str2));

                if ((lng1 > 0) && (lng2 > 0))
                {   // Success:ここでボタンを明るくする
                    strcat_s(str2, sizeof(str2), " ");
                    strcat_s(str2, sizeof(str2), str1);
                    CameraStatus->Text = gcnew String(str2);
                    CameraStatus->Text = CameraStatus->Text->Replace("\r\n", " ");

                    CameraDisconnect->Enabled = true;
                    PowerButton->Enabled = true;
                    CoolerButton->Enabled = true;
                    CoolerTimer->Enabled = true;
                    StartButton->Enabled = true;
					CalibrateButton->Enabled = true;

					button9->Enabled = true;
					button11->Enabled = true;

					errorProvider->Clear();
                }
                else
                {
                    errorProvider->SetError(ShutterTime, GetResourcesString(IDS_Failure));
                    CameraConnect->Enabled = true;
                }
            }
            else
            {
                errorProvider->SetError(CameraConnect, result.ToString());
                CameraConnect->Enabled = true;
            }
        }


        /////////////////////////////////////////////////////////////////////////////
        // This function releases the connection of the camera
        private: System::Void CameraDisconnect_Click(System::Object^  sender, System::EventArgs^  e)
        {
            // Calls of communicating functions should not overlap
            CoolerTimer->Stop();
            while (TimerBusy);

            PowerButton->Enabled = false;
            CoolerButton->Enabled = false;
            CoolerTimer->Enabled = false;
			StartButton->Enabled = false;
			CalibrateButton->Enabled = false;
			CameraDisconnect->Enabled = false;

            ControlBox = false;
            BitranCCDlibDestroy();//通信終了と接続解除
            ControlBox = true;
            CameraConnect->Enabled = true;
        }

        /////////////////////////////////////////////////////////////////////////////
        // You must release connection by all means before finishing software
        private: System::Void SampleClose(System::Object^  sender, System::Windows::Forms::FormClosingEventArgs^  e)
        {
            if (CoolerTimer->Enabled)
                CameraDisconnect_Click(sender, e);
        }

        /////////////////////////////////////////////////////////////////////////////
        // This function outputs cooling power
        private: System::Void PowerButton_Click(System::Object^  sender, System::EventArgs^  e)
        {
            // If you give priority to power, stop temperature control
            BitranCCDlibEnvironment(8, 8);//環境設定
            BitranCCDlibSetCoolerPower((int)CoolerPower->Value);//冷却パワー出力
        }

        /////////////////////////////////////////////////////////////////////////////
        // This function executes setting and a stop of the temperature control
        private: System::Void CoolerButton_Click(System::Object^  sender, System::EventArgs^  e)
        {
            errorProvider->Clear();
            try
            {
                int find = CoolerTemp->Text->IndexOf(GetResourcesString(IDS_Celsius));
                if (find > 0)
                    BitranCCDlibSetTemperatue(         // Setting of the control：冷却温度指定
                        (int)(Convert::ToDouble(CoolerTemp->Text->Substring(0, find)) * 10.0));
            }
            catch (FormatException^) 
            {
                errorProvider->SetError(CoolerTemp, GetResourcesString(IDS_Failure));
            }
        }

        /////////////////////////////////////////////////////////////////////////////
        // This function updates temperature and the voltage regularly
        private: System::Void CoolerTimer_Tick(System::Object^  sender, System::EventArgs^  e)
        {
            if (TimerBusy) return;
            TimerBusy = true;

            // The acquisition of the CCD temperature
            int temp = BitranCCDlibGetTemperatue(0);//温度読み込み:0→センサ温度、1→筐体温度
            if (temp != ILLEGAL_VALUE)
                CcdTemp->Text = String::Format(GetResourcesString(IDS_TempFormat), (double)temp / 10.0);

            // The acquisition of the ambient temperature
            temp = BitranCCDlibGetTemperatue(1);
            if (temp != ILLEGAL_VALUE)
                BodyTemp->Text = String::Format(GetResourcesString(IDS_TempFormat), (double)temp / 10.0);

            // The acquisition of the power supply voltage
            temp = BitranCCDlibGetVoltage();//引数無し
            if (temp != ILLEGAL_VALUE)
                PowerSupply->Text = String::Format(GetResourcesString(IDS_VoltFormat), (double)temp / 10.0);

            // The acquisition of the cooling power value
            temp = BitranCCDlibGetCoolerPower();//冷却パワー読み込み:引数無し
            if (temp != ILLEGAL_VALUE)
            {
                PowerProgress->Value = temp;
                PowerLabel->Text = String::Format(GetResourcesString(IDS_PowerFormat), temp);
                CoolerPowerToolTip->SetToolTip(PowerProgress, PowerLabel->Text);
            }

            TimerBusy = false;
        }

        /////////////////////////////////////////////////////////////////////////////
        // This function changes the central mode and trigger mode
        private: System::Void TreeCheck(System::Object^ sender, System::Windows::Forms::TreeViewEventArgs^  e)
        {
            if (e->Node == TREEVIEW_RECORD)
            {
                TREEVIEW_CONTINUE->Checked = TREEVIEW_RECORD->Checked;
                TREEVIEW_CSVMODE->ForeColor = Color::AntiqueWhite;
            }
            
            TREEVIEW_CSVMODE->ForeColor = TREEVIEW_CONTINUE->Checked ? Color::AntiqueWhite : Color::Black;
            TREEVIEW_CONVERT->ForeColor = (TREEVIEW_CONTINUE->Checked && !TREEVIEW_RECORD->Checked) ? Color::AntiqueWhite : Color::Black;
        }

        /////////////////////////////////////////////////////////////////////////////
        // This function shows a gain or hides it
        private: System::Void TreeSelect(System::Object^  sender, System::Windows::Forms::TreeViewEventArgs^  e)
        {
            CountBox->Visible = e->Node == TREEVIEW_RECORD;
            CountLabel->Visible = CountBox->Visible;
            ConvertRadio1->Visible = e->Node == TREEVIEW_CONVERT;
            ConvertRadio2->Visible = ConvertRadio1->Visible;
            ConvertRadio3->Visible = ConvertRadio1->Visible;
            DelayUpDown->Visible = e->Node == TREEVIEW_FANSTOP;
            DelayLabel->Visible = DelayUpDown->Visible;
            DelayMs->Visible = DelayUpDown->Visible;
        }

        /////////////////////////////////////////////////////////////////////////////
        // This function starts exposure and shows an image
        private: System::Void StartButton_Click(System::Object^  sender, System::EventArgs^  e)// exposu
        {
            StartFlag = true;
            CameraDisconnect->Enabled = false;
            StartButton->Enabled = false;
			StopButton->Enabled = true;
			CalibrateButton->Enabled = false;
			ImageNumber = 1;
            ImageCount = 0;

            // A check and the acquisition of the exposure condition
            int time, count, fan;
            if (StartCheck(time, count, fan))
            {
                // Calls of communicating functions should not overlap
                CoolerTimer->Stop();
                while (TimerBusy);

			    // Sample of the exposure method
			    switch (count)
			    {
			        // Basic exposure method
			        case 1:
                        time = SnapMode(time);
                        if (time == 0)
                        {   // error
                            statusBarInfo->Text = GetResourcesString(IDS_ExposeError);
                        }
                        else
                        {   // success
                            statusBarInfo->Text = String::Format(GetResourcesString(IDS_MsFormat), time);
			                ImageConvert();

                            if (TREEVIEW_CSVMODE->Checked)
			                    SaveCSV();
			            }
			            break;

                    // Exposure method for the focus adjustment
                    case 0:
						if (startCalibration) {
							std::cout << "Calibration_Impl\n";
							statusBarInfo->Text = String::Format(GetResourcesString(IDS_FpsFormat), Calibration_Impl());
						}
						else {
							std::cout << "ContinueMide\n";
							statusBarInfo->Text = String::Format(GetResourcesString(IDS_FpsFormat), ContinueMode());
						}
                        break;
                    // Record exposure repeatedly fast
                    default:
                        ImageNumber = ImageCount = RecordingMode(time, count);
                }
                
                // Restart of temperature acquisition
                CoolerTimer->Start();
            }

            StartFlag = false;
            CameraDisconnect->Enabled = true;
            StartButton->Enabled = true;
			StopButton->Enabled = false;
			//CalibrateButton->Enabled = false;
			CalibrateButton->Enabled = true;
			PictureImage->Enabled = true;
        }

        /////////////////////////////////////////////////////////////////////////////
        // The confirmation of the exposure condition
        private: bool StartCheck(int& time, int& count, int& fan)
        {
            errorProvider->Clear();

            // Exposure time
            try
            {
                time = Convert::ToInt32(ShutterTime->Text);
            }
            catch (FormatException^)
            {
                errorProvider->SetError(ShutterTime, GetResourcesString(IDS_Failure));
                return false;
            }

            // Exposure mode
            try
            {
                if (TREEVIEW_CONTINUE->Checked)
                {
                    if (TREEVIEW_RECORD->Checked)
                    {   // Record Mode
                        count = Convert::ToInt32(CountBox->Text);
                    }
                    else
                    {   // Continue Mode
                        count = 0;
                    }
                }
                else
                {       
					// One Shot
                    count = 1;
                }
            }
            catch (FormatException^) 
            {
                errorProvider->SetError(CountBox, GetResourcesString(IDS_Failure));
                return false;
            }

            // The fan does not stop quickly.Therefore you had better wait a little
			if (TREEVIEW_FANSTOP->Checked)
			{
			    // I get ready to operate a fan here
				BitranCCDlibEnvironment(1, 1);
				statusBarInfo->Text = GetResourcesString(IDS_FanWait);
				System::Threading::Thread::Sleep((int)DelayUpDown->Value * 1000);
			}
			
            return true;
        }

        /////////////////////////////////////////////////////////////////////////////
        // Basic exposure and image transformation
        private: int SnapMode(int time)
        {
            // A start of the exposure
            statusBarInfo->Text = GetResourcesString(IDS_Message);
            HBITMAP result = BitranCCDlibStartExposure(time, 0, 0, 0, 0, 0, 1);//撮影開始
            if (result == 0)
                return 0;
            
            // The acquisition of the number of the pixels and security of the image storage array
            if (pImageData)
                delete[] pImageData;
            pImageData = new unsigned short[128 * 128];
            
            // You must wait until exposure is finished
            int status = BitranCCDlibCameraState(1);//カメラ状態の取得：0→撮影枚数を返す、1→撮影が完了するまで待つ
            if (status != 1)
            {
                // Failure
                BitranCCDlibFinishExposure(-1);//引数は-1にする
                return 0;
            }

            // You acquire an image
            int lng = BitranCCDlibTransferImage(1, 1, pImageData);//画像の読み込み:読み込み開始コマ数、読み込み枚数
            if (lng != 128 * 128)
            {
                // Failure
                BitranCCDlibFinishExposure(-1);
                return 0;
            }

            BitranCCDlibImageInterpolation(pImageData);//センサの欠陥情報を基に画像データを補完する
            
            // You notify a camera that a sequence was over
            return BitranCCDlibFinishExposure(-1);
        }


        /////////////////////////////////////////////////////////////////////////////
        //  Consecutive exposure:連続撮影
		//private: double ContinueMode()
		double ContinueMode()
		{
			cv::FileStorage fs("parameter\\instrinsics_Nir.xml", cv::FileStorage::READ);
			if (!fs.isOpened()) {
				std::cout << "File can not be opened." << std::endl;
				return -1;
			};
			cv::Mat _camera_mtx, _camera_dist;
			fs["camera_matrix"] >> _camera_mtx;
			fs["distortion_coefficients"] >> _camera_dist;
			fs.release();

			//System::Console::WriteLine("mtx rcols = {0}", _camera_mtx.cols);
			
			std::cout << "camera_mtx=" << _camera_mtx << std::endl;
			std::cout <<"camera_dist="<< _camera_dist << std::endl;

			// A start of the exposure
            HBITMAP hBmp = BitranCCDlibStartExposure( Convert::ToInt32(ShutterTime->Text), 1, 0, 0, 0, 0, 0 );//撮影開始
            if (hBmp == 0)
                return 0;

			cv::Size image_size{128,128};
			std::cout << "image size=" << image_size << std::endl;
			
			//後続フレームに使う歪み補正マップを作成する
			cv::Mat map1, map2;
			cv::Mat mapR;// = cv::Mat::eye(3, 3, CV_64F);
			cv::initUndistortRectifyMap(
					_camera_mtx,//3*3のカメラ行列
					_camera_dist,//4,5,または8個の歪み係数
					mapR,//R,平行化変換（3*3）
					_camera_mtx,//新しいカメラ行列
					image_size,//歪み補正された画像のサイズ
					CV_16SC2,//map1の型；cv_16sc2,cv_32FC1,cv_32C2
					map1,//最初の出力マップ
					map2//二番目の出力マップ
					);
			
			// You acquire an image, and display it
            int count = 1;
			for (; StartFlag && hBmp; count++)
			{
				//ここでopencv処理
				cv::Mat mat = cvwin::BitmapToMat(hBmp);
			
				cv::Mat distImg;
				//cv::undistort( mat, distImg, _camera_mtx, _camera_dist);
				//cv::remap(mat, distImg, map1, map2, cv::INTER_LINEAR, cv::BORDER_CONSTANT, cv::Scalar());
				cv::remap(mat, distImg, map1, map2,cv::INTER_LANCZOS4);

				System::Drawing::Bitmap^ tmpImg = gcnew System::Drawing::Bitmap(distImg.cols, distImg.rows, distImg.step, System::Drawing::Imaging::PixelFormat::Format24bppRgb, (IntPtr)distImg.data);
				//System::Drawing::Bitmap^ tmpImg = gcnew System::Drawing::Bitmap(mat.cols, mat.rows, mat.step, System::Drawing::Imaging::PixelFormat::Format24bppRgb, (IntPtr)mat.data);
				System::Drawing::Bitmap^ showImg = gcnew System::Drawing::Bitmap(tmpImg);
				PictureImage->Image = showImg;

				// pattern 1
				//PictureImage->Image = Bitmap::FromHbitmap((IntPtr)hBmp);//PictureImageに描画
				// pattern 2
				//Bitmap^ image = Bitmap::FromHbitmap((IntPtr)hBmp);
				//PictureImage->Image = image;
				
				DeleteObject(hBmp);
                hBmp = BitranCCDlibStartExposure(Convert::ToInt32(ShutterTime->Text), 2, 0, 0, 0, 0, 0);
			}
			
			return 1000.0 / BitranCCDlibFinishExposure(-1) * count;
        }

        /////////////////////////////////////////////////////////////////////////////
        //  Record it in the memory of the camera
        private: int RecordingMode(int time, int count)
        {
            if (BitranCCDlibStartExposure(time, 0, 0, 0, 0, 0, count) == 0)
            {   // failure
                StartFlag = false;		
                statusBarInfo->Text = GetResourcesString(IDS_ExposeError);
            }
            else
                statusBarInfo->Text = "";

 			// It is recorded the memory of the camera
            int getnum = 0, newnum = 0;
			while (StartFlag && (newnum < count))
			{
				newnum = BitranCCDlibCameraState();
                if (newnum < 0)
                {
                    StartFlag = false;  // The camera is an error
                    statusBarInfo->Text = GetResourcesString(IDS_AcuisError);
                }
                else
				if (getnum != newnum)
				{
					getnum = newnum;
					statusBarCounter->Text = String::Format(GetResourcesString(IDS_CounterFormat), newnum, count);
				}
			}
            
            int id;
            if (!StartFlag)
            {   // It was canceled
                BitranCCDlibAbortExposure();
            }

            if (getnum > 0)
            {   // It stores all images
                if (pImageData)
                    delete[] pImageData;
                pImageData = new unsigned short[128 * 128 * getnum];
                BitranCCDlibTransferImage(1, getnum, pImageData);
                BitranCCDlibImageInterpolation(pImageData);
                id = IDS_Complete;
            }
            
            // You notify a camera that a sequence was over
            statusBarInfo->Text = GetResourcesString(id);

            BitranCCDlibFinishExposure();
            return getnum;
        }

        /////////////////////////////////////////////////////////////////////////////
        // Display of the image
        private: void ImageConvert()
        {
            unsigned int ConvData[4];
                
            if (TREEVIEW_CONVERT->Checked)
            {
                ConvData[0] = 4;
                ConvData[1] = ConvertRadio2->Checked;
                ConvData[2] = 65535;
                ConvData[3] = 0;
                
                int lng = 128 * 128;
                for (int i = 0; i < lng; i++)
                {
                    if (ConvData[2] > pImageData[i])
                        ConvData[2] = pImageData[i];
                    if (ConvData[3] < pImageData[i])
                        ConvData[3] = pImageData[i];
                }

                if (ConvertRadio3->Checked)
                {
                    ConvData[3] /= 2;
                    if (ConvData[3] <= ConvData[2])
                        ConvData[3] = ConvData[2] + 100;
                }
            }
            else
            {
                ConvData[0] = 1;
            }
            
			GC::Collect();
            GC::WaitForPendingFinalizers();
            GC::Collect();
            HBITMAP hBmp = BitranCCDlibImageConvert(ConvData, &pImageData[128 * 128 * (ImageNumber - 1)]);//撮像データからビットマップを生成
			Bitmap^ image = Bitmap::FromHbitmap((IntPtr)hBmp);
			
			//----------------------------------------------------------------opencv
			//cv::undistort(image,dist_img,cam_mat,dist_coefs);
			//PictureImage->Image = dist_img;
			PictureImage->Image = image;
			//PictureImage->Image = Bitmap::FromHbitmap((IntPtr)hBmp);//PictureImageに描画
            DeleteObject(hBmp);
        }
        

        /////////////////////////////////////////////////////////////////////////////
        // Cancellation of trigger and the exposure
        private: System::Void StopButton_Click(System::Object^  sender, System::EventArgs^  e)
        {
            StartFlag = false;
            statusBarInfo->Text = GetResourcesString(IDS_Stop);
        }

        /////////////////////////////////////////////////////////////////////////////
        // Change of the zoom size
        private: System::Void Zoom_Changed(System::Object^  sender, System::EventArgs^  e)
        {
            int size = ZoomTrackBar->Value * 128 / 100;
            PictureImage->Width=size;
            PictureImage->Height=size;
        }

        /////////////////////////////////////////////////////////////////////////////
        // Read the image which you recorded
        private: System::Void StatusBar_PanelClick(System::Object^  sender, System::Windows::Forms::StatusBarPanelClickEventArgs^  e)
        {
            if (!StartFlag && ImageCount > 1)
            {
                if (e->StatusBarPanel == statusBarFirst) 
                {
                    ImageNumber=1;
					ImageConvert();
					statusBarCounter->Text = String::Format(GetResourcesString(IDS_CounterFormat), ImageNumber, ImageCount);
                }
                else
                if (e->StatusBarPanel == statusBarBack) 
                {
                    if (ImageNumber > 1)
                    {
                        ImageNumber--;
			            ImageConvert();
					    statusBarCounter->Text = String::Format(GetResourcesString(IDS_CounterFormat), ImageNumber, ImageCount);
                    }
                }
                else
                if (e->StatusBarPanel == statusBarPlay) 
                {
                    if (ImageNumber < ImageCount)
                    {
                        ImageNumber++;
			            ImageConvert();
					    statusBarCounter->Text = String::Format(GetResourcesString(IDS_CounterFormat), ImageNumber, ImageCount);
                    }
                }
                else
                if (e->StatusBarPanel == statusBarLast) 
                {
                    ImageNumber=ImageCount;
			        ImageConvert();
				    statusBarCounter->Text = String::Format(GetResourcesString(IDS_CounterFormat), ImageNumber, ImageCount);
                }
            }
        }

        /////////////////////////////////////////////////////////////////////////////
        // If you want to store an image in a file, you may use this function
        private: System::Void PictureImage_Click(System::Object^  sender, System::EventArgs^  e)
        {
            if (!StartFlag)
            {
                if (SaveFileDialog->ShowDialog() == System::Windows::Forms::DialogResult::OK)
                {
                    marshal_context ^ context = gcnew marshal_context();
                    const char* str = context->marshal_as<const char*>(SaveFileDialog->FileName);
                    if (BitranCCDlibImageSave(SaveFileDialog->FilterIndex, (char*)str, pImageData) == 0)
                        statusBarInfo->Text = GetResourcesString(IDS_Failure);
                    delete context;
                }
            }
        }

        /////////////////////////////////////////////////////////////////////////////
        // If you want to store an image in a file, you may use this function
        private: void SaveCSV()
        {
            if (BitranCCDlibImageSave(4, "sample.csv", pImageData) == 0)
                statusBarInfo->Text = GetResourcesString(IDS_Failure);
            else
                CsvProcess->Start("sample.csv");
        }
	
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		
		private: double Calibration_Impl()
		{
			std::cout << "calibtation_Impl\n";
			startCalibration = false;
			
			//格納場所を確保
			std::vector< std::vector<cv::Point2f> >image_points;
			std::vector< std::vector<cv::Point3f> >object_points;

			double last_captured_timestamp=0;
			int n_boards = 20;
			int CLOCKS_PER_SEC;
			float image_sf = 0.5f;
			int board_w = 9;
			int board_h = 6;
			int board_n = board_w * board_h;

			// A start of the exposure
			HBITMAP hBmp = BitranCCDlibStartExposure(Convert::ToInt32(ShutterTime->Text), 1, 0, 0, 0, 0, 0);//撮影開始
			if (hBmp == 0)
				return 0;

			cv::Size image_size{ 128,128 };
			System::Console::WriteLine("=== image size: ({0}, {1}) ===", image_size.width, image_size.height);

			// You acquire an image, and display it
			int count = 1;
			int n = 0;
			//for (; StartFlag && hBmp; count++)
			while (image_points.size() < (size_t)n_boards)
			{
				//ここでopencv処理
				cv::Mat mat = cvwin::BitmapToMat(hBmp);
				cv::Mat image0, image;

				//ボードを探す
				cv::Size board_sz = cv::Size(board_w, board_h);
				std:: vector<cv::Point2f>corners;
				bool found = cv::findChessboardCorners(mat, board_sz, corners);
				drawChessboardCorners(mat, board_sz, corners, found);
				double timestamp = (double)clock() / CLOCKS_PER_SEC;

				if (found && timestamp - last_captured_timestamp > 1) {
					last_captured_timestamp = timestamp;
					image ^= cv::Scalar::all(255);

					cv::Mat mcorners(corners);
					mcorners *= (1. / image_sf);
					image_points.push_back(corners);
					object_points.push_back(std::vector<cv::Point3f>());
					std::vector<cv::Point3f>& opts = object_points.back();
					opts.resize(board_n);

					for (int j = 0; j < board_n; j++) {
						opts[j] = cv::Point3f((float)(j / board_w), (float)(j%board_w), 0.f);
					}
					std::cout << "collected our" << (int)image_points.size() << "of" << n_boards << "needed chessboard images\n" << std:: endl;
					cv::imwrite( ".\\calibImgs\\nir\\"+std::to_string(n) + ".png", mat);
					n++;
				}

				System::Drawing::Bitmap^ tmpImg = gcnew System::Drawing::Bitmap(mat.cols, mat.rows, mat.step, System::Drawing::Imaging::PixelFormat::Format24bppRgb, (IntPtr)mat.data);
				System::Drawing::Bitmap^ showImg = gcnew System::Drawing::Bitmap(tmpImg);
				PictureImage->Image = showImg;

				DeleteObject(hBmp);
				hBmp = BitranCCDlibStartExposure(Convert::ToInt32(ShutterTime->Text), 2, 0, 0, 0, 0, 0);

				if ((cv::waitKey(30) & 255) == 27) {
					return -1;
				}

			}// 画像の取り組みループ終わり
			std::cout << "finish roop" << std:: endl;

			//格子点の位置をメートルに変換
			float Size = 0.0271;
			for (int i = 0; i < object_points.size(); i++) {
				for (int i2 = 0; i2 < board_n; i2++) {
					object_points[i][i2].x = object_points[i][i2].x*Size;
					object_points[i][i2].y = object_points[i][i2].y*Size;
				}
			}

			for (int n = 0; n < n_boards;n++) {
				std::cout << "image_points =" << image_points[n] << std::endl;
				std::cout << "object_points =" << object_points[n] << std::endl;
			}

			//カメラキャリブレーション
			cv::Mat camera_matrix, distortion_coeffs;
			std::vector<cv::Mat>r, t;
			double err = cv::calibrateCamera(
				object_points,image_points,image_size,camera_matrix,distortion_coeffs,r,t,cv::CALIB_ZERO_TANGENT_DIST | cv::CALIB_FIX_PRINCIPAL_POINT
			);
			std::cout << "calibrate" << std::endl;

			std::cout << "image_width" << image_size.width << std::endl;
			std::cout << "image_height" << image_size.height << std::endl;
			std::cout << "camera_matrix" << camera_matrix << std::endl;
			std::cout << "distortion_coefficients" << distortion_coeffs << std::endl;
			std::cout <<"r"<<r[0] << std::endl;
			std::cout << "t" << t[0] << std::endl;
			std::cout << "t[0].at<double>(0,0) = " << t[0].at<double>(0, 0) << std::endl;
			std::cout << "t[0].at<double>(1,0) = " << t[0].at<double>(1, 0) << std::endl;
			std::cout << "t[0].at<double>(2,0) = " << t[0].at<double>(2, 0) << std::endl;

			//内部パラメータと歪み係数を保存
			cv::FileStorage fs("parameter\\instrinsics_nir.xml", cv::FileStorage::WRITE);
			cv::write(fs, "image_width", image_size.width);
			cv::write(fs, "image_height", image_size.height);
			cv::write(fs, "camera_matrix", camera_matrix);
			cv::write(fs, "distortion_coeffs", distortion_coeffs);
			cv::write(fs, "r", r);
			cv::write(fs, "t", t);
			fs.release();
			std::cout << "end" << std::endl;
			


			for (int n = 0; n < n_boards; n++) {
				double T = pow(t[n].at<double>(0,0), 2) + pow(t[n].at<double>(1, 0), 2) + pow(t[n].at<double>(2, 0), 2);
				T = sqrt( T );
				std::cout << "T = " << T << std::endl;
			}
			
		return 1000.0 / BitranCCDlibFinishExposure(-1) * count;
		}


		//////////////////////////////////// calibration button /////////////////////////////////////////
		private: System::Void ContinueMode2(System::Object^  sender, System::EventArgs^  e)
		{
			System::Console::WriteLine("calibration start");
			startCalibration = true;//----------------------------自分で定義したメンバ変数
			StartButton_Click(sender, e);//-----------------------exposuのボタン
			System::Console::WriteLine("calibration end");
		}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		
		private: double usb_cam() {
			cv::VideoCapture cap(0);
			if (!cap.isOpened())  // 成功したかどうかをチェック
				return -1;

			for (;;)
			{
				cv::Mat frame;
				cap >> frame; // カメラから新しいフレームを取得
				imshow("edges", frame);
				System::Drawing::Bitmap^ tmpImg = gcnew System::Drawing::Bitmap(frame.cols, frame.rows, frame.step, System::Drawing::Imaging::PixelFormat::Format24bppRgb, (IntPtr)frame.data);
				//System::Drawing::Bitmap^ tmpImg = gcnew System::Drawing::Bitmap(edges.cols, edges.rows, edges.step, System::Drawing::Imaging::PixelFormat::Format24bppRgb, (IntPtr)edges.data);
				System::Drawing::Bitmap^ showImg2 = gcnew System::Drawing::Bitmap(tmpImg);
				pictureImage2->Image = showImg2;
				PictureImage->Image = showImg2;
				if (cv::waitKey(30) >= 0) break;


			}
			return 0;
		}

		private: double usb_cam2() {
	cv::VideoCapture cap(0);
	if (!cap.isOpened())  // 成功したかどうかをチェック
		return -1;
	char key;
	int key2;
	for (;;)
	{
		cv::Mat frame;
		cap >> frame; // カメラから新しいフレームを取得
		imshow("edges", frame);
		//cv::waitKey(1);
		
		std::string name;
		std::cout << "あなたの名前を教えて下さい";
		getline(std::cin, name);
		
		key = cv::waitKey(0);

		if (name == "s")
		{
			std::cout << "kita----!!!" << std::endl;
		}

		if (name == "q") break;

	}
	return 0;
}

		private: System::Void button1_Click(System::Object^  sender, System::EventArgs^  e) {
			statusBarInfo->Text = String::Format(GetResourcesString(IDS_FpsFormat), usb_cam2());
		}

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		private: double rsExposure() {
			rs2::pipeline pipe;
			rs2::pipeline_profile profile;
			rs2::config cfg;
			cfg.enable_stream(RS2_STREAM_COLOR, 1280, 720, RS2_FORMAT_BGR8, 30); // RGB
			cfg.enable_stream(RS2_STREAM_DEPTH, 1280, 720, RS2_FORMAT_Z16, 30); // Depth
			profile = pipe.start(cfg);
			rs2::frameset frames;
			rs2::points points;
			rs2::pointcloud pc;
			auto depth_st = profile.get_stream(RS2_STREAM_COLOR).as<rs2::video_stream_profile>();
			auto depth_intr = depth_st.get_intrinsics();
			rs2::align align_to(RS2_STREAM_COLOR);

			//fileter
			rs2::decimation_filter dec_filter;
			dec_filter.set_option(RS2_OPTION_FILTER_MAGNITUDE, 3);
			rs2::disparity_transform depth_to_disparity(true);
			rs2::disparity_transform disparity_to_depth(false);
			rs2::spatial_filter spat_filter;
			spat_filter.set_option(RS2_OPTION_FILTER_MAGNITUDE, 2);
			spat_filter.set_option(RS2_OPTION_FILTER_SMOOTH_ALPHA, 0.5);
			spat_filter.set_option(RS2_OPTION_FILTER_SMOOTH_DELTA, 20);
			spat_filter.set_option(RS2_OPTION_HOLES_FILL, 0);
			rs2::temporal_filter temp_filter;
			temp_filter.set_option(RS2_OPTION_FILTER_SMOOTH_ALPHA, 0.4);
			temp_filter.set_option(RS2_OPTION_FILTER_SMOOTH_DELTA, 20);
			temp_filter.set_option(RS2_OPTION_HOLES_FILL, 3);
			rs2::hole_filling_filter hf_filter;
			hf_filter.set_option(RS2_OPTION_HOLES_FILL, 1);

			while (true)
			{
				if (pipe.poll_for_frames(&frames))
				{
					auto frame = pipe.wait_for_frames();

					frame = align_to.process(frame);

					auto depth_frame = frame.get_depth_frame();
					auto color_frame = frame.get_color_frame();

					//fileter
					depth_frame = dec_filter.process(depth_frame);
					depth_frame = depth_to_disparity.process(depth_frame);
					depth_frame = spat_filter.process(depth_frame);
					depth_frame = temp_filter.process(depth_frame);
					depth_frame = disparity_to_depth.process(depth_frame);
					depth_frame = hf_filter.process(depth_frame);

					std::cout << "2=" << std::endl;

					cv::Mat rs = cv::Mat(cv::Size(color_frame.get_width(), color_frame.get_height()), CV_8UC3, const_cast<void*>(color_frame.get_data()));
					cv::Mat ir(cv::Size(depth_frame.get_width(), depth_frame.get_height()), CV_16SC1, (void *)depth_frame.get_data(), cv::Mat::AUTO_STEP);
					std::cout << color_frame.get_width() << "　" << color_frame.get_height() << std::endl;
					std::cout << "rs.step=" << rs.step << std::endl;
					std::cout << "rs.elemSize1()=" << rs.elemSize1() << std::endl;
					std::cout << depth_frame.get_width() << "　" << depth_frame.get_height() << std::endl;
					std::cout << "ir.step=" << ir.step << std::endl;
					std::cout << "ir.elemSize1()=" << ir.elemSize1() << std::endl;

					cv::cvtColor(rs, rs, CV_RGB2BGR);
					ir.convertTo(ir, CV_8U, -255.0 / 10000.0, 255.0);// 行列をスケーリングして別のデータ型に変換します
					cv::equalizeHist(ir, ir);//グレースケール画像のヒストグラムを均一化します．
					cv::applyColorMap(ir, ir, cv::COLORMAP_JET);


					cv::imshow("depthFlame", ir);
					//cv::imwrite("for_tex\\howaie\\ir.png", ir);
					cv::imwrite("pet\\ir.png", ir);

					cv::imshow("rs", rs);
					//cv::imwrite("for_tex\\howaie\\rs.png", rs);
					cv::imwrite("pet\\rs.png", rs);

					cv::Mat rsGray;
					cvtColor(rs, rsGray, CV_RGB2GRAY);
					cv::imshow("rsGray", rsGray);
					//cv::imwrite("for_tex\\howaie\\rsGray.png", rsGray);
					cv::imwrite("pet\\rsGray.png", rsGray);

				}
			}






		}


		private: System::Void button2_Click(System::Object^  sender, System::EventArgs^  e) {//realsense
			rsExposure3();
		}
		

		private: System::Void button3_Click(System::Object^  sender, System::EventArgs^  e) {// rsGetPC
			rsGetPointCloud();
		}

		private: System::Void button6_Click(System::Object^  sender, System::EventArgs^  e) {// rsCalib
			int n = 0;
			rsCalib(n);
			n = 1;
			rsCalib(n);
		}


		
		
		
		






		////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		double testNir() {
			startCalibration = false;
			HBITMAP hBmp = BitranCCDlibStartExposure(Convert::ToInt32(ShutterTime->Text), 1, 0, 0, 0, 0, 0);//撮影開始
			if (hBmp == 0)
				return 0;
			cv::Size image_size{ 128,128 };

			int count = 1;
			//for (; StartFlag && hBmp; count++)
			while (true)
			{
				//ここでopencv処理
				cv::Mat mat = cvwin::BitmapToMat(hBmp);
				cv::Mat grayMat;
				cvtColor(mat, grayMat, CV_RGB2GRAY);

				cv::namedWindow("grayMat", CV_WINDOW_NORMAL);
				cv::imshow("grayMat", grayMat);

				System::Drawing::Bitmap^ tmpImg = gcnew System::Drawing::Bitmap(mat.cols, mat.rows, mat.step, System::Drawing::Imaging::PixelFormat::Format24bppRgb, (IntPtr)mat.data);
				System::Drawing::Bitmap^ showImg = gcnew System::Drawing::Bitmap(tmpImg);
				PictureImage->Image = showImg;

				DeleteObject(hBmp);
				hBmp = BitranCCDlibStartExposure(Convert::ToInt32(ShutterTime->Text), 2, 0, 0, 0, 0, 0);

			}

		}



		private: System::Void button9_Click(System::Object^  sender, System::EventArgs^  e) {//convCoordiRs2Nir
			coordiTransRs2NirPanTilt2();
		}


		private: System::Void button10_Click(System::Object^  sender, System::EventArgs^  e) {
			std::string num;
			switch (comboBox2->SelectedIndex) {
			case 0:
				num = "1";
				removeToFar(num);
				break;
			case 1:
				num = "2";
				removeToFar(num);
				break;
			case 2:
				num = "3";
				removeToFar(num);
				break;
			case 3:
				num = "4";
				removeToFar(num);
				break;
			case 4:
				num = "5";
				removeToFar(num);
				break;
			}
		}





		private: System::Void button4_Click(System::Object^  sender, System::EventArgs^  e) {// test function
			int n = 0;
			//nitika();
			blockNoise();
			//calicTest();
			//kMeans(cv::Mat src_img);
		}

		//なにもなくていい？
		private: System::Void pictureBox1_Click(System::Object^  sender, System::EventArgs^  e) {
			
		}
	
		 int A = 0;
	private: System::Void ShutterTime_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
	}


private: System::Void button11_Click(System::Object^  sender, System::EventArgs^  e) {
	getNirPanTilt();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
		 /////////////////////////////////////////////////////////////////////////////////////////
		 /////////////////////////////////////////////////////////////////////////////////////////


private: System::Void CameraCombo_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
}


		 ///////////////////////      comboBox realsense       /////////////////////////////
private: System::Void comboBox1_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
}

private: System::Void button12_Click(System::Object^  sender, System::EventArgs^  e) {
	
	switch (comboBox1->SelectedIndex) {
	case 0:
		std::cout << "case 0\n";
		rsExposure3();
		break;
	case 1:
		std::cout << "case 1\n";
		rsCalib(0);
		break;
	case 2:
		std::cout << "case 2\n";
		rsGetPointCloud();
		break;
	}

}

private: System::Void groupBox3_Enter(System::Object^  sender, System::EventArgs^  e) {
}


/////////////////////////////////   estimate water    ////////////////////////////////
private: System::Void comboBox2_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
}

private: System::Void button2_Click_1(System::Object^  sender, System::EventArgs^  e) {
	std::string num;
	switch (comboBox2->SelectedIndex)
	{
	case 0:
		num = "1";
		estimateWater(num);
		break;
	case 1:
		num = "2";
		estimateWater(num);
		break;
	case 2:
		num = "3";
		estimateWater(num);
		break;
	case 3:
		num = "4";
		estimateWater(num);
		break;
	case 4:
		num = "5";
		estimateWater(num);
		break;
	case 5:
		num = "5";
		createHistgram();
		break;
	}


}



private: System::Void button3_Click_1(System::Object^  sender, System::EventArgs^  e) {
	std::string num;
	switch (comboBox2->SelectedIndex)
	{
	case 0:
		num = "1";
		cordiTransBagFile();
		break;
	case 1:
		num = "2";
		cordiTransBagFile();
		break;
	case 2:
		num = "3";
		cordiTransBagFile();
		break;
	case 3:
		num = "4";
		cordiTransBagFile();
		break;
	case 4:
		num = "5";
		cordiTransBagFile();
		break;
	}
}
};

	


}


