#pragma once


// COX ThermalCamSDK //////////////////////////////////////////////////////////
#include "ThermalCamSDK.h"
#ifdef _M_X64
	#ifdef _DEBUG
		#pragma comment(lib, "ThermalCamSDK_x64_D.lib")
	#else
		#pragma comment(lib, "ThermalCamSDK_x64.lib")
	#endif
#endif
///////////////////////////////////////////////////////////////////////////////


#define __USE_BONJOUR_SDK__
// BonjourSDK /////////////////////////////////////////////////////////////////
#ifdef __USE_BONJOUR_SDK__
// Bonjour SDK 기본 설치 경로
#include "C:\\Program Files\\Bonjour SDK\Include\dns_sd.h"
#ifdef _M_X64
	#pragma comment(lib, "C:\\Program Files\\Bonjour SDK\\Lib\\x64\\dnssd.lib")
#else
	#pragma comment(lib, "C:\\Program Files\\Bonjour SDK\\Lib\\Win32\\dnssd.lib")
#endif
#endif
///////////////////////////////////////////////////////////////////////////////



// Direct2D ///////////////////////////////////////////////////////////////////
#include <d2d1.h>
#include <d2d1_1helper.h>
#include <dwrite.h>
#pragma comment(lib, "D2D1.lib") 
#pragma comment(lib, "DWrite.lib")
///////////////////////////////////////////////////////////////////////////////



// openCV /////////////////////////////////////////////////////////////////////
#include <opencv2\core.hpp>
#include <opencv2\highgui.hpp>
#include <opencv2\imgcodecs.hpp>
#include <opencv2\imgproc.hpp>
#include <opencv2\videoio.hpp>

#ifdef _M_X64
	#ifdef _DEBUG
		#pragma comment(lib, "opencv_core420d.lib")
		#pragma comment(lib, "opencv_highgui420d.lib")
		#pragma comment(lib, "opencv_imgcodecs420d.lib")
		#pragma comment(lib, "opencv_imgproc420d.lib")
		#pragma comment(lib, "opencv_videoio420d.lib")
	#else
		#pragma comment(lib, "opencv_core420.lib")
		#pragma comment(lib, "opencv_highgui420.lib")
		#pragma comment(lib, "opencv_imgcodecs420.lib")
		#pragma comment(lib, "opencv_imgproc420.lib")
		#pragma comment(lib, "opencv_videoio420.lib")
	#endif
#else
	#ifdef _DEBUG
	#else
	#endif
#endif
///////////////////////////////////////////////////////////////////////////////



// DShow //////////////////////////////////////////////////////////////////////
#include <dshow.h>
#pragma comment(lib, "strmiids.lib")
///////////////////////////////////////////////////////////////////////////////



