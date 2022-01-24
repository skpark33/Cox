/*! \file ciStringTokenizer.h
 *
 *  Copyright ⓒ 2003 WINCC Inc.
 *  All Rights Reserved.
 *
 *  \brief string 을 delimiters 로 Tokenning 한다.
 *  (Environment: OSF1 5.1A)
 *
 *  \author 	jhchoi
 *  \version
 *  \date 		2003년 4월 22일 18:00
 */


#ifndef _ciStringTokenizer_h_
#define _ciStringTokenizer_h_

#include "ciListType.h"

//ciStringTokenizer Class
/**
 * The string tokenizer class allows an application to break a 
 * string into tokens. The tokenization method is simple.
 *
 * @param	str 			: a string to be parsed.
 * @param	delim 		: the char type delimiters.
 * @param	deliList		: the string type delimiters.
 * @param	returnDelims	: flag indicating whether to return the delimiters as tokens.
 * @param	nullToken		: flag indicating whether to return the null string as tokens
 */
class  ciStringTokenizer {
public:

	//Default Delimiters \n,\t,\r,\f,space로 문자열을 Tokenning 한다.
	ciStringTokenizer(const ciString& str);
	ciStringTokenizer(const char* str);

 	ciStringTokenizer(const ciString& str, const ciString& delim, ciBoolean nullToken = ciFalse,
				ciBoolean returnDelims = ciFalse);
	ciStringTokenizer(const char* str, const char* delim, ciBoolean nullToken = ciFalse,
				ciBoolean returnDelims = ciFalse);
	//string delimiters
	ciStringTokenizer(const ciString& str, const ciStringList& deliList, ciBoolean nullToken = ciFalse,
				ciBoolean returnDelims  = ciFalse);
	ciStringTokenizer(const char* str, const ciStringList& deliList, ciBoolean nullToken = ciFalse,
				ciBoolean returnDelims = ciFalse);
				
	~ciStringTokenizer();

	ciBoolean hasMoreTokens();
	ciInt countTokens();	
	
	ciString nextToken();
	ciString nextToken(const ciString& delim);
	ciString nextToken(const char* delim);
	ciString nextToken(const ciStringList& deliList); //string delimiters
	ciString nextToken(ciInt n);
	ciString restTokens();
	
	void nextToken_r(ciString& out);
	void nextToken_r(const ciString& delim, ciString& out);
	void nextToken_r(const char* delim, ciString& out);
	void nextToken_r(const ciStringList& deliList, ciString& out); //string delimiters
	void nextToken_r(ciInt n, ciString& out);
	void restTokens_r(ciString& out);

private:
	void _ciStringTokenizer(const char* str, const char* delim = " \n\t\r\f", 
					ciBoolean nullToken = ciFalse, ciBoolean retDelims = ciFalse);
	//string delimiters
	void _ciStringTokenizer(const char* str, const ciStringList& deliList, 
					ciBoolean nullToken = ciFalse, ciBoolean retDelims = ciFalse);
	void _setMaxDelimChar();
	void _setMaxDelimString(); //string delimiters
	ciInt _skipDelimiters(ciInt startPos);
	ciInt _scanToken(ciInt startPos);
	
	ciString _str;
	ciString _delimiters;
	ciStringList _delimStrList; //string delimiters
	/**
	* maxDelimChar stores the value of the delimiter character with the
	* highest value. It is used to optimize the detection of delimiter
	* characters.
	*/	
	char _maxDelimChar;
	ciString _maxDelimString; //string delimiters
	ciBoolean _retDelims;
	ciBoolean _delimsChanged;
	ciBoolean _nullToken;
	ciBoolean _strDelims;
	ciInt _currentPosition;
	ciInt _newPosition;
	ciInt _maxPosition;
};

#endif //_ciStringTokenizer_h_

