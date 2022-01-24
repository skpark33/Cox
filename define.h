#include <ostream>	// for vs2019
#include <string>



#define ALARM_GO_ON_SEC		30

#define WM_UPDATEDATA					(WM_USER + 1)
#define WM_PROC_ALARM					(WM_USER + 5)		// alarm handle
#define	UM_WEATHER_MESSAGE				(WM_USER+201)		// weather, micro dust search
#define	UM_WEATHER_UPDATE_MESSAGE		(WM_USER+202)		// weather, micro dust updated
#define	UM_EVENT_UPDATE_MESSAGE			(WM_USER+203)		// Event Queue Update
#define	UM_AIR_MESSAGE					(WM_USER+204)		// 미세먼지정보 조회
#define UM_REFRESH_GUARDIAN_SETTINGS	(WM_USER+301)		// 설정변경 후 INI Reload요청

#define WM_FRRETRY_PUSH_EVENT			WM_USER + 110   // skpark  FR RETRY EVENT
#define WM_FRRETRY_FILE_DOWN_REQUEST	WM_USER + 111   // skpark  FR RETRY FILE DOWNLOAD
#define WM_FRRETRY_FILE_DOWN_RESPONSE	WM_USER + 112   // skpark  FR RETRY FILE DOWNLOAD_RES
#define WM_OLD_FILE_DELETE				WM_USER + 113   // skpark  OLD FILE DELETE
#define WM_BACK_TO_YOUR_POSTION   3000  //skpark browser should shrink
#define WM_BACK_TO_YOUR_POSTION_RESPONSE				3001   // skpark  OLD FILE DELETE
#define FG_COLOR_WHITE			RGB(0xff,0xff,0xff)


#define ALARM_FG_COLOR_1		RGB(0xff,0xd9,0x28)
#define	ALARM_FONT_1			_T("Noto Sans KR Bold") 
#define ALARM_FONT_SIZE_1		96



#define ALARM_FG_COLOR_4		RGB(0xe4,0xe5,0xeb)
#define	ALARM_FONT_4			_T("Noto Sans KR Black")
#define ALARM_FONT_SIZE_4		20


#define NORMAL_FG_COLOR_1		RGB(0xd6,0xd8,0xe4)
#define	NORMAL_FONT_1			_T("Noto Sans KR Black")
#define NORMAL_FONT_SIZE_1		60

#define NORMAL_FG_COLOR_2		RGB(0xd6,0xd8,0xe4)
#define	NORMAL_FONT_2			_T("Nanum Square R")
#define NORMAL_FONT_SIZE_2		40

#define NORMAL_FG_COLOR_3		RGB(0xe4,0xe5,0xeb)
#define	NORMAL_FONT_3			_T("Noto Sans KR Bold")
#define NORMAL_FONT_SIZE_3		60

#define	WEATHER_FONT_REGULAR	_T("Noto Sans KR Regular")
#define	WEATHER_FONT_REGULAR_EN	_T("Segoe UI Symbol")
#define WEATHER_FONT_SIZE_20	20
#define WEATHER_FONT_SIZE_24	24.5
#define WEATHER_FONT_SIZE_25	25
#define WEATHER_FONT_SIZE_30	30
#define WEATHER_FONT_SIZE_82	82.5


#define WEATHER_FG_COLOR_GOOD	RGB(0x0f,0xc2,0xf6)		// 미세먼지 등급 - 좋음
#define WEATHER_FG_COLOR_NRML	RGB(0x37,0xe9,0x07)		// 미세먼지 등급 - 보통
#define WEATHER_FG_COLOR_BAD	RGB(0xff,0xa5,0x07)		// 미세먼지 등급 - 나쁨
#define WEATHER_FG_COLOR_WORST	RGB(0xff,0x06,0x06)		// 미세먼지 등급 - 매무나쁨

#define NORMAL_FG_COLOR			RGB(0xff,0xff,0xff)


//skpark define start
// Original Source Resolution is 347:262
#define		SKIP_FRAME_COUNT			30
#define		VALID_EVENT_LIMIT			10
#define		MAX_PICTURE					6
//#define		RGB(r,g,b)          ((COLORREF)(((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16)))

#define	TIPPING_POINT  37.5f
#define	SAME_FACE_CONFIDENCE  0.9

#define	RES_1920x1080 0
#define	RES_640x480		1
#define	RES_800x600		2
#define	RES_1024x768	3
#define	RES_1280x720	4
#define	RES_1280x1024	5
#define	RES_1600x1200	6
#define	RES_2048x1536	7
#define	RES_2560x1440	8
#define	RES_2592x1944	9

#define	COX_RESOLTION	RES_640x480

#define		WIN_WIDTH				1080
#define		WIN_HEIGHT			1920
#define		STREAM_WIDTH		640
#define		STREAM_HEIGHT		480

#define TITLE_AREA_HEIGHT		225
#define STAT_AREA_HEIGHT		40
#define NOTICE_AREA_HEIGHT		96

#define TITLE_HEIGHT					96
#define SUBTITLE_HEIGHT			72
#define NOTICE_TITLE_HEIGHT		NOTICE_AREA_HEIGHT

#define THUMB_NAIL_WIDTH		(264/3)
#define THUMB_NAIL_HEIGHT		(285/3)

#define TEMPER_STR			_T("°C")
#define UBCBRW_INI				_T("UBCBrowser.ini")
#define UBC_CONFIG_PATH	_T("C:\\SQISoft\\UTV1.0\\execute\\data\\")
#define UBC_EXE_PATH	_T("C:\\SQISoft\\UTV1.0\\execute\\")
#define ENTRY_NAME		_T("GUARDIAN")
#define BRW_BIN		_T("UTV_brwClient2.exe")

#define UBC_GUARDIAN_PATH		_T("C:\\SQISoft\\UTV1.0\\Guardian\\")
#define UBC_WM_KEYBOARD_EVENT	1000
#define UBCVARS_INI				_T("UBCVariables.ini")
#define UBCBRW_INI				_T("UBCBrowser.ini")
#define UBC_CONTENTS_PATH		_T("C:\\SQISoft\\Contents\\Enc\\")
#define FACE_PATH				_T("C:\\SQISoft\\FACE\\")
#define UPLOAD_PATH				_T("C:\\SQISoft\\UPLOAD\\")
#define	PROPERTY_DELI1			_T("###")
#define	PROPERTY_DELI2			_T("+++")

#define  NORMAL_PAGE		"CTRL+1"
#define  FEVER_PAGE		"CTRL+2"
#define  MASK_PAGE		"CTRL+3"


// 알람 Level

#define	NORMAL_BG_COLOR			RGB(0x21,0x29,0x42)
#define	FEVER_BG_COLOR				RGB(0x93, 0x1f, 0x1f)
#define	NO_MASK_BG_COLOR			RGB(0xff, 0xd9, 0x28)

#define   ALARM_BG_COLOR			FEVER_BG_COLOR
#define   ALARM_BG_COLOR_MASK	NO_MASK_BG_COLOR

#define   MASK_THUMB_BG			RGB(0xff, 0xc2, 0x24)
#define   FEVER_THUMB_BG			RGB(0xff, 0x1f, 0x1f)

#define MAX_HUMAN				10
#define ALARM_CHECK_TIMER		24		// new alarm validate time

#define ALARM_FG_COLOR_2		RGB(0xff,0xd9,0x28)
#define	ALARM_FONT_2			_T("Noto Sans KR Bold") 
#define ALARM_FONT_SIZE_2		64

#define ALARM_FG_COLOR_3		RGB(0xe4,0xe5,0xeb)
#define	ALARM_FONT_3			_T("Noto Sans KR Bold")
#define ALARM_FONT_SIZE_3		30
