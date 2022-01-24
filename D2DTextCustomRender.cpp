#include "stdafx.h"
#include "D2DTextCustomRender.h"


// 
// \brief <pre>
// 내용을 넣으세요.
// </pre>
// \param   a_pTarget
// 
CD2DTextCustomRender::CD2DTextCustomRender(ID2D1RenderTarget* a_pTarget)
	: m_pTarget(a_pTarget)
{
}
// 
// \brief <pre>
// 내용을 넣으세요.
// </pre>
// 
CD2DTextCustomRender::~CD2DTextCustomRender()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

// 
// \brief <pre>
// 내용을 넣으세요.
// </pre>
// \param   a_pFactory
// \param   a_pTarget
// \param   a_bFill
// \param   a_pFillBrush
// \param   a_bStroke
// \param   a_pStrokeBrush
// \param   a_fStrokeWidth
// 
CD2DOutlineRender::CD2DOutlineRender(
	ID2D1Factory* a_pFactory,
	ID2D1RenderTarget* a_pTarget,
	BOOL a_bFill,
	ID2D1SolidColorBrush* a_pFillBrush,
	BOOL a_bStroke,
	ID2D1SolidColorBrush* a_pStrokeBrush,
	FLOAT a_fStrokeWidth)
	: CD2DTextCustomRender(a_pTarget)
	, m_pFactory(a_pFactory)
	, m_bFill(a_bFill)
	, m_bStroke(a_bStroke)
	, m_pStrokeBrush(a_pStrokeBrush)
	, m_pFillBrush(a_pFillBrush)
	, m_fStrokeWidth(a_fStrokeWidth)
{
}
// 
// \brief <pre>
// 내용을 넣으세요.
// </pre>
// \param   void
// 
CD2DOutlineRender::~CD2DOutlineRender(void)
{
}
// 
// \brief <pre>
// 내용을 넣으세요.
// </pre>
// \param   *pContext
// \param   OriginX
// \param   OriginY
// \param   Mode
// \param   *pGlyph
// \param   *pDesc
// \param   *pEffect
// \return  HRESULT 
// 
HRESULT CD2DOutlineRender::DrawGlyphRun(
	void *pContext,
	FLOAT OriginX,
	FLOAT OriginY,
	DWRITE_MEASURING_MODE Mode,
	DWRITE_GLYPH_RUN const *pGlyph,
	DWRITE_GLYPH_RUN_DESCRIPTION const *pDesc,
	IUnknown *pEffect)
{
	const DWRITE_GLYPH_RUN &glyph = *pGlyph;
	CComPtr<ID2D1PathGeometry>			pPath = NULL;
	CComPtr<ID2D1TransformedGeometry>	pPathX = NULL;
	CComPtr<ID2D1GeometrySink>			pSink = NULL;
	CComPtr<IDWriteFontFace>			pFontFace = pGlyph->fontFace;

	HRESULT hr = S_OK;
	hr = m_pFactory->CreatePathGeometry(&pPath);
	if( FAILED(hr) ) {
		return hr;
	}
	hr = pPath->Open(&pSink);
	if( FAILED(hr) ) {
		return hr;
	}
	hr = pFontFace->GetGlyphRunOutline(
		glyph.fontEmSize,
		glyph.glyphIndices,
		glyph.glyphAdvances,
		glyph.glyphOffsets,
		glyph.glyphCount,
		glyph.isSideways,
		glyph.bidiLevel % 2,
		pSink
	);
	if( FAILED(hr) ) {
		return hr;
	}
	hr = pSink->Close();
	if( FAILED(hr) ) {
		return hr;
	}
	
	D2D1::Matrix3x2F const matrix = D2D1::Matrix3x2F(
		1.0f, 0.0f, 0.0f, 1.0f, OriginX, OriginY);
	m_pFactory->CreateTransformedGeometry(pPath, &matrix, &pPathX);
	if( FAILED(hr) ) {
		return hr;
	}

	if( pEffect != NULL ) {
		CComPtr<ID2D1SolidColorBrush> pBrush = NULL;
		pEffect->QueryInterface(__uuidof(ID2D1SolidColorBrush), reinterpret_cast<void**>(&pBrush));
		m_pTarget->FillGeometry(pPathX, pBrush);
		if( m_bStroke ) {
			CComPtr<ID2D1SolidColorBrush> pOutlineBrush = NULL;
			m_pTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &pOutlineBrush);
			m_pTarget->DrawGeometry(pPathX, pOutlineBrush, m_fStrokeWidth);
		}
	}
	else {
		if( m_bFill ) {
			if( m_pFillBrush == nullptr ) {
				hr = m_pTarget->CreateSolidColorBrush(
					D2D1::ColorF(1.0f, 1.0f, 1.0f), &m_pFillBrush);
				if( FAILED(hr) ) {
					return hr;
				}
				m_pTarget->FillGeometry(pPathX, m_pFillBrush);
			} else {
				m_pTarget->FillGeometry(pPathX, m_pFillBrush);
			}
		}
		if( m_bStroke ) {
			m_pTarget->DrawGeometry(pPathX, m_pStrokeBrush, m_fStrokeWidth);
		}
	}
	return hr;
}


// 
// \brief <pre>
// 내용을 넣으세요.
// </pre>
// \param   float				a_Point
// \return  float 
// 
float CD2DOutlineRender::ConvertPointSizeToDIP(
	IN float				a_Point)
{
	return (a_Point / 72.f) * 96.f;
}