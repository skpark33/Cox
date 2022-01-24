/*! \file ciStringUtil.h
 *
 *  Copyright ⓒ 2003 WINCC Inc.
 *  All Rights Reserved.
 *
 *  \brief 문자열 조작 관련 Utility function 들을 모아놓은 class
 *  (Environment: OSF1 5.1A)
 *
 *  \author 	jhchoi
 *  \version
 *  \date 		2003년 4월 22일 18:00
 */

#ifndef _ciStringUtil_h_
#define _ciStringUtil_h_

#include "ciListType.h"

/* Bits set in the FLAGS argument to `fnmatch'.  */
#define CI_PATHNAME    (1 << 0) /* No wildcard can ever match `/'.  */
#define CI_NOESCAPE    (1 << 1) /* Backslashes don't quote special chars.  */
#define CI_PERIOD  		(1 << 2) /* Leading `.' is matched only explicitly.  */
#define CI_FILE_NAME   CI_PATHNAME /* Preferred GNU name.  */
#define CI_LEADING_DIR (1 << 3) /* Ignore `/...' after a match.  */
#define CI_CASEFOLD    (1 << 4) /* Compare without regard to case.  */

class ciStringUtil {
public:
	ciStringUtil();
	~ciStringUtil();

	static ciBoolean contain(const char* target, char pDeli);
	static void divide(const ciString& pTarget, char pDeli, ciString* pFirst, ciString* pOther);

	static void getBraceValue(ciString& target, const char* lb, const char* rb);
	static ciBoolean  hasBrace(ciString& target, const char* lb, const char* rb);
	static ciBoolean  hasBrace(const char* target, const char* lb, const char* rb);
	static int  getBraceValues(ciString& target, ciStringList* result, const char* lb, const char* rb);
	static int  getBraceValues(ciString& target, const char* deli,ciStringList* result, const char* lb, const char* rb);
	static int  getBraceValues(ciString& target, const char* deli,ciStringVector* result, const char* lb, const char* rb);
	static int 	getBraceValues(ciString& target, ciString& before, ciString& after, const char* deli, ciStringList* result, 
								const char* lb, const char* rb);

	static void getBraceValue(ciString& target);
	static ciBoolean  hasBrace(ciString& target);
	static ciBoolean  hasBrace(const char* target);
	static int  getBraceValues(ciString& target, const char* deli,ciStringList* result);
	static int 	getBraceValues(ciString& target, ciString& before, ciString& after, const char* deli, ciStringList* result);

	static void toUpper(const ciString& str, ciString& retStr);
	static void toLower(const ciString& str, ciString& retStr);    
	static void toUpper(ciString& str);
	static void toLower(ciString& str);
	static void safeToUpper(ciString& str);
	static void safeToLower(ciString& str);

	static void	trimPreSpace(ciString& str);
	static void trim(char* str);
	static ciString& trim(ciString& str);
	static void rightTrim(char* str);
	static void rightTrim(ciString& str);
	static void leftTrim(char* str);
	static void leftTrim(ciString& str);
	static void bothTrim(ciString& str);

	static void safeRemoveSpace(const char* src, char* dest);
	static void safeRemoveSpace(ciString& src);

	static void safeRemoveSpaceAndQuote(const char* src, char* dest);
	static void removeSpace(ciString& str);
	static void removeSpace(const char* src, char* dest);
	static void removeSpace(const char* src, ciString& dest);

	static ciString& removeQuote(ciString& str);
	static ciString& removeAllQuote(ciString& str);
	static ciInt addSingleQuote(const char* p_str, ciString& p_buf);

	static void charReplace(ciString& target, const char from, const char to);
	static void stringReplace(ciString& target, const char* from, const char* to);
	static void stringReplace(ciWString& target, const wchar_t* from, const wchar_t* to);
	static void stringReplaceWithToken(ciString& target, const char* from, const char* to, ciStringList& deli);

	static ciBoolean startsWith(const ciString& p_source, const ciString& p_dest);
	static ciBoolean endsWith(const ciString& p_source, const ciString& p_dest);

	static ciInt getFirstInteger(const char* p_str, const char* deli);

	static void btoa(ciBoolean tf, char* pStr);
	static void itoa(int p_int, char* pStr);	
	static void uitoa(unsigned int p_uint, char* pStr);	
	static void shtoa(short p_short, char* pStr);	
	static void ushtoa(unsigned short p_ushort, char* pStr);	
	static void ltoa(long p_long, char* pStr);	
	static void ultoa(unsigned long p_ulong, char* pStr);	
	static void lltoa(ciLongLong p_longlong, char* pStr);	
	static void ulltoa(ciULongLong p_ulonglong, char* pStr);	
	static void dtoa(double p_double, char* pStr);	
	static void ftoa(float p_float, char* pStr);	
	
	static void btoa(ciBoolean tf, ciString& str);
	static void itoa(int p_int, ciString& str);
	static void uitoa(unsigned int p_uint, ciString& str);
	static void shtoa(short p_short, ciString& str);
	static void ushtoa(unsigned short p_ushort, ciString& str);
	static void ltoa(long p_long, ciString& str);
	static void ultoa(unsigned long p_ulong, ciString& str);
	static void lltoa(ciLongLong p_longlong, ciString& str);
	static void ulltoa(ciULongLong p_ulonglong, ciString& str);
	static void dtoa(double p_double, ciString& str);
	static void ftoa(float p_float, ciString& str);	

	static ciBoolean	atoB(ciString& str,ciBoolean &pBool);
	static unsigned long long atoull(const char* str);
	
	static ciString& makeDigit(ciString& str);
	static ciBoolean isOnlyDigit(const char* str);
	static ciBoolean isOnlyLetter(const char* str);	
	static const char* int2String(ciInt p_src, ciString& p_dst);

	static void getPrefix(const ciString& target, ciString& retval, const char *prefix);
	static void cutPrefix(ciString& target, const char *prefix);
	static void cutPostfix(ciString& target, const char *prefix);
	static void cutPostfix2(ciString& target, const char *prefix);
	static void cutPrefix(const ciString& target, ciString& retval, const char *prefix);
	static void cutPostfix(const ciString& target, ciString& retval, const char *prefix);
	static void cutPostfix2(const ciString& target, ciString& retval, const char *prefix);

	static ciString getLastToken(const ciString& p_string, const ciString& p_token);
	static ciString getFirstToken(const ciString& p_string, const ciString& p_token);
	static ciString getFirstScopedName(const char* pScopedName);
	static ciString getParentScopedName(const char* pScopedName);

	static void getLastToken(const ciString& p_string, const char p_token, ciString& retval);
	static void getLastToken(const ciString& p_string, const ciString& p_token,ciString& retString);
	static void getFirstToken(const ciString& p_string, const ciString& p_token,ciString& retString);
	static void getFirstScopedName(const char* pScopedName, ciString& retString);
	static void getParentScopedName(const char* pScopedName, ciString& retString);
		
	static int Tokenizer(const char *object, 
							const char *deli, 
							list<ciString> *resultList);
	static int Tokenizer(const char *object, 
							const char *deli, 
							vector<ciString> *resultVector);

	static int multiTokenizer(const char *object,
									ciStringList& deliList, 
									ciStringList& resultList, 
									ciBoolean includeDeli );
	static ciString curTimeStr();
	static ciString time2Str(time_t tm);
	static ciString time2Str4CLI(time_t tm);
	static ciString time2StrD(time_t tm);

	static void curTimeStr(ciString& pOutTimeStr);
	static void time2Str(time_t tm, ciString& pOutTimeStr);
	static void time2StrD(time_t tm, ciString& pOutTimeStr);
	static void time2Str4CLI(time_t tm, ciString& pOutTimeStr);

	static time_t str2Time(const ciString& str);
	static time_t str2Time4CLI(const ciString& str);
	static time_t str2Time4GW(const ciString& str);

	static ciBoolean string2Bool(const char* str);

	static void getDisplayable(unsigned char* msgStream,
										int tokenCounter, 
										const char* deliminator,
										ciString& displayable);

	static void getDisplayableList(unsigned char* msgStream,
										int tokenCounter, 
										ciStringList& displayableList);


	static void getDisplayableList(unsigned char* msgStream,
                                    int tokenCounter,
                                    const char* deliminator,
                                    ciString& displayable,
                                    ciStringList& displayableList);

	static void splitString(const char* src, int size, ciStringList* resultList);

	static void StringListToString(ciStringList& src, ciString& target);

	static void uniqPush(ciStringList& src, const char* str);

#ifndef _COP_MSC_
	static ciBoolean regexpMatch(const char* string, const char* pattern);
#endif
	static ciBoolean simpleMatch(const char* string, const char* pattern);
	static int fnmatch(const char* pattern, const char* string, int flags);

	static int wildcmp(const char *wild, const char *string);
	static ciBoolean getWildPart(const char* pattern, ciString& value);

	static bool is_base64(unsigned char c) ;
	static string base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len); 
	static string base64_decode(string const& encoded_string) ;
	static unsigned char* base64_decode(string const& encoded_string, size_t& outlen);
};

#endif //_ciStringUtil_h_
