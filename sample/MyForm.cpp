//#include<opencv2/opencv.hpp>
//#include<opencv2/imgproc/imgproc.hpp>
#include "stdafx.h"
#include "MyForm.h"


using namespace System;
using namespace System::Windows::Forms;



[STAThreadAttribute]
int main(array<String^>^ args) {
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false);
	Application::Run(gcnew sample::MyForm());
	return 0;
}
