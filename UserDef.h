#pragma once

// Enum Define
enum USER_MSG
{
	NOTI_MSG_BONJR_FINISH = 0,
	NOTI_MSG_BONJR_ADD_CAM,
};


enum MASK_STATE
{
	MASK_WEARING_WELL = 0,
	MASK_NOT_RIGHT_WAY_WEARING,
	MASK_NOT_WEARING,
};


enum CAMERA_STATUS
{
	CAM_NONE = 0,
	CAM_CONNECTING,
	CAM_STREAMING,
	CAM_DISCONNECT,
};


#define		DEF_MAX_VISABLE_CAM_W					1920
#define		DEF_MAX_VISABLE_CAM_H					1080
#define		DEF_MAX_THERMAL_CAM_W					1024
#define		DEF_MAX_THERMAL_CAM_H					(768 + 1)

#define		DEF_MAX_FACE_COUNT						30
#define		DEF_MAX_MATCHING_POINT					4
#define		DEF_DATA_DEPTH							5
#define		DEF_MAX_PALETTE_SIZE					256 * 3
#define		DEF_MAX_PALETTE_COUNT					13
#define		DEF_USER_EEPROM_IDX						4096

// D2D
#define		D2DBMPTARGET							ID2D1BitmapRenderTarget
#define		D2DTARGET								ID2D1RenderTarget
#define		D2DHWNDTARGET							ID2D1HwndRenderTarget
#define		D2DWRITEFAC								IDWriteFactory
#define		D2DWICFAC								IWICImagingFactory
#define		D2DFAC									ID2D1Factory
#define		D2DBRUSH								ID2D1SolidColorBrush


// COLOR
#define		RGB_WHITE								RGB(255, 255, 255)
#define		RGB_BLACK								RGB(0, 0, 0)
#define		RGB_RED									RGB(255, 0, 0)
#define		RGB_GREEN								RGB(0, 255, 0)
#define		RGB_BLUE								RGB(0, 0, 255)
#define		RGB_DARK_BLUE							RGB(0, 0, 70)
#define		RGB_ORANGE								RGB(255, 127, 0)
#define		RGB_YELLOW								RGB(255, 255, 0)
#define		RGB_AQUA								RGB(0, 255, 255)
#define		RGB_PINK								RGB(255, 102, 204)
#define		RGB_PURPLE								RGB(128, 0, 128)
#define		RGB_BROWN								RGB(139, 69, 19)
#define		RGB_SKY_BLUE							RGB(0, 255, 255)


#define		THREADFUNC								UINT WINAPI


// UserDefine Include Header Files
#include "UserInclude.h"
#include "UserFunc.h"
#include "UserStruct.h"
#include "UserMessage.h"