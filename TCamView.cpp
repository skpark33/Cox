#include "stdafx.h"
#include "TCamView.h"
#include "MyHelperMan.h"
#include "VisableCamera.h"
#include "ThermalCamera.h"
#include "resource.h"
#include "INIManager.h"
#include "skpark/TraceLog.h"


CTCamView::CTCamView()
	: m_pFactory(nullptr)
	, m_pTarget(nullptr)
	, m_pBitmapTarget(nullptr)
	, m_pWICFactory(nullptr)
	, m_pWriteFactory(nullptr)
	, m_pBrush(nullptr)
	, m_pTextFmt(nullptr)
	, m_pBitmap(nullptr)
	, m_pLogoBmp(nullptr)
	, m_pParentWnd(nullptr)
	, m_nWidth(0)
	, m_nHeight(0)
	, m_fScaleX(0.f)
	, m_fScaleY(0.f)
	, m_pCamFrameInfo(nullptr)
	, m_nStatus(CAM_NONE)
	, m_nCutLeft(0)
	, m_nCutTop(0)
	, m_nCutRight(0)
	, m_nCutBottom(0)
{
}


CTCamView::~CTCamView()
{
}


BEGIN_MESSAGE_MAP(CTCamView, CStatic)
	ON_WM_MOUSEMOVE()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()


void CTCamView::OnMouseMove(UINT nFlags, CPoint point)
{
	CStatic::OnMouseMove(nFlags, point);
}


void CTCamView::OnPaint()
{
	CPaintDC dc(this);
	if( m_pTarget != nullptr && m_pBitmapTarget != nullptr ) {
		D2D1_SIZE_F szTarget = m_pTarget->GetSize();
		D2D1_SIZE_F szBmpTarget = m_pBitmapTarget->GetSize();

		if( szTarget.width != szBmpTarget.width
		   || szTarget.height != szBmpTarget.height )
		{
			m_pBitmapTarget->Release();
			m_pBitmapTarget = nullptr;
			m_pTarget->CreateCompatibleRenderTarget(&m_pBitmapTarget);
		}

		CRect rc;
		GetClientRect(&rc);

		DrawScreen();
		CComPtr<ID2D1Bitmap> pBitmap = nullptr;
		m_pBitmapTarget->GetBitmap(&pBitmap);

		m_pTarget->BeginDraw();
		m_pTarget->Clear(CVTCLR(RGB_BLACK));
		m_pTarget->SetTransform(D2D1::Matrix3x2F::Identity());
		m_pTarget->DrawBitmap(pBitmap, CD2DRectF(rc));
		m_pTarget->EndDraw();
	}
}


void CTCamView::OnSize(UINT nType, int cx, int cy)
{
	if( m_pTarget != nullptr ) {
		m_pTarget->Resize(CD2DSizeU(cx, cy));
	}

	RecalcLayout();
	CStatic::OnSize(nType, cx, cy);
}


// 
// \brief <pre>
// �̹��� ������ �� ������ ���� ���
// </pre>
// 
void CTCamView::RecalcLayout()
{
	CRect rc;
	GetClientRect(&rc);

	// ���� �̹����� ��ũ��View�� ���� ���
	m_fScaleX = (float)rc.Width() / m_nWidth;
	m_fScaleY = (float)rc.Height() / m_nHeight;

	if( theINIMan().IsThermalZoom() ) {
		m_rcZoom.SetRect(m_nCutLeft,
						 m_nCutTop,
						 m_nWidth - m_nCutRight,
						 m_nHeight - m_nCutBottom);
		m_fZoomScaleX = (float)m_nWidth / m_rcZoom.Width();
		m_fZoomScaleY = (float)m_nHeight / m_rcZoom.Height();
	}
	else {
		m_rcZoom.SetRect(0, 0, m_nWidth, m_nHeight);
		m_fZoomScaleX = 1.f;
		m_fZoomScaleY = 1.f;
	}
}


// 
// \brief <pre>
// �ʱ�ȭ
// </pre>
// \param   CWnd*			a_pParent
// 
void CTCamView::InitializeEx(
	_In_ CWnd*			a_pParent)
{
	m_pParentWnd = a_pParent;

	theHelperMan().InitDirect2D(GetSafeHwnd(),
								&m_pFactory,
								&m_pTarget,
								&m_pBitmapTarget,
								&m_pWICFactory,
								&m_pWriteFactory);

	theHelperMan().InitD2DTextfmt(m_pWriteFactory,
								  _T("Arial"),
								  20.f,
								  &m_pTextFmt);

	m_pBitmapTarget->CreateSolidColorBrush(CVTCLR(RGB_BLACK),
										   &m_pBrush);

	theHelperMan().InitD2DBitmapFromPNG(m_pWICFactory,
										m_pBitmapTarget,
										IDB_PNG1,
										_T("PNG"),
										&m_pLogoBmp);

	// �����̹������� �̹����� �ڸ� ��
	if( theINIMan().IsThermalZoom() ) {
		CRect rcCut(theINIMan().GetTCamCutRect());
		
		// �¿���� �̻��
		if( !theINIMan().IsMirror() ) {
			m_nCutLeft		= rcCut.left;
			m_nCutTop		= rcCut.top;
			m_nCutRight		= rcCut.right;
			m_nCutBottom	= rcCut.bottom;
		}
		// �¿���� ���
		else {
			m_nCutLeft		= rcCut.right;
			m_nCutTop		= rcCut.top;
			m_nCutRight		= rcCut.left;
			m_nCutBottom	= rcCut.bottom;
		}
	}
}


// 
// \brief <pre>
// ȭ�� Draw
// </pre>
// 
void CTCamView::DrawScreen()
{
	switch( m_nStatus ) {
		case CAM_NONE:
			DrawEmpty();
			break;

		case CAM_CONNECTING:
			DrawConnecting();
			break;

		case CAM_STREAMING:
			DrawStreaming();
			break;

		case CAM_DISCONNECT:
			DrawDisconnect();
			break;
	}
}


// 
// \brief <pre>
// �ʱ���� Draw
// </pre>
// 
void CTCamView::DrawEmpty()
{
	CRect rc;
	GetClientRect(&rc);
	m_pBitmapTarget->BeginDraw();
	m_pBitmapTarget->Clear(CVTCLR(RGB_BLACK));
	m_pBitmapTarget->SetTransform(D2D1::Matrix3x2F::Identity());

	CRect rcLogo(rc.CenterPoint(), rc.CenterPoint());
	rcLogo.InflateRect(170, 50);
	m_pBitmapTarget->DrawBitmap(m_pLogoBmp, CD2DRectF(rcLogo));

	m_pBitmapTarget->EndDraw();
}


// 
// \brief <pre>
// Connecting Draw
// </pre>
// 
void CTCamView::DrawConnecting()
{
	CRect rc;
	GetClientRect(&rc);
	m_pBitmapTarget->BeginDraw();
	m_pBitmapTarget->Clear(CVTCLR(RGB_BLACK));
	m_pBitmapTarget->SetTransform(D2D1::Matrix3x2F::Identity());

	m_pBrush->SetOpacity(1.f);
	m_pBrush->SetColor(CVTCLR(RGB_WHITE));

	CString strTxt;
	strTxt.Format(_T("������..."));
	m_pBitmapTarget->DrawText(strTxt,
							  strTxt.GetLength(),
							  m_pTextFmt,
							  CD2DRectF(rc),
							  m_pBrush);

	m_pBitmapTarget->EndDraw();
}


// 
// \brief <pre>
// Streaming Draw
// </pre>
// 
void CTCamView::DrawStreaming()
{
	if( m_pBitmap == nullptr ) {
		m_pBitmapTarget->BeginDraw();
		m_pBitmapTarget->Clear(CVTCLR(RGB_BLACK));
		m_pBitmapTarget->SetTransform(D2D1::Matrix3x2F::Identity());
		m_pBitmapTarget->EndDraw();
		return;
	}

	CRect rc;
	GetClientRect(&rc);
	m_pBitmapTarget->BeginDraw();
	m_pBitmapTarget->Clear(CVTCLR(RGB_BLACK));
	m_pBitmapTarget->SetTransform(D2D1::Matrix3x2F::Identity());

	// SetData�� ������ ������ data�� ������
	// ��Ʈ���� ���Ž�Ų��.
	if( m_pCamFrameInfo != nullptr ) {
		m_pBitmap->CopyFromMemory(nullptr,
								  m_pCamFrameInfo->thermal_image.data,
								  m_nWidth * 4);
	}

	if( m_pBitmap ) {
		// �����̹����� �߶��ٸ�
		if( theINIMan().IsVisableZoom() ) {
			// �����̹������� m_rcZoom ������ rc������ Ȯ���ؼ� �׷��ش�.
			m_pBitmapTarget->DrawBitmap(m_pBitmap,
										CD2DRectF(rc),
										1.f,
										D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
										CD2DRectF(m_rcZoom));
		}
		// �����̹��� �״�� ���
		else {
			m_pBitmapTarget->DrawBitmap(m_pBitmap, CD2DRectF(rc));
		}
	}

	m_nFaceCount = theVCMan().GetFaceDetectInfo(m_arrFace);
	if( m_nFaceCount > 0 ) {
		DrawFaceBox();
	}

	// DrawFPS
	CRect rcFPS(rc);
	rcFPS.right = rcFPS.left + 200;
	rcFPS.bottom = rcFPS.top + 70;
	m_pBrush->SetOpacity(0.5f);
	m_pBrush->SetColor(CVTCLR(RGB_BLACK));
	m_pBitmapTarget->FillRectangle(CD2DRectF(rcFPS), m_pBrush);

	m_pBrush->SetOpacity(1.f);
	m_pBrush->SetColor(CVTCLR(RGB_WHITE));
	CString strFPS;
	strFPS.Format(_T("StreamFPS: %d"),
				  theTCMan().GetStreamFPS());
	m_pBitmapTarget->DrawText(strFPS,
							  strFPS.GetLength(),
							  m_pTextFmt,
							  CD2DRectF(rcFPS),
							  m_pBrush);

	// ���콺 ��ġ -> �µ� ���
	// ������ǥ�� (x, y)
	if( m_pCamFrameInfo ) {
		CPoint point;
		GetCursorPos(&point);
		ScreenToClient(&point);
		CRect rcTxt(point, point);

		if( PtInRect(rc, point) ) {
			point = GetZoom2Img2ScalePoint(point);

			rcTxt.InflateRect(50, 30);
			rcTxt.OffsetRect(0, -25);

			if( rc.left > rcTxt.left ) {
				rcTxt.MoveToX(rc.left);
			}
			if( rc.top > rcTxt.top ) {
				rcTxt.MoveToY(rc.top);
			}
			if( rc.right < rcTxt.right ) {
				rcTxt.MoveToX(rc.right - rcTxt.Width());
			}
			if( rc.bottom < rcTxt.bottom ) {
				rcTxt.MoveToY(rc.bottom - rcTxt.Height());
			}

			m_pBrush->SetOpacity(0.5f);
			m_pBrush->SetColor(CVTCLR(RGB_BLACK));
			m_pBitmapTarget->FillRectangle(CD2DRectF(rcTxt), m_pBrush);

			m_pBrush->SetOpacity(1.f);
			m_pBrush->SetColor(CVTCLR(RGB_WHITE));
			float fTemp = m_pCamFrameInfo->thermal_temp.data[point.y * m_nWidth + point.x];
			CString strTemp;
			strTemp.Format(_T("%.1f\r\n[%d, %d]"), fTemp, point.x, point.y);
			m_pBitmapTarget->DrawText(strTemp,
									  strTemp.GetLength(),
									  m_pTextFmt,
									  CD2DRectF(rcTxt),
									  m_pBrush);
		}
	}

	// ���ٵ� ����Ѵٸ� ��ġ Draw
	if( theINIMan().IsUseBlackbody() && m_pCamFrameInfo ) {
		CRect rcBB(theINIMan().GetBlackbodyPos(),
				   theINIMan().GetBlackbodyPos());
		rcBB.InflateRect(2, 2);

		if( theINIMan().IsMirror() ) {
			rcBB.left		= m_nWidth - rcBB.left;
			rcBB.right		= m_nWidth - rcBB.right;
		}
		rcBB = GetImg2ZoomScaleRect(rcBB);

		m_pBitmapTarget->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);
		m_pBrush->SetOpacity(1.f);
		m_pBrush->SetColor(CVTCLR(RGB_RED));
		m_pBitmapTarget->DrawRectangle(CD2DRectF(rcBB), m_pBrush);
		m_pBitmapTarget->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
	}

	m_pBitmapTarget->EndDraw();
}


// 
// \brief <pre>
// Disconenct Draw
// </pre>
// 
void CTCamView::DrawDisconnect()
{
	CRect rc;
	GetClientRect(&rc);
	m_pBitmapTarget->BeginDraw();
	m_pBitmapTarget->Clear(CVTCLR(RGB_BLACK));
	m_pBitmapTarget->SetTransform(D2D1::Matrix3x2F::Identity());

	m_pBrush->SetOpacity(1.f);
	m_pBrush->SetColor(CVTCLR(RGB_WHITE));

	CString strTxt;
	strTxt.Format(_T("��������"));
	m_pBitmapTarget->DrawText(strTxt,
							  strTxt.GetLength(),
							  m_pTextFmt,
							  CD2DRectF(rc),
							  m_pBrush);

	m_pBitmapTarget->EndDraw();
}


// 
// \brief <pre>
// Draw�� ���۸� ����
// </pre>
// \param   CAMF_INFO*			a_pCamFrameInfo
// 
void CTCamView::SetData(
	_In_ CAMF_INFO*			a_pCamFrameInfo)
{
	m_pCamFrameInfo = a_pCamFrameInfo;
}


// 
// \brief <pre>
// �̹��� �ػ� ���� �� ������ ���� ���
// </pre>
// \param   int			a_nWidth
// \param   int			a_nHeight
// 
void CTCamView::SetImageSize(
	_In_ int			a_nWidth,
	_In_ int			a_nHeight)
{
	m_nWidth	= a_nWidth;
	m_nHeight	= a_nHeight;
	
	RecalcLayout();

	// �̹��� ������� Direct2D ��Ʈ�� ����
	if( m_pBitmap ) {
		m_pBitmap->Release();
		m_pBitmap = nullptr;
	}
	if( m_pBitmap == nullptr ) {
		m_pBitmapTarget->CreateBitmap(CD2DSizeU(m_nWidth, m_nHeight),
									  D2D1::BitmapProperties(
										  D2D1::PixelFormat(
											  DXGI_FORMAT_B8G8R8A8_UNORM,
											  D2D1_ALPHA_MODE_IGNORE)),
									  &m_pBitmap);
	}
}


// 
// \brief <pre>
// �� �ڽ� �� �µ� Draw
// </pre>
// 
void CTCamView::DrawFaceBox()
{
	CRect rc;
	GetClientRect(&rc);

	int offset = 100;

	for( UINT i = 0 ; i < m_nFaceCount ; ++i ) {
		auto pFace = &m_arrFace[i];

		CRect rcTemp	= DrawBox(pFace);
		rcTemp.top		-= 30;
		rcTemp.bottom	= rcTemp.top;
		rcTemp.left		= rcTemp.CenterPoint().x - offset;
		rcTemp.right	= rcTemp.left + (offset * 2);
		
		m_pBrush->SetOpacity(0.5f);
		m_pBrush->SetColor(CVTCLR(RGB_BLACK));
		m_pBitmapTarget->FillRectangle(CD2DRectF(rcTemp), m_pBrush);

		// �µ��� ����� Rect������ ȭ�� ������ ���� �� ���
		// ��ġ�� �������ش�.
		if( rcTemp.left < rc.left ) {
			rcTemp.MoveToX(rc.left);
		}
		if( rcTemp.top < rc.top ) {
			rcTemp.MoveToY(rc.top);
		}
		if( rcTemp.right > rc.right ) {
			rcTemp.MoveToX(rc.right - rcTemp.Width());
		}
		if( rcTemp.bottom > rc.bottom ) {
			rcTemp.MoveToY(rc.bottom - rcTemp.Height());
		}

		// �µ� ���
		CString strTemp;
		strTemp.Format(_T("%.1f (%s)"),
					   pFace->face_temp,
					   pFace->mask_state == MASK_WEARING_WELL
							? _T("O")
							: _T("X"));

		D2D_DRAW_TEXT_INFO txtInfo(
			strTemp,
			_T("Arial"),						// ��Ʈ
			40.f,								// ��Ʈ ������
			rcTemp,								// ����
			DWRITE_TEXT_ALIGNMENT_CENTER,		// Horizontal �߾� ����
			DWRITE_PARAGRAPH_ALIGNMENT_CENTER,	// Vertical �߾� ����
			TRUE,
			CVTCLR(RGB_WHITE),					// Font���� ����
			TRUE,
			CVTCLR(RGB_BLUE),					// Font Outline ����
			3.f,								// Outline �β�
			DWRITE_FONT_WEIGHT_ULTRA_BLACK);	// Font �β�

		// Outline�� �ִ� Text�� �׷��ش�.
		theHelperMan().D2DDrawOutlineText(m_pFactory,
										  m_pBitmapTarget,
										  m_pWriteFactory,
										  txtInfo);
	}
}


// 
// \brief <pre>
// �� �ڽ��� �׷��ش�.
//
// ��	��
//
//
// ��	�� ���·�
//
// </pre>
// \param   FACE_INFO*		a_pFaceInfo
// \return  CRect 
// 
CRect CTCamView::DrawBox(
	_In_ FACE_INFO*		a_pFaceInfo)
{
	CRect box;
	
	// �����̹����� �߶��ٸ�
	if( theINIMan().IsVisableZoom() ) {
		// ���� -> ��ũ��View �����ϸ� -> ��ȭ�� �����ϸ�
		box = GetImg2ZoomScaleRect(a_pFaceInfo->thermal_facebox);
	}
	else {
		// ���� -> ��ũ��View �����ϸ�
		box = GetImg2ScreenScaleRect(a_pFaceInfo->thermal_facebox);
	}

	m_pBrush->SetOpacity(1.f);
	m_pBrush->SetColor(CVTCLR(RGB_BLUE));

	float fThickness	= 4.f;
	int length			= 15;
	int offset			= (int)(fThickness / 2);
	CD2DPointF pt1, pt2, pt3, pt4;

	// ��
	pt1 = CPoint(box.TopLeft().x, box.TopLeft().y - offset);
	pt2 = CPoint(box.TopLeft().x, box.TopLeft().y + length);
	pt3 = CPoint(box.TopLeft().x - offset, box.TopLeft().y);
	pt4 = CPoint(box.TopLeft().x + length, box.TopLeft().y);
	m_pBitmapTarget->DrawLine(pt1, pt2, m_pBrush, fThickness);
	m_pBitmapTarget->DrawLine(pt3, pt4, m_pBrush, fThickness);

	// ��
	pt1 = CPoint(box.right, box.top - offset);
	pt2 = CPoint(box.right, box.top + length);
	pt3 = CPoint(box.right + offset, box.top);
	pt4 = CPoint(box.right - length, box.top);
	m_pBitmapTarget->DrawLine(pt1, pt2, m_pBrush, fThickness);
	m_pBitmapTarget->DrawLine(pt3, pt4, m_pBrush, fThickness);

	// ��
	pt1 = CPoint(box.right, box.bottom + offset);
	pt2 = CPoint(box.right, box.bottom - length);
	pt3 = CPoint(box.right + offset, box.bottom);
	pt4 = CPoint(box.right - length, box.bottom);
	m_pBitmapTarget->DrawLine(pt1, pt2, m_pBrush, fThickness);
	m_pBitmapTarget->DrawLine(pt3, pt4, m_pBrush, fThickness);

	// ��
	pt1 = CPoint(box.left, box.bottom + offset);
	pt2 = CPoint(box.left, box.bottom - length);
	pt3 = CPoint(box.left - offset, box.bottom);
	pt4 = CPoint(box.left + length, box.bottom);
	m_pBitmapTarget->DrawLine(pt1, pt2, m_pBrush, fThickness);
	m_pBitmapTarget->DrawLine(pt3, pt4, m_pBrush, fThickness);

	return box;
}


// 
// \brief <pre>
// ī�޶� ���°� ����
// </pre>
// \param   CAMERA_STATUS	a_nStatus
// 
void CTCamView::SetCamStatus(
	_In_ CAMERA_STATUS	a_nStatus)
{
	m_nStatus = a_nStatus;
	TraceLog((_T("CTCamView::SetCamStatus End, Invalidate!!!!")));
	Invalidate(FALSE);
}


// 
// \brief <pre>
// Direct2D �ڿ��� �����Ѵ�.
// </pre>
// 
void CTCamView::OnDestroy()
{
	CStatic::OnDestroy();

	SAFE_RELEASE(m_pLogoBmp);
	SAFE_RELEASE(m_pBitmap);
	SAFE_RELEASE(m_pTextFmt);
	SAFE_RELEASE(m_pBrush);
	SAFE_RELEASE(m_pWriteFactory);
	SAFE_RELEASE(m_pWICFactory);
	SAFE_RELEASE(m_pBitmapTarget);
	SAFE_RELEASE(m_pTarget);
	SAFE_RELEASE(m_pFactory);
}


// 
// \brief <pre>
// ����ǥ�� ������ǥ�� ��ȯ
// </pre>
// \param   CPoint			a_pt
// \return  CPoint 
// 
CPoint CTCamView::GetZoom2Img2ScalePoint(
	_In_ CPoint			a_pt)
{
	CPoint ptCvt;
	ptCvt.x =	(int)
				(
					(
						((float)a_pt.x / m_fZoomScaleX)
						/ m_fScaleX
					)
					+ m_rcZoom.TopLeft().x
				);
	ptCvt.y =	(int)
				(
					(
						((float)a_pt.y / m_fZoomScaleY)
						/ m_fScaleY
					)
					+ m_rcZoom.TopLeft().y
				);
	return ptCvt;
}


// 
// \brief <pre>
// ��ũ����ǥ�� ������ǥ�� ��ȯ
// </pre>
// \param   CPoint			a_pt
// \return  CPoint 
// 
CPoint CTCamView::GetScr2Img2ScalePoint(
	_In_ CPoint			a_pt)
{
	CPoint ptCvt;
	ptCvt.x = (int)((float)a_pt.x / m_fScaleX);
	ptCvt.y = (int)((float)a_pt.y / m_fScaleY);
	return ptCvt;
}


// 
// \brief <pre>
// �����̹��� ��ǥ�踦 ��ȭ�� ��ǥ��� ��ȯ
// </pre>
// \param   CPoint			a_pt
// \return  CPoint 
// 
CPoint CTCamView::GetImg2ZoomScalePoint(
	_In_ CPoint			a_pt)
{
	CPoint ptCvt;
	ptCvt.x			=	(int)
						(
							(
								((float)a_pt.x - m_rcZoom.TopLeft().x)
								* m_fScaleX
							)
							* m_fZoomScaleX
						);
	ptCvt.y			=	(int)
						(
							(
								((float)a_pt.y - m_rcZoom.TopLeft().y)
								* m_fScaleY
							)
							* m_fZoomScaleY
						);
	return ptCvt;
}


// 
// \brief <pre>
// �����̹��� ��ǥ�踦 ��ũ��View ��ǥ��� ��ȯ
// </pre>
// \param   CPoint			a_pt
// \return  CPoint 
// 
CPoint CTCamView::GetImg2ScreenScalePoint(
	_In_ CPoint			a_pt)
{
	CPoint ptCvt;
	ptCvt.x = (int)((float)a_pt.x * m_fScaleX);
	ptCvt.y = (int)((float)a_pt.y * m_fScaleY);
	return ptCvt;
}


// 
// \brief <pre>
// �����̹��� ��ǥ�踦 ��ȭ�� ��ǥ��� ��ȯ
// </pre>
// \param   CRect			a_rc
// \return  CRect 
// 
CRect CTCamView::GetImg2ZoomScaleRect(
	_In_ CRect			a_rc)
{
	CRect rcCvt;
	rcCvt.left		=	(int)
						(
							(
								((float)a_rc.left - m_rcZoom.TopLeft().x)
								* m_fScaleX
							)
							* m_fZoomScaleX
						);
	rcCvt.top		=	(int)
						(
							(
								((float)a_rc.top - m_rcZoom.TopLeft().y)
								* m_fScaleY
							)
							* m_fZoomScaleY
						);
	rcCvt.right		=	(int)
						(
							(
								((float)a_rc.right - m_rcZoom.TopLeft().x)
								* m_fScaleX
							)
							* m_fZoomScaleX
						);
	rcCvt.bottom	=	(int)
						(
							(
								((float)a_rc.bottom - m_rcZoom.TopLeft().y)
								* m_fScaleY
							)
							* m_fZoomScaleY
						);
	return rcCvt;
}


// 
// \brief <pre>
// �����̹��� ��ǥ�踦 ��ũ��View ��ǥ��� ��ȯ
// </pre>
// \param   CRect			a_rc
// \return  CRect 
// 
CRect CTCamView::GetImg2ScreenScaleRect(
	_In_ CRect			a_rc)
{
	CRect rcCvt;
	rcCvt.left		= (int)((float)a_rc.left * m_fScaleX);
	rcCvt.top		= (int)((float)a_rc.top * m_fScaleY);
	rcCvt.right		= (int)((float)a_rc.right * m_fScaleX);
	rcCvt.bottom	= (int)((float)a_rc.bottom * m_fScaleY);
	return rcCvt;
}


// 
// \brief <pre>
// Zoom������ ����
// </pre>
// \return  CRect 
// 
CRect CTCamView::GetZoomRect()
{
	return m_rcZoom;
}