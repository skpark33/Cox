#include "stdafx.h"
#include "PlaneHomography.h"
#include <math.h>


void CPlaneHomography::SetValue(IN usrPOINT *ptReal,IN usrPOINT *ptPixel)
{
	int i;

	SMatrix mtxA(POINTS_NUM*2,8 ); 
	SMatrix mtxReal(POINTS_NUM,2);
	SMatrix mtxPixel(POINTS_NUM,2);
	SVector mtxWB(POINTS_NUM*2);

	for(i=0; i<POINTS_NUM; i++)
	{
		m_ptReal[i].x=ptReal[i].x;
		m_ptReal[i].y=ptReal[i].y;
		m_ptPixel[i].x=ptPixel[i].x;
		m_ptPixel[i].y=ptPixel[i].y;

		mtxReal.SetValue(i,0,m_ptReal[i].x);
		mtxReal.SetValue(i,1,m_ptReal[i].y);
		mtxPixel.SetValue(i,0,m_ptPixel[i].x);
		mtxPixel.SetValue(i,1,m_ptPixel[i].y);
	}

	for(i=0; i<POINTS_NUM; i++)
	{
		mtxA.SetValue(2*i,0,mtxReal.GetValue(i,0));
		mtxA.SetValue(2*i,1,mtxReal.GetValue(i,1));
		mtxA.SetValue(2*i,2,1.0);
		mtxA.SetValue(2*i,3,0.0);
		mtxA.SetValue(2*i,4,0.0);
		mtxA.SetValue(2*i,5,0.0);
		mtxA.SetValue(2*i,6,-mtxPixel.GetValue(i,0)*mtxReal.GetValue(i,0));
		mtxA.SetValue(2*i,7,-mtxPixel.GetValue(i,0)*mtxReal.GetValue(i,1));

		mtxA.SetValue(2*i+1,0,0.0);
		mtxA.SetValue(2*i+1,1,0.0);
		mtxA.SetValue(2*i+1,2,0.0);
		mtxA.SetValue(2*i+1,3,mtxReal.GetValue(i,0));
		mtxA.SetValue(2*i+1,4,mtxReal.GetValue(i,1));
		mtxA.SetValue(2*i+1,5,1.0);
		mtxA.SetValue(2*i+1,6,-mtxPixel.GetValue(i,1)*mtxReal.GetValue(i,0));
		mtxA.SetValue(2*i+1,7,-mtxPixel.GetValue(i,1)*mtxReal.GetValue(i,1));

		mtxWB.SetValue(i*2  ,mtxPixel.GetValue(i,0));
		mtxWB.SetValue(i*2+1,mtxPixel.GetValue(i,1));
	}


	SMatrix ta(8,8);
	ta=(mtxA.Transpose()*mtxA);
 	ta=ta.Inverse();
#ifdef _DEBUG
	//TRACE("-- Inverse ---\n");
	//int j;
	//for(j=0; j<8; j++)
	//{
	//	for(i=0; i<8;i++)
	//	{
	//		TRACE("%f ",ta.GetValue(j,i));
	//	}
	//	TRACE("\n");
	//}
#endif

	ta*=mtxA.Transpose();

	SVector r(8);
	r=ta*mtxWB;
//////////////////////////////////////////////////
	//ta=mtxA.Inverse();
	//ta*=mtxWB;
//////////////////////////////////////////////////

 
	m_h.SetValue(0,0,r[0]);
	m_h.SetValue(0,1,r[1]);
	m_h.SetValue(0,2,r[2]);
	
	m_h.SetValue(1,0,r[3]);
	m_h.SetValue(1,1,r[4]);
	m_h.SetValue(1,2,r[5]);
	
	m_h.SetValue(2,0,r[6]);
	m_h.SetValue(2,1,r[7]);
	m_h.SetValue(2,2,1.0);
	
#ifdef _DEBUG
	////TRACE("\n");

	////for(j=0; j<3; j++)
	////{
	////	for(i=0; i<3;i++)
	////	{
	////		TRACE("%f ",m_h.GetValue(j,i));
	////	}
	////	TRACE("\n");
	////}
#endif

	ta.Free();
	r.Free();

	mtxA.Free();
	mtxReal.Free();
	mtxPixel.Free();
	mtxWB.Free();

	return;
}



void CPlaneHomography::SetValue(IN usrPOINT *ptReal,IN POINT *ptPixel)
{
	int i;
	SMatrix mtxA(POINTS_NUM*2,8 ); 
	SMatrix mtxReal(POINTS_NUM,2);
	SMatrix mtxPixel(POINTS_NUM,2);
	SVector mtxWB(POINTS_NUM*2);

	for(i=0; i<POINTS_NUM; i++)
	{
		m_ptReal[i].x=ptReal[i].x;
		m_ptReal[i].y=ptReal[i].y;
		m_ptPixel[i].x=(double)ptPixel[i].x;
		m_ptPixel[i].y=(double)ptPixel[i].y;

		mtxReal.SetValue(i,0,m_ptReal[i].x);
		mtxReal.SetValue(i,1,m_ptReal[i].y);
		mtxPixel.SetValue(i,0,m_ptPixel[i].x);
		mtxPixel.SetValue(i,1,m_ptPixel[i].y);
	}

	for(i=0; i<POINTS_NUM; i++)
	{
		mtxA.SetValue(2*i,0,mtxReal.GetValue(i,0));
		mtxA.SetValue(2*i,1,mtxReal.GetValue(i,1));
		mtxA.SetValue(2*i,2,1.0);
		mtxA.SetValue(2*i,3,0.0);
		mtxA.SetValue(2*i,4,0.0);
		mtxA.SetValue(2*i,5,0.0);
		mtxA.SetValue(2*i,6,-mtxPixel.GetValue(i,0)*mtxReal.GetValue(i,0));
		mtxA.SetValue(2*i,7,-mtxPixel.GetValue(i,0)*mtxReal.GetValue(i,1));

		mtxA.SetValue(2*i+1,0,0.0);
		mtxA.SetValue(2*i+1,1,0.0);
		mtxA.SetValue(2*i+1,2,0.0);
		mtxA.SetValue(2*i+1,3,mtxReal.GetValue(i,0));
		mtxA.SetValue(2*i+1,4,mtxReal.GetValue(i,1));
		mtxA.SetValue(2*i+1,5,1.0);
		mtxA.SetValue(2*i+1,6,-mtxPixel.GetValue(i,1)*mtxReal.GetValue(i,0));
		mtxA.SetValue(2*i+1,7,-mtxPixel.GetValue(i,1)*mtxReal.GetValue(i,1));

		mtxWB.SetValue(i*2  ,mtxPixel.GetValue(i,0));
		mtxWB.SetValue(i*2+1,mtxPixel.GetValue(i,1));
	}


	SMatrix ta(8,8);
	ta=(mtxA.Transpose()*mtxA);
 	ta=ta.Inverse();
#ifdef _DEBUG
	//TRACE("-- Inverse ---\n");
	//int j;

	//for(j=0; j<8; j++)
	//{
	//	for(i=0; i<8;i++)
	//	{
	//		TRACE("%f ",ta.GetValue(j,i));
	//	}
	//	TRACE("\n");
	//}
#endif

	ta*=mtxA.Transpose();

	SVector r(8);
	r=ta*mtxWB;
//////////////////////////////////////////////////
	//ta=mtxA.Inverse();
	//ta*=mtxWB;
//////////////////////////////////////////////////

 
	m_h.SetValue(0,0,r[0]);
	m_h.SetValue(0,1,r[1]);
	m_h.SetValue(0,2,r[2]);
	
	m_h.SetValue(1,0,r[3]);
	m_h.SetValue(1,1,r[4]);
	m_h.SetValue(1,2,r[5]);
	
	m_h.SetValue(2,0,r[6]);
	m_h.SetValue(2,1,r[7]);
	m_h.SetValue(2,2,1.0);
	
#ifdef _DEBUG
	//TRACE("\n");

	//for(j=0; j<3; j++)
	//{
	//	for(i=0; i<3;i++)
	//	{
	//		TRACE("%f ",m_h.GetValue(j,i));
	//	}
	//	TRACE("\n");
	//}
#endif

	ta.Free();
	r.Free();

	mtxA.Free();
	mtxReal.Free();
	mtxPixel.Free();
	mtxWB.Free();

	return;
}



void CPlaneHomography::SetValue(IN POINT *ptReal, IN POINT *ptPixel)
{
	int i;
	SMatrix mtxA(POINTS_NUM * 2, 8);
	SMatrix mtxReal(POINTS_NUM, 2);
	SMatrix mtxPixel(POINTS_NUM, 2);
	SVector mtxWB(POINTS_NUM * 2);

	for (i = 0; i<POINTS_NUM; i++)
	{
		m_ptReal[i].x = (double)ptReal[i].x;
		m_ptReal[i].y = (double)ptReal[i].y;
		m_ptPixel[i].x = (double)ptPixel[i].x;
		m_ptPixel[i].y = (double)ptPixel[i].y;

		mtxReal.SetValue(i, 0, m_ptReal[i].x);
		mtxReal.SetValue(i, 1, m_ptReal[i].y);
		mtxPixel.SetValue(i, 0, m_ptPixel[i].x);
		mtxPixel.SetValue(i, 1, m_ptPixel[i].y);
	}

	for (i = 0; i<POINTS_NUM; i++)
	{
		mtxA.SetValue(2 * i, 0, mtxReal.GetValue(i, 0));
		mtxA.SetValue(2 * i, 1, mtxReal.GetValue(i, 1));
		mtxA.SetValue(2 * i, 2, 1.0);
		mtxA.SetValue(2 * i, 3, 0.0);
		mtxA.SetValue(2 * i, 4, 0.0);
		mtxA.SetValue(2 * i, 5, 0.0);
		mtxA.SetValue(2 * i, 6, -mtxPixel.GetValue(i, 0)*mtxReal.GetValue(i, 0));
		mtxA.SetValue(2 * i, 7, -mtxPixel.GetValue(i, 0)*mtxReal.GetValue(i, 1));

		mtxA.SetValue(2 * i + 1, 0, 0.0);
		mtxA.SetValue(2 * i + 1, 1, 0.0);
		mtxA.SetValue(2 * i + 1, 2, 0.0);
		mtxA.SetValue(2 * i + 1, 3, mtxReal.GetValue(i, 0));
		mtxA.SetValue(2 * i + 1, 4, mtxReal.GetValue(i, 1));
		mtxA.SetValue(2 * i + 1, 5, 1.0);
		mtxA.SetValue(2 * i + 1, 6, -mtxPixel.GetValue(i, 1)*mtxReal.GetValue(i, 0));
		mtxA.SetValue(2 * i + 1, 7, -mtxPixel.GetValue(i, 1)*mtxReal.GetValue(i, 1));

		mtxWB.SetValue(i * 2, mtxPixel.GetValue(i, 0));
		mtxWB.SetValue(i * 2 + 1, mtxPixel.GetValue(i, 1));
	}


	SMatrix ta(8, 8);
	ta = (mtxA.Transpose()*mtxA);
	ta = ta.Inverse();
#ifdef _DEBUG
	//TRACE("-- Inverse ---\n");
	//int j;

	//for(j=0; j<8; j++)
	//{
	//	for(i=0; i<8;i++)
	//	{
	//		TRACE("%f ",ta.GetValue(j,i));
	//	}
	//	TRACE("\n");
	//}
#endif

	ta *= mtxA.Transpose();

	SVector r(8);
	r = ta*mtxWB;
	//////////////////////////////////////////////////
	//ta=mtxA.Inverse();
	//ta*=mtxWB;
	//////////////////////////////////////////////////


	m_h.SetValue(0, 0, r[0]);
	m_h.SetValue(0, 1, r[1]);
	m_h.SetValue(0, 2, r[2]);

	m_h.SetValue(1, 0, r[3]);
	m_h.SetValue(1, 1, r[4]);
	m_h.SetValue(1, 2, r[5]);

	m_h.SetValue(2, 0, r[6]);
	m_h.SetValue(2, 1, r[7]);
	m_h.SetValue(2, 2, 1.0);

#ifdef _DEBUG
	//TRACE("\n");

	//for(j=0; j<3; j++)
	//{
	//	for(i=0; i<3;i++)
	//	{
	//		TRACE("%f ",m_h.GetValue(j,i));
	//	}
	//	TRACE("\n");
	//}
#endif

	ta.Free();
	r.Free();

	mtxA.Free();
	mtxReal.Free();
	mtxPixel.Free();
	mtxWB.Free();

	return;
}



usrPOINT CPlaneHomography::GetRealCoord(usrPOINT ptPixel)
{
	usrPOINT p;

	SMatrix h(3,3);
	SVector w(3);
	SVector c(3);

	w.SetValue(0,ptPixel.x);
	w.SetValue(1,ptPixel.y);
	w.SetValue(2,1.0);
#ifdef _DEBUG
	/*TRACE("\n");
	int i,j;

	for(j=0; j<3; j++)
	{
		for(i=0; i<3;i++)
		{
			TRACE("%f ",m_h.GetValue(j,i));
		}
		TRACE("\n");
	}*/
#endif
    /////////////
	h=m_h.Inverse();
#ifdef _DEBUG
	/*TRACE("\n");

	for(j=0; j<3; j++)
	{
		for(i=0; i<3;i++)
		{
			TRACE("%f ",h.GetValue(j,i));
		}
		TRACE("\n");
	}*/
#endif


	c=h*w;
	 
#ifdef _DEBUG
	/*TRACE("\n");

	for(i=0; i<3;i++)
	{
		TRACE("c[%d]=%f \n",i,c[i]);
	}
	TRACE("\n");*/
#endif

	p.x=c[0]/c[2];
	p.y=c[1]/c[2];

	// 2019.11.18 Ãß°¡
	h.Free();
	w.Free();
	c.Free();

	return p;
}


usrPOINT CPlaneHomography::GetRealCoord(double x, double y)
{
	usrPOINT ptPixel;

	ptPixel.x=x;
	ptPixel.y=y;


	return GetRealCoord(ptPixel);
}

void CPlaneHomography::GetRealCoord(double x, double y,usrPOINT *pt)
{
	usrPOINT ptPixel;

	ptPixel.x=x;
	ptPixel.y=y;


	*pt= GetRealCoord(ptPixel);
}


void CPlaneHomography::GetRealCoord(double x, double y,double *rx,double *ry)
{
	usrPOINT ptPixel;
	usrPOINT pt;

	ptPixel.x=x;
	ptPixel.y=y;


	pt= GetRealCoord(ptPixel);
	*rx=pt.x;
	*ry=pt.y;


	return;
}


double CPlaneHomography::GetDistance(usrPOINT p1,usrPOINT p2)
{
	usrPOINT pt1;
	usrPOINT pt2;
	double distance;

	pt1=GetRealCoord(p1);
	pt2=GetRealCoord(p2);



	distance=sqrt( (pt1.x-pt2.x)*(pt1.x-pt2.x) +(pt1.y-pt2.y)*(pt1.y-pt2.y));
	return distance;
	
}