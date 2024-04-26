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

    //ê≥ãKâª
    Vector3 Normalize(const Vector3& v);
    //ì‡êœ
    float Dot(const Vector3& v1, const Vector3& v2);
    //í∑Ç≥ÅiÉmÉãÉÄÅj
    float Length(const Vector3& v);
  
    void VectorScreenPrintf(int x, int y, const Vector3& vector, const char* label);
};

