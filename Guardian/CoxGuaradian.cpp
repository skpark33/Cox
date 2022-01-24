#include "stdafx.h"
#include "skpark/common.h"
#include "skpark/Statistics.h"
#include "skpark/TraceLog.h"
#include "CoxGuardian.h"
#include "HotKeyDlg.h"
#include "CoxConfig.h"
#include "COXFDSampleDlg.h"
#include "ci/libAes256/ciElCryptoAes256.h"
#include "FacePainterStatic.h"

CoxGuardian::CoxGuardian(CCOXFDSampleDlg* pWnd, CVCamView*	vcam, CTCamView*	 tcam, CWnd* statPlate, 
	CFacePainterStatic* faceArea)
: m_parent(pWnd) // 	should be ReleaseDC(pDC);
, m_wndVCam(vcam)
, m_wndTCam(tcam)
, m_statPlate(statPlate)
, m_bFullScreen(true)
, m_currentCtrlKey(NORMAL_PAGE)
, m_alarmState(NO_ALARM)
, m_faceArea(faceArea)
{
	int width = WIN_WIDTH;
	double  ratio = getRatio(COX_RESOLTION);
	double height = static_cast<double>(width)* ratio;
	m_cameraHeight = static_cast<int>(height);
	TraceLog((_T("m_cameraHeight=%d"), m_cameraHeight))   // 640x480==> 1080x810   ,  1280x1024 ==>  1080x864

	m_config = new CoxConfig();
	m_stat = new CStatistics();

	CDC* pDC = m_parent->GetDC();
	InitFont(pDC);
	m_parent->ReleaseDC(pDC);

	TCHAR locale[8];
	int lang_len = GetLocaleInfo(LOCALE_NAME_USER_DEFAULT, LOCALE_SISO639LANGNAME, locale, sizeof(locale));
	m_localeLang = locale;
	TraceLog((_T("Locale Language [%s]"), m_localeLang));
	if (m_localeLang.IsEmpty())
		m_localeLang = _T("ko");

	InitStat();
	m_parent->SetWindowText(_T("CoxGuardian1.0"));

	m_faceArea->SetGuardian(this);

}
CoxGuardian::~CoxGuardian()
{
	DETECTED_LIST::iterator itr;
	for (itr = m_detectedList.begin(); itr != m_detectedList.end(); itr++)
	{
		Detected* d = (*itr);
		if(d) delete d;
	}
}

void CoxGuardian::InitFont(CDC* pDc)
{
	InitFont(m_titleFont,			pDc, m_config->m_title.font,				m_config->m_title.fontSize);
	InitFont(m_subTitleFont,		pDc, m_config->m_subTitle.font,			m_config->m_subTitle.fontSize);
	InitFont(m_noticeTitleFont,	pDc, m_config->m_noticeTitle.font,		m_config->m_noticeTitle.fontSize);
	InitFont(m_alarmTitleFont, pDc, m_config->m_alarmTitle.font, m_config->m_alarmTitle.fontSize);
	InitFont(m_weatherFontTitle, pDc, WEATHER_FONT_REGULAR, WEATHER_FONT_SIZE_20);
	InitFont(m_alarmFont2, pDc, ALARM_FONT_2, ALARM_FONT_SIZE_2);
	InitFont(m_alarmFont3, pDc, ALARM_FONT_3, ALARM_FONT_SIZE_3);
	InitFont(m_alarmFont4, pDc, ALARM_FONT_4, ALARM_FONT_SIZE_4);
	InitFont(m_alarmFont5, pDc, WEATHER_FONT_REGULAR_EN, ALARM_FONT_SIZE_4);
	InitFont(m_alarmFont6, pDc, WEATHER_FONT_REGULAR_EN, ALARM_FONT_SIZE_2);
}

void CoxGuardian::InitFont(CFont& targetFont, CDC* pDc, LPCTSTR fontName, int fontSize, float fontWidth)
{
	//fontSize = static_cast<int>(static_cast<float>(fontSize)*0.6666666f);
	int lfHeight = -MulDiv(fontSize, pDc->GetDeviceCaps(LOGPIXELSY), 72);

	// TraceLog((_T("skpark font=%s(%d), fontSize=%d, lfHeight=%d", fontName, wcslen(fontName), fontSize, lfHeight))

	LOGFONT* LogFont = new LOGFONT;
	memset(LogFont, 0x00, sizeof(LOGFONT));
	memset(LogFont->lfFaceName, 0x00, 32);
	//_tcsncpy_s(LogFont->lfFaceName, LF_FACESIZE, fontName, wcslen(fontName));
	wcsncpy(LogFont->lfFaceName, fontName, wcslen(fontName));
	LogFont->lfHeight = lfHeight;
	LogFont->lfQuality = ANTIALIASED_QUALITY;
	LogFont->lfOrientation = 0;
	LogFont->lfEscapement = 0;
	LogFont->lfItalic = 0;
	LogFont->lfUnderline = 0;
	LogFont->lfStrikeOut = 0;
	LogFont->lfWidth = 0;// lfHeight*fontWidth; //85% 수준의 장평을 준다.

	targetFont.CreateFontIndirect(LogFont);
}

void CoxGuardian::MaxWindow()
{
	m_parent->ShowWindow(SW_MAXIMIZE);

	CRect rc;
	m_parent->GetClientRect(&rc);
		
	if (m_wndTCam->GetSafeHwnd()) {
		m_wndTCam->SetWindowPos(NULL,
			rc.left,
			rc.top + TITLE_AREA_HEIGHT,
			WIN_WIDTH,
			m_cameraHeight,
			m_config->m_is_normal ? SWP_HIDEWINDOW : SWP_SHOWWINDOW);  //skpark  <-- SWP_SHOWWINDOW); 
	}
	if (m_wndVCam->GetSafeHwnd()) {
		m_wndVCam->SetWindowPos(NULL,
			rc.left,
			rc.top + TITLE_AREA_HEIGHT,
			WIN_WIDTH,
			m_cameraHeight,
			m_config->m_is_normal ? SWP_SHOWWINDOW : SWP_HIDEWINDOW);
	}

	m_faceArea->MoveWindow(rc.left, TITLE_AREA_HEIGHT + m_cameraHeight, WIN_WIDTH, NOTICE_AREA_HEIGHT*2);
	m_statPlate->MoveWindow(rc.left, rc.bottom - STAT_AREA_HEIGHT, WIN_WIDTH, STAT_AREA_HEIGHT);
}

void CoxGuardian::MinWindow()
{
	//m_parent->ShowWindow(SW_MAXIMIZE);
	//m_parent->MoveWindow(0, 0, static_cast<int>(STREAM_WIDTH), static_cast<int>(STREAM_HEIGHT), TRUE);

	if (m_wndTCam->GetSafeHwnd()) {
		m_wndTCam->SetWindowPos(NULL,
			0, 0,STREAM_WIDTH,STREAM_HEIGHT,
			m_config->m_is_normal ? SWP_HIDEWINDOW : SWP_SHOWWINDOW);  //skpark  <-- SWP_SHOWWINDOW); 
	}
	if (m_wndVCam->GetSafeHwnd()) {
		m_wndVCam->SetWindowPos(NULL,
			0, 0, STREAM_WIDTH, STREAM_HEIGHT, 
			m_config->m_is_normal ? SWP_SHOWWINDOW : SWP_HIDEWINDOW);
	}
}

bool CoxGuardian::ToggleFullScreen()
{
	m_bFullScreen = !m_bFullScreen;

	if (m_bFullScreen)
	{
		m_parent->ModifyStyle(WS_CAPTION | WS_SYSMENU | WS_MAXIMIZEBOX | WS_SIZEBOX, NULL);
		MaxWindow();
		m_parent->MoveWindow(0, 0, WIN_WIDTH, WIN_HEIGHT, TRUE);

	}
	else
	{
		m_parent->ModifyStyle(NULL, WS_CAPTION | WS_SYSMENU | WS_MAXIMIZEBOX | WS_SIZEBOX);
		m_parent->MoveWindow(0, 0, STREAM_WIDTH, STREAM_HEIGHT, TRUE);
		MinWindow();

	}
	return m_bFullScreen;
}

bool CoxGuardian::ToggleCamera()
{
	m_config->m_is_normal = !m_config->m_is_normal;

	TraceLog((_T("skpark VK_F5...")));

	m_wndTCam->ShowWindow(m_config->m_is_normal ? SW_HIDE : SW_SHOW);
	m_wndVCam->ShowWindow(m_config->m_is_normal ? SW_SHOW : SW_HIDE);

	CString iniPath = UBC_CONFIG_PATH;
	iniPath += UBCBRW_INI;
	WritePrivateProfileString(ENTRY_NAME, _T("IS_NORMAL"), (m_config->m_is_normal ? _T("1") : _T("0")), iniPath);

	return m_config->m_is_normal;
}
void CoxGuardian::KillEverything(bool killFirmwareView)
{
	TraceLog((_T("skpark KillEverything()")));
	if (killFirmwareView) {
		RunCLI(UBC_EXE_PATH, _T("kill.exe"), _T("UBCFirmwareView"));
	}
	RunCLI(UBC_EXE_PATH, _T("ubckill.bat"), _T(""));
}

void CoxGuardian::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_F12)
		{
			TraceLog((_T("skpark VK_F12...")));
			showTaskbar(true);
			ToggleFullScreen();
		}
		else if (pMsg->wParam == VK_DELETE)
		{
			showTaskbar(true);
			TraceLog((_T("skpark VK_DELETE...")));
			::SendMessage(m_parent->GetSafeHwnd(), WM_CLOSE, NULL, NULL); 
		}
		else if (pMsg->wParam == VK_F8)
		{
			OpenAuthorizer();
		}
		else if (pMsg->wParam == VK_F7)
		{
			//OpenUploader();
		}
		else if (pMsg->wParam == VK_F5)
		{
			ToggleCamera();
		}
		else if (pMsg->wParam == VK_F2)
		{
			TraceLog((_T("skpark VK_F2...")));
			CHotKeyDlg aDlg(this->m_parent);
			aDlg.SetParentInfo(this, m_stat, m_config);
			aDlg.DoModal();
		}
	}
}
void CoxGuardian::Invalidate(ALARM_STATE alarmState)
{
	if (alarmState == NO_ALARM)	{
		m_faceArea->ShowWindow(SW_HIDE);
	}else{
		m_faceArea->ShowWindow(SW_SHOW);  // showWindow contain Invalidate
		if (alarmState == m_alarmState) {
			m_faceArea->Invalidate();
			return;
		}
	}
	m_parent->Invalidate();

}
void CoxGuardian::OnPaint()
{
	TraceLog((_T("DrawOnPaint()")));
	CPaintDC dc(m_parent); // device context for painting

	// 깜빡임 방지 코드 start
	CRect rect;
	m_parent->GetClientRect(&rect);
	CDC* pDC = (CDC*)&dc;
	CDC dcMem;
	CBitmap bitmap;
	dcMem.CreateCompatibleDC(pDC);
	bitmap.CreateCompatibleBitmap(pDC, rect.Width(), rect.Height());
	CBitmap* pOldBitmap = dcMem.SelectObject(&bitmap);
	// 깜박임 방지 코드 end
	
	ALARM_STATE alarmLevel = GetAlarmLevel();
	switch (alarmLevel)
	{
		case NO_ALARM:	
			TraceLog((_T("Normal !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!")));
			DrawNormal(dcMem); 
			break;
		case FEVER_ALARM:
			TraceLog((_T("FEVER !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!")));
			DrawFever(dcMem);
			break;
		case NO_MASK_ALARM:
			TraceLog((_T("NO MASK !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!")));
			DrawNoMask(dcMem);
			break;
	}
	//DrawFaces(&dcMem);
	TraceLog((_T("DrawFaces end")));
	
	//깜박임 방지 코드 start
	pDC->BitBlt(rect.left, rect.top, rect.Width(), rect.Height(), &dcMem, 0, 0, SRCCOPY);
	dcMem.SelectObject(pOldBitmap);
	//깜박임 방지 코드 end
}

void CoxGuardian::DrawTemperature(CDC* dc, Detected* detected, int videoEndPosY)
{
	if (!detected  || detected->isFever == false) {
		return;
	}

	CString temperature;
	temperature.Format(_T("%2.1f %s"), detected->temperature, TEMPER_STR);
	CString currentTime(_T(""));
	CTime aNow = CTime::GetCurrentTime();
	currentTime.Format(_T("%02d:%02d:%02d"), aNow.GetHour(), aNow.GetMinute(), aNow.GetSecond());

	int letterTop = videoEndPosY + 30;
	int letterLeft = 30;
	int letterWide = 300;
	int letterHeight1 = 96;
	int letterHeight2 = 48;
	int letterLineGap = 10;

	CRect fillRect(0, 
		videoEndPosY + 4,
					letterLeft + letterWide,
					videoEndPosY + letterHeight1 + letterHeight2 + letterLineGap);
	COLORREF bgColor = NORMAL_BG_COLOR;
	if (detected->isFever) { bgColor = FEVER_BG_COLOR; }
	else 	if (detected->isNoMask) 	{ bgColor = NO_MASK_BG_COLOR; }
	dc->FillSolidRect(fillRect, bgColor);

	WriteSingleLine(temperature, dc,
		(_T("ko") == m_localeLang) ? m_alarmFont2 : m_alarmFont6,
		ALARM_FG_COLOR_3, letterTop, letterHeight1, DT_LEFT, letterLeft, letterWide);

	WriteSingleLine(currentTime, dc, m_alarmFont3, ALARM_FG_COLOR_3, letterTop + letterHeight1, letterHeight2, DT_LEFT, letterLeft, letterWide);
}

int CoxGuardian::DrawFaces(CDC* dc)
{
	DETECTED_LIST	list;
	GetDetectedList(list);
	int index = 0;
	DETECTED_LIST::iterator itr;
	for (itr = list.begin(); itr != list.end();itr++)
	{
		//if (DrawFace(dc, (*itr), index, TITLE_AREA_HEIGHT + m_cameraHeight))
		if (DrawFace(dc, (*itr), index,0))
		{
			index++;
		}
		//DrawTemperature(dc, (*itr), TITLE_AREA_HEIGHT + m_cameraHeight);
		DrawTemperature(dc, (*itr), 0);
	}
	return index;
}

bool  CoxGuardian::DrawFace(CDC* dc, Detected* detected, int index, int videoEndPosY)
{
	TraceLog((_T("DrawFaces=%d(%s))"), index, detected->key));

	int w_gap = 20;  // 사진사이의 간격
	int h_gap = 40;  // 사진과 비디오 사이의 간격
	int w_thick = 5;  // 사진 옆면 테두리 두께
	int h_thick = h_gap - 4 ;  // 사진 위아래 테두리 두께

	//int videoEndPosY = TITLE_AREA_HEIGHT + m_cameraHeight ;
	int faceStartPosX = static_cast<int>(WIN_WIDTH * 0.9f);
	int counter = index % MAX_PICTURE;
	// 사진의 위치
	int top = videoEndPosY + h_gap;
	int left = faceStartPosX - counter*(THUMB_NAIL_WIDTH + w_gap);
	//if ((left) <= WIN_WIDTH + THUMB_NAIL_WIDTH - faceStartPosX)
	//{
	//	left = faceStartPosX;
	//}
	CRect rc(left, top, left + THUMB_NAIL_WIDTH, top + THUMB_NAIL_HEIGHT);

	TraceLog((_T("DrawFaces >>> rc=%d,%d,%d,%d idx[%d]"), rc.left - w_thick, rc.top - h_thick, rc.right + w_thick, rc.bottom + h_thick, counter));
	CRect fillRect(rc.left - w_thick, rc.top - h_thick, rc.right + w_thick, rc.bottom + h_thick);

	COLORREF bgColor = RGB(0x00, 0x00, 0x00);
	if (detected->isFever)
	{
		bgColor = FEVER_THUMB_BG;  // yellow
	}
	else 	if (detected->isNoMask)
	{
		bgColor = MASK_THUMB_BG;  // red
	}
	dc->FillSolidRect(fillRect, bgColor);
	
	if (detected->image) {
		detected->image->StretchBlt(dc->m_hDC, rc.left, rc.top, THUMB_NAIL_WIDTH, THUMB_NAIL_HEIGHT, SRCCOPY);
	}

	// 온도/마스크 표시
	CString state;
	if (detected->temperature > 0.0f)
	{
		state.Format(_T("%2.1f"), detected->temperature);
	}
	if (!detected->isFever && detected->isNoMask)
	{
		state = (m_localeLang == _T("ko") ? _T("마스크") : _T("Mask"));
	}
	WriteSingleLine(state, dc, m_weatherFontTitle, NORMAL_FG_COLOR, top - 10, 24, DT_CENTER, left - 40, THUMB_NAIL_WIDTH + 80);

	return true;
}

bool CoxGuardian::DrawNormal(CDC& dc)
{
	TraceLog((_T("DrawNormal()")));
	KeyboardSimulator(NORMAL_PAGE);

	//if (m_alarmState == NO_ALARM) return false;

	SetBkMode(dc.m_hDC, TRANSPARENT);
	dc.FillSolidRect(0, 0, WIN_WIDTH, WIN_HEIGHT, NORMAL_BG_COLOR);
	WriteNormal(dc, 0, 0);

	m_alarmState = NO_ALARM;
	return true;
}

bool CoxGuardian::DrawFever(CDC& dc)
{
	TraceLog((_T("DrawFever()")));
	KeyboardSimulator(FEVER_PAGE);

	//if (m_alarmState == FEVER_ALARM) return false;

	SetBkMode(dc.m_hDC, TRANSPARENT);
	dc.FillSolidRect(0, 0, WIN_WIDTH, WIN_HEIGHT, FEVER_BG_COLOR);

	WriteFever(dc, 0, 0);
	m_alarmState = FEVER_ALARM;
	return true;
}

bool CoxGuardian::DrawNoMask(CDC& dc)
{
	TraceLog((_T("DrawNoMask()")));
	KeyboardSimulator(MASK_PAGE);

	//if (m_alarmState == NO_MASK_ALARM) return false;

	SetBkMode(dc.m_hDC, TRANSPARENT);
	dc.FillSolidRect(0, 0, WIN_WIDTH, WIN_HEIGHT, NO_MASK_BG_COLOR);

	WriteNoMask(dc, 0, 0);
	m_alarmState = NO_MASK_ALARM;

	return true;
}

bool CoxGuardian::WriteNormal(CDC& dc, int x, int y)
{
	int posy = (TITLE_AREA_HEIGHT - TITLE_HEIGHT) / 2 - 20;
	int posx = 0;

	TraceLog((_T("tilte=%s"), m_config->m_title.text));
	TraceLog((_T("subTilte=%s"), m_config->m_subTitle.text));

	WriteSingleLine(m_config->m_title.text, &dc, m_titleFont, m_config->m_title.fgColor, posy, TITLE_HEIGHT);
	posy += m_config->m_title.fontSize + 36;
	WriteSingleLine(m_config->m_subTitle.text, &dc, m_subTitleFont, m_config->m_subTitle.fgColor, posy, SUBTITLE_HEIGHT);
	posy = TITLE_AREA_HEIGHT + m_cameraHeight + NOTICE_AREA_HEIGHT/2 + 5;
	WriteSingleLine(m_config->m_noticeTitle.text, &dc, m_noticeTitleFont, m_config->m_noticeTitle.fgColor, posy, NOTICE_TITLE_HEIGHT);

	return true;
}

bool CoxGuardian::WriteFever(CDC& dc, int x, int y)
{
	int posy = (TITLE_AREA_HEIGHT - TITLE_HEIGHT) / 2 - 20;
	int posx = 0;

	TraceLog((_T("tilte=%s"), m_config->m_alarmTitle.text));
	TraceLog((_T("subTilte=%s"), m_config->m_subTitle.text));

	WriteSingleLine(m_config->m_alarmTitle.text, &dc, m_alarmTitleFont, m_config->m_alarmTitle.fgColor, posy, TITLE_HEIGHT + 20);
	//posy += m_config->m_alarmTitle.fontSize + 36;
	//WriteSingleLine(_T("Fever"),	&dc, m_subTitleFont, m_config->m_subTitle.fgColor, posy, SUBTITLE_HEIGHT);
	//posy = TITLE_AREA_HEIGHT + m_cameraHeight + 48;
	//WriteSingleLine(m_config->m_noticeTitle.text, &dc, m_noticeTitleFont, m_config->m_noticeTitle.fgColor, posy, NOTICE_TITLE_HEIGHT);

	return true;
}

bool CoxGuardian::WriteNoMask(CDC& dc, int x, int y)
{
	int posy = (TITLE_AREA_HEIGHT - TITLE_HEIGHT) / 2 - 20;
	int posx = 0;

	TraceLog((_T("tilte=%s"), m_config->m_alarmTitle.text));
	TraceLog((_T("subTilte=%s"), m_config->m_subTitle.text));

	WriteSingleLine(m_config->m_alarmTitle.text, &dc, m_alarmTitleFont, m_config->m_alarmTitle.fgColor, posy, TITLE_HEIGHT + 20);
	//posy += m_config->m_alarmTitle.fontSize + 36;
	//WriteSingleLine(_T("No Mask"), &dc, m_subTitleFont, m_config->m_subTitle.fgColor, posy, SUBTITLE_HEIGHT);
	//posy = TITLE_AREA_HEIGHT + m_cameraHeight + 48;
	//WriteSingleLine(m_config->m_noticeTitle.text, &dc, m_noticeTitleFont, m_config->m_noticeTitle.fgColor, posy, NOTICE_TITLE_HEIGHT);

	return true;
}



bool CoxGuardian::KeyboardSimulator(const char* command)
{
	if (m_currentCtrlKey == command) {
		return false;
	}
	m_currentCtrlKey = command;

	std::string msg = "keyboardSimulator(";
	msg += command;
	msg += ")";

	AsciiLog(msg.c_str());

	HWND brwHwnd = getWHandle(BRW_BIN);

	if (!brwHwnd) {
		AsciiLog("skpark process not found(UTV_brwClient2.exe)");
		return true;
	}

	COPYDATASTRUCT appInfo;
	appInfo.dwData = UBC_WM_KEYBOARD_EVENT;  // 1000
	appInfo.lpData = (char*)command;
	appInfo.cbData = strlen(command) + 1;
	AsciiLog(msg.c_str());
	::SendMessage(brwHwnd, WM_COPYDATA, NULL, (LPARAM)&appInfo);

	return true;
}


void CoxGuardian::WriteSingleLine(LPCTSTR text,
	CDC* pDc,
	CFont& targetFont,
	COLORREF color,
	int posY,
	int height,
	int align, /*=DT_CENTER*/
	int posX, /*=0*/
	int width /*=WIN_WIDTH*/
	)
{
	HDC hDc = pDc->m_hDC;

	HGDIOBJ pOld = SelectObject(hDc, targetFont);
	SetTextColor(pDc->m_hDC, color);

	posY -= 20;  // 조금 올려준다.

	if (width == 0)
		width = WIN_WIDTH;

	CRect rc(posX, posY, posX + width, posY + height);
	pDc->DrawText(text, &rc, align | DT_VCENTER | DT_SINGLELINE);
}

bool CoxGuardian::CheckFever(FACE_INFO* faceInfo)
{
	if (m_stat->IsUsed() && m_stat->IsValid()) {
		return (faceInfo->face_temp >= m_stat->m_threshold + m_stat->m_avg);
	}
	return (faceInfo->face_temp >= m_config->m_tipping_point);
}

time_t CoxGuardian::CaseDetected(FACE_INFO* faceInfo, CRect& faceRect, uint32_t dataSize, BYTE* faceImage)
{
	TraceLog((_T("Case Detected !!!!")));

	time_t now = time(NULL);
	CString timeKey = makeTimeKey('0');

	Detected* detected = new Detected();
	detected->key = timeKey;
	detected->temperature = faceInfo->face_temp;
	detected->isFever = CheckFever(faceInfo);//(faceInfo->face_temp >= m_config->m_tipping_point);
	detected->isNoMask = (faceInfo->mask_state != MASK_WEARING_WELL);
	detected->detectTime = now;
	detected->faceFeature.Copy(faceInfo->facefeature);

	TraceLog((_T("New Face Detected (isFever=%d,  noMask=%d)"), detected->isFever, detected->isNoMask));
	TraceLog((_T("New Face Detected (%2.1f)"), detected->temperature));
	TraceLog((_T("ImageData Size(%d)"), dataSize));
	//camfInfo->visable_image.data  :  //전체화면 이미지
	//camfInfo->face_detect[i].visable_facebox :  화면에서 얼굴위치...그렇다면...

	CString iniInfoStr = detected->toString();
	CImage* aImage = CropGdiImage(faceImage, dataSize, faceRect, timeKey, iniInfoStr);
	detected->image = aImage;
	m_cs.Lock();
	m_detectedList.push_back(detected);
	m_cs.Unlock();
	TraceLog((_T("Case Detected End, Invalidate!!!!")));
	Invalidate(detected->isFever ? FEVER_ALARM : (detected->isNoMask ? NO_MASK_ALARM : NO_ALARM));

	return now;

}

BOOL CoxGuardian::IsSameMan(FaceMeSDK::IFaceMeRecognizer* a_pFMRecognizer, FACE_FEATURE_INFO* newFaceFeature)
{
	m_cs.Lock();
	DETECTED_LIST::iterator itr;
	for (itr = m_detectedList.begin(); itr != m_detectedList.end(); itr++)
	{
		Detected* detected = (*itr);
		if (!detected->isFever && !detected->isNoMask) continue;

		FaceMeSDK::FR_FaceFeature oldFeature;
		FaceMeSDK::FR_FaceFeature newFeature;

		detected->faceFeature.CopyTo(oldFeature);
		newFaceFeature->CopyTo(newFeature);

		float confidence = 0;
		a_pFMRecognizer->CompareFaceFeature(&oldFeature, &newFeature, &confidence);

		TraceLog((_T("==========confidence=%f"), confidence));
		if (confidence > m_config->m_same_face_confidence)
		{
			TraceLog((_T("IS SAME FACE ==========confidence=%f > %f"), confidence, m_config->m_same_face_confidence));
			m_cs.Unlock();
			return true;
		}
		else
		{
			TraceLog((_T("==========confidence=%f"), confidence));
		}
	}
	m_cs.Unlock();
	return false;
}

BOOL CoxGuardian::IsFaceBoxInTheramlArea(FACE_INFO*	a_pFaceInfo, CTCamView*		a_pTCamView)
{
	CRect rcBox(a_pFaceInfo->thermal_facebox);

	CRect org(a_pTCamView->GetZoomRect());
	int offsetW = (org.Width() / 20);  
	int offsetH = (org.Height() / 20);

	//TraceLog((_T("TCamView area : w=%d,h=%d"), org.Width(), org.Height()));
	// 유효영역을 줄여준다.
	CRect rcFilter(org.left + offsetW , org.top + offsetH, org.right - offsetW, org.bottom - offsetH);
	
	CRect rcDst;

	// rcFilter와 rcBox의 교집합
	// 결과는 rcDst
	rcDst.IntersectRect(rcFilter, rcBox);

	// rcDst와 rcBox와 일치하지 않는다면
	// 열상영역 외부 or 걸침
	if (!rcDst.EqualRect(rcBox)) {
		return FALSE;
	}
	return TRUE;
}

CRect CoxGuardian::DrawValidAreaBox(CRect& rect, ID2D1BitmapRenderTarget*	a_pBitmapTarget,
													ID2D1SolidColorBrush*	a_pBrush )
{
	//CRect pTCamViewRect(m_wndTCam->GetZoomRect());
	int offsetW = (WIN_WIDTH / 20);
	int offsetH = (m_cameraHeight / 20);

	//TraceLog((_T("TCamView area : w=%d,h=%d"), rect.Width(), rect.Height()));
	// 유효영역을 줄여준다.
	CRect rcFilter(rect.left + offsetW,
		rect.top + offsetH,
		WIN_WIDTH - offsetW,
		rect.top + m_cameraHeight - offsetH);

	a_pBrush->SetOpacity(1.f);
	a_pBrush->SetColor(GetD2DColor(RGB_BLUE));
	float fThickness = 4.f;

	D2D1_RECT_F  tRect;
	tRect.left = rcFilter.left;
	tRect.top = rcFilter.top;
	tRect.right = rcFilter.right;
	tRect.bottom = rcFilter.bottom;
	//ID2D1Brush *brush,

	a_pBitmapTarget->DrawRectangle(&tRect, a_pBrush, fThickness);

	return rcFilter;
}

D2D1_COLOR_F CoxGuardian::GetD2DColor(
	 COLORREF			a_clr,
	float				a_fAlpha)
{
	D2D1_COLOR_F clr;
	clr.r = (float)GetRValue(a_clr) / (float)255;
	clr.g = (float)GetGValue(a_clr) / (float)255;
	clr.b = (float)GetBValue(a_clr) / (float)255;
	clr.a = a_fAlpha;
	return clr;
}

void CoxGuardian::NewFaceDetected(CAMF_INFO* camfInfo, int faceCount)
{
	for (int i = 0; i < faceCount; i++)
	{
		if (isCaseOrAll(&(camfInfo->face_detect[i])))
		{
			CaseDetected(
				&(camfInfo->face_detect[i]),
				camfInfo->face_detect[i].visable_facebox,
				camfInfo->visable_image.size,
				camfInfo->visable_image.data
				);
		}
	}
}

bool CoxGuardian::isCase(FACE_INFO* faceInfo)
{
	return  (CheckFever(faceInfo)
		|| (m_config->m_use_mask_check && faceInfo->mask_state == MASK_NOT_WEARING));
}
bool CoxGuardian::isFeverCase(FACE_INFO* faceInfo)
{
	return  CheckFever(faceInfo);
}

bool CoxGuardian::isCaseOrAll(FACE_INFO* faceInfo)
{
	if (m_config->m_record_all) {
		return true;
	}
	return  (CheckFever(faceInfo)
		|| (m_config->m_use_mask_check && faceInfo->mask_state == MASK_NOT_WEARING));
}


CImage* CoxGuardian::CropGdiImage(BYTE * buffer, DWORD size, CRect& cropArea, LPCTSTR timeKey, LPCTSTR iniInfo)
{
	
	// buffer 에는 mat 타입으로 값이 온다.
	cv::Mat  aMat(m_wndVCam->m_nHeight,
		m_wndVCam->m_nWidth,
		CV_8UC4,
		buffer);


	// int nImgType = CxImage::GetTypeIdFromName("jpg");
	CImage* aImage = Mat2CImage(&aMat);
	if (aImage == NULL || aImage->IsNull()) {
		TraceLog((_T("Mat2CImage failed")));
		return NULL;
	}

	TraceLog((_T("Mat2CImage succeed")));

	/*
		Crop 해야함.
	*/
	int left = cropArea.left - 80;
	int top = cropArea.top - 80;
	int width = cropArea.right - cropArea.left + 160;
	int height = cropArea.bottom - cropArea.top + 160;

	TraceLog((_T("CropGdiImage : %d,%d,%d,%d"), left, top, width, height));

	CImage* cropedImage = new CImage();
	cropedImage->Create(width, height, aImage->GetBPP());

	if (!aImage->BitBlt(cropedImage->GetDC(), 0, 0, width, height, left, top, SRCCOPY))
	{
		TraceLog((_T("%s crop failed(%d,%d,%d,%d)"), timeKey, left, top, width, height));
		cropedImage->ReleaseDC();
		return aImage;
	}

	cropedImage->ReleaseDC();
	delete aImage;

	SaveImage(cropedImage, timeKey);
	SaveIni(timeKey, iniInfo);

	return cropedImage;
}

bool CoxGuardian::SaveImage(CImage* pImage, LPCTSTR filePrefix)
{
	CString strMediaFullPath;
	strMediaFullPath.Format(_T("%s%sPICT.jpg"), UPLOAD_PATH, filePrefix);
	HRESULT err = pImage->Save(strMediaFullPath);
		if (err < 0) {
		TraceLog((_T("Save %s file failed %d"), strMediaFullPath, err));
		return pImage;
	}

	TraceLog((_T("Save %s file succeed"), strMediaFullPath));
	
	// 암호화 작업 해야함....

	if (!m_config->m_not_use_encrypt) {
		
		CString strJpg = strMediaFullPath;
		strMediaFullPath.Format(_T("%s%sPICT.tmp"), UPLOAD_PATH,filePrefix);
		::MoveFile(strJpg, strMediaFullPath);

		CString decryptedPath;
		decryptedPath.Format(_T("%s%sPICT.jpg"), UPLOAD_PATH,filePrefix);
		if (!ciAes256Util::EncryptFile(strMediaFullPath, decryptedPath, false)) {
			TraceLog((_T("WARN : Encrypt Failed")));
		}
		else{
			::DeleteFile(strMediaFullPath);
			strMediaFullPath = decryptedPath;
		}
		
	}
	TraceLog((_T("Save %s file succeed"), strMediaFullPath));
	return true;

	
}

void CoxGuardian::SaveIni(LPCTSTR key, LPCTSTR iniInfo)
{
	//----------------------------------- ini write --------------------------------//
	CString fullpath;
	fullpath.Format(_T("%s%sINFO.ini"), UPLOAD_PATH, key);

	CString szPointInfo = iniInfo;
	szPointInfo += m_config->ToIniString();
	szPointInfo += m_stat->ToIniString();
	TraceLog((_T("SaveFile=%s"), szPointInfo));
	SaveFile(fullpath, (void*)(LPCTSTR)szPointInfo, szPointInfo.GetLength(), 0);
	//-----------------------------------------------------------------------------//
}


int CoxGuardian::GetDetectedList(DETECTED_LIST& list)
{
	//TraceLog((_T("GetDetectedList before Lock()")));

	m_cs.Lock();
	int  retval = 0;
	DETECTED_LIST::iterator itr;
	for (itr = m_detectedList.begin(); itr != m_detectedList.end(); itr++)
	{
		Detected* detected = (*itr);
		list.push_back(detected);
		retval++;
	}
	m_cs.Unlock();
	//TraceLog((_T("GetDetectedList UnLock()")));
	return retval;
}

ALARM_STATE CoxGuardian::GetAlarmLevel()
{
	//TraceLog((_T("GetAlarmLevel before Lock()")));
	m_cs.Lock();
	ALARM_STATE  retval = NO_ALARM;
	DETECTED_LIST::iterator itr;
	for (itr = m_detectedList.begin(); itr != m_detectedList.end(); itr++)
	{
		Detected* detected = (*itr);
		if (detected->isFever)
		{
			m_cs.Unlock();
			return FEVER_ALARM;
		}
		if (detected->isNoMask)
		{
			retval = NO_MASK_ALARM;
		}

	}
	m_cs.Unlock();
	//TraceLog((_T("GetAlarmLevel  UnLock()")));

	return retval;
}

int CoxGuardian::EraseOldDetected(time_t now)
{
	int retval = 0;
	//TraceLog((_T("EraseOldDetected before Lock()")));
	m_cs.Lock();
	DETECTED_LIST::iterator itr;
	for (itr = m_detectedList.begin(); itr != m_detectedList.end();) {
		Detected* aInfo = (*itr);
		if (aInfo->detectTime > 0 && now - aInfo->detectTime > m_config->m_alarmValidSec) {
			TraceLog((_T("EraseOldDetected erased >>> detectTime[%d] m_alarmValidSec[%d]"),
				now - aInfo->detectTime, m_config->m_alarmValidSec));
			delete aInfo;
			m_detectedList.erase(itr++);
			retval++;
		}
		else {
			++itr;
		}
	}
	m_cs.Unlock();
	//TraceLog((_T("EraseOldDetected before UnLock()")));

	return retval;
}

void CoxGuardian::OnDestroy()
{
	Clear();
	if (m_stat->IsUsed() && m_stat->IsValid())
	{
		m_stat->Write();
	}
	showTaskbar(true);
}

void CoxGuardian::OnTimer(UINT_PTR nIDEvent)
{
	time_t now = time(NULL);
	//static unsigned long counter = 0;
	int deleted = 0;
	switch (nIDEvent)
	{
	case ALARM_CHECK_TIMER: 	// check alarm time
		deleted = EraseOldDetected(now);
		if (deleted > 0)
		{
			TraceLog((_T("EraseOldDetected End, Invalidate!!!!")));
			ALARM_STATE state = NO_ALARM;
			if (m_detectedList.size())
			{
				state = GetAlarmLevel();
			}
			Invalidate(state);
		}
		//counter++;
		//if (counter % (5 * 60) == 5) {
			// 매 5분마다
			// m_parent->PostMessage(WM_OLD_FILE_DELETE, 0, 0);
		//}
		break;
	default:
		break;
	}
}

void CoxGuardian::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	static time_t pre_time = 0;
	static int aCount = 0;

	if (aCount == 0){
		pre_time = time(NULL);
		aCount++;
		return;
	}

	time_t now = time(NULL);
	if (now - pre_time > 1){
		aCount = 0;
		return;
	}

	if (aCount >= 5){
		//ToggleFullScreen();
		aCount = 0;

		CHotKeyDlg aDlg(m_parent);
		aDlg.SetParentInfo(this, m_stat, m_config);
		aDlg.DoModal();

		return;
	}
	aCount++;
}


HBRUSH CoxGuardian::OnCtlColor(int statisticsPlateId, CDC* pDC, CWnd* pWnd, UINT nCtlColor, HBRUSH hbr)
{
	//if (pWnd->GetDlgCtrlID() == IDC_STATIC_STAT)  //stat plate
	if (pWnd->GetDlgCtrlID() == statisticsPlateId)  //stat plate
	{
		if (m_stat->IsUsed() && m_stat->IsValid())
		{
			pDC->SetBkColor(NORMAL_BG_COLOR);
			pDC->SetTextColor(NORMAL_FG_COLOR_2);
			pDC->SetBkMode(TRANSPARENT);
			CFont* statFont = &m_alarmFont4;
			if (_T("ko") != m_localeLang)
			{
				statFont = &m_alarmFont5;
			}
			CFont* oldFont = pDC->SelectObject(statFont);
			return (HBRUSH)m_statBrush;
		}
	}
	return hbr;
}

void CoxGuardian::DrawStat(CDC& dc, int posY, int height)
{
	CString msg = m_stat->ToString(m_localeLang);
	TraceLog((_T("STAT=%s"), msg));
	if (_T("ko") == m_localeLang)
		WriteSingleLine(msg, &dc, m_alarmFont4, NORMAL_FG_COLOR_2, posY, height);
	else
		WriteSingleLine(msg, &dc, m_alarmFont5, NORMAL_FG_COLOR_2, posY, height);
}

void CoxGuardian::AddSample(float temper)
{
	m_cs.Lock();
	TraceLog((_T("AddSample(%f)"), temper));
	m_stat->AddSample(temper);
	m_cs.Unlock();
	if (m_stat->IsValid()) {
		CString msg = m_stat->ToString(m_localeLang);
		m_statPlate->SetWindowText(msg);
		m_statPlate->Invalidate();
	}
}


void CoxGuardian::InitStat()
{
	m_statBrush.CreateSolidBrush(NORMAL_BG_COLOR); // 평균값 배경 색 변경
	//CWnd* statPlate = GetDlgItem(IDC_STATIC_STAT);
	if (m_stat->IsUsed() && m_stat->IsValid()) {
		CString msg = m_stat->ToString(m_localeLang);
		m_statPlate->SetWindowText(msg);
		m_statPlate->MoveWindow(0, WIN_HEIGHT - 42, WIN_WIDTH, 42);
		m_statPlate->ShowWindow(SW_SHOW);
		m_statPlate->SetForegroundWindow();
	}
	else
	{
		m_statPlate->ShowWindow(SW_HIDE);
	}
}
 
void CoxGuardian::Clear()
{
	DeleteObject(m_titleFont);
	DeleteObject(m_subTitleFont);
	DeleteObject(m_noticeTitleFont);
	DeleteObject(m_alarmTitleFont);
	DeleteObject(m_weatherFontTitle);
	DeleteObject(m_alarmFont2);
	DeleteObject(m_alarmFont3);
	DeleteObject(m_alarmFont4);
	DeleteObject(m_alarmFont5);
	DeleteObject(m_alarmFont6);

	DeleteObject(m_alarmFont2);
	DeleteObject(m_alarmFont3);
	DeleteObject(m_alarmFont4);
	DeleteObject(m_alarmFont5);
	DeleteObject(m_alarmFont6);
	DeleteObject(m_weatherFontTitle);

	EraseAll();

	//if (m_logo)
	//{
	//	m_logo->Destroy();
	//	delete m_logo;
	//}
	//if (m_notice)
	//{
	//	m_notice->Destroy();
	//	delete m_notice;
	//}
	//if (m_title)
	//{
	//	m_title->Destroy();
	//	delete m_title;
	//}
}


void CoxGuardian::EraseAll()
{
	m_cs.Lock();
	DETECTED_LIST::iterator itr;
	for (itr = m_detectedList.begin(); itr != m_detectedList.end();) {
		Detected* aInfo = (*itr);
		delete aInfo;
		m_detectedList.erase(itr++);
	}
	m_cs.Unlock();
}

void CoxGuardian::OpenAuthorizer()
{
	showTaskbar(false);

	TraceLog((_T("skpark VK_F8...")));
	CString authorizer = _T("UBCHostAuthorizer.exe");
	//CString param = ("ko" == m_localeLang ? " +ENT +authOnly " : " +ENT  +authOnly  +global");
	CString param = (_T("ko") == m_localeLang ? _T(" +ENT +authOnly ") : _T(" +ENT  +authOnly  +global"));
	unsigned long pid = getPid(authorizer);
	if (pid)
	{
		killProcess(pid);
	}
	createProcess(authorizer, param, UBC_EXE_PATH);
}
