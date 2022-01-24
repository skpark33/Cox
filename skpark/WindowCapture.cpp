// WindowCapture.cpp: implementation of the CWindowCapture class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "WindowCapture.h"



//ciSET_DEBUG(9, "agtScreenshotTimer");

//////////////////////////////////////////////////////////////////////
// Thread Functions
//////////////////////////////////////////////////////////////////////
BOOL m_isClientRunning;
DWORD		m_dwThreadID = 0;
HANDLE		m_hdService = NULL;

void ThreadClient(void *lParam)
{
	DWORD dwSleep = 10000;
	DWORD dwLastTick = GetTickCount();
	m_isClientRunning = TRUE;
	/*
	char *pFind;
	int nRet;
	int nRcv;
	int nHeaderSize;
	char szBuffer[102400];
	char szPort[16];
	*/
	UINT uTimerID = 0;
	while( m_isClientRunning )
	{
		if( m_hdService )
		{
//			TRACE( "클라이언트 대기\r\n" ); 
//			SetDlgItemText( hWnd, IDC_STATIC_CLIENT_MESSAGE,  (LPCTSTR)"클라이언트 대기" ); 
//			SuspendThread(m_hdService);
//			TRACE( "클라이언트 대기 - 해제\r\n" ); 
//			if( uTimerID == 0 )
//				uTimerID = SetTimer( NULL, 1, 10000, (TIMERPROC)CWindowCapture::OnCapture );
			CWindowCapture::OnCapture();
			//레지스트리에서 지연시간 읽기
			Sleep(dwSleep-(GetTickCount()-dwLastTick));
			dwLastTick = GetTickCount();
		}
		else
		{
			m_isClientRunning  = FALSE;
		}
	}
	if( uTimerID > 0 )
		KillTimer( NULL, uTimerID );
	uTimerID = 0;
//	TRACE( "클라이언트 종료\r\n" ); 
//	SetDlgItemText( hWnd, IDC_STATIC_CLIENT_MESSAGE,  (LPCTSTR)"클라이언트 종료" ); 
	m_isClientRunning  = FALSE;
	m_hdService = NULL;
}


//////////////////////////////////////////////////////////////////////
// Global Functions
//////////////////////////////////////////////////////////////////////

HBITMAP CaptureDesktop()
{
    HWND hWnd = NULL;

    hWnd = GetDesktopWindow();          // Get handle to desktop window.

    return CaptureWindow(hWnd, FALSE);  // Capture an image of this window.
}

HBITMAP CaptureDesktopRect(RECT& rect)
{
    HWND hWnd = NULL;

    hWnd = GetDesktopWindow();          // Get handle to desktop window.

    return CaptureWindowRect(hWnd, rect, FALSE);  // Capture an image of this window.
}


HBITMAP CaptureForegroundWindow(BOOL bClientAreaOnly)
{
    HWND hWnd = NULL;

    hWnd = ::GetForegroundWindow();             // Get the foreground window.

    return CaptureWindow(hWnd, bClientAreaOnly);// Capture an image of this window.
}

HBITMAP CaptureWindow(HWND hWnd, BOOL bClientAreaOnly)
{
    if(!hWnd)
        return NULL;

    HDC hdc;
    RECT rect;
    if(bClientAreaOnly)
    {
        hdc = GetDC(hWnd);
        GetClientRect(hWnd, &rect);
    }
    else
    {
        hdc = GetWindowDC(hWnd);
        GetWindowRect(hWnd, &rect);
    }

    if(!hdc)
        return NULL;

    HDC hMemDC = CreateCompatibleDC(hdc);
    if(hMemDC == NULL)
        return NULL;

    SIZE size;
    size.cx = rect.right - rect.left;
    if(rect.right < rect.left)
        size.cx = -size.cx;
    size.cy = rect.bottom - rect.top;
    if(rect.bottom < rect.top)
        size.cy = -size.cy;

    HBITMAP hDDBmp = CreateCompatibleBitmap(hdc, size.cx, size.cy);
    if(hDDBmp == NULL)
    {
        DeleteDC(hMemDC);
        ReleaseDC(hWnd, hdc);
        return NULL;
    }

    HBITMAP hOldBmp = static_cast<HBITMAP>(SelectObject(hMemDC, hDDBmp));
    BitBlt(hMemDC, 0, 0, size.cx, size.cy, hdc, 0, 0, SRCCOPY);
    SelectObject(hMemDC, hOldBmp);
    DeleteDC(hMemDC);
    ReleaseDC(hWnd, hdc);

    HBITMAP hBmp = static_cast<HBITMAP>(CopyImage(hDDBmp,
                                                    IMAGE_BITMAP,
                                                    0,
                                                    0,
                                                    LR_CREATEDIBSECTION));

    DeleteObject(hDDBmp);

	HBITMAP hRBmp = ScaleBitmap(hBmp, size.cx/2, size.cy/2);
	DeleteObject(hBmp);

    return hRBmp;
}

HBITMAP CaptureWindowRect(HWND hWnd, RECT& rect, BOOL bClientAreaOnly)
{
	//ciDEBUG(1,("CaptureWindowRect()"));

    if(!hWnd)
        return NULL;

    //RECT full_rect;
    HDC hdc = NULL;
    if(bClientAreaOnly)
    {
        hdc = GetDC(hWnd);
       // GetClientRect(hWnd, &full_rect);

    }
    else
    {
        hdc = GetWindowDC(hWnd);
       // GetWindowRect(hWnd, &full_rect);

    }

    if(hdc == NULL)
        return NULL;

    HDC hMemDC = CreateCompatibleDC(hdc);
    if(hMemDC == NULL)
        return NULL;

    SIZE size;
    size.cx = rect.right - rect.left;
    if(rect.right < rect.left)
        size.cx = -size.cx;
    size.cy = rect.bottom - rect.top;
    if(rect.bottom < rect.top)
        size.cy = -size.cy;

	//ciDEBUG(1,("CaptureWindowRect(%ld,%ld)", size.cx, size.cy));


    HBITMAP hDDBmp = CreateCompatibleBitmap(hdc, size.cx, size.cy);
	//HBITMAP hDDBmp = CreateCompatibleBitmap(hdc, full_rect.right-full_rect.left, full_rect.bottom-full_rect.top);
    if(hDDBmp == NULL)
    {
        DeleteDC(hMemDC);
        ReleaseDC(hWnd, hdc);
        return NULL;
    }

    HBITMAP hOldBmp = static_cast<HBITMAP>(SelectObject(hMemDC, hDDBmp));
    //BitBlt(hMemDC, rect.left, rect.top, size.cx, size.cy, hdc, 0, 0, SRCCOPY);
    BitBlt(hMemDC, 0, 0, size.cx, size.cy, hdc, rect.left, rect.top, SRCCOPY);
    SelectObject(hMemDC, hOldBmp);
    DeleteDC(hMemDC);
    ReleaseDC(hWnd, hdc);

    HBITMAP hBmp = static_cast<HBITMAP>(CopyImage(hDDBmp,
                                                    IMAGE_BITMAP,
                                                    0,
                                                    0,
                                                    LR_CREATEDIBSECTION));

    DeleteObject(hDDBmp);

	//HBITMAP hRBmp = ScaleBitmap(hBmp, size.cx/2, size.cy/2);
	//DeleteObject(hBmp);

	//ciDEBUG(1,("CaptureWindowRect() complete"));

    //return hRBmp;
    return hBmp;
}


int GetEncoderCLSID(const WCHAR* format, CLSID* pClsid)
{
   UINT  num = 0;          // number of image encoders
   UINT  size = 0;         // size of the image encoder array in bytes

   ImageCodecInfo* pImageCodecInfo = NULL;

   GetImageEncodersSize(&num, &size);
   if(size == 0)
      return -1;  // Failure

   pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
   if(pImageCodecInfo == NULL)
      return -1;  // Failure

   GetImageEncoders(num, size, pImageCodecInfo);

   for(UINT j = 0; j < num; ++j)
   {
      if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 )
      {
         *pClsid = pImageCodecInfo[j].Clsid;
         free(pImageCodecInfo);
         return j;  // Success
      }    
   }

   free(pImageCodecInfo);
   return -1;  // Failure
}
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWindowCapture::CWindowCapture()
{
	GdiplusStartupInput gpsi;

	if (GdiplusStartup(&m_gpToken,&gpsi,NULL) != Ok)
	{
//		AfxMessageBox("GDI+ 라이브러리를 초기화할 수 없습니다.");
//		return FALSE;
	}
}

CWindowCapture::~CWindowCapture()
{
	GdiplusShutdown(m_gpToken);
}

HRESULT LPTSTR_to_BSTR (BSTR *pbstr, LPCTSTR psz)
{
#ifndef UNICODE
	BSTR bstr;
	int i;
	HRESULT hr;
	// compute the length of the required BSTR
	//
	i =  MultiByteToWideChar(CP_ACP, 0, psz, -1, NULL, 0);
	if (i <= 0) { return E_UNEXPECTED; };
	// allocate the widestr, +1 for terminating null
	//
	bstr = SysAllocStringLen(NULL, i-1);
	// SysAllocStringLen adds 1
	if (bstr != NULL)
	{
		MultiByteToWideChar(CP_ACP, 0, psz, -1, (LPWSTR)bstr, i);
		((LPWSTR)bstr)[i - 1] = 0;
		*pbstr = bstr; hr = S_OK;
	}
	else
	{
		hr = E_OUTOFMEMORY;
	};
	return hr;
#else
	BSTR bstr;
	bstr = SysAllocString(psz);
	if (bstr != NULL)
	{
		*pbstr = bstr;
		return S_OK;
	}
	else
	{
		return E_OUTOFMEMORY;
	};
#endif
	// UNICODE
}

#define _UBC_CD(xxxx)	xxxx

long CWindowCapture::OnCapture(CAP_TYPE type, 
	const TCHAR* fileName, const TCHAR* filePath)
{
	BOOL bRet;
	TCHAR szData[1024];
	DWORD dwNumberOfBytesWritten;
	TCHAR tszPath[_MAX_PATH];
	TCHAR tszPathLog[_MAX_PATH];
	HANDLE hFileWrite = INVALID_HANDLE_VALUE;

	memset(tszPath, 0x00, _MAX_PATH);
	if (_tcslen(filePath)) {
		_swprintf(tszPath, _T("%s"), filePath);
		memset(tszPathLog, 0x00, _MAX_PATH);
		_swprintf( tszPathLog, _T("%s\\Time.log"), filePath);
	} else {
		_swprintf(tszPath, _T(_UBC_CD("C:\\SQIsoft\\ScreenShot")));
		wcscpy_s( tszPathLog, _T(_UBC_CD("C:\\SQIsoft\\ScreenShot\\Time.log")) );
	}
	// 디렉토리 생성
	//CreateDirectory(tszPath, NULL); // 디렉토리 생성
	MakeFolder(tszPath); // 리커시브 디렉토리 생성

	hFileWrite = CreateFile(	tszPathLog,
						GENERIC_READ | GENERIC_WRITE,
						FILE_SHARE_READ|FILE_SHARE_WRITE| FILE_SHARE_DELETE,
						NULL,
						OPEN_ALWAYS,
						FILE_ATTRIBUTE_NORMAL,
						NULL);
	if( hFileWrite != INVALID_HANDLE_VALUE )
	{
		SetFilePointer( hFileWrite, 0, NULL, FILE_END );
	}

	DWORD dwTickAdd = 0;
	DWORD dwTickStart = GetTickCount();
	HBITMAP hBmp = CaptureDesktop();
	DWORD dwTick = GetTickCount();

	if( hFileWrite != INVALID_HANDLE_VALUE )
	{
		_stprintf( szData, _T("CaptureDesktop :\t%08d\r\n"), dwTick-dwTickStart-dwTickAdd );
		bRet = WriteFile( hFileWrite, szData, wcslen(szData), &dwNumberOfBytesWritten, NULL );
	}
	dwTickAdd = dwTick-dwTickStart;


    if (hBmp)
	{
		SYSTEMTIME st;
		GetLocalTime(&st);
		Bitmap *pImage = Bitmap::FromHBITMAP( hBmp, NULL );
		dwTick = GetTickCount();
		if( hFileWrite != INVALID_HANDLE_VALUE )
		{
			_stprintf( szData, _T("FromHBITMAP :\t%08d\r\n"), dwTick-dwTickStart-dwTickAdd );
			bRet = WriteFile( hFileWrite, szData, wcslen(szData), &dwNumberOfBytesWritten, NULL );
		}
		dwTickAdd = dwTick-dwTickStart;

		Status	statImage;
		INT		nResult;
		CLSID	encoderClsidPNG = {0};
		CLSID	encoderClsidGIF = {0};
		CLSID	encoderClsidJPG = {0};
		EncoderParameters ep;
		BSTR	bstrFilePath;
		TCHAR tszFilePath[_MAX_PATH];

		memset(tszFilePath, 0x00, _MAX_PATH);
		if (_tcslen(fileName)) {			
			_stprintf( tszFilePath, _T("%s\\%s"), tszPath, fileName);
		} else {
			_stprintf( tszFilePath, _T("%s\\out_%04d%02d%02d_%02d%02d%02d_%03d.")
				,tszPath, st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds );
		}

		//
		// PNG
		//

		if (type == CAP_PNG) {

			nResult = GetEncoderCLSID(L"image/png", &encoderClsidPNG);
			dwTick = GetTickCount();
			if( hFileWrite != INVALID_HANDLE_VALUE )
			{
				_stprintf( szData, _T("GetEncoderCLSID(PNG) :\t%08d\r\n"), dwTick-dwTickStart-dwTickAdd );
				bRet = WriteFile( hFileWrite, szData, wcslen(szData), &dwNumberOfBytesWritten, NULL );
			}
			dwTickAdd = dwTick-dwTickStart;

			ULONG quality = 50;
			ep.Count = 1;
			ep.Parameter[0].Guid = EncoderQuality;
			ep.Parameter[0].Type = EncoderParameterValueTypeLong;
			ep.Parameter[0].NumberOfValues = 1;
			ep.Parameter[0].Value = &quality;

			_stprintf( tszFilePath, _T("%s.png"), tszFilePath);
			//bstrFilePath = L"D:\\LogFiles\\out.png";
			if( LPTSTR_to_BSTR( &bstrFilePath, tszFilePath) ==S_OK )
			{
				statImage = pImage->Save( bstrFilePath, &encoderClsidPNG, &ep);
				if(statImage == Ok)
				{
					//WriteLog( 0, "변환 To PNG(%02d) [%s]->[%s]", nIdx, pstCutListPos->szFileName, strFilePath );
				}
				else
				{
					//WriteLog( 2, "변환 To PNG(%02d) ErrorCode = %d, [%s]->[%s]", nIdx, statImage, pstCutListPos->szFileName, strFilePath );
				}
				SysFreeString( bstrFilePath);
			}
			dwTick = GetTickCount();
			if( hFileWrite != INVALID_HANDLE_VALUE )
			{
				wsprintf( szData, _T("Save PNG :\t%08d\r\n"), dwTick-dwTickStart-dwTickAdd );
				bRet = WriteFile( hFileWrite, szData, wcslen(szData), &dwNumberOfBytesWritten, NULL );
			}
			dwTickAdd = dwTick-dwTickStart;

		} else if (type == CAP_JPG) {

			//
			// JPG
			//

			nResult = GetEncoderCLSID(L"image/jpeg", &encoderClsidJPG);
			dwTick = GetTickCount();
			if( hFileWrite != INVALID_HANDLE_VALUE )
			{
				wsprintf( szData, _T("GetEncoderCLSID(JPG) :\t%08d\r\n"), dwTick-dwTickStart-dwTickAdd );
				bRet = WriteFile( hFileWrite, szData, wcslen(szData), &dwNumberOfBytesWritten, NULL );
			}
			dwTickAdd = dwTick-dwTickStart;

			ULONG quality = 50;
			ep.Count = 1;
			ep.Parameter[0].Guid = EncoderQuality;
			ep.Parameter[0].Type = EncoderParameterValueTypeLong;
			ep.Parameter[0].NumberOfValues = 1;
			ep.Parameter[0].Value = &quality;

			_stprintf( tszFilePath, _T("%s.jpg"), tszFilePath);
			//bstrFilePath = L"D:\\LogFiles\\out.png";
			if( LPTSTR_to_BSTR( &bstrFilePath, tszFilePath) ==S_OK )
			{
				statImage = pImage->Save( bstrFilePath, &encoderClsidJPG, &ep);
				if(statImage == Ok)
				{
					//WriteLog( 0, "변환 To PNG(%02d) [%s]->[%s]", nIdx, pstCutListPos->szFileName, strFilePath );
				}
				else
				{
					//WriteLog( 2, "변환 To PNG(%02d) ErrorCode = %d, [%s]->[%s]", nIdx, statImage, pstCutListPos->szFileName, strFilePath );
				}
				SysFreeString( bstrFilePath);
			}
			dwTick = GetTickCount();
			if( hFileWrite != INVALID_HANDLE_VALUE )
			{
				wsprintf( szData, _T("Save JPG :\t%08d\r\n"), dwTick-dwTickStart-dwTickAdd );
				bRet = WriteFile( hFileWrite, szData, wcslen(szData), &dwNumberOfBytesWritten, NULL );
			}
			dwTickAdd = dwTick-dwTickStart;

		} else {
		//else if (type == CAP_GIF) {

			//
			// GIF
			//

			nResult = GetEncoderCLSID(L"image/gif", &encoderClsidGIF);
			dwTick = GetTickCount();
			if( hFileWrite != INVALID_HANDLE_VALUE )
			{
				wsprintf( szData, _T("GetEncoderCLSID(GIF) :\t%08d\r\n"), dwTick-dwTickStart-dwTickAdd );
				bRet = WriteFile( hFileWrite, szData, wcslen(szData), &dwNumberOfBytesWritten, NULL );
			}
			dwTickAdd = dwTick-dwTickStart;

			_stprintf( tszFilePath, _T("%s.gif"), tszFilePath);
			if( LPTSTR_to_BSTR( &bstrFilePath, tszFilePath) ==S_OK )
			{
				statImage = pImage->Save( bstrFilePath, &encoderClsidGIF, NULL);
				if(statImage == Ok)
				{
					//WriteLog( 0, "변환 To PNG(%02d) [%s]->[%s]", nIdx, pstCutListPos->szFileName, strFilePath );
				}
				else
				{
					//WriteLog( 2, "변환 To PNG(%02d) ErrorCode = %d, [%s]->[%s]", nIdx, statImage, pstCutListPos->szFileName, strFilePath );
				}
				SysFreeString( bstrFilePath);
			}
			dwTick = GetTickCount();
			if( hFileWrite != INVALID_HANDLE_VALUE )
			{
				wsprintf( szData, _T("Save GIF :\t%08d\r\n"), dwTick-dwTickStart-dwTickAdd );
				bRet = WriteFile( hFileWrite, szData, wcslen(szData), &dwNumberOfBytesWritten, NULL );
				wsprintf( szData, _T("END :\t%08d\r\n"), dwTick-dwTickStart );
				bRet = WriteFile( hFileWrite, szData, wcslen(szData), &dwNumberOfBytesWritten, NULL );
			}		
		}

		DeleteObject(hBmp); // release hbitmap
	}
	if( hFileWrite != INVALID_HANDLE_VALUE )
	{
		CloseHandle(hFileWrite);
		hFileWrite = INVALID_HANDLE_VALUE;
	}
	return 1;

}


long CWindowCapture::OnCaptureRect(CAP_TYPE type, 
	const TCHAR* fullPath, RECT& rect)
{
	//BOOL bRet;
	//char szData[1024];
	//DWORD dwNumberOfBytesWritten;
	HANDLE hFileWrite = INVALID_HANDLE_VALUE;
	Bitmap *pImage = 0;

	HBITMAP hBmp = CaptureDesktopRect(rect);

    if (hBmp)
	{
		SYSTEMTIME st;
		GetLocalTime(&st);
		pImage = Bitmap::FromHBITMAP( hBmp, NULL );

		Status	statImage;
		INT		nResult;
		CLSID	encoderClsidPNG = {0};
		CLSID	encoderClsidGIF = {0};
		CLSID	encoderClsidJPG = {0};
		EncoderParameters ep;
		BSTR	bstrFilePath;

		//
		// PNG
		//

		if (type == CAP_PNG) {

			nResult = GetEncoderCLSID(L"image/png", &encoderClsidPNG);

			ULONG quality = 50;
			ep.Count = 1;
			ep.Parameter[0].Guid = EncoderQuality;
			ep.Parameter[0].Type = EncoderParameterValueTypeLong;
			ep.Parameter[0].NumberOfValues = 1;
			ep.Parameter[0].Value = &quality;

			if( LPTSTR_to_BSTR( &bstrFilePath, fullPath) ==S_OK )
			{
				statImage = pImage->Save( bstrFilePath, &encoderClsidPNG, &ep);
				if(statImage == Ok)
				{
					//WriteLog( 0, "변환 To PNG(%02d) [%s]->[%s]", nIdx, pstCutListPos->szFileName, strFilePath );
				}
				else
				{
					//WriteLog( 2, "변환 To PNG(%02d) ErrorCode = %d, [%s]->[%s]", nIdx, statImage, pstCutListPos->szFileName, strFilePath );
				}
				SysFreeString( bstrFilePath);
			}

		} else if (type == CAP_JPG) {

			//
			// JPG
			//

			nResult = GetEncoderCLSID(L"image/jpeg", &encoderClsidJPG);

			ULONG quality = 50;
			ep.Count = 1;
			ep.Parameter[0].Guid = EncoderQuality;
			ep.Parameter[0].Type = EncoderParameterValueTypeLong;
			ep.Parameter[0].NumberOfValues = 1;
			ep.Parameter[0].Value = &quality;

			if (LPTSTR_to_BSTR(&bstrFilePath, fullPath) == S_OK)
			{
				statImage = pImage->Save( bstrFilePath, &encoderClsidJPG, &ep);
				if(statImage == Ok)
				{
					//WriteLog( 0, "변환 To PNG(%02d) [%s]->[%s]", nIdx, pstCutListPos->szFileName, strFilePath );
				}
				else
				{
					//WriteLog( 2, "변환 To PNG(%02d) ErrorCode = %d, [%s]->[%s]", nIdx, statImage, pstCutListPos->szFileName, strFilePath );
				}
				SysFreeString( bstrFilePath);
			}

		} else {
		//else if (type == CAP_GIF) {

			//
			// GIF
			//

			nResult = GetEncoderCLSID(L"image/gif", &encoderClsidGIF);

			if (LPTSTR_to_BSTR(&bstrFilePath, fullPath) == S_OK)
			{
				statImage = pImage->Save( bstrFilePath, &encoderClsidGIF, NULL);
				if(statImage == Ok)
				{
					//WriteLog( 0, "변환 To PNG(%02d) [%s]->[%s]", nIdx, pstCutListPos->szFileName, strFilePath );
				}
				else
				{
					//WriteLog( 2, "변환 To PNG(%02d) ErrorCode = %d, [%s]->[%s]", nIdx, statImage, pstCutListPos->szFileName, strFilePath );
				}
				SysFreeString( bstrFilePath);
			}
		}

		DeleteObject(hBmp); // release hbitmap
	}
	if( hFileWrite != INVALID_HANDLE_VALUE )
	{
		CloseHandle(hFileWrite);
		hFileWrite = INVALID_HANDLE_VALUE;
	}
	if (pImage) delete pImage;
	return 1;

}

void CWindowCapture::MakeFolder(LPCTSTR pszPath)
{
	TCHAR DirName[256];
	LPCTSTR p = pszPath;
	TCHAR* q = DirName; 
	while(*p)
	{
		if (('\\' == *p) || ('/' == *p))
		{
			if (':' != *(p-1))
			{
				CreateDirectory(DirName, NULL);
			}
		}
		*q++ = *p++;
		*q = '\0';
	}
	CreateDirectory(DirName, NULL);
}

void CWindowCapture::Start()
{
	m_hdService = CreateThread( 0, 0,
		(LPTHREAD_START_ROUTINE) ThreadClient, 
		(VOID *)this, 0, &m_dwThreadID );
}

//
// resize function
//

#include <math.h>

HBITMAP ScaleBitmap(HBITMAP hBmp, 
					WORD wNewWidth, 
					WORD wNewHeight) 
{ 
	BITMAP bmp; 
	::GetObject(hBmp, sizeof(BITMAP), &bmp); 

	// check for valid size 
	if((bmp.bmWidth > wNewWidth && bmp.bmHeight < wNewHeight) || 
		(bmp.bmWidth < wNewWidth && bmp.bmHeight > wNewHeight)) 
		return NULL; 

	HDC hDC = ::GetDC(NULL); 

	BITMAPINFO *pbi = PrepareRGBBitmapInfo((WORD)bmp.bmWidth, 
		(WORD)bmp.bmHeight); 

	BYTE *pData = new BYTE[pbi->bmiHeader.biSizeImage]; 

	::GetDIBits(hDC, hBmp, 0, bmp.bmHeight, pData, 
		pbi, DIB_RGB_COLORS); 

	delete pbi; 

	pbi = PrepareRGBBitmapInfo(wNewWidth, wNewHeight); 
	BYTE *pData2 = new BYTE[pbi->bmiHeader.biSizeImage]; 

	if(bmp.bmWidth >= wNewWidth && bmp.bmHeight >= wNewHeight) 
		ShrinkData(pData, (WORD)bmp.bmWidth, (WORD)bmp.bmHeight, 
		pData2, wNewWidth, wNewHeight); 
	else 
		EnlargeData(pData, (WORD)bmp.bmWidth, (WORD)bmp.bmHeight, 
		pData2, wNewWidth, wNewHeight); 

	delete pData; 

	HBITMAP hResBmp = ::CreateCompatibleBitmap(hDC, 
		wNewWidth, 
		wNewHeight); 

	::SetDIBits(hDC, hResBmp, 0, wNewHeight, pData2, 
		pbi, DIB_RGB_COLORS); 

	::ReleaseDC(NULL, hDC); 

	delete pbi; 
	delete pData2; 

	return hResBmp; 
} 

BITMAPINFO* PrepareRGBBitmapInfo(WORD wWidth, WORD wHeight) 
{ 
	BITMAPINFO *pRes = new BITMAPINFO; 
	::ZeroMemory(pRes, sizeof(BITMAPINFO)); 
	pRes->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	pRes->bmiHeader.biWidth = wWidth; 
	pRes->bmiHeader.biHeight = wHeight; 
	pRes->bmiHeader.biPlanes = 1; 
	pRes->bmiHeader.biBitCount = 24; 

	pRes->bmiHeader.biSizeImage = ((3 * wWidth + 3) & ~3) 
		* wHeight; 
	return pRes; 
} 

float* CreateCoeff(int nLen, int nNewLen, BOOL bShrink) 
{ 
	int nSum = 0, nSum2; 
	float *pRes = new float[2 * nLen]; 
	float *pCoeff = pRes; 
	float fNorm = (bShrink)? (float)nNewLen / nLen : 1; 
	int      nDenom = (bShrink)? nLen : nNewLen; 

	::ZeroMemory(pRes, 2 * nLen * sizeof(float));
	for(int i = 0; i < nLen; i++, pCoeff += 2) 
	{ 
		nSum2 = nSum + nNewLen; 
		if(nSum2 > nLen) 
		{ 
			*pCoeff = (float)(nLen - nSum) / nDenom;
			pCoeff[1] = (float)(nSum2 - nLen) / nDenom; 
			nSum2 -= nLen; 
		} else { 
			*pCoeff = fNorm; 
			if(nSum2 == nLen) 
			{ 
				pCoeff[1] = -1; 
				nSum2 = 0; 
			} 
		} 
		nSum = nSum2; 
	} 
	return pRes; 
} 

#define F_DELTA        0.0001f 

void ShrinkData(BYTE *pInBuff, WORD wWidth, WORD wHeight, 
				BYTE *pOutBuff, WORD wNewWidth, WORD wNewHeight) 
{ 
	BYTE  *pLine = pInBuff, *pPix; 
	BYTE  *pOutLine = pOutBuff; 
	DWORD dwInLn = (3 * wWidth + 3) & ~3; 
	DWORD dwOutLn = (3 * wNewWidth + 3) & ~3; 
	int   x, y, i, ii; 
	BOOL  bCrossRow, bCrossCol; 

	float *pRowCoeff = CreateCoeff(wWidth, 
		wNewWidth, 
		TRUE); 
	float *pColCoeff = CreateCoeff(wHeight, 
		wNewHeight, 
		TRUE); 

	float fTmp, *pXCoeff, *pYCoeff = pColCoeff; 
	DWORD dwBuffLn = 3 * wNewWidth * sizeof(float); 
	float *fBuff = new float[6 * wNewWidth]; 

	float *fCurrLn = fBuff, *fCurrPix, *fNextLn 
		= fBuff + 3 * wNewWidth, *fNextPix; 

	::ZeroMemory(fBuff, 2 * dwBuffLn); 

	y = 0; 
	while(y < wNewHeight) 
	{ 
		pPix = pLine; 
		pLine += dwInLn; 

		fCurrPix = fCurrLn; 
		fNextPix = fNextLn; 

		x = 0; 
		pXCoeff = pRowCoeff; 
		bCrossRow = pYCoeff[1] > F_DELTA; 
		while(x < wNewWidth) 
		{ 
			fTmp = *pXCoeff * *pYCoeff; 

			for(i = 0; i < 3; i++) 
				fCurrPix[i] += fTmp * pPix[i]; 

			bCrossCol = pXCoeff[1] > F_DELTA;

			if(bCrossCol) 
			{ 
				fTmp = pXCoeff[1] * *pYCoeff;
				for(i = 0, ii = 3; i < 3; i++, ii++) 
					fCurrPix[ii] += fTmp * pPix[i]; 
			} 

			if(bCrossRow) 
			{ 
				fTmp = *pXCoeff * pYCoeff[1]; 
				for(i = 0; i < 3; i++) 
					fNextPix[i] += fTmp * pPix[i]; 
				if(bCrossCol) 
				{ 
					fTmp = pXCoeff[1] * pYCoeff[1]; 
					for(i = 0, ii = 3; i < 3; i++, ii++) 
						fNextPix[ii] += fTmp * pPix[i]; 
				} 
			} 
			if(fabs(pXCoeff[1]) > F_DELTA) 
			{ 
				x++; 
				fCurrPix += 3; 
				fNextPix += 3; 
			} 
			pXCoeff += 2; 
			pPix += 3; 
		} 

		if(fabs(pYCoeff[1]) > F_DELTA) 
		{ 
			// set result line 
			fCurrPix = fCurrLn; 
			pPix = pOutLine; 
			for(i = 3 * wNewWidth; i > 0; i--, fCurrPix++, pPix++) 
				*pPix = (BYTE)*fCurrPix; 

			// prepare line buffers 
			fCurrPix = fNextLn; 
			fNextLn = fCurrLn; 
			fCurrLn = fCurrPix; 
			::ZeroMemory(fNextLn, dwBuffLn); 

			y++; 
			pOutLine += dwOutLn; 
		} 
		pYCoeff += 2; 
	} 

	delete pRowCoeff; 
	delete pColCoeff; 
	delete fBuff; 

} 

void EnlargeData(BYTE *pInBuff, WORD wWidth, WORD wHeight, 
				 BYTE *pOutBuff, WORD wNewWidth, WORD wNewHeight) 
{ 
	BYTE  *pLine = pInBuff, *pPix 
		= pLine, *pPixOld, *pUpPix, *pUpPixOld; 
	BYTE  *pOutLine = pOutBuff, *pOutPix; 
	DWORD dwInLn = (3 * wWidth + 3) & ~3; 
	DWORD dwOutLn = (3 * wNewWidth + 3) & ~3; 
	int   x, y, i; 
	BOOL  bCrossRow, bCrossCol; 
	float *pRowCoeff = CreateCoeff(wNewWidth, wWidth, FALSE); 
	float *pColCoeff = CreateCoeff(wNewHeight, wHeight, FALSE); 
	float fTmp, fPtTmp[3], *pXCoeff, *pYCoeff = pColCoeff; 

	y = 0; 
	while(y < wHeight) 
	{ 
		bCrossRow = pYCoeff[1] > F_DELTA; 
		x = 0; 
		pXCoeff = pRowCoeff; 
		pOutPix = pOutLine; 
		pOutLine += dwOutLn; 
		pUpPix = pLine; 
		if(fabs(pYCoeff[1]) > F_DELTA) 
		{ 
			y++; 
			pLine += dwInLn; 
			pPix = pLine; 
		} 

		while(x < wWidth) 
		{ 
			bCrossCol = pXCoeff[1] > F_DELTA;
			pUpPixOld = pUpPix; 
			pPixOld = pPix; 
			if(fabs(pXCoeff[1]) > F_DELTA) 
			{ 
				x++; 
				pUpPix += 3; 
				pPix += 3; 
			} 
			fTmp = *pXCoeff * *pYCoeff; 
			for(i = 0; i < 3; i++) 
				fPtTmp[i] = fTmp * pUpPixOld[i]; 
			if(bCrossCol) 
			{ 
				fTmp = pXCoeff[1] * *pYCoeff; 
				for(i = 0; i < 3; i++) 
					fPtTmp[i] += fTmp * pUpPix[i]; 
			} 
			if(bCrossRow) 
			{ 
				fTmp = *pXCoeff * pYCoeff[1]; 
				for(i = 0; i < 3; i++) 
					fPtTmp[i] += fTmp * pPixOld[i]; 
				if(bCrossCol) 
				{ 
					fTmp = pXCoeff[1] * pYCoeff[1]; 
					for(i = 0; i < 3; i++) 
						fPtTmp[i] += fTmp * pPix[i]; 
				} 
			} 
			for(i = 0; i < 3; i++, pOutPix++) 
				*pOutPix = (BYTE)fPtTmp[i]; 
			pXCoeff += 2; 
		} 
		pYCoeff += 2; 
	} 

	delete pRowCoeff; 
	delete pColCoeff; 
} 

