#include<Novice.h>
#include "Vector3.h"
#include<math.h>


const float kColumnWidth = 60;


Vector3::Vector3() {
	x = 0;
	y = 0;
	z = 0;
}

Vector3::Vector3(float numX, float numY,float numZ) {
	x = numX;
	y = numY;
	z = numZ;
}


Vector3 Vector3:: operator+(const Vector3& obj) {

	Vector3 result;

	result.x = x + obj.x;
	result.y = y + obj.y;
	result.z = z + obj.z;

	return result;

}

Vector3 Vector3:: operator-(const Vector3& obj) {

	Vector3 result;

	result.x = x - obj.x;
	result.y = y - obj.y;

	return result;

}

Vector3 Vector3:: operator*(const Vector3& obj) {

	Vector3 result;

	result.x = x * obj.x;
	result.y = y * obj.y;
	result.z = z * obj.z;

	return result;

}

Vector3 Vector3:: operator*(const float& obj) {
	Vector3 result;

	result.x = x * obj;
	result.y = y * obj;
	result.z = z * obj;

	return result;
}

Vector3 Vector3:: operator/(const Vector3& obj) {

	Vector3 result;

	if (obj.x != 0) {
		result.x = x / obj.x;

	}
	else {
		result.x = 0;

	}

	if (obj.y != 0) {
		result.y = y / obj.y;

	}
	else {
		result.y = 0;

	}

	if (obj.z != 0) {
		result.z = z / obj.z;

	}
	else {
		result.z = 0;

	}

	return result;

}


//ì‡êœ
float Vector3::Dot(const Vector3& v1, const Vector3& v2) {

	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;

}

//í∑Ç≥ÅiÉmÉãÉÄÅj
float Vector3::Length(const Vector3& v) {


	return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

//ê≥ãKâª
Vector3 Vector3::Normalize(const Vector3& v) {
	Vector3 result;

	float length = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
	if (length != 0) {
		result.x = v.x / length;
		result.y = v.y / length;
		result.z = v.z / length;
	}
	else {
		result.x = 0;
		result.y = 0;
		result.z = 0;
	}
	return result;
}

void Vector3::VectorScreenPrintf(int x, int y, const Vector3& vector, const char* label) {
	Novice::ScreenPrintf(x, y, "%.02f", vector.x);
	Novice::ScreenPrintf(x + kColumnWidth, y, "%.02f", vector.y);
	Novice::ScreenPrintf(x + kColumnWidth * 2, y, "%.02f", vector.z);
	Novice::ScreenPrintf(x + kColumnWidth * 3, y, "%s", label);
}