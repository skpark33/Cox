#include "stdafx.h"
#include "MyHelperMan.h"
#include "D2DTextCustomRender.h"


// 
// \brief <pre>
// 내용을 넣으세요.
// </pre>
// 
CMyHelper& theHelperMan()
{
	static CMyHelper man;
	return man;
}


CMyHelper::CMyHelper()
{
}


CMyHelper::~CMyHelper()
{
}


// 
// \brief <pre>
// 내용을 넣으세요.
// </pre>
// \param   HWND						a_hWnd
// \param   ID2D1Factory**				a_ppFactory
// \param   ID2D1HwndRenderTarget**		a_ppTarget
// \param   ID2D1BitmapRenderTarget**	a_ppBitmapTarget
// \param   IWICImagingFactory**		a_ppWICFactory
// \param   IDWriteFactory**			a_ppWriteFactory
// \return  HRESULT 
// 
HRESULT CMyHelper::InitDirect2D(
	_In_ HWND						a_hWnd,
	_Out_ ID2D1Factory**			a_ppFactory,
	_Out_ ID2D1HwndRenderTarget**	a_ppTarget,
	_Out_ ID2D1BitmapRenderTarget**	a_ppBitmapTarget,
	_Out_ IWICImagingFactory**		a_ppWICFactory,
	_Out_ IDWriteFactory**			a_ppWriteFactory)
{
	if( !a_hWnd || a_ppFactory == nullptr || a_ppTarget == nullptr ) {
		return S_FALSE;
	}

	ASSERT(a_ppFactory);
	ASSERT(a_ppTarget);

	CRect rc;
	GetClientRect(a_hWnd, &rc);

	HRESULT hr = S_OK;
#ifdef _DEBUG
	D2D1_FACTORY_OPTIONS op;
	op.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, op, a_ppFactory);
#else
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, a_ppFactory);
#endif
	if( FAILED(hr) ) {
		DebugString(_T("UseEnableD2D D2D1CreateFactory (%d)"), hr);
		return hr;
	}

	hr = (*a_ppFactory)->CreateHwndRenderTarget(
		D2D1::RenderTargetProperties(
			D2D1_RENDER_TARGET_TYPE_DEFAULT,
			D2D1::PixelFormat(
				DXGI_FORMAT_B8G8R8A8_UNORM,
				D2D1_ALPHA_MODE_IGNORE),
			96.f,
			96.f),
		D2D1::HwndRenderTargetProperties(
			a_hWnd,
			D2D1::SizeU(rc.Width(), rc.Height()),
			D2D1_PRESENT_OPTIONS_IMMEDIATELY),
		a_ppTarget);
	if( FAILED(hr) ) {
		DebugString(_T("UseEnableD2D CreateHwndRenderTarget (%d)"), hr);
		return hr;
	}

	if( a_ppWICFactory ) {
		if( FAILED(hr) ) {
			DebugString(_T("UseEnableD2D ConInitialize (%d)"), hr);
			return hr;
		}
		hr = CoCreateInstance(
			CLSID_WICImagingFactory,
			NULL,
			CLSCTX_INPROC_SERVER,
			IID_IWICImagingFactory,
			(LPVOID*)a_ppWICFactory);
			//IID_PPV_ARGS(a_ppWICFactory));
		if( FAILED(hr) ) {
			DebugString(_T("UseEnableD2D CoCreateInstance (%d)"), hr);
			return hr;
		}
	}

	if( a_ppWriteFactory ) {
		hr = DWriteCreateFactory(
			DWRITE_FACTORY_TYPE_SHARED,
			__uuidof((*a_ppWriteFactory)),
			(IUnknown**)a_ppWriteFactory);
		if( FAILED(hr) ) {
			DebugString(_T("UseEnableD2D DWriteCreateFactory (%d)"), hr);
			return hr;
		}
	}
	if( a_ppBitmapTarget ) {
		hr = (*a_ppTarget)->CreateCompatibleRenderTarget(a_ppBitmapTarget);
		if( FAILED(hr) ) {
			DebugString(_T("UseEnableD2D CreateCompatibleRenderTarget (%d)"), hr);
			return hr;
		}
	}
	return hr;
}


// 
// \brief <pre>
// 내용을 넣으세요.
// </pre>
// \param   IDWriteFactory*			a_pWriteFactory
// \param   CString&				a_strFontName
// \param   float					a_fFontSize
// \param   IDWriteTextFormat**		a_ppTextFormat
// \param   DWRITE_FONT_WEIGHT		a_nFontWeight
// \param   DWRITE_FONT_STYLE		a_nFontStyle
// \param   DWRITE_FONT_STRETCH		a_nFontStretch
// \return  HRESULT 
// 
HRESULT CMyHelper::InitD2DTextfmt(
	_In_ IDWriteFactory*			a_pWriteFactory,
	_In_ const CString&				a_strFontName,
	_In_ float						a_fFontSize,
	_Out_ IDWriteTextFormat**		a_ppTextFormat,
	_In_ DWRITE_FONT_WEIGHT			a_nFontWeight,
	_In_ DWRITE_FONT_STYLE			a_nFontStyle,
	_In_ DWRITE_FONT_STRETCH		a_nFontStretch)
{
	HRESULT hr = S_OK;
	hr = a_pWriteFactory->CreateTextFormat(
		a_strFontName,
		NULL,
		a_nFontWeight,
		a_nFontStyle,
		a_nFontStretch,
		a_fFontSize,
		_T(""),
		a_ppTextFormat
	);
	if( SUCCEEDED(hr) ) {
		hr = (*a_ppTextFormat)->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		if( FAILED(hr) ) {
			DebugString(_T("D2DSetTextFormat SetTextAlignment (%d)"), hr);
			return hr;
		}
		hr = (*a_ppTextFormat)->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		if( FAILED(hr) ) {
			DebugString(_T("D2DSetTextFormat SetParagraphAlignment (%d)"), hr);
			return hr;
		}
	}
	else {
		DebugString(_T("D2DSetTextFormat CreateTextFormat (%d)"), hr);
		return hr;
	}
	return hr;
}


// 
// \brief <pre>
// 내용을 넣으세요.
// </pre>
// \param   IWICImagingFactory*		a_pWICFactory
// \param   ID2D1RenderTarget*		a_pTarget
// \param   CString&				a_strFileName
// \param   ID2D1Bitmap**			a_ppBitmap
// \return  HRESULT 
// 
HRESULT CMyHelper::InitD2DBitmapFromPNG(
	_In_ IWICImagingFactory*		a_pWICFactory,
	_In_ ID2D1RenderTarget*			a_pTarget,
	_In_ const CString&				a_strFileName,
	_Out_ ID2D1Bitmap**				a_ppBitmap)
{
	HRESULT hr = S_OK;
	CComPtr<IWICBitmapDecoder>		pWICBitmapDecoder	= nullptr;
	CComPtr<IWICBitmapFrameDecode>	pWICFrameDecoder	= nullptr;
	CComPtr<IWICFormatConverter>	pFormatConverter	= nullptr;
	
	hr = a_pWICFactory->CreateDecoderFromFilename(
												a_strFileName,
												nullptr,
												GENERIC_READ,
												WICDecodeMetadataCacheOnDemand,
												&pWICBitmapDecoder);

	if( FAILED(hr) ) {
		DebugString(_T("D2DGetBitmapFromPNG CreateDecoderFromFilename (%d)"), hr);
		return hr;
	}

	hr = pWICBitmapDecoder->GetFrame(0, &pWICFrameDecoder);
	if( FAILED(hr) ) {
		DebugString(_T("D2DGetBitmapFromPNG GetFrame (%d)"), hr);
		return hr;
	}

	hr = a_pWICFactory->CreateFormatConverter(&pFormatConverter);
	if( FAILED(hr) ) {
		DebugString(_T("D2DGetBitmapFromPNG CreateFormatConverter (%d)"), hr);
		return hr;
	}

	hr = pFormatConverter->Initialize(
									pWICFrameDecoder,
									GUID_WICPixelFormat32bppPBGRA,
									WICBitmapDitherTypeNone,
									NULL,
									0.f,
									WICBitmapPaletteTypeCustom);
	if( FAILED(hr) ) {
		DebugString(_T("D2DGetBitmapFromPNG Initialize (%d)"), hr);
		return hr;
	}

	hr = a_pTarget->CreateBitmapFromWicBitmap(
											pFormatConverter,
											NULL,
											a_ppBitmap);
	if( FAILED(hr) ) {
		DebugString(_T("D2DGetBitmapFromPNG CreateBitmapFromWicBitmap (%d)"), hr);
		return hr;
	}
	return hr;
}


// 
// \brief <pre>
// 내용을 넣으세요.
// </pre>
// \param   IWICImagingFactory*		a_pWICFactory
// \param   ID2D1RenderTarget*		a_pTarget
// \param   UINT					a_nResourceID
// \param   CString&				a_strFileExtName
// \param   ID2D1Bitmap**			a_ppBitmap
// \return  HRESULT 
// 
HRESULT CMyHelper::InitD2DBitmapFromPNG(
	_In_ IWICImagingFactory*		a_pWICFactory,
	_In_ ID2D1RenderTarget*			a_pTarget,
	_In_ UINT						a_nResourceID,
	_In_ const CString&				a_strFileExtName,
	_Out_ ID2D1Bitmap**				a_ppBitmap)
{
	CComPtr<IWICBitmapDecoder>			pWICBitmapDecoder = NULL;
	CComPtr<IWICBitmapFrameDecode>		pWICFrameDecoder = NULL;
	CComPtr<IWICFormatConverter>		pFormatConverter = NULL;
	CComPtr<IStream>					pStream = NULL;
	HINSTANCE	hModule	= AfxGetInstanceHandle();
	HRSRC		hrSrc	= FindResource(
									hModule,
									MAKEINTRESOURCE(a_nResourceID),
									a_strFileExtName);

	HGLOBAL	hGlobal		= LoadResource(hModule, hrSrc);
	PVOID pResourceData	= LockResource(hGlobal);
	int nSize			= SizeofResource(hModule, hrSrc);
	DWORD dwReadWrite	= 0;
	HRESULT hr			= CreateStreamOnHGlobal(NULL, TRUE, &pStream);

	if( FAILED(hr) ) {
		DebugString(_T("D2DGetBitmapFromPNG CreateStreamOnHGlobal (%d)"), hr);
		goto Finish;
	}
	hr = pStream->Write(pResourceData, nSize, &dwReadWrite);
	if( FAILED(hr) ) {
		DebugString(_T("D2DGetBitmapFromPNG Write (%d)"), hr);
		goto Finish;
	}
	hr = a_pWICFactory->CreateDecoderFromStream(
											pStream,
											NULL,
											WICDecodeMetadataCacheOnDemand,
											&pWICBitmapDecoder
	);
	if( FAILED(hr) ) {
		DebugString(_T("D2DGetBitmapFromPNG CreateDecoderFromStream (%d)"), hr);
		goto Finish;
	}
	hr = pWICBitmapDecoder->GetFrame(0, &pWICFrameDecoder);
	if( FAILED(hr) ) {
		DebugString(_T("D2DGetBitmapFromPNG GetFrame (%d)"), hr);
		goto Finish;
	}
	hr = a_pWICFactory->CreateFormatConverter(&pFormatConverter);
	if( FAILED(hr) ) {
		DebugString(_T("D2DGetBitmapFromPNG CreateFormatConverter (%d)"), hr);
		goto Finish;
	}
	hr = pFormatConverter->Initialize(
									pWICFrameDecoder,
									GUID_WICPixelFormat32bppPBGRA,
									WICBitmapDitherTypeNone,
									NULL,
									0.f,
									WICBitmapPaletteTypeCustom);
	if( FAILED(hr) ) {
		DebugString(_T("D2DGetBitmapFromPNG Initialize (%d)"), hr);
		goto Finish;
	}

	hr = a_pTarget->CreateBitmapFromWicBitmap(
										pFormatConverter,
										NULL,
										a_ppBitmap);
	if( FAILED(hr) ) {
		DebugString(_T("D2DGetBitmapFromPNG CreateBitmapFromWicBitmap (%d)"), hr);
		goto Finish;
	}

Finish:
	pFormatConverter.Release();
	pFormatConverter = nullptr;

	pWICFrameDecoder.Release();
	pWICFrameDecoder = nullptr;

	pWICBitmapDecoder.Release();
	pWICBitmapDecoder = nullptr;

	pStream.Release();
	pStream = nullptr;

	::UnlockResource(hGlobal);
	::FreeResource(hGlobal);
	::FreeLibrary(hModule);
	return hr;
}


// 
// \brief <pre>
// 내용을 넣으세요.
// </pre>
// \param   CString&		a_strSrc
// \param   CString&		a_strToken
// \param   CStringList*	a_pList
// \return  int 
// 
int CMyHelper::Split(
	_In_ const CString&		a_strSrc,
	_In_ const CString&		a_strToken,
	_Out_ CStringList*		a_pList)
{
	int nCnt = 0;
	CString strData = a_strSrc;
	do {
		int nPos = strData.Find(a_strToken);
		if( nPos != -1 ) {
			++nCnt;
			a_pList->AddTail(strData.Mid(0, nPos));
			strData.Delete(0, nPos + 1);
		} else {
			++nCnt;
			a_pList->AddTail(strData);
			break;
		}
	} while( !strData.IsEmpty() );
	return nCnt;
}


// 
// \brief <pre>
// 내용을 넣으세요.
// </pre>
// \param   CString&		a_strSrc
// \param   CString&		a_strToken
// \param   int				a_nListCnt
// \param   int				a_nGetIdx
// \return  CString 
// 
CString CMyHelper::Split(
	_In_ const CString&		a_strSrc,
	_In_ const CString&		a_strToken,
	_In_ int				a_nListCnt,
	_In_ int				a_nGetIdx)
{
	CStringList strList;
	if( a_nListCnt == Split(a_strSrc, a_strToken, &strList) ) {
		POSITION pos = strList.FindIndex(a_nGetIdx);
		return strList.GetAt(pos);
	}
	return _T("");
}


// 
// \brief <pre>
// 내용을 넣으세요.
// </pre>
// \param   COLORREF		a_clr
// \param   float			a_fAlpha
// \return  D2D1_COLOR_F 
// 
D2D1_COLOR_F CMyHelper::GetD2DColor(
	_In_ COLORREF			a_clr,
	_In_ float				a_fAlpha)
{
	D2D1_COLOR_F clr;
	clr.r = (float)GetRValue(a_clr) / (float)255;
	clr.g = (float)GetGValue(a_clr) / (float)255;
	clr.b = (float)GetBValue(a_clr) / (float)255;
	clr.a = a_fAlpha;
	return clr;
}


// 
// \brief <pre>
// 현재 실행파일 경로를 리턴한다. 마지막 \\ 은 없다.
// ex) C:\\MyProject\\HelloWorld
// </pre>
// \return  CString 
// 
CString CMyHelper::GetCurrDir()
{
	TCHAR szPath[BUFSIZ] = { 0, };
	GetModuleFileName(NULL, szPath, BUFSIZ);
	PathRemoveFileSpec(szPath);
	return CString(szPath);
}


// 
// \brief <pre>
// 현재 실행파일 경로에 추가경로를 붙인다.
// </pre>
// \param   CString&		a_strPath
// \return  CString 
// 
CString CMyHelper::GetAppendCurrDir(
	_In_ const CString&		a_strPath)
{
	return GetMakeString(_T("%s\\%s"), GetCurrDir(), a_strPath);
}


// 
// \brief <pre>
// 내용을 넣으세요.
// </pre>
// \param   _eeprom_matching_info_*	a_pEEPInfo
// 
void CMyHelper::SetMatchingValue(
	_In_ _eeprom_matching_info_*	a_pEEPInfo)
{
	a_pEEPInfo->m_ptFever;

	usrPOINT ptFever[4];
	usrPOINT ptReal[4];


	for( int i = 0 ; i < DEF_MAX_MATCHING_POINT ; ++i ) {
		ptFever[i].x	= (double)a_pEEPInfo->m_ptFever[i].x;
		ptFever[i].y	= (double)a_pEEPInfo->m_ptFever[i].y;

		ptReal[i].x		= (double)a_pEEPInfo->m_ptReal[i].x;
		ptReal[i].y		= (double)a_pEEPInfo->m_ptReal[i].y;
	}

	m_phFever2Real.SetValue(ptReal, ptFever);
	m_phReal2Fever.SetValue(ptFever, ptReal);
}


// 
// \brief <pre>
// 내용을 넣으세요.
// </pre>
// \param   usrPOINT*					a_ptFever
// \param   usrPOINT*					a_ptReal
// 
void CMyHelper::SetMatchingValue(
	_In_ usrPOINT*					a_ptFever,
	_In_ usrPOINT*					a_ptReal)
{
	for( int i = 0 ; i < DEF_MAX_MATCHING_POINT ; ++i ) {
		CPoint ptFever((int)a_ptFever[i].x, (int)a_ptFever[i].y);
		CPoint ptReal((int)a_ptReal[i].x, (int)a_ptReal[i].y);
	}
	m_phFever2Real.SetValue(a_ptReal, a_ptFever);
	m_phReal2Fever.SetValue(a_ptFever, a_ptReal);
}


CPoint CMyHelper::GetVisable2ThermalPos(
	_In_ CPoint						a_VisablePt)
{
	usrPOINT pos	= { (double)a_VisablePt.x, (double)a_VisablePt.y };
	usrPOINT cvtPos = m_phReal2Fever.GetRealCoord(pos);

	return CPoint((int)cvtPos.x, (int)cvtPos.y);
}


CPoint CMyHelper::GetThermal2VisablePos(
	_In_ CPoint						a_ThermalPt)
{
	usrPOINT pos = { (double)a_ThermalPt.x, (double)a_ThermalPt.y };
	usrPOINT cvtPos = m_phFever2Real.GetRealCoord(pos);

	return CPoint((int)cvtPos.x, (int)cvtPos.y);
}


HRESULT CMyHelper::D2DDrawOutlineText(
	_In_ ID2D1Factory*				a_pFactory,
	_In_ ID2D1RenderTarget*			a_pTarget,
	_In_ IDWriteFactory*			a_pWriteFactory,
	_In_ D2D_DRAW_TEXT_INFO&		a_TxtInfo)
{
	CComPtr<IDWriteTextFormat>		pWriteFormat = NULL;
	CComPtr<IDWriteTextLayout>		pTextLayout = NULL;
	CComPtr<ID2D1SolidColorBrush>	pFillBrush = NULL;
	CComPtr<ID2D1SolidColorBrush>	pStrokeBrush = NULL;

	a_pTarget->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

	HRESULT hr = S_OK;
	if( a_TxtInfo.m_bFillDrawText ) {
		hr = a_pTarget->CreateSolidColorBrush(
			a_TxtInfo.m_clrFill, &pFillBrush);
		if( FAILED(hr) ) {
			return hr;
		}
	}
	if( a_TxtInfo.m_bStrokeDrawText ) {
		hr = a_pTarget->CreateSolidColorBrush(
			a_TxtInfo.m_clrStroke, &pStrokeBrush);
		if( FAILED(hr) ) {
			return hr;
		}
	}
	CD2DOutlineRender Outline(
		a_pFactory,
		a_pTarget,
		a_TxtInfo.m_bFillDrawText,
		pFillBrush,
		a_TxtInfo.m_bStrokeDrawText,
		pStrokeBrush,
		a_TxtInfo.m_fStrokeWidth);

	hr = a_pWriteFactory->CreateTextFormat(
		a_TxtInfo.m_strFontName,
		NULL,
		a_TxtInfo.m_FontWeight,
		a_TxtInfo.m_FontStyle,
		a_TxtInfo.m_FontStretch,
		a_TxtInfo.m_fFontSize,
		_T("default"),
		&pWriteFormat);
	if( FAILED(hr) ) {
		return hr;
	}

	hr = a_pWriteFactory->CreateTextLayout(
		a_TxtInfo.m_strTxt,
		a_TxtInfo.m_strTxt.GetLength(),
		pWriteFormat,
		(FLOAT)a_TxtInfo.m_rcTxt.Width(),
		(FLOAT)a_TxtInfo.m_rcTxt.Height(),
		&pTextLayout);

	if( FAILED(hr) ) {
		return hr;
	}

	hr = pTextLayout->SetTextAlignment(a_TxtInfo.m_TextAlign);
	if( FAILED(hr) ) {
		return hr;
	}

	hr = pTextLayout->SetParagraphAlignment(a_TxtInfo.m_ParagraphAlign);
	if( FAILED(hr) ) {
		return hr;
	}

	hr = pTextLayout->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
	if( FAILED(hr) ) {
		return hr;
	}

	DWRITE_TRIMMING stTrimming;
	stTrimming.granularity = DWRITE_TRIMMING_GRANULARITY_CHARACTER;
	stTrimming.delimiter = 0;
	stTrimming.delimiterCount = 0;
	CComPtr<IDWriteInlineObject> pInlineObj = NULL;
	hr = a_pWriteFactory->CreateEllipsisTrimmingSign(
		pTextLayout, &pInlineObj);
	if( FAILED(hr) ) {
		return hr;
	}

	hr = pTextLayout->SetTrimming(&stTrimming, pInlineObj);
	if( FAILED(hr) ) {
		return hr;
	}

	if( a_TxtInfo.m_nDrawEffectCnt != 0 ) {
		for( int i = 0 ; i < a_TxtInfo.m_nDrawEffectCnt ; ++i ) {
			pTextLayout->SetDrawingEffect(
				a_TxtInfo.m_arrEffect[i].m_pBrush,
				a_TxtInfo.m_arrEffect[i].m_stTextRange);
		}
	}

	hr = pTextLayout->Draw(
		NULL,
		&Outline,
		(FLOAT)a_TxtInfo.m_rcTxt.left,
		(FLOAT)a_TxtInfo.m_rcTxt.top);

	if( FAILED(hr) ) {
		return hr;
	}

	a_pTarget->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);
	return hr;
}


// 
// \brief <pre>
// 두 Rect의 겹쳐져 있는 비율을 계산한다.
// </pre>
// \param   CRect						a_Rect1
// \param   CRect						a_Rect2
// \return  float 
// 
float CMyHelper::GetOverlapRectScore(
	_In_ CRect						a_Rect1,
	_In_ CRect						a_Rect2)
{
	CRect rcBox;
	rcBox.IntersectRect(a_Rect1, a_Rect2);

	if( rcBox.IsRectEmpty() ) {
		return 0.f;
	}

	return (((float)rcBox.Width() * rcBox.Height()) * 100)
				/ a_Rect2.Width() * a_Rect2.Height();
}