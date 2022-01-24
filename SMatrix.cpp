#include "StdAfx.h"
#include "SMatrix.h"

#include <math.h>

// SVector
SVector::SVector(const SVector& rVector)
{
	int i;
	miRow = rVector.miRow;
	m_dpValue = new double[miRow];

	for( i = 0; i < miRow; i++ ) {
		m_dpValue[i] = rVector.m_dpValue[i];
	}
}

SVector::SVector(int iRow)
{
	miRow = iRow;
	m_dpValue = new double[miRow];
	SetZero();
}

SVector::SVector()
{
	miRow = 0;
	m_dpValue = NULL;
}

SVector::~SVector()
{
	if( m_dpValue != NULL ) delete[] m_dpValue;
}

void SVector::Free()
{
	if( m_dpValue != NULL ) delete[] m_dpValue;
	m_dpValue = NULL;
}

void SVector::Init(int iRow)
{
	miRow = iRow;
	m_dpValue = new double[miRow];
	SetZero();
}

double SVector::operator[](const int& iRow)
{
	return m_dpValue[iRow];
}

void SVector::SetValue(int iRow, double dValue)
{
	m_dpValue[iRow] = dValue;
}

void SVector::SetZero()
{
	int i;
	for( i = 0; i < miRow; i++ ) {
		m_dpValue[i] = 0.;
	}
}

double SVector::operator^(const SVector& rVector)
{
	int i;
	double dReturnValue = 0.;

	for( i = 0; i < miRow; i++ ) {
		dReturnValue += m_dpValue[i] * rVector.m_dpValue[i];
	}

	return dReturnValue;
}

SVector SVector::operator/(const SMatrix& rMatrix)
{
	int i, j, k, l;

	double **dppA;
	double *dpB;

	double dSum, dTemp;

	SVector RVector(miRow);
	SVector FVector(miRow);

	dpB = new double[miRow];
	dppA = new double *[miRow];

	for( i = 0; i < rMatrix.miRow; i++ ) {
		dppA[i] = new double[rMatrix.miCol + 1];
	}

	l = rMatrix.miRow;
	for( i = 0; i < rMatrix.miRow; i++ ) {
		for( j = 0; j <rMatrix.miCol; j++ ) {
			dppA[i][j] = rMatrix.m_dppValue[i][j];
		}
		dppA[i][rMatrix.miCol] = m_dpValue[i];
	}

	for( k = 0; k < l; k++ ) {
		if( !dppA[k][k] ) // dppA[k][k] == 0
		{
			for( i = k + 1; i < l; i++ ) {
				if( !dppA[i][k] ) continue; // dppA[i][k] == 0
				for( j = 0; j < l + 1; j++ ) {
					dTemp = dppA[k][j];
					dppA[k][j] = dppA[i][j];
					dppA[i][j] = dTemp;
				}
				break;
			}
		}
		if( !dppA[k][k] ) // dppA[k][k] == 0
		{
			return FVector;
		}
		for( i = k + 1; i < l; i++ ) {
			dppA[i][k] = dppA[i][k] / dppA[k][k];
			for( j = k + 1; j < l + 1; j++ ) dppA[i][j] -= dppA[i][k] * dppA[k][j];
		}
	}

	for( i = l - 1; i >= 0; i-- ) {
		for( j = i + 1, dSum = 0.; j < l; j++ ) {
			dSum += dppA[i][j] * dpB[j];
		}
		dpB[i] = (dppA[i][l] - dSum) / dppA[i][i];
	}

	for( i = 0; i < miRow; i++ ) {
		RVector.m_dpValue[i] = dpB[i];
	}

	for( i = 0 ; i < miRow; i++ ) {
		delete dppA[i];
	}
	delete dppA;
	delete dpB;

	return RVector;
}

SVector& SVector::operator/=(const SMatrix& rMatrix)
{
	int i, j, k, l;

	double **dppA;
	double *dpB;
	double dSum, dTemp;

	SVector RVector(miRow);
	SVector FVector(miRow);

	dpB = new double[miRow];
	dppA = new double *[miRow];

	for( i = 0; i < rMatrix.miRow; i++ ) {
		dppA[i] = new double[rMatrix.miCol + 1];
	}

	l = rMatrix.miRow;
	for( i = 0; i < rMatrix.miRow; i++ ) {
		for( j = 0; j < rMatrix.miCol; j++ ) {
			dppA[i][j] = rMatrix.m_dppValue[i][j];
		}
		dppA[i][rMatrix.miCol] = m_dpValue[i];
	}

	for( k = 0; k < l; k++ ) {
		if( !dppA[k][k] ) // dppA[k][k] == 0
		{
			for( i = k + 1; i < l; i++ ) {
				if( !dppA[i][k] ) continue; // dppA[i][k] == 0
				for( j = 0; j < l + 1; j++ ) {
					dTemp = dppA[k][j];
					dppA[k][j] = dppA[i][j];
					dppA[i][j] = dTemp;
				}
				break;
			}
		}
		if( !dppA[k][k] ) // dppA[k][k] == 0
		{
			*this = FVector;
			return *this;
		}
		for( i = k + 1; i < l; i++ ) {
			dppA[i][k] = dppA[i][k] / dppA[k][k];
			for( j = k + 1; j < l + 1; j++ ) dppA[i][j] -= dppA[i][k] * dppA[k][j];
		}
	}

	for( i = l - 1; i >= 0; i-- ) {
		for( j = i + 1, dSum = 0.; j < l; j++ ) {
			dSum += dppA[i][j] * dpB[j];
		}
		dpB[i] = (dppA[i][l] - dSum) / dppA[i][i];
	}

	for( i = 0; i < miRow; i++ ) {
		RVector.m_dpValue[i] = dpB[i];
	}

	for( i = 0; i < miRow; i++ ) {
		delete dppA[i];
	}

	delete dppA;
	delete dpB;

	*this = RVector;

	return *this;
}

SVector SVector::operator/(const double& rdDouble)
{
	int i;
	SVector RVector(miRow);

	for( i = 0; i < miRow; i++ ) {
		RVector.m_dpValue[i] = m_dpValue[i] / rdDouble;
	}

	return RVector;
}

SVector SVector::operator*(const double& rdDouble)
{
	int i;
	SVector RVector(miRow);

	for( i = 0; i < miRow; i++ ) {
		RVector.m_dpValue[i] = rdDouble * m_dpValue[i];
	}
	return RVector;
}

SVector SVector::operator+(const double& rdDouble)
{
	int i;
	SVector RVector(miRow);

	for( i = 0; i < miRow; i++ ) {
		RVector.m_dpValue[i] = rdDouble + m_dpValue[i];
	}
	return RVector;
}

SVector SVector::operator-(const double& rdDouble)
{
	int i;
	SVector RVector(miRow);

	for( i = 0; i < miRow; i++ ) {
		RVector.m_dpValue[i] = rdDouble - m_dpValue[i];
	}
	return RVector;
}

SVector SVector::operator+(const SVector& rVector)
{
	int i;
	SVector RVector(miRow);

	for( i = 0; i < miRow; i++ ) {
		RVector.m_dpValue[i] = m_dpValue[i];
		RVector.m_dpValue[i] += rVector.m_dpValue[i];
	}
	return RVector;
}

SVector SVector::operator*(const SVector& rVector)
{
	int i;

	SVector RVector(miRow);
	for( i = 0; i < miRow; i++ ) {
		RVector.m_dpValue[0] = m_dpValue[1] * rVector.m_dpValue[2] - m_dpValue[2] * rVector.m_dpValue[1];
		RVector.m_dpValue[1] = m_dpValue[2] * rVector.m_dpValue[0] - m_dpValue[0] * rVector.m_dpValue[2];
		RVector.m_dpValue[2] = m_dpValue[0] * rVector.m_dpValue[1] - m_dpValue[1] * rVector.m_dpValue[0];
	}
	return RVector;
}

SVector& SVector::operator*=(const SVector& rVector)
{
	int i;

	SVector RVector(miRow);
	RVector = *this;

	for( i = 0; i < miRow; i++ ) {
		m_dpValue[0] = RVector.m_dpValue[1] * rVector.m_dpValue[2] - RVector.m_dpValue[2] * rVector.m_dpValue[1];
		m_dpValue[1] = RVector.m_dpValue[2] * rVector.m_dpValue[0] - RVector.m_dpValue[0] * rVector.m_dpValue[2];
		m_dpValue[2] = RVector.m_dpValue[0] * rVector.m_dpValue[1] - RVector.m_dpValue[1] * rVector.m_dpValue[0];
	}
	return *this;
}

SVector SVector::UnitVector()
{
	SVector RVector(miRow);

	RVector = *this / GetLength();
	return RVector;
}

SVector SVector::NormalVector()
{
	SVector RVector(miRow);
	SVector NVector(miRow);

	RVector = UnitVector();
	NVector.SetValue(0, -RVector[1]);
	NVector.SetValue(1, RVector[0]);
	NVector *= GetLength();

	return NVector;
}

SVector SVector::operator-(const SVector& rVector)
{
	int i;
	SVector RVector(miRow);

	for( i = 0; i < miRow; i++ ) {
		RVector.m_dpValue[i] = m_dpValue[i];
		RVector.m_dpValue[i] -= rVector.m_dpValue[i];
	}
	return RVector;
}

SVector& SVector::operator=(const SVector& rVector)
{
	int i;

	for( i = 0; i < miRow; i++ ) {
		m_dpValue[i] = rVector.m_dpValue[i];
	}
	return *this;
}

SVector& SVector::operator+=(const double& rdDouble)
{
	int i;

	for( i = 0; i < miRow; i++ ) {
		m_dpValue[i] = m_dpValue[i] + rdDouble;
	}
	return *this;
}

SVector& SVector::operator-=(const double& rdDouble)
{
	int i;

	for( i = 0; i < miRow; i++ ) {
		m_dpValue[i] = m_dpValue[i] - rdDouble;
	}
	return *this;
}

SVector& SVector::operator*=(const double& rdDouble)
{
	int i;

	for( i = 0; i < miRow; i++ ) {
		m_dpValue[i] = m_dpValue[i] * rdDouble;
	}
	return *this;
}

SVector& SVector::operator/=(const double& rdDouble)
{
	int i;

	for( i = 0; i < miRow; i++ ) {
		m_dpValue[i] = m_dpValue[i] / rdDouble;
	}

	return *this;
}

SVector& SVector::operator+=(const SVector& rVector)
{
	int i;

	for( i = 0; i < miRow; i++ ) {
		m_dpValue[i] += rVector.m_dpValue[i];
	}
	return *this;
}

SVector& SVector::operator-=(const SVector& rVector)
{
	int i;

	for( i = 0; i < miRow; i++ ) {
		m_dpValue[i] -= rVector.m_dpValue[i];
	}
	return *this;
}

double SVector::GetLength()
{
	int i;
	double dLength = 0.;

	for( i = 0; i < miRow; i++ ) {
		dLength += m_dpValue[i] * m_dpValue[i];
	}

	dLength = sqrt(dLength);

	return dLength;
}

//***********************************************************************************
//MATRIX CLASS MEMBER FUNCTION..
//***********************************************************************************

SMatrix::SMatrix(int iRow, int iCol)
{
	int i;

	miRow = iRow;
	miCol = iCol;
	m_dppValue = new double *[miRow];

	for( i = 0; i < miRow; i++ ) {
		m_dppValue[i] = new double[miCol];
	}
	SetZero();
}

SMatrix::SMatrix()
{
	m_dppValue = NULL;
	miRow = 0;
	miCol = 0;
}

SMatrix::SMatrix(const SMatrix& rMatrix)
{
	int i, j;

	miRow = rMatrix.miRow;
	miCol = rMatrix.miCol;

	m_dppValue = new double*[miRow];

	for( i = 0; i < miRow; i++ ) {
		m_dppValue[i] = new double[miCol];
	}

	for( i = 0; i < miRow; i++ ) {
		for( j = 0; j < miCol; j++ ) {
			m_dppValue[i][j] = rMatrix.m_dppValue[i][j];
		}
	}
}

SMatrix::~SMatrix()
{
	int i;
	if( m_dppValue != NULL ) {
		for( i = 0; i < miRow; i++ ) {
			delete[] m_dppValue[i];
		}
		delete[] m_dppValue;
	}
}

void SMatrix::Free()
{
	int i;
	if( m_dppValue != NULL ) {
		for( i = 0; i < miRow; i++ ) {
			delete[] m_dppValue[i];
		}
		delete[] m_dppValue;

		m_dppValue = NULL;
	}
}

void SMatrix::Init(int iRow, int iCol)
{
	int i;

	miRow = iRow;
	miCol = iCol;
	m_dppValue = new double *[miRow];

	for( i = 0; i < miRow; i++ ) {
		m_dppValue[i] = new double[miCol];
	}
	SetZero();
}

void SMatrix::Empty()
{
	int i, j;

	for( i = 0; i < miRow; i++ ) {
		for( j = 0; j < miCol; j++ ) {
			m_dppValue[i][j] = 0.;
		}
	}
}

void SMatrix::SetValue(int iRow, int iCol, double dValue)
{
	m_dppValue[iRow][iCol] = dValue;
}

void SMatrix::SetIdentity()
{
	int i, j;

	for( i = 0; i < miRow; i++ ) {
		for( j = 0; j < miCol; j++ ) {
			if( i == j ) m_dppValue[i][j] = 1.0;
			else m_dppValue[i][j] = 0.0;
		}
	}
}

void SMatrix::SetRotationMatrix(double dAngleX, double dAngleY, double dAngleZ)
{
	SetIdentity();

	SetValue(0, 0, cos(dAngleZ));
	SetValue(0, 1, -sin(dAngleZ));
	SetValue(1, 0, sin(dAngleZ));
	SetValue(1, 1, cos(dAngleZ));
}

void SMatrix::SetZero()
{
	int i, j;

	for( i = 0; i < miRow; i++ ) {
		for( j = 0; j < miCol; j++ ) {
			m_dppValue[i][j] = 0.;
		}
	}
}

double SMatrix::GetValue(int iRow, int iCol)
{
	return m_dppValue[iRow][iCol];
}

SMatrix SMatrix::Transpose()
{
	int i, j;

	SMatrix RMatrix(miCol, miRow);
#ifdef _DEBUG
	//for( i = 0; i < miRow; i++ ) {
	//	for( j = 0; j < miCol; j++ ) {
	//		TRACE("%f ", this->m_dppValue[i][j]);
	//	}
	//	TRACE("\n");
	//}
#endif


	for( i = 0; i < miRow; i++ ) {
		for( j = 0; j < miCol; j++ ) {
			RMatrix.m_dppValue[j][i] = this->m_dppValue[i][j];
		}
	}
#ifdef _DEBUG
	//TRACE("\n");
	//TRACE("\n");

	//for( i = 0; i < miRow; i++ ) {
	//	for( j = 0; j < miCol; j++ ) {
	//		TRACE("%f ", RMatrix.m_dppValue[i][j]);
	//	}
	//	TRACE("\n");
	//}
#endif

	return RMatrix;
}

SMatrix SMatrix::Inverse()
{
	int	i, j, k;
	int	irow;
	double dtemp, pivot;
	int nSy = miRow;
	int nSx = miCol;
	int    nW = 2 * miCol;
	double *nDataExt = new double[nW*nSy];
	double *nDataInv = new double[nSx*nSy];
	double *nData = new double[nSx*nSy];
	SMatrix RMatrix(miRow, miCol);



	for( i = 0; i < miRow; i++ ) {
		for( j = 0; j < miCol; j++ ) {
			*(nData + miCol*i + j) = this->m_dppValue[i][j];
		}
	}


	for( j = 0; j<nSy; j++ ) {
		for( i = 0; i<nSx; i++ )
			*(nDataInv + nSx*j + i) = 0.0;
		for( i = 0; i<nSx; i++ ) {
			*(nDataExt + nW*j + i) = *(nData + nSx*j + i);
			if( i == j ) *(nDataExt + nW*j + i + nSx) = 1.0;
			else     *(nDataExt + nW*j + i + nSx) = 0.0;
		}
	}

	for( j = 0; j<nSy - 1; j++ ) {
		irow = j;
		for( k = j + 1; k<nSx; k++ ) {
			if( fabs(*(nDataExt + nW*irow + j))<fabs(*(nDataExt + nW*k + j)) ) irow = k;
		}
		for( i = 0; i<nW; i++ ) {
			dtemp = *(nDataExt + nW*j + i);
			*(nDataExt + nW*j + i) = *(nDataExt + nW*irow + i);
			*(nDataExt + nW*irow + i) = dtemp;
		}

	}


	for( j = 0; j<nSy; j++ ) {
		pivot = *(nDataExt + nW*j + j);
		if( pivot == 0.0 ) pivot = 10E-10;

		for( i = 0; i<nW; i++ ) *(nDataExt + nW*j + i) /= pivot;

		for( k = 0; k<nSy; k++ ) {
			if( k == j ) continue;
			dtemp = *(nDataExt + nW*k + j);
			for( i = j; i<nW; i++ )
				*(nDataExt + nW*k + i) -= (dtemp*(*(nDataExt + nW*j + i)));
		}
	}

	for( j = 0; j<nSy; j++ )
		for( i = 0; i<nSx; i++ )
			*(nDataInv + nSx*j + i) = *(nDataExt + nW*j + i + nSx);

	for( i = 0; i < miRow; i++ ) {
		for( j = 0; j < miCol; j++ ) {
			RMatrix.m_dppValue[i][j] = *(nDataInv + miCol*i + j);
		}
	}

	delete[] nDataExt;
	delete[] nDataInv;
	delete[] nData;

	return RMatrix;
}

SMatrix SMatrix::operator + (const SMatrix& rMatrix)
{
	int i, j;

	SMatrix RMatrix(miRow, miCol);

	for( i = 0; i < miRow; i++ ) {
		for( j = 0; j < miCol; j++ ) {
			RMatrix.m_dppValue[i][j] = m_dppValue[i][j];
			RMatrix.m_dppValue[i][j] += rMatrix.m_dppValue[i][j];
		}
	}

	return RMatrix;
}






SMatrix SMatrix::operator - (const SMatrix& rMatrix)
{
	int i, j;

	SMatrix RMatrix(miRow, miCol);

	for( i = 0; i < miRow; i++ ) {
		for( j = 0; j < miCol; j++ ) {
			RMatrix.m_dppValue[i][j] = m_dppValue[i][j];
			RMatrix.m_dppValue[i][j] -= rMatrix.m_dppValue[i][j];
		}
	}
	return RMatrix;
}

SMatrix SMatrix::operator * (const SMatrix& rMatrix)
{
	int i, j, k;

	SMatrix RMatrix(miRow, rMatrix.miCol);

	for( i = 0; i < RMatrix.miRow; i++ ) {
		for( j = 0; j < RMatrix.miCol; j++ ) {
			RMatrix.m_dppValue[i][j] = 0.;
			for( k = 0; k < miCol; k++ ) {
				RMatrix.m_dppValue[i][j] += m_dppValue[i][k] * rMatrix.m_dppValue[k][j];
			}
		}
	}
	return RMatrix;
}

SMatrix SMatrix::operator * (const double& rdDouble)
{
	int i, j;

	SMatrix RMatrix(miRow, miCol);
	for( i = 0; i < miRow; i++ ) {
		for( j = 0; j < miCol; j++ ) {
			RMatrix.m_dppValue[i][j] = m_dppValue[i][j];
			RMatrix.m_dppValue[i][j] *= rdDouble;
		}
	}
	return RMatrix;
}

SVector SMatrix::operator * (const SVector& rVector)
{
	int i, j;

	SVector RVector(rVector.miRow);
	for( i = 0; i < miRow; i++ ) {
		RVector.m_dpValue[i] = 0.;
		for( j = 0; j < miCol; j++ ) {
			RVector.m_dpValue[i] += m_dppValue[i][j] * rVector.m_dpValue[j];
		}
	}
	return RVector;
}

SMatrix& SMatrix::operator = (const SMatrix& rMatrix)
{
	int i, j;

	for( i = 0; i < miRow; i++ ) {
		for( j = 0; j < miCol; j++ ) {
			m_dppValue[i][j] = rMatrix.m_dppValue[i][j];
		}
	}
	return *this;
}

SMatrix& SMatrix::operator += (const SMatrix& rMatrix)
{
	int i, j;

	for( i = 0; i < miRow; i++ ) {
		for( j = 0; j < miCol; j++ ) {
			m_dppValue[i][j] += rMatrix.m_dppValue[i][j];
		}
	}
	return *this;
}

SMatrix& SMatrix::operator -= (const SMatrix& rMatrix)
{
	for( int i = 0; i<miRow; i++ ) {
		for( int j = 0; j<miCol; j++ ) {
			m_dppValue[i][j] -= rMatrix.m_dppValue[i][j];
		}
	}
	return *this;
}

SMatrix& SMatrix::operator *= (const double& rdDouble)
{
	int i, j;

	SMatrix RMatrix(miRow, miCol);

	RMatrix = *this;
	for( i = 0; i < miRow; i++ ) {
		for( j = 0; j < miCol; j++ ) {
			RMatrix.m_dppValue[i][j] = m_dppValue[i][j];
			RMatrix.m_dppValue[i][j] *= rdDouble;
		}
	}
	return *this;
}

SMatrix& SMatrix::operator *=(const SMatrix& rMatrix)
{
	int i, j, k;

	SMatrix RMatrix(miRow, miCol);

	RMatrix = *this;
	for( i = 0; i < RMatrix.miRow; i++ ) {
		for( j = 0; j < RMatrix.miCol; j++ ) {
			m_dppValue[i][j] = 0.;
			for( k = 0; k < miCol; k++ ) {
				m_dppValue[i][j] += RMatrix.m_dppValue[i][k] * rMatrix.m_dppValue[k][j];
			}
		}
	}
	return *this;
}

