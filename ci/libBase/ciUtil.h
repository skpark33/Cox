/*! \file ciUtil.h
 *
 *  Copyright ⓒ 2003 WINCC Inc.
 *  All Rights Reserved.
 *
 *  \brief 각종 Utility function 들을 모아놓은 class
 *  (Environment: OSF1 5.1A)
 *
 *  \author 	jhchoi
 *  \version
 *  \date 		2003년 4월 22일 18:00
 */

#ifndef __ciUtil_h__
#define __ciUtil_h__

#include "ciListType.h"

class  ciUtil {
public:
	ciUtil();
	~ciUtil();

	static ciUInt getThreadId();
	static ciString getIpAddress();
	static ciInt system(const char* cmd); 
};

#ifdef __ASSERT_FUNCTION    // in GCC LIBRARY
	#define ASSERT_FUNC " operation : "<<__ASSERT_FUNCTION
#else
	#define ASSERT_FUNC ""
#endif

#define COP_ASSERT(exp) \
    if(!(exp)) { \
        cerr<<"in "<<__FILE__<<" line : "<<__LINE__<<" "<<ASSERT_FUNC; \
        cerr<<" Assertion Failed !!"; \
        abort(); \
    }

#endif //__ciUtil_h__
