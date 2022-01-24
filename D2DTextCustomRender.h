#pragma once

// \brief
// 모듈의 기능 및 설명 사용방법 기술.
//
// \code
// - 샘플 코드를 넣어주세요.
// \encode
// \warning
// \sa
// \author	이현석
// \date		2017-12-18 15:47:43		최초작성
class CD2DTextCustomRender : public IDWriteTextRenderer {
public:
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID RefIID, void **pObj) { return(E_NOINTERFACE); };
	ULONG STDMETHODCALLTYPE AddRef(void) { return(++RefCount); };
	ULONG STDMETHODCALLTYPE Release(void) {
		if( --RefCount <= 0 ) {
			delete this;
			return (0);
		}
		return (RefCount);
	};
	CD2DTextCustomRender(ID2D1RenderTarget* a_pTarget);
	virtual ~CD2DTextCustomRender(void);

protected:
	virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IsPixelSnappingDisabled(void *pContext, BOOL *pIsDisabled) {
		*pIsDisabled = 0;
		return (ERROR_SUCCESS);
	};
	virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetCurrentTransform(void *pContext, DWRITE_MATRIX* pXForm) {
		m_pTarget->GetTransform(reinterpret_cast<D2D1::Matrix3x2F*>(pXForm));
		return (ERROR_SUCCESS);
	};
	virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetPixelsPerDip(void *pContext, FLOAT *pPixelsPerDip) {
		FLOAT DpiX, DpiY;
		m_pTarget->GetDpi(&DpiX, &DpiY);
		*pPixelsPerDip = DpiX / 96;
		return (ERROR_SUCCESS);
	};
	virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE DrawGlyphRun(
		void *pContext,
		FLOAT OriginX,
		FLOAT OriginY,
		DWRITE_MEASURING_MODE Mode,
		DWRITE_GLYPH_RUN const *pGlyph,
		DWRITE_GLYPH_RUN_DESCRIPTION const *pDesc,
		IUnknown* pEffect)
	{
		return (ERROR_CALL_NOT_IMPLEMENTED);
	}
	virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE DrawUnderline(
		void *pContext,
		FLOAT OriginX,
		FLOAT OriginY,
		DWRITE_UNDERLINE const *pUnderline,
		IUnknown *pEffect)
	{
		return (ERROR_CALL_NOT_IMPLEMENTED);
	}
	virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE DrawStrikethrough(
		void *pContext,
		FLOAT OriginX,
		FLOAT OriginY,
		DWRITE_STRIKETHROUGH const *pStrike,
		IUnknown *pEffect)
	{
		return (ERROR_CALL_NOT_IMPLEMENTED);
	}
	virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE DrawInlineObject(
		void *pContext,
		FLOAT PosX,
		FLOAT PosY,
		IDWriteInlineObject *pObj,
		BOOL IsSideways,
		BOOL IsRTL,
		IUnknown *pEffect)
	{
		return (ERROR_CALL_NOT_IMPLEMENTED);
	}

public:
	ID2D1RenderTarget*	m_pTarget;

private:
	int  RefCount;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


class ColorDrawingEffect : public IUnknown
{
public:
	ColorDrawingEffect(D2D1::ColorF  a_color)
		: m_color(a_color)
	{}

	virtual ~ColorDrawingEffect() {}

	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID RefIID, void **pObj) { return(E_NOINTERFACE); };
	ULONG STDMETHODCALLTYPE AddRef(void) { return(++RefCount); };
	ULONG STDMETHODCALLTYPE Release(void)
	{
		if( --RefCount <= 0 ) {
			delete this;
			return (0);
		}
		return (RefCount);
	};	

	template<class Q>
	HRESULT STDMETHODCALLTYPE QueryInterface(_COM_Outptr_ Q** pp)
	{
		return QueryInterface(__uuidof(Q), (void **)pp);
	}

	D2D1::ColorF		GetColor()
	{
		return m_color;
	}

private:
	D2D1::ColorF		m_color;
	int					RefCount;
};







// \brief
// 모듈의 기능 및 설명 사용방법 기술.
//
// \code
// - 샘플 코드를 넣어주세요.
// \encode
// \warning
// \sa
// \author	이현석
// \date		2017-12-18 15:47:47		최초작성
class CD2DOutlineRender : public CD2DTextCustomRender {
public:
	CD2DOutlineRender(
		ID2D1Factory* a_pFactory,
		ID2D1RenderTarget* a_pTarget,
		BOOL a_bFill = TRUE,
		ID2D1SolidColorBrush* a_pFillBrush = NULL,
		BOOL a_bStroke = FALSE,
		ID2D1SolidColorBrush* a_pStrokeBrush = NULL,
		FLOAT a_fStrokeWidth = 0
	);

	~CD2DOutlineRender(void);

	void		SetFillText(BOOL a_bFill) { m_bFill = a_bFill; }

protected:
	COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE DrawGlyphRun(
		void *pContext,
		FLOAT OriginX,
		FLOAT OriginY,
		DWRITE_MEASURING_MODE Mode,
		DWRITE_GLYPH_RUN const *pGlyph,
		DWRITE_GLYPH_RUN_DESCRIPTION const *pDesc,
		IUnknown *pEffect);

private:
	float				ConvertPointSizeToDIP(
							IN float				a_Point);

private:
	FLOAT							m_fStrokeWidth;
	CComPtr<ID2D1SolidColorBrush>	m_pStrokeBrush;
	CComPtr<ID2D1SolidColorBrush>	m_pFillBrush;
	ID2D1Factory*					m_pFactory;
	BOOL							m_bFill;
	BOOL							m_bStroke;
};