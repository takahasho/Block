#pragma once
class VECTOR {
public:
    float x, y, z;
    VECTOR(float x = 0, float y = 0, float z = 0);
    void set(float x, float y, float z = 0);
    VECTOR operator+(const VECTOR& v) const;
    VECTOR operator-(const VECTOR& v) const;
    VECTOR operator*(float f) const;
    VECTOR operator/(float f) const;
    VECTOR operator-() const;
    void operator+=(const VECTOR& v);
    void operator-=(const VECTOR& v);
    void operator*=(float f);
    void operator/=(float f);
    bool operator==(const VECTOR& v);
    bool operator!=(const VECTOR& v);
    float sqMag() const;
    float magSq() const;//ã@î\ÇÕsqMagÇ∆ìØÇ∂
    float mag() const;
    VECTOR setMag(float mag);
    VECTOR limmit(float maxMag);
    VECTOR normalize();
    float dot(const VECTOR& v)const;
    float crossZ(const VECTOR& v)const;
    VECTOR cross(const VECTOR& v)const;
    VECTOR leap(const VECTOR a, const VECTOR b, const float t);
};

VECTOR operator* (float f, const VECTOR& v);// VECTOR = float * VECTOR
VECTOR normalize(const VECTOR& a);
float dot(const VECTOR& a, const VECTOR& b);
float dot(const VECTOR& a, const VECTOR* b);
float dot(const VECTOR* a, const VECTOR& b);
float dot(const VECTOR* a, const VECTOR* b);
float crossZ(const VECTOR& a, const VECTOR& b);//äOêœZê¨ï™ÇÃÇ›åvéZ(ÇQÇcóp)
VECTOR cross(const VECTOR& a, const VECTOR& b);
VECTOR cross(const VECTOR& a, const VECTOR* b);
VECTOR cross(const VECTOR* a, const VECTOR& b);
VECTOR cross(const VECTOR* a, const VECTOR* b);
// ÇQì_ä‘ÇÃãóó£
float distance_of(const VECTOR& a, const VECTOR& b);
// // ------------------ êF ------------------------
const VECTOR red(1.0f, 0.0f, 0.0f);
const VECTOR green(0.0f, 1.0f, 0.0f);
const VECTOR blue(0.0f, 0.0f, 1.0f);
const VECTOR whiteblue(0.5f, 1.0f, 1.0f);
const VECTOR white(1.0f, 1.0f, 1.0f);
const VECTOR black(0.0f, 0.0f, 0.0f);
const VECTOR yellow(1.0f, 1.0f, 0.0f);
const VECTOR magenta(1.0f, 0.0f, 1.0f);
const VECTOR cyan(0.0f, 1.0f, 1.0f);
const VECTOR parple(1.0f, 0.5f, 1.0f);