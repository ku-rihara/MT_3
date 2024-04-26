#pragma once
#include"Vector3.h"

class Matrix4x4 {

public:
	float m[4][4];
public:

	Matrix4x4();
	/*Matrix4x4();*/

	Matrix4x4 operator+(const Matrix4x4& obj);//���Z

	Matrix4x4 operator-(const Matrix4x4& obj);//���Z

	Matrix4x4 operator*(const Matrix4x4& obj);//��

	Matrix4x4 Transpose(const Matrix4x4& m);//�]�u�s��

	Matrix4x4 MakeIdentity4x4();//�P�ʍs��

	Matrix4x4 MakeTranslateMatrix(const Vector3& translate);//���s�ړ��s��

	Matrix4x4 MakeScaleMatrix(const Vector3& scale);//�g��k���s��

	Matrix4x4 MakeRotateXMatrix(float radian);// X����]�s��

	Matrix4x4 MakeRotateYMatrix(float radian);// Y����]�s��

	Matrix4x4 MakeRotateZMatrix(float radian);// Z����]�s��

	Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3 translate);//�A�t�B���ϊ�

	Matrix4x4 Inverse(const Matrix4x4& m);//�t�s��

	Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip);//�������e�s��

	Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip);//���ˉe�s��

	Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth);//�r���[�|�[�g�s��

	Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix);//�X�N���[���ϊ�
	Vector3 ScreenTransform(const Vector3& local, const Matrix4x4& worldviewprojection, const Matrix4x4& viewport);

	void MatrixScreenPrintf(int x, int y, Matrix4x4& matrix, const char* label);
};


