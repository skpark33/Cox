/*! \file ciPointer.h
 *
 *  Copyright ⓒ 2003 WINCC Inc.
 *  All Rights Reserved.
 *
 *  \brief Memory leak을 방지하기 위한 smart safe pointer를 제공한다.
 *  	일단 assigned된 pointer는 ciPointer가 소유권을 가지며 
 *  	ciPointer의 life cycle이 끝나서  소멸자 호출시 자동으로 delete 하여 
 *  	memory leak을 방지한다. CORBA의 _var pointer와 개념이 비슷하다. 
 *  (Environment: OSF1 5.1A)
 *
 *  \author 	jhchoi
 *  \version
 *  \date 		2003년 4월 22일 18:00
 */

#ifndef _ciPointer_h_
#define _ciPointer_h_

#include "ciListType.h"

template <class OBJ>
class ciPointer {
public:
	ciPointer();
	ciPointer(OBJ *p_obj);	
   	~ciPointer();
   
	// OBJ* this()	{ return _obj; }
	void operator=(const OBJ *p_obj);
	void operator=(const ciPointer& ptr);

private: 
	OBJ* _obj;
};

#ifdef COP_CI_EXPORTS
template <class OBJ>
ciPointer<OBJ>::ciPointer() {
	_obj = NULL;
}

template <class OBJ>
ciPointer<OBJ>::ciPointer(OBJ *p_obj) {
	if ( _obj != NULL ) delete _obj;
	_obj = p_obj;
}

template <class OBJ>
ciPointer<OBJ>::~ciPointer() {
	if ( _obj != NULL ) delete _obj;
	_obj = NULL;
}

template <class OBJ>
void
ciPointer<OBJ>::operator=(const OBJ *p_obj) {
	if ( _obj != NULL ) delete _obj;
	_obj = p_obj;
}

template <class OBJ>
void
ciPointer<OBJ>::operator=(const ciPointer& ptr) {
	if ( _obj != NULL ) delete _obj;
	_obj = ptr;
}
#endif //COP_CI_EXPORTS

#endif //_ciPointer_h_
