#pragma once
#include "PlaneHomography.h"



typedef struct _d2d_draw_text_info_
{
	CString						m_strTxt;
	CString						m_strFontName;
	FLOAT						m_fFontSize;
	CRect						m_rcTxt;
	BOOL						m_bFillDrawText;
	D2D1_COLOR_F				m_clrFill;
	BOOL						m_bStrokeDrawText;
	D2D1_COLOR_F				m_clrStroke;
	FLOAT						m_fStrokeWidth;
	DWRITE_FONT_WEIGHT			m_FontWeight;
	DWRITE_FONT_STYLE			m_FontStyle;
	DWRITE_FONT_STRETCH			m_FontStretch;
	DWRITE_TEXT_ALIGNMENT		m_TextAlign;
	DWRITE_PARAGRAPH_ALIGNMENT	m_ParagraphAlign;

	struct _effect_info_
	{
		_effect_info_() : m_pBrush(NULL) {}
		DWRITE_TEXT_RANGE				m_stTextRange;
		CComPtr<ID2D1SolidColorBrush>	m_pBrush;
	};

	int							m_nDrawEffectCnt;
	_effect_info_				m_arrEffect[4];

	_d2d_draw_text_info_(
		const CString& a_strTxt,
		const CString& a_strFontName,
		FLOAT a_fFontSize,
		CRect a_rcTxt,
		DWRITE_TEXT_ALIGNMENT a_TextAlign = DWRITE_TEXT_ALIGNMENT_CENTER,
		DWRITE_PARAGRAPH_ALIGNMENT a_ParagraphAlign = DWRITE_PARAGRAPH_ALIGNMENT_CENTER,
		BOOL a_bFillDrawText = TRUE,
		D2D1_COLOR_F a_clrFill = D2D1::ColorF(D2D1::ColorF::White),
		BOOL a_bStrokeDrawText = TRUE,
		D2D1_COLOR_F a_clrStroke = D2D1::ColorF(D2D1::ColorF::Black),
		FLOAT a_fStrokeWidth = 1.f,
		DWRITE_FONT_WEIGHT a_FontWeight = DWRITE_FONT_WEIGHT_EXTRA_BLACK,
		DWRITE_FONT_STYLE a_FontStyle = DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH a_FontStretch = DWRITE_FONT_STRETCH_NORMAL
	)
		: m_strTxt(a_strTxt)
		, m_strFontName(a_strFontName)
		, m_fFontSize(a_fFontSize)
		, m_rcTxt(a_rcTxt)
		, m_TextAlign(a_TextAlign)
		, m_ParagraphAlign(a_ParagraphAlign)
		, m_bFillDrawText(a_bFillDrawText)
		, m_clrFill(a_clrFill)
		, m_bStrokeDrawText(a_bStrokeDrawText)
		, m_clrStroke(a_clrStroke)
		, m_fStrokeWidth(a_fStrokeWidth)
		, m_FontWeight(a_FontWeight)
		, m_FontStyle(a_FontStyle)
		, m_FontStretch(a_FontStretch)
		, m_nDrawEffectCnt(0)
	{
	}
} D2D_DRAW_TEXT_INFO;





class CMyHelper
{
public:
	~CMyHelper();

	friend CMyHelper& theHelperMan();

	// Direct2D
	HRESULT				InitDirect2D(
							_In_ HWND						a_hWnd,
							_Out_ ID2D1Factory**			a_ppFactory,
							_Out_ ID2D1HwndRenderTarget**	a_ppTarget,
							_Out_ ID2D1BitmapRenderTarget**	a_ppBitmapTarget = nullptr,
							_Out_ IWICImagingFactory**		a_ppWICFactory = nullptr,
							_Out_ IDWriteFactory**			a_ppWriteFactory = nullptr);

	HRESULT				InitD2DTextfmt(
							_In_ IDWriteFactory*			a_pWriteFactory,
							_In_ const CString&				a_strFontName,
							_In_ float						a_fFontSize,
							_Out_ IDWriteTextFormat**		a_ppTextFormat,
							_In_ DWRITE_FONT_WEIGHT			a_nFontWeight = DWRITE_FONT_WEIGHT_NORMAL,
							_In_ DWRITE_FONT_STYLE			a_nFontStyle = DWRITE_FONT_STYLE_NORMAL,
							_In_ DWRITE_FONT_STRETCH		a_nFontStretch = DWRITE_FONT_STRETCH_NORMAL);

	HRESULT				InitD2DBitmapFromPNG(
							_In_ IWICImagingFactory*		a_pWICFactory,
							_In_ ID2D1RenderTarget*			a_pTarget,
							_In_ const CString&				a_strFileName,
							_Out_ ID2D1Bitmap**				a_ppBitmap);

	HRESULT				InitD2DBitmapFromPNG(
							_In_ IWICImagingFactory*		a_pWICFactory,
							_In_ ID2D1RenderTarget*			a_pTarget,
							_In_ UINT						a_nResourceID,
							_In_ const CString&				a_strFileExtName,
							_Out_ ID2D1Bitmap**				a_ppBitmap);

	HRESULT				D2DDrawOutlineText(
							_In_ ID2D1Factory*				a_pFactory,
							_In_ ID2D1RenderTarget*			a_pTarget,
							_In_ IDWriteFactory*			a_pWriteFactory,
							_In_ D2D_DRAW_TEXT_INFO&		a_TxtInfo);

	int					Split(
							_In_ const CString&				a_strSrc,
							_In_ const CString&				a_strToken,
							_Out_ CStringList*				a_pList);

	CString				Split(
							_In_ const CString&				a_strSrc,
							_In_ const CString&				a_strToken,
							_In_ int						a_nListCnt,
							_In_ int						a_nGetIdx);

	D2D1_COLOR_F		GetD2DColor(
							_In_ COLORREF					a_clr,
							_In_ float						a_fAlpha = 1.f);

	CString				GetCurrDir();

	CString				GetAppendCurrDir(
							_In_ const CString&				a_strPath);

	void				SetMatchingValue(
							_In_ _eeprom_matching_info_*	a_pEEPInfo);

	void				SetMatchingValue(
							_In_ usrPOINT*					a_ptFever,
							_In_ usrPOINT*					a_ptReal);

	CPoint				GetVisable2ThermalPos(
							_In_ CPoint						a_VisablePt);

	CPoint				GetThermal2VisablePos(
							_In_ CPoint						a_ThermalPt);

	float				GetOverlapRectScore(
							_In_ CRect						a_Rect1,
							_In_ CRect						a_Rect2);



private:
	CMyHelper();




private:
	CPlaneHomography						m_phFever2Real;
	CPlaneHomography						m_phReal2Fever;

};

