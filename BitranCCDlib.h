// BitranCCDlib.h : A main header file of BITRAN COOLED CCD CAMERA system order DLL
//

#define ILLEGAL_VALUE 10000

using namespace System::Runtime::InteropServices;
//typedef void* HWND;//NIR画像にOPENCVを使えるようにコメントアウト,BITRAN様の使いたくない
//typedef void* HBITMAP;
//typedef void* HINSTANCE;
[DllImport("Gdi32", CharSet=CharSet::Auto)]
//extern "C" int DeleteObject(HBITMAP);
int DeleteObject(HBITMAP);

[DllImport("BK50USBlib.dll")]
extern "C" int BitranCCDlibCreate(const char*, int*, int*);
[DllImport("BK50USBlib.dll")]
extern "C" void BitranCCDlibDestroy();
[DllImport("BK50USBlib.dll")]
extern "C" int BitranCCDlibCameraInfo(int,char*,int);
[DllImport("BK50USBlib.dll")]
extern "C" int BitranCCDlibGetStatus();
[DllImport("BK50USBlib.dll")]
extern "C" int BitranCCDlibGetVoltage();
[DllImport("BK50USBlib.dll")]
extern "C" void BitranCCDlibSetCoolerPower(int);
[DllImport("BK50USBlib.dll")]
extern "C" int BitranCCDlibGetCoolerPower();
[DllImport("BK50USBlib.dll")]
extern "C" void BitranCCDlibSetTemperatue(int);
[DllImport("BK50USBlib.dll")]
extern "C" int BitranCCDlibGetTemperatue(int);
[DllImport("BK50USBlib.dll")]
extern "C" int BitranCCDlibEnvironment(int flag, int mask);
[DllImport("BK50USBlib.dll")]
extern "C" HBITMAP BitranCCDlibStartExposure(int,int,int,int,int,int,int);/////
[DllImport("BK50USBlib.dll")]
extern "C" int BitranCCDlibCameraState(int = 0);
[DllImport("BK50USBlib.dll")]
extern "C" int BitranCCDlibAbortExposure(int = -5);
[DllImport("BK50USBlib.dll")]
extern "C" int BitranCCDlibFinishExposure(int = -1);
[DllImport("BK50USBlib.dll")]
extern "C" int BitranCCDlibTransferImage(int,int,unsigned short*);
[DllImport("BK50USBlib.dll")]
extern "C" void BitranCCDlibImageInterpolation(unsigned short*);
[DllImport("BK50USBlib.dll")]
extern "C" HBITMAP BitranCCDlibImageConvert(unsigned int*,unsigned short*); 
[DllImport("BK50USBlib.dll")]
extern "C" int BitranCCDlibImageSave(int, char*, unsigned short*);
