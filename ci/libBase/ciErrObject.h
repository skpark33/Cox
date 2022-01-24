/*! \class ciErrObject
 *  Copyright ¨Ï 2004, SQIsoft. All rights reserved.
 *
 *  \brief CCI Call Value
 *  (Environment: VisiBroker 5.2 HP-UX B.11.11)
 *
 *  \author jhchoi
 *  \version 1.0
 *  \date 2004/08/18 13:01:00
 */

#ifndef __ciErrObject_h__
#define __ciErrObject_h__

#include <ci/libBase/ciBaseType.h>
#include <ci/libBase/ciMacro.h>

#define CI_SUCCESS   1
#define CI_INIT	     0
#define CI_WARN	    -1
#define CI_ERROR	-2

class ciErrObject {
public:
	ciErrObject();
	virtual ~ciErrObject();

	void			init() { _errMsg = "";  _errCode = CI_INIT;}

	void			error(const char* fmt,...);
	void			error(string& pErrMsg) { _errMsg = pErrMsg;  _errCode = CI_ERROR;}

	void			warn(const char* fmt,...);
	void			warn(string& pErrMsg) { _errMsg = pErrMsg;  _errCode = CI_WARN;}

	void			success(const char* fmt,...);
	void			success(string& pErrMsg) { _errMsg = pErrMsg;  _errCode = CI_SUCCESS;}

	const char*		getMsg() { return _errMsg.c_str(); }
	int				getCode() {return _errCode;}

protected :

	int				_errCode;
	string			_errMsg;
};

#endif	// __ciErrObject_h__
