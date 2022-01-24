#pragma once




class ILock
{
public:
	ILock() {}
	virtual ~ILock() {}

	virtual void Lock() = 0;
	virtual void UnLock() = 0;
};



class CSection : public ILock
{
public:
	CSection() {
		InitializeCriticalSectionAndSpinCount(&m_hLock, 2000);
	}

	virtual ~CSection() { DeleteCriticalSection(&m_hLock); }

	virtual void Lock() { EnterCriticalSection(&m_hLock); }
	virtual void UnLock() { LeaveCriticalSection(&m_hLock); }
private:
	CRITICAL_SECTION	m_hLock;
};



class CLock
{
public:
	explicit CLock(ILock& a_pLock) : m_pLock(&a_pLock) { m_pLock->Lock(); }
	~CLock() { m_pLock->UnLock(); }

	void UnLock() { m_pLock->UnLock(); }
private:
	ILock* m_pLock;
	void Lock() { m_pLock->Lock(); }
};

