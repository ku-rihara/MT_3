#include "Vector3.h"

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