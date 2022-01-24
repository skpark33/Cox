#pragma once

class SMatrix;

class SVector
{
public:
	SVector(const SVector& rVector);
	SVector(int iRow);
	SVector();
	~SVector();

	void Free();
public:
	int miRow;
	double *m_dpValue;

public:
	double operator^(const SVector& rVector); // Inner Product	// Vector and Matrix operator
	double operator[](const int& irRow); // Operator Refer
	double GetLength();

	void Init(int iRow);
	void SetValue(int iRow, double dValue);
	void SetZero();

	SVector UnitVector();
	SVector NormalVector();

	SVector operator+(const double& rdDouble); // Addition // Scalar Operator // Operator
	SVector operator-(const double& rdDouble); // Subtraction
	SVector operator*(const double& rdDouble); // Multiplication
	SVector operator/(const double& rdDouble); // Division
	SVector operator+(const SVector& rVector); // Addition
	SVector operator-(const SVector& rVector); // Subtraction
	SVector operator*(const SVector& rVector); // Cross Product
	SVector operator/(const SMatrix& rMatrix); // M^-1 * V
	SVector& operator=(const SVector& rVector); // Copy // Operator With Copy
	SVector& operator+=(const double& rdDouble); // Addition	// Scalar Operator With Copy
	SVector& operator-=(const double& rdDouble); // Subtraction
	SVector& operator*=(const double& rdDouble); // Multiplication
	SVector& operator/=(const double& rdDouble); // Division
	SVector& operator+=(const SVector& rVector); // Addition // Vector and Matrix Operator With Copy
	SVector& operator-=(const SVector& rVector); // Subtraction
	SVector& operator*=(const SVector& rVector); // Cross Product
	SVector& operator/=(const SMatrix& rMatrix); // M^-1 * V
};

class SMatrix
{
public:
	SMatrix(const SMatrix& rMatrix);
	SMatrix(int iRow, int iCol);
	SMatrix();
	~SMatrix();

	void Free();
public:
	int miRow;
	int miCol;
	double **m_dppValue;

public:
	double GetValue(int iRow, int iCol);

	void Init(int iRow, int iCol);
	void SetValue(int iRow, int iCol, double dValue);
	void SetZero();
	void Empty();
	void SetIdentity();
	void SetRotationMatrix(double dAngleX, double dAngleY, double dAngleZ);

	SMatrix Transpose();
	SMatrix Inverse();

	SMatrix operator * (const double& rdDouble); // OPERATOR..	// SCALAR OPERATOR..				
	SMatrix operator + (const SMatrix& rMatrix); // VECTOR AND MATRIX OPERATOR..
	SMatrix operator - (const SMatrix& rMatrix);
	SMatrix operator * (const SMatrix& rMatrix);
	SVector operator * (const SVector& rVector);
	SMatrix& operator = (const SMatrix& rMatrix); // OPERATOR WITH COPY..
	SMatrix& operator *= (const double& rdDouble); // SCALAR OPERATOR WITH COPY..				
	SMatrix& operator += (const SMatrix& rMatrix); // VECTOR AND MATRIX OPERATOR WITH COPY..
	SMatrix& operator -= (const SMatrix& rMatrix);
	SMatrix& operator *= (const SMatrix& rMatrix);
};
