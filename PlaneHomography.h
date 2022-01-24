#pragma once


#include <io.h> // for access()
#include <direct.h> // for mkdir()
 
#include "SMatrix.h"
// ��� ���
//
//
// 4���� ���� �Է��Ѵ�.
//     // ���� ��ǥ
//		usrPOINT p1[4]={{0,0},{140,0},{140,90},{0,90}}; 
//      �Ǽ��� ��ǥ
//		usrPOINT p2[4]={{295.6825,24.6023},{49.2821,303.4359},{87.9020,574.4706},{424.3928,465.4169}}; // �ȼ� ��ǥ
// 		SetValue(p1,p2);
//      ....
//      ....
//     	GetRealCoord(p2[1]); // ���� ����Ʈ�� ���� �Ǽ��� ��ǥ�� ��´�.
//
//

#define POINTS_NUM 4


typedef struct {
	double x;
	double y;
}usrPOINT;

class CPlaneHomography:public CObject
{
 
public:
	usrPOINT m_ptReal[POINTS_NUM];
	usrPOINT m_ptPixel[POINTS_NUM];
	SMatrix  m_h;
	CPlaneHomography() 
	{
		m_h.Init(3,3);


#ifdef _DEBUG
		//////usrPOINT p1[4]={{0,0},{140,0},{140,90},{0,90}};
		//////usrPOINT p2[4]={{295.6825,24.6023},{49.2821,303.4359},{87.9020,574.4706},{424.3928,465.4169}};

		//usrPOINT p1[POINTS_NUM]={{0,0},{10.0,0},{10.0,10.0},{0,10.0}};
		//usrPOINT p2[POINTS_NUM]={{32.0,24.001},{42.0,24.0},{42.0,34.0},{0,34.0}};
		//

		//SetValue(p1,p2);

		//GetRealCoord(p2[1]);
		//GetRealCoord(10,20);
#endif
	}
	~CPlaneHomography(){}

	double   GetDistance(usrPOINT p1,usrPOINT p2);
	// ��ȯ�� ��ǥ, �Է���ǥ
	void     SetValue(IN usrPOINT *ptReal,IN usrPOINT *ptPixel);

	void     SetValue(IN usrPOINT *ptReal,IN POINT *ptPixel);
	void     SetValue(IN POINT    *ptReal,IN POINT *ptPixel);

	usrPOINT GetRealCoord(usrPOINT ptPixel);
	usrPOINT GetRealCoord(double x, double y);
	void     GetRealCoord(double x, double y,usrPOINT *pt);
	void     GetRealCoord(double x, double y,double *rx,double *ry);

};


