#include "stdafx.h"
#include <list>
#include <map>
#include <Gdiplus.h>
#include "TraceLog.h"
#include <tlhelp32.h>
#include <shlwapi.h>
#include <winternl.h>
#include <afxinet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <io.h>
#include "common.h"
#include <WinCrypt.h>        


unsigned long GetFileSize(LPCTSTR fullpath)
{
	unsigned long retval = 0;

	int fdes = _wopen(fullpath, O_RDONLY);
	if (fdes<0){
		return retval;
	}
	struct stat statBuf;
	if (fstat(fdes, &statBuf) == 0){
		retval = statBuf.st_size;
	}
	close(fdes);
	return retval;
}

bool IsLocalExist(LPCTSTR fullpath)
{
	//TraceLog((_T("IsLocalExist(%s)"), fullpath));
	CFileFind ff;
	BOOL bFind = ff.FindFile(fullpath);
	ff.Close();
	if (bFind)
	{
		TraceLog((_T("LocalExist(%s)"), fullpath));
		return true;
	}
	TraceLog((_T("Local Not Exist(%s)"), fullpath));
	return false;
}
CString	FindExtension(const CString& strFileName)
{
	int nlen = strFileName.GetLength();
	int i;
	for (i = nlen - 1; i >= 0; i--){
		if (strFileName[i] == '.'){
			return strFileName.Mid(i + 1);
		}
	}
	return CString(_T(""));
}

LPSTREAM LoadStreamFromFile(LPCTSTR szFileName, HGLOBAL& hGlobal)
{
	TraceLog((_T("LoadImageFromFrile(%s)"), szFileName));
	HANDLE	hFile = INVALID_HANDLE_VALUE;
	LPVOID	pvData = NULL;

	hGlobal = NULL;

	try
	{
		HRESULT hr;
		DWORD	dwFileSize;

		// 파일 열기
		hFile = CreateFile(szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);

		// 파일이 없을 경우
		if (hFile == INVALID_HANDLE_VALUE)
		{
			TraceLog((_T("GetBitmapFromFile() File Open Error")));
			throw - 1;
		}

		// 파일 크기 얻기
		dwFileSize = GetFileSize(hFile, NULL);
		if (dwFileSize == -1)
		{
			TraceLog((_T("GetBitmapFromFile() File Read Error")));
			throw - 1;
		}

		// 파일 크기만큼 동적할당
		hGlobal = GlobalAlloc(GMEM_MOVEABLE, dwFileSize);

		if (hGlobal == NULL)
		{
			TraceLog((_T("GetBitmapFromFile() GlobalAlloc Error")));
			throw - 1;
		}

		pvData = GlobalLock(hGlobal);

		if (pvData == NULL)
		{
			TraceLog((_T("GetBitmapFromFile() GlobalLock Error")));
			throw - 1;
		}

		DWORD dwBytesRead = 0;
		BOOL bRead = ReadFile(hFile, pvData, dwFileSize, &dwBytesRead, NULL);

		if (!bRead)
		{
			TraceLog((_T("GetBitmapFromFile() ReadFile Error")));
			throw - 1;
		}

		GlobalUnlock(hGlobal);
		CloseHandle(hFile);

		LPSTREAM pstm = NULL;

		hr = CreateStreamOnHGlobal(hGlobal, TRUE, &pstm);

		if (S_OK != hr || pstm == NULL)
		{
			TraceLog((_T("CreateStreamOnHGlobal failed")));
			throw - 1;
		}
		return pstm;
		/*
		pstm->Release();
		GlobalFree(hGlobal);
		*/
	}

	catch (int e)
	{
		UNREFERENCED_PARAMETER(e);

		if (hFile != INVALID_HANDLE_VALUE)
			CloseHandle(hFile);

		if (hGlobal)
		{
			GlobalUnlock(hGlobal);
			GlobalFree(hGlobal);
		}
		return NULL;
	}
	return NULL;
}

void* LoadImageFromFileWithoutLocking(const WCHAR* fileName)
{
	//using namespace Gdiplus;
	Gdiplus::Bitmap src(fileName);
	if (src.GetLastStatus() != Gdiplus::Ok) {
		return 0;
	}
	Gdiplus::Bitmap *dst = new Gdiplus::Bitmap(src.GetWidth(), src.GetHeight(), PixelFormat32bppARGB);

	Gdiplus::BitmapData srcData;
	Gdiplus::BitmapData dstData;
	Gdiplus::Rect rc(0, 0, src.GetWidth(), src.GetHeight());

	if (src.LockBits(&rc, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, &srcData) == Gdiplus::Ok)
	{
		if (dst->LockBits(&rc, Gdiplus::ImageLockModeWrite, PixelFormat32bppARGB, &dstData) == Gdiplus::Ok) {
			unsigned char * srcBits = (unsigned char *)srcData.Scan0;
			unsigned char * dstBits = (unsigned char *)dstData.Scan0;
			unsigned int stride;
			if (srcData.Stride > 0) {
				stride = srcData.Stride;
			}
			else {
				stride = -srcData.Stride;
			}
			memcpy(dstBits, srcBits, src.GetHeight() * stride);

			dst->UnlockBits(&dstData);
		}
		src.UnlockBits(&srcData);
	}
	return dst;
}


void* LoadImageFromFileWithoutLocking(const WCHAR* fileName, int left, int top, int width, int height)
{
	//using namespace Gdiplus;
	Gdiplus::Bitmap src(fileName);
	if (src.GetLastStatus() != Gdiplus::Ok) {
		return 0;
	}

	if (src.GetWidth() < left + width)
	{
		width = src.GetWidth() - left;
	}
	if (src.GetHeight() < top + height)
	{
		height = src.GetHeight() - top;
	}

	Gdiplus::Bitmap *dst = new Gdiplus::Bitmap(width, height, PixelFormat32bppARGB);

	Gdiplus::BitmapData srcData;
	Gdiplus::BitmapData dstData;
	Gdiplus::Rect rcSrc(0, 0, width, height);
	Gdiplus::Rect rcDst(0, 0, width, height);

	if (src.LockBits(&rcSrc, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, &srcData) == Gdiplus::Ok)
	{
		if (dst->LockBits(&rcSrc, Gdiplus::ImageLockModeWrite, PixelFormat32bppARGB, &dstData) == Gdiplus::Ok) {
			unsigned char * srcBits = (unsigned char *)srcData.Scan0;
			unsigned char * dstBits = (unsigned char *)dstData.Scan0;
			unsigned int stride;
			if (srcData.Stride > 0) {
				stride = srcData.Stride;
			}
			else {
				stride = -srcData.Stride;
			}
			memcpy(dstBits, srcBits, src.GetHeight() * stride);

			dst->UnlockBits(&dstData);
		}
		src.UnlockBits(&srcData);
	}
	return dst;
}

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
	UINT  num = 0;          // number of image encoders
	UINT  size = 0;         // size of the image encoder array in bytes

	Gdiplus::ImageCodecInfo* pImageCodecInfo = NULL;

	Gdiplus::GetImageEncodersSize(&num, &size);
	if (size == 0)
		return -1;  // Failure

	pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
	if (pImageCodecInfo == NULL)
		return -1;  // Failure

	Gdiplus::GetImageEncoders(num, size, pImageCodecInfo);

	for (UINT j = 0; j < num; ++j)
	{
		if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;  // Success
		}
	}

	free(pImageCodecInfo);
	return -1;  // Failure
}

bool SaveImageFile(LPCTSTR saveFileName, Gdiplus::Image* gdiImage)
{
	const WCHAR* format = _T("image/jpeg");

		CLSID pngClsid;
	if (GetEncoderClsid(format, &pngClsid) < 0)
	{
		TraceLog((_T("ERROR : Get codec failed")));
	}
	gdiImage->Save(saveFileName, &pngClsid);

	delete gdiImage;
	TraceLog((_T("%s Thumbnail file created"), saveFileName));

	return true;
}

CString	GetExt(const CString& strFileName)
{
	int nlen = strFileName.GetLength();
	int i;
	for (i = nlen - 1; i >= 0; i--){
		if (strFileName[i] == '.'){
			return strFileName.Mid(i + 1);
		}
	}
	return CString(_T(""));
}

unsigned long
getPid(LPCTSTR exename, bool likeCond/*=false*/)
{
	TraceLog((_T("getPid(%s)"), exename));

	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnapshot == INVALID_HANDLE_VALUE) {
		TraceLog((_T("HANDLE을 생성할 수 없습니다")));
		return 0;
	}
	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(PROCESSENTRY32);

	//char strProcessName[512];

	if (!Process32First(hSnapshot, &pe32))	{
		TraceLog((_T("Process32First failed.")));
		::CloseHandle(hSnapshot);
		return 0;
	}


	do 	{
		//memset(strProcessName, 0, sizeof(strProcessName));
		//size_t stringLength = strlen(pe32.szExeFile);
		//for(int i=0; i<stringLength; i++) // 대문자로 전환
		//	strProcessName[i] = toupper( pe32.szExeFile[i] );

		if (likeCond)
		{
			std::wstring exename1 = pe32.szExeFile;
			std::wstring exename2 = exename;
			::wcslwr((wchar_t*)exename1.c_str());
			::wcslwr((wchar_t*)exename2.c_str());

			if (wcsstr(exename1.c_str(), exename2.c_str()) != NULL) {
				TraceLog((_T("process founded(%s)"), exename));
				::CloseHandle(hSnapshot);
				return pe32.th32ProcessID;
			}
		}
		else if (wcsicmp(pe32.szExeFile, exename) == 0) {
			TraceLog((_T("process founded(%s)"), exename));
			::CloseHandle(hSnapshot);
			return pe32.th32ProcessID;
		}

	} while (Process32Next(hSnapshot, &pe32));
	TraceLog((_T("process not founded(%s)"), exename));
	::CloseHandle(hSnapshot);

	return 0;
}

typedef struct {
	DWORD pid;
	HWND hwnd;
} find_hwnd_from_pid_t;

static BOOL CALLBACK find_hwnd_from_pid_proc(HWND hwnd, LPARAM lParam)
{
	if (!IsWindowVisible(hwnd)) return TRUE;
	DWORD pid;
	GetWindowThreadProcessId(hwnd, &pid);
	find_hwnd_from_pid_t *pe = (find_hwnd_from_pid_t *)lParam;
	if (pe->pid != pid) return TRUE;
	pe->hwnd = hwnd;
	return FALSE;
}


HWND
getWHandle(unsigned long pid)
{
	TraceLog((_T("getWHandle(%ld)"), pid));
	if (pid>0){
		find_hwnd_from_pid_t e;
		e.pid = pid;
		e.hwnd = NULL;
		EnumWindows(find_hwnd_from_pid_proc, (LPARAM)&e);

		HWND child = e.hwnd;
		HWND parent = NULL;
		while (1){
			parent = GetParent(child);
			if (!parent){
				return child;
			}
			child = parent;
		}
		return e.hwnd;
	}
	return (HWND)0;
}
HWND
getWHandle(LPCTSTR exename, bool likeCond/*=false*/)
{
	TraceLog((_T("getWHandle(%s)"), exename));
	return getWHandle(getPid(exename, likeCond));
}

bool HttpUploadFile(TCHAR * pszUrl, TCHAR * pszFilePath)
{
	bool bRes = false;

	// pszUrl 에서 host, path 를 가져온다.
	TCHAR * pszHost = NULL;
	DWORD dwFlag = 0;
	int iPort = 80;

	if (!_wcsnicmp(pszUrl, _T("http://"), 7))
	{
		pszHost = pszUrl + 7;
	}
	else if (!_wcsnicmp(pszUrl, _T("https://"), 8))
	{
		// https 에서 테스트하지 않았음.?
		pszHost = pszUrl + 8;
		dwFlag = INTERNET_FLAG_SECURE;
		iPort = 443;
	}
	else
	{
		return false;
	}

	const TCHAR * pszPath = wcsstr(pszHost, _T("/"));
	if (pszPath == NULL)
	{
		return false;
	}

	std::wstring strHost;
	strHost.append(pszHost, pszPath - pszHost);

	const TCHAR * pszPort = wcsstr(strHost.c_str(), _T(":"));
	if (pszPort)
	{
		iPort = _wtoi(pszPort + 1);
		if (iPort <= 0) return false;

		strHost.erase(pszPort - strHost.c_str());
	}

	// 파일 경로에서 파일 이름을 가져온다.
	int iLen = wcslen(pszFilePath);
	TCHAR * pszFileName = NULL;
	for (int i = iLen - 1; i >= 0; --i)
	{
		if (pszFilePath[i] == L'\\')
		{
			pszFileName = pszFilePath + i + 1;
			break;
		}
	}

	// 파일 크기를 가져온다.
	struct _stat sttStat;
	if (_wstat(pszFilePath, &sttStat) == -1)
	{
		return false;
	}

	FILE * fd = _wfopen(pszFilePath, _T("rb"));
	if (fd == NULL)
	{
		return false;
	}
	else
	{
		char szBuf[8192];
		CInternetSession clsSession;

		// HTTP 연결하고 파일을 전송한다.
		CHttpConnection * pclsHttpConn = clsSession.GetHttpConnection(strHost.c_str(), dwFlag, (INTERNET_PORT)iPort, NULL, NULL);
		if (pclsHttpConn)
		{
			CHttpFile * pclsHttpFile = pclsHttpConn->OpenRequest(CHttpConnection::HTTP_VERB_POST, pszPath);
			if (pclsHttpFile)
			{
				// HTTP 요청 header 를 전송한다.
				pclsHttpFile->AddRequestHeaders(_T("Content-Type: application/octet-stream\r\n"));

				/* 업로드 파일 이름을 Content-Disposition 헤더에 저장하여서 전달하는 경우에 사용한다.
				std::string strContentDisposition = _T("Content-Disposition: ");
				strContentDisposition.append( _T("attachment; filename=\"") );
				strContentDisposition.append( pszFileName );
				strContentDisposition.append( _T("\"\r\n") );
				pclsHttpFile->AddRequestHeaders( strContentDisposition.c_str() );
				*/
				try
				{
					pclsHttpFile->SendRequestEx(sttStat.st_size, HSR_SYNC | HSR_INITIATE);
					// HTTP 요청 body 를 전송한다.
					while (1)
					{
						iLen = fread(szBuf, 1, sizeof(szBuf), fd);
						if (iLen <= 0) break;

						pclsHttpFile->Write(szBuf, iLen);
					}

					fclose(fd);
					pclsHttpFile->EndRequest(HSR_SYNC);

					// HTTP 응답 body 를 수신한다.
					std::string strResponse;
					DWORD dwCode;
					while (1)
					{
						iLen = pclsHttpFile->Read(szBuf, sizeof(szBuf));
						if (iLen <= 0) break;
						strResponse.append(szBuf, iLen);
					}

					// 응답 코드가 200 OK 인지 확인한다.
					pclsHttpFile->QueryInfoStatusCode(dwCode);
					if (dwCode == HTTP_STATUS_OK)
					{
						bRes = true;
					}
				}
				catch (CInternetException * pclsException)
				{
					// 웹서버 연결에 실패하면 ?CInternetException 이 발생한다.
				}
				// CHttpFile 의 Close 메소드를 호출해도 메모리 누수가 발생하는 것 같습니다.
				// delete 를 해 주어야 메모리 누수 현상이 없어지는 것 같네요.
				delete pclsHttpFile;
			}
			delete pclsHttpConn;
		}
	}
	return bRes;
}

CString GetLocalIP(void)
{
	WSADATA wsaData;
	char name[255];

	CString ip; // ip 저장.

	PHOSTENT hostinfo;

	if (WSAStartup(MAKEWORD(2, 0), &wsaData) == 0)
	{
		if (gethostname(name, sizeof(name)) == 0)
		{
			if ((hostinfo = gethostbyname(name)) != NULL)
			{
				ip = inet_ntoa(*(struct in_addr *)*hostinfo->h_addr_list);
			}
		}
		WSACleanup();
	}
	return ip;

}

bool GetValueFromJson(CString& targetJson, LPCTSTR name, LPCTSTR deli, bool isString, CString& outVal)
{
	CString token = name;
	int startpos = targetJson.Find(name);
	if (startpos > 0)
	{
		startpos += token.GetLength();
		CString halfLine = targetJson.Mid(startpos);
		int endpos = halfLine.Find(deli);
		if (endpos > 0)
		{
			outVal = halfLine.Mid(0, endpos);
			outVal.Remove(_T(' '));
			if (isString)
			{
				outVal.Remove(_T('\"'));
			}
			TraceLog((_T("skpark json (%s=%s)"), name, outVal));
			return true;
		}
	}
	return false;
}

unsigned long
createProcess(LPCTSTR exename, LPCTSTR arg, LPCTSTR dir, BOOL minFlag)
{
	TraceLog((_T("_createProcess : exename(%s %s)\n"), exename, arg));

	STARTUPINFO            si;
	PROCESS_INFORMATION     pi;
	ZeroMemory(&si, sizeof(si));
	if (minFlag) {
		si.dwFlags = STARTF_USESHOWWINDOW;
		si.wShowWindow = SW_MINIMIZE;
	}
	si.lpDesktop = _T("Winsta0\\Default");

	if (!dir || !wcslen(dir)){
		dir = _T("C:\\SQISOFT\\UTV1.0\\execute\\");
	}

	CString c_exename = dir;
	c_exename += exename;
	CString c_dir = dir;

	BOOL bRun = CreateProcess(
		c_exename,
		(LPTSTR)arg,
		NULL,
		NULL,
		FALSE,
		NORMAL_PRIORITY_CLASS,
		NULL,
		dir,
		&si,
		&pi
		);

	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	if (bRun)
	{
		TraceLog((_T("createProcess : pid(%d)"), pi.dwProcessId));
		return pi.dwProcessId;
	}
	TraceLog((_T("createProcess : fail to CreateProcess")));
	return 0;
}

void SetForegroundWindowForce(HWND hWnd)
{
	HWND hForeground = ::GetForegroundWindow();
	if (hForeground == hWnd) return;

	DWORD foreground_id = ::GetWindowThreadProcessId(hForeground, NULL);
	DWORD id = ::GetWindowThreadProcessId(hWnd, NULL);
	if (AttachThreadInput(id, foreground_id, TRUE))
	{
		::SetForegroundWindow(hWnd);
		::BringWindowToTop(hWnd);
		::AttachThreadInput(id, foreground_id, FALSE);
	}
}

int killProcess(unsigned long pid)
{

	if (pid <= 0) return -1;

	//printf(("killProcess : pid(%ld)",pid));
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if ((int)hSnapshot == -1) {
		//ciWARN( ("killProcess : fail to CreateToolhelp32Snapshot") );
		return 0;
	}
	//ciDEBUG(("killProcess : success to CreateToolhelp32Snapshot") );

	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(PROCESSENTRY32);
	BOOL bContinue;

	if (!Process32First(hSnapshot, &pe32)) {
		//ciWARN( ("killProcess : fail to killProcess") );
		return 0;
	}

	do {
		if (pe32.th32ProcessID == pid) {
			HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, pe32.th32ProcessID);
			if (hProcess) {
				DWORD   dwExitCode;
				GetExitCodeProcess(hProcess, &dwExitCode);
				::TerminateProcess(hProcess, dwExitCode);
				//SafeTerminateProcess( hProcess, dwExitCode );
				CloseHandle(hProcess);
				CloseHandle(hSnapshot);
				//ciDEBUG(1,("killProcess : success to Closehandle") );
				return 1;
			}
		}
		bContinue = Process32Next(hSnapshot, &pe32);
	} while (bContinue);
	CloseHandle(hSnapshot);
	//ciERROR(("killProcess : Process is not found.") );
	return -1;
}

void OpenIExplorer(CString strParam, int cx/*=1024*/, int cy/*=768*/)
{
	HRESULT hr;
	IWebBrowser2* pWebBrowser = NULL;

	if (NULL != pWebBrowser)
	{
		pWebBrowser->Quit();
		pWebBrowser->Release();
		pWebBrowser = NULL;
	}

	if (FAILED(hr = ::CoCreateInstance(CLSID_InternetExplorer, NULL, CLSCTX_LOCAL_SERVER, IID_IWebBrowser2, (void**)&pWebBrowser)))
	{
		//		CoUninitialize();
	}

	CString strHeader, strTarget, strUrl;
	strHeader = _T("Content-Type: application/x-www-form-urlencoded\r\n");
	strTarget = _T("_top"); // 새로운 창으로 띄움
	strUrl = strParam;

	VARIANT vtHeader = { 0 }, vtTarget = { 0 }, vtEmpty = { 0 }, vtPostData = { 0 };

	vtHeader.vt = VT_BSTR;
	vtHeader.bstrVal = strHeader.AllocSysString();

	vtTarget.vt = VT_BSTR;
	vtTarget.bstrVal = strTarget.AllocSysString();

	::VariantInit(&vtEmpty);

	pWebBrowser->put_ToolBar(VARIANT_FALSE);		// 익스플로어 툴바 없앰
	pWebBrowser->put_MenuBar(VARIANT_FALSE);		// 메뉴바 없앰
	pWebBrowser->put_AddressBar(VARIANT_FALSE);		// 주소창 없앰
	pWebBrowser->put_StatusBar(VARIANT_FALSE);		// 상태바 없앰
	pWebBrowser->put_Visible(VARIANT_TRUE);
	pWebBrowser->put_Top(0);
	pWebBrowser->put_Left(0);
	pWebBrowser->put_Width(cx);
	pWebBrowser->put_Height(cy);

	pWebBrowser->put_Visible(VARIANT_TRUE);
	hr = pWebBrowser->Navigate(strUrl.AllocSysString(), &vtEmpty, &vtTarget, &vtPostData, &vtHeader);

	SHANDLE_PTR IEHwnd;
	pWebBrowser->get_HWND(&IEHwnd);

	SetForegroundWindow((HWND)IEHwnd);

	if (!SUCCEEDED(hr))
	{
		CString msg = _T("HyperLink Error");
		if (E_INVALIDARG == hr)
			msg += _T(": Invalid Parameters.");
		else if (E_OUTOFMEMORY == hr)
			msg += _T(": Out of memory.");
	}

	SysFreeString(vtHeader.bstrVal);
	SysFreeString(vtTarget.bstrVal);
	pWebBrowser->Release();
}

void showTaskbar(bool bShow)
{
	CWnd* pWnd = CWnd::FindWindow(_T("Shell_TrayWnd"), _T(""));
	if (pWnd){
		if (bShow){
			pWnd->ShowWindow(SW_SHOW);
			pWnd->SetFocus();
		}
		else{
			pWnd->ShowWindow(SW_HIDE);
		}
	}
}

void yv12toYUV(char *outYuv, char *inYv12, int width, int height, int widthStep)
{
	int col, row;
	unsigned int Y, U, V;
	int tmp;
	int idx;
	for (row = 0; row<height; row++) {
		idx = row * widthStep;
		int rowptr = row*width;

		for (col = 0; col<width; col++) {

			tmp = (row / 2)*(width / 2) + (col / 2);

			Y = (unsigned int)inYv12[row*width + col];
			U = (unsigned int)inYv12[width*height + width*height / 4 + tmp];
			V = (unsigned int)inYv12[width*height + tmp];

			if ((idx + col * 3 + 2)>(1200 * widthStep)) {
				//printf("row * widthStep=%d,idx+col*3+2=%d.\n",1200 * widthStep,idx+col*3+2);  
			}
			outYuv[idx + col * 3] = Y;
			outYuv[idx + col * 3 + 1] = U;
			outYuv[idx + col * 3 + 2] = V;
		}
	}
}

void decodeI420(U8 yuv[], int width, int height, int pitch, U8 rgb[])
{
	int frameSize = width * height;
	int w, h;
	int yp, y, y1m, up, u, vp, v, rgbp, r, g, b;

	for (h = height - 1; h >= 0; h--)
	{
		yp = h * width;
		up = frameSize + (h >> 1) * (width >> 1);
		vp = frameSize + (frameSize >> 2) + (h >> 1) * (width >> 1);
		rgbp = (height - h - 1) * pitch;
		for (w = 0; w < width; w++, yp++)
		{
			y = (int)yuv[yp];
			if ((w & 1) == 0)
			{
				u = ((int)yuv[up++]) - 128;
				v = ((int)yuv[vp++]) - 128;
			}

			y1m = y << 20;
			r = (y1m + 1475871 * v);
			g = (y1m - 751724 * v - 362283 * u);
			b = (y1m + 1865417 * u);

			r = (r + 524288);
			g = (g + 524288);
			b = (b + 524288);
			if (r < 0) r = 0; else if (r > 268435455) r = 268435455;
			if (g < 0) g = 0; else if (g > 268435455) g = 268435455;
			if (b < 0) b = 0; else if (b > 268435455) b = 268435455;

			rgb[rgbp++] = b >> 20;
			rgb[rgbp++] = g >> 20;
			rgb[rgbp++] = r >> 20;
		}
	}
}

void decodeYV12(U8 yuv[], int width, int height, int pitch, U8 rgb[])
{
	int frameSize = width * height;
	int w, h;
	int yp, y, y1m, up, u, vp, v, rgbp, r, g, b;

	for (h = height - 1; h >= 0; h--)
	{
		yp = h * width;
		vp = frameSize + (h >> 1) * (width >> 1);
		up = frameSize + (frameSize >> 2) + (h >> 1) * (width >> 1);
		rgbp = (height - h - 1) * pitch;
		for (w = 0; w < width; w++, yp++)
		{
			y = (int)yuv[yp];
			if ((w & 1) == 0)
			{
				v = ((int)yuv[vp++]) - 128;
				u = ((int)yuv[up++]) - 128;
			}

			y1m = y << 20;
			r = (y1m + 1475871 * v);
			g = (y1m - 751724 * v - 362283 * u);
			b = (y1m + 1865417 * u);

			r = (r + 524288);
			g = (g + 524288);
			b = (b + 524288);
			if (r < 0) r = 0; else if (r > 268435455) r = 268435455;
			if (g < 0) g = 0; else if (g > 268435455) g = 268435455;
			if (b < 0) b = 0; else if (b > 268435455) b = 268435455;

			rgb[rgbp++] = b >> 20;
			rgb[rgbp++] = g >> 20;
			rgb[rgbp++] = r >> 20;
		}
	}
}

void decodeNV12(U8 yuv[], int width, int height, int pitch, U8 rgb[])
{
	int frameSize = width * height;
	int w, h;
	int yp, y, y1m, uvp, u, v, rgbp, r, g, b;

	for (h = height - 1; h >= 0; h--)
	{
		yp = h * width;
		uvp = frameSize + (h >> 1) * width;
		rgbp = (height - h - 1) * pitch;
		for (w = 0; w < width; w++, yp++)
		{
			y = (int)yuv[yp];
			if ((w & 1) == 0)
			{
				u = ((int)yuv[uvp++]) - 128;
				v = ((int)yuv[uvp++]) - 128;
			}

			y1m = y << 20;
			r = (y1m + 1475871 * v);
			g = (y1m - 751724 * v - 362283 * u);
			b = (y1m + 1865417 * u);

			r = (r + 524288);
			g = (g + 524288);
			b = (b + 524288);
			if (r < 0) r = 0; else if (r > 268435455) r = 268435455;
			if (g < 0) g = 0; else if (g > 268435455) g = 268435455;
			if (b < 0) b = 0; else if (b > 268435455) b = 268435455;

			rgb[rgbp++] = b >> 20;
			rgb[rgbp++] = g >> 20;
			rgb[rgbp++] = r >> 20;
		}
	}
}

void decodeNV21(U8 yuv[], int width, int height, int pitch, U8 rgb[])
{
	int frameSize = width * height;
	int w, h;
	int yp, y, y1m, uvp, u, v, rgbp, r, g, b;

	for (h = height - 1; h >= 0; h--)
	{
		yp = h * width;
		uvp = frameSize + (h >> 1) * width;
		rgbp = (height - h - 1) * pitch;
		for (w = 0; w < width; w++, yp++)
		{
			y = (int)yuv[yp];
			if ((w & 1) == 0)
			{
				v = ((int)yuv[uvp++]) - 128;
				u = ((int)yuv[uvp++]) - 128;
			}

			y1m = y << 20;
			r = (y1m + 1475871 * v);
			g = (y1m - 751724 * v - 362283 * u);
			b = (y1m + 1865417 * u);

			r = (r + 524288);
			g = (g + 524288);
			b = (b + 524288);
			if (r < 0) r = 0; else if (r > 268435455) r = 268435455;
			if (g < 0) g = 0; else if (g > 268435455) g = 268435455;
			if (b < 0) b = 0; else if (b > 268435455) b = 268435455;

			rgb[rgbp++] = b >> 20;
			rgb[rgbp++] = g >> 20;
			rgb[rgbp++] = r >> 20;
		}
	}
}

BitmapRGB* yuv2bmp(YUV_FORMAT format, U8 yuv[], int width, int height)
{
	BitmapRGB* bmp = NULL;
	BitmapInfo* bmpInfo = NULL;
	U32 pitch = ((width * 24 + 31) >> 5) << 2; // size of a row, include pad.
	U32 dataSize = pitch * height;

	bmp = (BitmapRGB*)malloc(sizeof(BitmapInfo)+dataSize);
	memset(bmp, 0x00, sizeof(BitmapInfo)+dataSize);
	bmpInfo = &bmp->bInfo;

	//Let user write it by himself.
	//bmpInfo->bfHeader.bfType = 0x4D42;  // "BM"
	bmpInfo->bfHeader.bfSize = 2 + sizeof(BitmapInfo)+dataSize;
	bmpInfo->bfHeader.bfReserved1 = 0;
	bmpInfo->bfHeader.bfReserved2 = 0;
	bmpInfo->bfHeader.bfOffBits = 2 + sizeof(BitmapInfo);

	bmpInfo->biHeader.biSize = sizeof(BitmapInfoHeader);
	bmpInfo->biHeader.biWidth = width;
	bmpInfo->biHeader.biHeight = height;
	bmpInfo->biHeader.biPlanes = 1;
	bmpInfo->biHeader.biBitCount = 24;
	bmpInfo->biHeader.biCompression = BI_RGB;
	bmpInfo->biHeader.biSizeImage = dataSize;
	bmpInfo->biHeader.biXPelsPerMeter = 0;
	bmpInfo->biHeader.biYPelsPerMeter = 0;
	bmpInfo->biHeader.biClrUsed = 0;
	bmpInfo->biHeader.biClrImportant = 0;

	switch (format)
	{
	case YUV_I420:
		decodeI420(yuv, width, height, pitch, bmp->bData.rgb);
		break;
	case YUV_YV12:
		decodeYV12(yuv, width, height, pitch, bmp->bData.rgb);
		break;
	case YUV_NV12:
		decodeNV12(yuv, width, height, pitch, bmp->bData.rgb);
		break;
	case YUV_NV21:
		decodeNV21(yuv, width, height, pitch, bmp->bData.rgb);
		break;
	default:
		break;
	}

	return bmp;
}


int yuv2bmpfile(YUV_FORMAT format, U8 yuv[], int width, int height, LPCTSTR file)
{
	FILE* fp = NULL;
	int size = 0;
	BitmapRGB* bmp = yuv2bmp(format, yuv, width, height);

	if (bmp == NULL)
	{
		TraceLog((_T("skpark yuv2bmpfile failed")))
			return 0;
	}


	fp = _wfopen(file, _T("wb"));
	if (fp == NULL)
	{
		free(bmp);
		return 0;
	}

	fwrite(BITMAP_HEADER_TYPE, 1, wcslen(BITMAP_HEADER_TYPE), fp);
	size = bmp->bInfo.bfHeader.bfSize;
	fwrite(bmp, 1, size - 2, fp);
	fclose(fp);
	free(bmp);

	return size;
}

int yuv2jpgfile(YUV_FORMAT format, U8 yuv[], int width, int height, LPCTSTR file)
{
	int size = 0;
	BitmapRGB* bmp = yuv2bmp(format, yuv, width, height);

	if (bmp == NULL)
	{
		TraceLog((_T("skpark FRRetry bmp is null")));
		return 0;
	}


	BITMAPINFO bmi;
	bmi.bmiHeader.biSize = bmp->bInfo.biHeader.biSize;
	bmi.bmiHeader.biWidth = bmp->bInfo.biHeader.biWidth;
	bmi.bmiHeader.biHeight = bmp->bInfo.biHeader.biHeight;
	bmi.bmiHeader.biPlanes = bmp->bInfo.biHeader.biPlanes;
	bmi.bmiHeader.biBitCount = bmp->bInfo.biHeader.biBitCount;
	bmi.bmiHeader.biCompression = bmp->bInfo.biHeader.biCompression;
	bmi.bmiHeader.biSizeImage = bmp->bInfo.biHeader.biSizeImage;
	bmi.bmiHeader.biXPelsPerMeter = bmp->bInfo.biHeader.biXPelsPerMeter;
	bmi.bmiHeader.biYPelsPerMeter = bmp->bInfo.biHeader.biYPelsPerMeter;
	bmi.bmiHeader.biClrUsed = bmp->bInfo.biHeader.biClrUsed;
	bmi.bmiHeader.biClrImportant = bmp->bInfo.biHeader.biClrImportant;
	bmi.bmiColors->rgbRed = 0;
	bmi.bmiColors->rgbBlue = 0;
	bmi.bmiColors->rgbGreen = 0;
	bmi.bmiColors->rgbReserved = 0;

	TraceLog((_T("skpark bmp = (%ld x %ld)"), bmp->bInfo.biHeader.biWidth, bmp->bInfo.biHeader.biHeight));

	Gdiplus::Bitmap* gdiBmp = Gdiplus::Bitmap::FromBITMAPINFO(&bmi, bmp->bData.rgb);
	//Gdiplus::Bitmap* gdiBmp = new Gdiplus::Bitmap(L"plot.bmp");

	CLSID   encoderClsid;
	Gdiplus::Status  stat;
	Gdiplus::EncoderParameters encoderParameters;
	ULONG    quality;

	//Gdiplus::REAL dpi = 72;
	Gdiplus::REAL dpi = 72;
	gdiBmp->SetResolution(dpi, dpi);


	// Get the CLSID of the PNG encoder.
	GetEncoderClsid(_T("image/jpeg"), &encoderClsid);

	encoderParameters.Count = 1;
	encoderParameters.Parameter[0].Guid = Gdiplus::EncoderQuality;
	encoderParameters.Parameter[0].Type = Gdiplus::EncoderParameterValueTypeLong;
	encoderParameters.Parameter[0].NumberOfValues = 1;

	quality = 50;
	encoderParameters.Parameter[0].Value = &quality;

	/*WCHAR wszPath[250];
	size_t mbstowcs_ret = 0;
	mbstowcs_s(&mbstowcs_ret, wszPath, file, wcslen(file));*/

	//stat = gdiBmp->Save(wszPath, &encoderClsid, &encoderParameters);
	stat = gdiBmp->Save(file, &encoderClsid, &encoderParameters);

	delete gdiBmp;

	size = bmp->bInfo.biHeader.biSize;
	free(bmp);
	TraceLog((_T("skpark FRRetry=%d"), size));
	return  size;
}

bool Image2Bytes(const CImage& img, BYTE** bytes, size_t& byteSize)
{
	if (img.IsNull()) return false;
	IStream* pStrImg = NULL;
	if (CreateStreamOnHGlobal(NULL, TRUE, &pStrImg) != S_OK) return false;
	img.Save(pStrImg, Gdiplus::ImageFormatBMP);
	HGLOBAL hGlobalImg = NULL;
	GetHGlobalFromStream(pStrImg, &hGlobalImg);
	BYTE* pBits = (BYTE*)GlobalLock(hGlobalImg);
	byteSize = GlobalSize(pBits);
	*bytes = new BYTE[byteSize];
	memcpy(*bytes, pBits, byteSize);
	GlobalUnlock(hGlobalImg);
	pStrImg->Release();
	GlobalFree(hGlobalImg);
	return true;
}

bool Bytes2Image(const BYTE* bytes, const size_t byteSize, CImage& img)
{
	if (bytes == NULL) return false;
	HGLOBAL hGlobalImg = GlobalAlloc(GMEM_MOVEABLE, byteSize);
	BYTE* pBits = (BYTE*)GlobalLock(hGlobalImg);
	memcpy(pBits, bytes, byteSize);
	GlobalUnlock(hGlobalImg);
	IStream* pStrImg = NULL;
	if (CreateStreamOnHGlobal(hGlobalImg, TRUE, &pStrImg) != S_OK) {
		GlobalFree(hGlobalImg);
		return false;
	}
	//if (!img.IsNull()) img.Destroy();
	if (E_FAIL == img.Load(pStrImg)) {
		pStrImg->Release();
		GlobalFree(hGlobalImg);
		return false;
	}
	pStrImg->Release();
	GlobalFree(hGlobalImg);
	return true;
}


void ProcessWindowMessage()
{
	MSG msg;
	while (::PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
	{
		::SendMessage(msg.hwnd, msg.message, msg.wParam, msg.lParam);
	}
}

CString RunCLI(LPCTSTR path, LPCTSTR command, LPCTSTR parameter)
{
	CString strPath;
	if (path == NULL)
	{
		TCHAR szWin32Path[MAX_PATH] = { 0, };
		::SHGetSpecialFolderPath(NULL, szWin32Path, CSIDL_SYSTEM, FALSE);
		strPath = szWin32Path;
	}
	else
	{
		strPath = path;
	}
	strPath += "\\";
	strPath += command;

	TCHAR szParam[MAX_PATH * 4];
	wsprintf(szParam, _T("%s %s"), strPath, parameter);

	TraceLog((_T("skpark face %s"), szParam));

	SECURITY_ATTRIBUTES sa = { 0 };
	sa.nLength = sizeof(sa);
	sa.bInheritHandle = TRUE;
	TCHAR szOutputFilePath[MAX_PATH];
	lstrcpy(szOutputFilePath, TEXT("stdout.txt"));

	::DeleteFile(szOutputFilePath);
	HANDLE hFile = CreateFile(szOutputFilePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, &sa, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	STARTUPINFO si = { 0 };
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESTDHANDLES;
	si.hStdInput = stdin;
	si.hStdOutput = hFile;
	//si.hStdError = hFile;

	/*
	STARTUPINFO si = {0};
	si.cb = sizeof(STARTUPINFO);
	si.wShowWindow = SW_HIDE;
	si.dwFlags = STARTF_USESHOWWINDOW;	// 콘솔창이 보이지 않도록 한다
	si.hStdInput = stdin;
	si.hStdOutput = hFile;
	si.hStdError = hFile;
	*/

	PROCESS_INFORMATION pi;

	if (!CreateProcess((LPTSTR)(LPCTSTR)strPath
		, (LPTSTR)(LPCTSTR)szParam
		, NULL
		, NULL
		, TRUE
		, CREATE_NO_WINDOW
		, NULL
		, NULL
		, &si
		, &pi
		))
	{
		DWORD dwErrNo = GetLastError();

		LPVOID lpMsgBuf = NULL;
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER
			| FORMAT_MESSAGE_IGNORE_INSERTS
			| FORMAT_MESSAGE_FROM_SYSTEM
			, NULL
			, dwErrNo
			, 0
			, (LPTSTR)&lpMsgBuf
			, 0
			, NULL);

		CString strErrMsg;
		if (!lpMsgBuf)
		{
			strErrMsg.Format(_T("Unknow Error - CreateProcess [%d]"), dwErrNo);
		}
		else
		{
			strErrMsg.Format(_T("%s [%d]"), (LPCTSTR)lpMsgBuf, dwErrNo);
		}

		LocalFree(lpMsgBuf);
		TraceLog((_T("skpark face error : %s"), strErrMsg));
		return _T("0 ");
	}

	int counter = 0;
	while (::WaitForSingleObject(pi.hProcess, 0))
	{
		//if (!m_bRunning)
		if (counter > 5)
		{
			TraceLog((_T("Createprocess break!!!")));
			TerminateProcess(pi.hProcess, 0);
			::WaitForSingleObject(pi.hProcess, 2000);
			CloseHandle(pi.hProcess);
			CloseHandle(pi.hThread);
			return _T("0");
		}
		counter++;
		ProcessWindowMessage();
		Sleep(500);
	}
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	CloseHandle(hFile);

	CString msg = _T("");
	FILE* fp = _wfopen(szOutputFilePath, _T("r"));
	if (fp != NULL)
	{
		char buf[1024 + 1];
		memset(buf, 0x00, sizeof(buf));
		while (fgets(buf, 1024, fp) != 0)
		{
			if (strlen(buf) > 0)
			{
				msg += buf;
			}
			memset(buf, 0x00, sizeof(buf));
		}
		fclose(fp);
	}
	else
	{
		TraceLog((_T("skpark face error : %s file read error"), szOutputFilePath));
	}
	return msg;
}


bool deleteOldFile(LPCTSTR rootDir, int duration, LPCTSTR filter)
{
	TraceLog((_T("_deleteOldFile(%s,%d,%s)"), rootDir, duration, filter));

	std::wstring dirPath = rootDir;
	dirPath += filter;

	TraceLog((_T("target files=%s"), dirPath.c_str()));

	HANDLE hFile = NULL;
	WIN32_FIND_DATA FileData;
	hFile = FindFirstFile(dirPath.c_str(), &FileData);
	if (hFile == INVALID_HANDLE_VALUE) {
		TraceLog((_T("screenshot file not found")));
		return false;
	}

	int deleted_counter = 0;

	CTime referTime = CTime::GetCurrentTime();
	int day = duration;
	int hour = 0;
	CTimeSpan spanTime(day, hour, 0, 0);
	referTime = referTime - spanTime; // 2일이전

	do {
		if (wcsncmp(FileData.cFileName, _T(".."), 2) == 0 || wcsncmp(FileData.cFileName, _T("."), 1) == 0) {
			continue;
		}
		if (FileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			std::wstring subDir = rootDir;
			subDir += FileData.cFileName;
			subDir += _T("//");
			deleteOldFile(subDir.c_str(), duration, filter);
			continue;
		}

		std::wstring filename = FileData.cFileName;

		CTime fileTime(FileData.ftLastWriteTime);
		if (fileTime < referTime) {
			std::wstring delFilePath = rootDir;
			delFilePath += filename;
			::_wremove(delFilePath.c_str());
			deleted_counter++;
			TraceLog((_T("%s file deleted"), delFilePath.c_str()));
		}

	} while (FindNextFile(hFile, &FileData));

	FindClose(hFile);
	return true;
}

bool deleteOldFile(LPCTSTR rootDir, int day,int hour,int min, LPCTSTR filter)
{
	TraceLog((_T("deleteOldFileByMin(%s,%d,%d,%d%s)"), rootDir, day,hour,min, filter));

	std::wstring dirPath = rootDir;
	dirPath += filter;

	TraceLog((_T("target files=%s"), dirPath.c_str()));

	HANDLE hFile = NULL;
	WIN32_FIND_DATA FileData;
	hFile = FindFirstFile(dirPath.c_str(), &FileData);
	if (hFile == INVALID_HANDLE_VALUE) {
		TraceLog((_T("old file not found")));
		return false;
	}

	int deleted_counter = 0;

	CTime referTime = CTime::GetCurrentTime();
	CTimeSpan spanTime(day, hour, min, 0);
	referTime = referTime - spanTime; 

	do {
		if (wcsncmp(FileData.cFileName, _T(".."), 2) == 0 || wcsncmp(FileData.cFileName, _T("."), 1) == 0) {
			continue;
		}
		if (FileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			std::wstring subDir = rootDir;
			subDir += FileData.cFileName;
			subDir += _T("//");
			deleteOldFile(subDir.c_str(), day,hour,min, filter);
			continue;
		}

		std::wstring filename = FileData.cFileName;

		CTime fileTime(FileData.ftLastWriteTime);
		if (fileTime < referTime) {
			std::wstring delFilePath = rootDir;
			delFilePath += filename;
			::_wremove(delFilePath.c_str());
			deleted_counter++;
			TraceLog((_T("%s file deleted"), delFilePath.c_str()));
		}

	} while (FindNextFile(hFile, &FileData));

	FindClose(hFile);
	return true;
}

void KillBrowserOnly()
{
	unsigned long pid = getPid(_T("UTV_brwClient2.exe"));
	if (pid) {
		killProcess(pid);
	}
}

void ShowFirmwareView(bool show)
{
	HWND hwnd = getWHandle(_T("UBCFirmwareView.exe"));
	if (hwnd) {
		CWnd* cWnd = CWnd::FromHandle(hwnd);
		if (cWnd)  {
			TraceLog((_T("UBCFirmwareView.exe founded")));
			cWnd->ShowWindow(show ? SW_SHOW : SW_MINIMIZE);
		}
	}
}


bool getVersion(CString& version)
{
	CString release_file = UBC_EXE_PATH;
	release_file += _T("release.txt");

	FILE* fVer = _wfopen(release_file, _T("r"));
	if (!fVer) {
		return false;
	}
	if (feof(fVer)) {
		fclose(fVer);
		return false;
	}
	char buffer[512];
	memset(buffer, 0x00, 512);
	fgets(buffer, 512, fVer);
	buffer[5] = 0; //version 은 4자를 넘지 않는다. x.xx
	version = buffer + 1;
	fclose(fVer);
	return true;
}


char * ConvertWCtoC(const wchar_t* str)
{
	//반환할 char* 변수 선언
	char* pStr;

	//입력받은 wchar_t 변수의 길이를 구함
	int strSize = WideCharToMultiByte(CP_ACP, 0, str, -1, NULL, 0, NULL, NULL);
	//char* 메모리 할당
	pStr = new char[strSize];

	//형 변환 
	WideCharToMultiByte(CP_ACP, 0, str, -1, pStr, strSize, 0, 0);
	return pStr;
}
void ConvertWCtoC(const wchar_t* str, std::string& outString)
{
	//반환할 char* 변수 선언
	char* pStr;

	//입력받은 wchar_t 변수의 길이를 구함
	int strSize = WideCharToMultiByte(CP_ACP, 0, str, -1, NULL, 0, NULL, NULL);
	//char* 메모리 할당
	pStr = new char[strSize];

	//형 변환 
	WideCharToMultiByte(CP_ACP, 0, str, -1, pStr, strSize, 0, 0);

	outString = pStr;
	delete pStr;
}

///////////////////////////////////////////////////////////////////////
//char 에서 wchar_t 로의 형변환 함수
wchar_t* ConvertCtoWC(const char* str)
{
	//wchar_t형 변수 선언
	wchar_t* pStr;
	//멀티 바이트 크기 계산 길이 반환
	int strSize = MultiByteToWideChar(CP_ACP, 0, str, -1, NULL, NULL);
	//wchar_t 메모리 할당
	pStr = new WCHAR[strSize];
	//형 변환
	MultiByteToWideChar(CP_ACP, 0, str, strlen(str) + 1, pStr, strSize);
	return pStr;
}

void ConvertCtoWC(const char* str, CString& outString)
{
	//wchar_t형 변수 선언
	wchar_t* pStr;
	//멀티 바이트 크기 계산 길이 반환
	int strSize = MultiByteToWideChar(CP_ACP, 0, str, -1, NULL, NULL);
	//wchar_t 메모리 할당
	pStr = new WCHAR[strSize];
	//형 변환
	MultiByteToWideChar(CP_ACP, 0, str, strlen(str) + 1, pStr, strSize);
	outString = pStr;
	delete pStr;
}

/****************************************
Function:    AnsiToUTF8
Description: Ansi俚륜瘻뻣냥UTF-8俚륜
Input:       szAnsiString Ansi俚륜
Return:      UTF-8俚륜
****************************************/
char* AnsiToUTF8(const char *szAnsiString)
{
	if (szAnsiString == NULL || strlen(szAnsiString) == 0)
	{
		OutputDebugString(_T("AnsiToUTF8 >>> input param is NULL!"));
		return NULL;
	}
	// AnsiToUnicode
	// 1. ANSI(multibyte) Length
	int wcsLen = ::MultiByteToWideChar(CP_ACP, NULL, szAnsiString, (int)strlen(szAnsiString), NULL, 0);
	wchar_t* wszUnicodeString = new wchar_t[(size_t)wcsLen + 1];

	// 2. ANSI(multibyte) ---> unicode
	::MultiByteToWideChar(CP_ACP, NULL, szAnsiString, (int)strlen(szAnsiString), wszUnicodeString, wcsLen);
	wszUnicodeString[wcsLen] = '\0';

	// unicode to UTF8
	// 3. utf8 Length
	int UTF8Len = ::WideCharToMultiByte(CP_UTF8, NULL, wszUnicodeString, (int)wcslen(wszUnicodeString), NULL, 0, NULL, NULL);
	char* szUTF8 = new char[(size_t)UTF8Len + 1];

	//4. unicode ---> utf8
	::WideCharToMultiByte(CP_UTF8, NULL, wszUnicodeString, (int)wcslen(wszUnicodeString), szUTF8, UTF8Len, NULL, NULL);
	szUTF8[UTF8Len] = '\0';

	delete[] wszUnicodeString;
	wszUnicodeString = NULL;

	return szUTF8;
}

/****************************************
Function:    UTF8ToAnsi
Description: UTF-8俚륜瘻뻣냥Ansi俚륜
Input:       szAnsiString UTF-8俚륜
Return:      Ansi俚륜
****************************************/
char* UTF8ToAnsi(const char *szUTF8String)
{
	WCHAR* strSrc = NULL;
	char* szRes = NULL;

	int i = MultiByteToWideChar(CP_UTF8, 0, szUTF8String, -1, NULL, 0);

	strSrc = new WCHAR[(size_t)i + 1];
	MultiByteToWideChar(CP_UTF8, 0, szUTF8String, -1, strSrc, i);

	i = WideCharToMultiByte(CP_ACP, 0, strSrc, -1, NULL, 0, NULL, NULL);

	szRes = new char[(size_t)i + 1];
	WideCharToMultiByte(CP_ACP, 0, strSrc, -1, szRes, i, NULL, NULL);

	delete[] strSrc;
	strSrc = NULL;

	return szRes;
}

string UTF8ToANSIString(const char *pUtf8String)
{
	string strANSI;
	if (pUtf8String != NULL)
	{
		char *pAnsi = UTF8ToAnsi(pUtf8String);
		if (pAnsi != NULL)
		{
			strANSI = pAnsi;
			delete[]pAnsi;
		}
	}

	return strANSI;
}

std::string UTF8ToANSIString(string strRetData){
	strRetData = UTF8ToANSIString(strRetData.c_str());
}

std::wstring UTF8ToANSIString(const wchar_t* utf8_input)
{
	char*  utf8_output = ConvertWCtoC(utf8_input);
	string ansi_input = UTF8ToANSIString(utf8_output);
	wchar_t*  ansi_output = ConvertCtoWC(ansi_input.c_str());

	wstring retval = ansi_output;
	delete[]  utf8_output;
	delete[]  ansi_output;

	return retval;
}



std::vector<std::wstring> SplitWithCharacters(const std::wstring& str, int splitLength) {
	int NumSubstrings = str.length() / splitLength;
	std::vector<std::wstring> ret;

	for (int i = 0; i < NumSubstrings; i++) {
		ret.push_back(str.substr(i * splitLength, splitLength));
	}

	// If there are leftover characters, create a shorter item at the end.
	if (str.length() % splitLength != 0) {
		ret.push_back(str.substr(splitLength * NumSubstrings));
	}


	return ret;
}

COLORREF hex2rgb(std::wstring hex) {
	if (hex.at(0) == '#') {
		hex.erase(0, 1);
	}
	while (hex.length() != 6) {
		hex += _T("0");
	}
	std::vector<wstring> colori = SplitWithCharacters(hex, 2);
	COLORREF ref = RGB(stoi(colori[0], nullptr, 16), stoi(colori[1], nullptr, 16), stoi(colori[2], nullptr, 16));
	return ref;
}

std::wstring rgb2hex(COLORREF col)
{
	TCHAR buf[16];
	wsprintf(buf, _T("#%02d%02d%02d"), GetRValue(col), GetGValue(col), GetBValue(col));
	std::wstring retval = buf;
	return retval;
}


double getRatio(int resType)
{
	/*
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
	*/
	switch (resType)
	{
	case 0:  return   1080.0 / 1920.0;
	case 1:  return   480.0 / 640.0;
	case 2:  return   600.0 / 800.0;
	case 3:  return   720.0 / 1024.0;
	case 4:  return   768.0 / 1024.0;
	case 5:  return   1024.0 / 1280.0;
	case 6:  return   1200.0 / 1600.0;
	case 7:  return   1536.0 / 2048.0;
	case 8:  return   1440.0 / 2560.0;
	case 9:  return   1944.0 / 2592.0;
	}
	return 480.0 / 640.0;
}

//skpark b49 2019.5.13 end


void CImage2Mat(CImage& Image, cv::Mat& src)
{
	int channels = src.channels();
	TraceLog((_T("channels=%d"), channels))
	// CImage to Mat
	if (Image.IsNull()) {
		 TraceLog((_T("CImage is null")));
		 return;
	}
	if (1 == channels)
	{
		src.create(Image.GetHeight(), Image.GetWidth(), CV_8UC1);
		TraceLog((_T("It's gray image")));
	}
	else if (3 == channels)
	{
		src.create(Image.GetHeight(), Image.GetWidth(), CV_8UC3);
		TraceLog((_T("It's color image")));
	}
	else
	{
		TraceLog((_T("Invalid Channel(%d)"), channels));

	}

	// Copy data
	uchar* pucRow;								//point to the line pointer of the Mat data area
	uchar* pucImage = (uchar*)Image.GetBits();	//Pointer to the CImage data area
	int nStep = Image.GetPitch();				//The number of bytes per line, note that this return value has positive and negative
	for (int nRow = 0; nRow < Image.GetHeight(); nRow++) {
		pucRow = (src.ptr<uchar>(nRow));
		for (int nCol = 0; nCol < Image.GetWidth(); nCol++)	{
			if (1 == channels) {
				pucRow[nCol] = *(pucImage + nRow * nStep + nCol);
			}
			else if (3 == channels) {
				for (int nCha = 0; nCha < 3; nCha++) {
					pucRow[nCol * 3 + nCha] = *(pucImage + nRow * nStep + nCol * 3 + nCha);
				}
			}
		}
	}
}

CImage* Mat2CImage(cv::Mat* mat)
{
	if (!mat || mat->empty())
		return NULL;

	int nBPP = mat->channels() * 8;
	CImage* pImage = new CImage();
	pImage->Destroy();
	pImage->Create(mat->cols, mat->rows, nBPP);
	if (nBPP == 8) {
		static RGBQUAD pRGB[256];
		for (int i = 0; i < 256; i++) {
			pRGB[i].rgbBlue = pRGB[i].rgbGreen = pRGB[i].rgbRed = i;
		}
		pImage->SetColorTable(0, 256, pRGB);
	}

	uchar* psrc = mat->data;
	uchar* pdst = (uchar*)pImage->GetBits();
	int imgPitch = pImage->GetPitch();
	for (int y = 0; y < mat->rows; y++) {
		memcpy(pdst, psrc, mat->cols * mat->channels());	// mat->step is incorrect for those images created by roi (sub-images!)
		psrc += mat->step;
		pdst += imgPitch;
	}
	return pImage;
}

CString  makeTimeKey(TCHAR postfix)
{
	SYSTEMTIME t;
	GetLocalTime(&t);
	TCHAR chTime[128];
	memset(chTime, 0x00, 128);
	wsprintf(chTime, _T("[%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d_%6.6d_%c]"),
		t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond, t.wMilliseconds, postfix);

	return chTime;
}

bool SaveFile(LPCTSTR fullpath, void* targetText, int targetLen, int iDeviceIndex)
{
	TraceLog((_T("SaveFile(%s)"), fullpath));
	char* pBuf = ConvertWCtoC((const wchar_t* )targetText);

	FILE* fp = _wfopen(fullpath, _T("wb"));
	if (fp == NULL)
	{
		return false;
	}
	fwrite(pBuf, 1, sizeof(char)* strlen(pBuf), fp);
	fflush(fp);
	fclose(fp);
	delete[] pBuf;
	TraceLog((_T("SaveFile end")));
	return true;

	//DWORD dwWrittenBytes = 0;
	//HANDLE hFile = CreateFile(fullpath, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	//if (hFile == INVALID_HANDLE_VALUE) {
	//	return false;
	//}

	//DWORD dwRet = WriteFile(hFile, targetText, targetLen, &dwWrittenBytes, NULL);
	//if (dwRet == 0 || dwWrittenBytes < targetLen) {
	//	DWORD dwError = GetLastError();
	//	return false;
	//}

	//CloseHandle(hFile);
	//TraceLog((_T("SaveFile end")));
	//return true;
}

bool EncryptFile(LPCTSTR inFilePath, LPCTSTR outFilePath, bool removeInFile)
{
	// 선택한 파일을 바이너리 형식으로 열기
	FILE* pFile = _wfopen(inFilePath, _T("rb"));
	if (!pFile)
	{
		TraceLog((_T("Encrypt (%s) file read error "), inFilePath));
		return false;
	}

	DWORD dwFileLen = _filelength(_fileno(pFile));
	BYTE* bBuff = new BYTE[dwFileLen];

	// 파일 읽어서 버퍼에 저장
	fread(bBuff, 1, dwFileLen, pFile);
	fclose(pFile);


	HCRYPTPROV    hProv;
	HCRYPTHASH    hHash;
	HCRYPTKEY    hKey;
	CString        csPass = _T("JYT20140315197402171994082501234");

	// CSP(Crystographic Service Provider) 핸들 얻기
	if (!CryptAcquireContext(&hProv, NULL, MS_ENHANCED_PROV, PROV_RSA_FULL, 0))
	{
		if (!CryptAcquireContext(&hProv, NULL, MS_ENHANCED_PROV, PROV_RSA_FULL, CRYPT_NEWKEYSET))
		{
			TraceLog((_T("Fail to Encrypt")));
			return false;
		}
	}

	// 해쉬 만들기
	CryptCreateHash(hProv, CALG_SHA, 0, 0, &hHash);
	// 해쉬 값 계산
	CryptHashData(hHash, (BYTE*)(LPCTSTR)csPass, csPass.GetLength(), 0);
	// 키 만들기\tab
	CryptDeriveKey(hProv, CALG_RC4, hHash, 0x0080 * 0x10000, &hKey);
	// 암호화\tab
	CryptEncrypt(hKey, 0, TRUE, 0, bBuff, &dwFileLen, dwFileLen);
	// 해쉬 없애기
	CryptDestroyHash(hHash);
	// CSP 핸들 풀어주기
	CryptReleaseContext(hProv, 0);

	// 암호화된 파일 저장하기
	pFile = _wfopen(outFilePath, _T("wb"));
	fwrite(bBuff, 1, dwFileLen, pFile);
	fclose(pFile);

	// 버퍼 삭제
	delete[] bBuff;

	if (removeInFile)
	{
		if (!::DeleteFile(inFilePath))
		{
			TraceLog((_T("WARN : (%s) delete failed"), inFilePath));
		}
	}
	return true;
}