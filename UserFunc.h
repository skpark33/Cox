#pragma once

#include <vector>


// 
// \brief <pre>
// 내용을 넣으세요.
// </pre>
// \param   FMT
// \param   ...
// 
inline void DebugString(wchar_t const * FMT, ...)
{
	va_list args;
	va_start(args, FMT);
	size_t nLen = 0;
	nLen = _vsctprintf(FMT, args) + 3;
	std::vector<wchar_t> Buff(nLen, '\0');
	_vsnwprintf_s(&Buff[0], Buff.size(), nLen, FMT, args);
	va_end(args);
	_tcscat_s(&Buff[0], Buff.size(), _T("\r\n"));
	OutputDebugString(&Buff[0]);
}


// 
// \brief <pre>
// 내용을 넣으세요.
// </pre>
// \param   FMT
// \param   ...
// \return  CString
// 
inline CString GetMakeString(wchar_t const * FMT, ...)
{
	CString strTxt = _T("");
	va_list args;
	va_start(args, FMT);
	size_t nLen = 0;
	nLen = _vsctprintf(FMT, args) + 3;
	std::vector<wchar_t > Buff(nLen, '\0');
	_vsnwprintf_s(&Buff[0], Buff.size(), nLen, FMT, args);
	strTxt = &Buff[0];
	va_end(args);
	return strTxt;
}


// \brief
// 카메라의 FrameRate를 계산해준다.
//
// \code
// - 샘플 코드를 넣어주세요.
//
//	CFrameRate m_FrameRate;
//	m_FrameRate.CalcFrameRate();
//	..
//	..
//	..
//	DebugString(_T("FPS : %d"), m_FrameRate.GetFrameRate());
//
// \encode
// \warning
// \sa
// \author	이현석
// \date		2021-12-23 21:26:10		최초작성
class CFrameRate
{
public:
	CFrameRate(IN DWORD a_dwInterval = 1000)
		: m_dwInterval(a_dwInterval)
		, m_dwTick(::GetTickCount())
		, m_nFrameRate(0)
		, m_nFrameCount(0)
	{
	}

	~CFrameRate() {}

	void		CalcFrameRate(BOOL a_bShowFrame = FALSE)
	{
		DWORD dwCurr = GetTickCount();
		if( m_dwTick == 0 ) {
			m_nFrameCount++;
			m_dwTick = dwCurr;
		}
		else if( dwCurr - m_dwTick < m_dwInterval ) {
			m_nFrameCount++;
		}
		else if( dwCurr - m_dwTick >= m_dwInterval ) {
			m_nFrameCount++;
			m_nFrameRate = m_nFrameCount;
			m_dwTick = dwCurr;
			m_nFrameCount = 0;
			if( a_bShowFrame ) {
				DebugString(_T("FPS : %d"), m_nFrameRate);
			}
		}
	}

	uint32_t	GetFrameRate()
	{
		return m_nFrameRate;
	}

	uint32_t	GetElapsed()
	{
		DWORD dwTick = GetTickCount();
		DWORD dwRet = dwTick - m_dwTick;
		m_dwTick = dwTick;
		return dwRet;
	}

private:
	DWORD			m_dwInterval;
	DWORD			m_dwTick;
	uint32_t		m_nFrameRate;
	uint32_t		m_nFrameCount;
};


// \brief
// Tick계산을 해준다.
//
// \code
// - 샘플 코드를 넣어주세요.
// \encode
// \warning
// \sa
// \author	이현석
// \date		2018-07-30 15:25:08		최초작성
class CTickCheck
{
public:
	CTickCheck() { m_dwTick = GetTickCount(); m_bOneCheck = TRUE; }
	~CTickCheck() {}

	void operator=(CTickCheck rhs)
	{
		this->m_dwTick = rhs.m_dwTick;
	}

	BOOL IsPass(IN DWORD a_dwElapse, IN BOOL a_bKeepTick = FALSE)
	{
		BOOL bElapse = FALSE;
		DWORD dwTick = ::GetTickCount();
		if( dwTick - m_dwTick > a_dwElapse ) {
			bElapse = TRUE;

			if( !a_bKeepTick ) {
				m_dwTick = dwTick;
			}
		}
		else {
			bElapse = FALSE;
		}
		return bElapse;
	}

	DWORD	GetElapsed()
	{
		return ::GetTickCount() - m_dwTick;
	}

	void ResetTick()
	{
		m_dwTick = ::GetTickCount();
	}

	BOOL IsOneCheck()
	{
		if( m_bOneCheck ) {
			m_bOneCheck = FALSE;
			return TRUE;
		}
		return FALSE;
	}

	DWORD	GetTick()
	{
		return m_dwTick;
	}

	void	SetTick(IN DWORD a_dwTick)
	{
		m_dwTick = a_dwTick;
	}

	int		GetSecond()
	{
		return (int)((::GetTickCount() - m_dwTick) / 1000);
	}

private:
	DWORD		m_dwTick;
	BOOL		m_bOneCheck;
};



#define		CVTCLR(c)						theHelperMan().GetD2DColor(c)


#define		DeleteObj(x)					do {							\
												if( nullptr != x ) {		\
													delete x;				\
													x = nullptr;			\
												}							\
											} while( 0 )					

#define		DeleteAry(x)					do {							\
												if( nullptr != x ) {		\
													delete [] x;			\
													x = nullptr;			\
												}							\
											} while( 0 )					

#define		CloseHdl(x)						do {							\
												if( nullptr != x ) {		\
													CloseHandle(x);			\
													x = nullptr;			\
												}							\
											} while( 0 )					


#define SAFE_RELEASE(x) if( x != nullptr ) { x->Release(); x = nullptr; }
