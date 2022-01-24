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

#include <ci/libBase/ciErrObject.h>
#include <stdarg.h>

ciErrObject::ciErrObject()
{
	init();
}

ciErrObject::~ciErrObject()
{
	init();
}

void
ciErrObject::error(const char* fmt,...)
{
	va_list ap;
	char buf[1024];
	memset(buf, 0x00, sizeof(buf));
	va_start(ap, fmt);
	vsprintf(buf, fmt, ap);
	va_end(ap);

	_errMsg = buf;
	_errCode = CI_ERROR;
}

void
ciErrObject::warn(const char* fmt,...)
{
	va_list ap;
	char buf[1024];
	memset(buf, 0x00, sizeof(buf));
	va_start(ap, fmt);
	vsprintf(buf, fmt, ap);
	va_end(ap);

	_errMsg = buf;
	_errCode = CI_WARN;
}

void
ciErrObject::success(const char* fmt,...)
{
	va_list ap;
	char buf[1024];
	memset(buf, 0x00, sizeof(buf));
	va_start(ap, fmt);
	vsprintf(buf, fmt, ap);
	va_end(ap);

	_errMsg = buf;
	_errCode = CI_SUCCESS;
}
