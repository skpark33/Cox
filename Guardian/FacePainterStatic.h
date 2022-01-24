#pragma once

// CFacePainterStatic

class CFacePainterStatic : public CStatic
{
	DECLARE_DYNAMIC(CFacePainterStatic)

public:
	CFacePainterStatic();
	virtual ~CFacePainterStatic();

	void SetGuardian(CoxGuardian*	g) { m_cox_guardian = g; }


protected:
	DECLARE_MESSAGE_MAP()

public:

	afx_msg void OnPaint();

	CoxGuardian*	m_cox_guardian;
};


