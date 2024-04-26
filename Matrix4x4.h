#pragma once
#include"Vector3.h"

class Matrix4x4 {

public:
	float m[4][4];
public:
	Matrix4x4();
	/*Matrix4x4();*/
	Matrix4x4 MakeIdentity4x4();

	Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2);

	Matrix4x4 MakeTranslateMatrix(const Vector3& translate);

	Matrix4x4 MakeScaleMatrix(const Vector3& scale);

	// X²‰ñ“]s—ñ
	Matrix4x4 MakeRotateXMatrix(float radian);

	// Y²‰ñ“]s—ñ
	Matrix4x4 MakeRotateYMatrix(float radian);

	// Z²‰ñ“]s—ñ
	Matrix4x4 MakeRotateZMatrix(float radian);

	Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix);

	Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3 translate);

	Matrix4x4 Inverse(const Matrix4x4& m);

	Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip);

	Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip);

	Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth);
};


