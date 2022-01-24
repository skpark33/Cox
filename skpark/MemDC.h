#pragma once


// CMemDC
class CMemDC : public CDC
{
public:

	// constructor sets up the memory DC
	CMemDC(CDC* pDC) : CDC()
	{
		ASSERT(pDC != NULL);

		m_pDC = pDC;
		m_pOldBitmap = NULL;
		m_bMemDC = !pDC->IsPrinting();

		if (m_bMemDC)    // Create a Memory DC
		{
			pDC->GetClipBox(&m_rect);
			CreateCompatibleDC(pDC);
			m_bitmap.CreateCompatibleBitmap(pDC, m_rect.Width(), m_rect.Height());
			m_pOldBitmap = SelectObject(&m_bitmap);
			SetWindowOrg(m_rect.left, m_rect.top);
		}
		else        // Make a copy of the relevent parts of the current DC for printing
		{
			m_bPrinting = pDC->m_bPrinting;
			m_hDC       = pDC->m_hDC;
			m_hAttribDC = pDC->m_hAttribDC;
		}
	}

	CMemDC(CDC* pDC, CRect rect) : CDC()
	{
		ASSERT(pDC != NULL);

//		if(rect.Width() > 2000)
//			rect.right = rect.left + 2000;

		if(rect.Height() > 16380)
			rect.bottom = rect.top + 16380;

		m_pDC = pDC;
		m_pOldBitmap = NULL;
		m_bMemDC = !pDC->IsPrinting();
	          
		if (m_bMemDC)    // Create a Memory DC
		{
			m_rect = rect ;
			CreateCompatibleDC(pDC);
			m_bitmap.CreateCompatibleBitmap(pDC, m_rect.Width(), m_rect.Height());
			m_pOldBitmap = SelectObject(&m_bitmap);
			SetWindowOrg(m_rect.left, m_rect.top);
		}
		else        // Make a copy of the relevent parts of the current DC for printing
		{
			m_bPrinting = pDC->m_bPrinting;
			m_hDC       = pDC->m_hDC;
			m_hAttribDC = pDC->m_hAttribDC;
		}
	}

	// Destructor copies the contents of the mem DC to the original DC
	~CMemDC()
	{
		if (m_bMemDC) 
		{    
			// Copy the offscreen bitmap onto the screen.
			if(m_rect.Width() != 0 && m_rect.Height() != 0 )
			m_pDC->BitBlt(m_rect.left, m_rect.top, m_rect.Width(), m_rect.Height(),
						  this, m_rect.left, m_rect.top, SRCCOPY);

			//Swap back the original bitmap.
			SelectObject(m_pOldBitmap);
		} else {
			// All we need to do is replace the DC with an illegal value,
			// this keeps us from accidently deleting the handles associated with
			// the CDC that was passed to the constructor.
			m_hDC = m_hAttribDC = NULL;
		}
	}

	// Allow usage as a pointer
	CMemDC* operator->() {return this;}

	// Allow usage as a pointer
	operator CMemDC*() {return this;}

private:
	CBitmap  m_bitmap;      // Offscreen bitmap
	CBitmap* m_pOldBitmap;  // bitmap originally found in CMemDC
	CDC*     m_pDC;         // Saves CDC passed in constructor
	CRect    m_rect;        // Rectangle of drawing area.
	BOOL     m_bMemDC;      // TRUE if CDC really is a Memory DC.
};


// CMemDCAlt
class CMemDCAlt : public CDC
{
public:
	// constructor sets up the memory DC
	CMemDCAlt(CDC* pDC, CRect rect, int nXTickness, int nYTickness, DWORD dwRop)
	: CDC()
	, m_pDC(pDC)
	, m_dwRop(dwRop)
	, m_rect(rect)
	, m_top(rect)
	, m_bottom(rect)
	, m_left(rect)
	, m_right(rect)
	{
		ASSERT(pDC != NULL);

		m_top.bottom = m_top.top + nYTickness;
		m_bottom.top = m_bottom.bottom - nYTickness;
		m_left.right = m_left.left + nXTickness;
		m_right.left = m_right.right - nXTickness;

		CreateCompatibleDC(pDC);
		m_bitmap.CreateCompatibleBitmap(pDC, m_rect.Width(), m_rect.Height());
		m_pOldBitmap = SelectObject(&m_bitmap);
		SetWindowOrg(m_rect.left, m_rect.top);
	}

	// Destructor copies the contents of the mem DC to the original DC
	~CMemDCAlt()
	{
		// Copy the offscreen bitmap onto the screen.
		if(m_top.Width() != 0 && m_top.Height() != 0) m_pDC->BitBlt(m_top.left,    m_top.top,    m_top.Width(),    m_top.Height(),    this, m_top.left,    m_top.top,    m_dwRop);
		if(m_bottom.Width() != 0 && m_bottom.Height() != 0) m_pDC->BitBlt(m_bottom.left, m_bottom.top, m_bottom.Width(), m_bottom.Height(), this, m_bottom.left, m_bottom.top, m_dwRop);
		if(m_left.Width() != 0 && m_left.Height() != 0) m_pDC->BitBlt(m_left.left,   m_left.top,   m_left.Width(),   m_left.Height(),   this, m_left.left,   m_left.top,   m_dwRop);
		if(m_right.Width() != 0 && m_right.Height() != 0) m_pDC->BitBlt(m_right.left,  m_right.top,  m_right.Width(),  m_right.Height(),  this, m_right.left,  m_right.top,  m_dwRop);
//		m_pDC->BitBlt(m_rect.left, m_rect.top, m_rect.Width(), m_rect.Height(), this, m_rect.left, m_rect.top, m_dwRop);

		//Swap back the original bitmap.
		SelectObject(m_pOldBitmap);
	}

	// Allow usage as a pointer
	CMemDCAlt* operator->() {return this;}

	// Allow usage as a pointer
	operator CMemDCAlt*() {return this;}

private:
	CBitmap		m_bitmap;      // Offscreen bitmap
	CBitmap*	m_pOldBitmap;  // bitmap originally found in CMemDCAlt
	CDC*		m_pDC;         // Saves CDC passed in constructor
	CRect		m_rect;        // Rectangle of drawing area.
	DWORD		m_dwRop;
	CRect		m_top;
	CRect		m_bottom;
	CRect		m_left;
	CRect		m_right;
};
