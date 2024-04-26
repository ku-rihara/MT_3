#pragma once
class Vector3{

public:
    float x;
    float y;
    float z;

public:

    Vector3();
    Vector3(float numX, float numY,float numZ);

    Vector3 operator+(const Vector3& obj);

    Vector3 operator-(const Vector3& obj);

    Vector3 operator*(const Vector3& obj);
    Vector3 operator*(const float& obj);

    Vector3 operator/(const Vector3& obj);

  
    Vector3 Normalize(const Vector3& v);  //���K��
  
    Vector3 Cross(const Vector3& v1, const Vector3& v2);  //�N���X��
  
    float Dot(const Vector3& v1, const Vector3& v2);  //����
 
    float Length(const Vector3& v);   //�����i�m�����j
  
    void VectorScreenPrintf(int x, int y, const Vector3& vector, const char* label);
};

