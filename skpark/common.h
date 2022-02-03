#pragma once

#include "afxmt.h"
#include "define.h"

unsigned long GetFileSize(LPCTSTR fullpath);
bool IsLocalExist(LPCTSTR fullpath);
CString	FindExtension(const CString& strFileName);
LPSTREAM LoadStreamFromFile(LPCTSTR szFileName, HGLOBAL& hGlobal);

void* LoadImageFromFileWithoutLocking(const WCHAR* fileName);
void* LoadImageFromFileWithoutLocking(const WCHAR* fileName, int left, int top, int width, int height);
int GetEncoderClsid(const WCHAR* format, CLSID* pClsid);
bool SaveImageFile(LPCTSTR thumbName, Gdiplus::Image* pThumbnail);

CString	GetExt(const CString& strFileName);
unsigned long getPid(LPCTSTR exename, bool likeCond = false);
extern BOOL CALLBACK find_hwnd_from_pid_proc(HWND hwnd, LPARAM lParam);
HWND getWHandle(unsigned long pid);
HWND getWHandle(LPCTSTR exename, bool likeCond = false);
bool HttpUploadFile(TCHAR * pszUrl, TCHAR * pszFilePath);
CString GetLocalIP(void);
bool GetValueFromJson(CString& targetJson, LPCTSTR name, LPCTSTR deli, bool isString, CString& outVal);
unsigned long createProcess(LPCTSTR exename, LPCTSTR arg, LPCTSTR dir, BOOL minFlag = FALSE);
void SetForegroundWindowForce(HWND hWnd);
int killProcess(unsigned long pid);
void OpenIExplorer(CString strParam, int cx=1024, int cy=768);
void showTaskbar(bool bShow);

typedef unsigned char U8;
typedef unsigned short int U16;
typedef unsigned long int U32;

typedef int YUV_FORMAT;
enum
{
	YUV_I420,
	YUV_YV12,
	YUV_NV12,
	YUV_NV21,
};

#define BITMAP_HEADER_TYPE   _T("BM")

typedef struct tagBitmapFileHeader
{
	//U16 bfType;
	U32 bfSize;
	U16 bfReserved1;
	U16 bfReserved2;
	U32 bfOffBits;
} BitmapFileHeader;

typedef struct tagBitmapInfoHeader
{
	U32 biSize;
	U32 biWidth;
	U32 biHeight;
	U16 biPlanes;
	U16 biBitCount;
	U32 biCompression;
	U32 biSizeImage;
	U32 biXPelsPerMeter;
	U32 biYPelsPerMeter;
	U32 biClrUsed;
	U32 biClrImportant;
} BitmapInfoHeader;

typedef struct tagBitmapInfo
{
	BitmapFileHeader bfHeader;
	BitmapInfoHeader biHeader;
} BitmapInfo;

typedef struct tagRGBQuad
{
	U8 rgbBlue;
	U8 rgbGreen;
	U8 rgbRed;
	U8 rgbReserved;
} RGBQuad;

typedef union tagBitmapData
{
	RGBQuad rgbQuad[0];
	U8 rgb[0];
} BitmapRGBData;

typedef struct tagBitmap
{
	BitmapInfo bInfo;
	BitmapRGBData bData;
} BitmapRGB;

void yv12toYUV(char *outYuv, char *inYv12, int width, int height, int widthStep);
void decodeI420(U8 yuv[], int width, int height, int pitch, U8 rgb[]);
void decodeYV12(U8 yuv[], int width, int height, int pitch, U8 rgb[]);
void decodeNV12(U8 yuv[], int width, int height, int pitch, U8 rgb[]);
void decodeNV21(U8 yuv[], int width, int height, int pitch, U8 rgb[]);
BitmapRGB* yuv2bmp(YUV_FORMAT format, U8 yuv[], int width, int height);
int yuv2bmpfile(YUV_FORMAT format, U8 yuv[], int width, int height, LPCTSTR file);
int yuv2jpgfile(YUV_FORMAT format, U8 yuv[], int width, int height, LPCTSTR file);

bool Image2Bytes(const CImage& img, BYTE** bytes, size_t& byteSize);
bool Bytes2Image(const BYTE* bytes, const size_t byteSize, CImage& img);


void ProcessWindowMessage();
CString RunCLI(LPCTSTR path, LPCTSTR command, LPCTSTR parameter);


bool deleteOldFile(LPCTSTR rootDir, int duration, LPCTSTR filter);
bool deleteOldFile(LPCTSTR rootDir, int day, int hour, int min, LPCTSTR filter);

void KillBrowserOnly();
void ShowFirmwareView(bool show);
bool getVersion(CString& version);

char * ConvertWCtoC(const wchar_t* str);
void ConvertWCtoC(const wchar_t* str, std::string& outString);

wchar_t* ConvertCtoWC(const char* str);
void ConvertCtoWC(const char* str, CString& outString);

char* AnsiToUTF8(const char *szAnsiString);
char* UTF8ToAnsi(const char *szUTF8String);
std::string UTF8ToANSIString(const char *pUtf8String);
std::string UTF8ToANSIString(std::string strRetData);
std::wstring UTF8ToANSIString(const wchar_t* strRetData);

CString LoadStringById(UINT nID, CString lang);

#include <opencv2/core.hpp>		// Basic OpenCV structures (cv::Mat, Scalar)
#include <afxcontrolbars.h>

typedef struct _ST_EVENT {
	int		eventType;			// 0:Thermometry, 1:QRCode, 2:MaskDetection 
	CString detectedType;		// Detected Code Type(QR-Code, Bar-Code, Car-No)
	CString detectedCode;		// Detected Code
	cv::Mat	cropFace;			// Face Crop (with temperature)
	float	temperature;		// Face temperature
	int		maskLevel;			// Mask Detection Level(미검증:-1, 미착용:0, 착용:1)
	int		alarmLevel;			// (0:체온정상, 1:발열)
} ST_EVENT;

COLORREF hex2rgb(std::wstring hex);
std::wstring rgb2hex(COLORREF col);
double getRatio(int resType);
void CImage2Mat(CImage& Image, cv::Mat& src);
CImage* Mat2CImage(cv::Mat* mat);
CString  makeTimeKey(TCHAR postfix);
CString  getTimeStr();
bool SaveFile(LPCTSTR fullpath, void* targetText, int targetLen, int iDeviceIndex);
bool EncryptFile(LPCTSTR inFilePath, LPCTSTR outFilePath, bool removeInFile);

using namespace std;






