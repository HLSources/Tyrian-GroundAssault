/*====================================================================

 Purpose: Reinvented wheel: Vector classes
 Copyright (c) 2010+ Xawari, includes code from Valve Software
 Header is C and C++ compliant
 Basic operations only, no SIMD instructions yet

//==================================================================*/
#ifndef VECTOR_H
#define VECTOR_H
#ifdef _WIN32
#ifndef __MINGW32__
#pragma once
#endif /* !__MINGW32__ */
#endif

#include "platform.h"
/* Misc C-runtime library headers */
//#include <stdio.h>
/*#include <stdlib.h>*/
#include <math.h>

// GCC doesn't like this
//#define ALLOW_ANONYMOUS_STRUCT to use anonymous unions

#ifndef PITCH
#define	PITCH	0
#define	YAW		1
#define	ROLL	2
#endif


/* Define globally in project setting to enable validity checks */
#ifdef VALIDATE_VECTORS
#define CHECK_VALID(_v)	ASSERT((_v).IsValid())
#else
#define CHECK_VALID(_v)	0
#endif

/* movemove this */
#ifndef M_PI
	#define M_PI		3.1415926535897932384626433832
#endif

#define M_PI_F		((float)(M_PI))/* special floating point version */

/* HL2: NJS: Inlined to prevent floats from being autopromoted to doubles, as with the old system */
#ifndef RAD2DEG
	#define RAD2DEG(x) ((float)(x) * (float)(180.f / M_PI_F))
#endif

#ifndef DEG2RAD
	#define DEG2RAD(x) ((float)(x) * (float)(M_PI_F / 180.f))
#endif


typedef float vec_t;


/*=========================================================
 2D Vector - same data-layout as vec_t[2]
=========================================================*/
#ifdef __cplusplus

class Vector2D
{
public:
#if defined (ALLOW_ANONYMOUS_STRUCT)
	union
	{
		vec_t array[2];
		struct
		{
			vec_t x;
			vec_t y;
		};
	};
#else
	vec_t x;
	vec_t y;
#endif

	// construction/destruction
	inline Vector2D(void): x(0.0f), y(0.0f)					{ }
	inline Vector2D(const float &X, const float &Y)			{ x = X; y = Y; }
	inline Vector2D(const double &X, const double &Y)		{ x = (float)X; y = (float)Y;	}
	inline Vector2D(const int &X, const int &Y)				{ x = (float)X; y = (float)Y;	}
	inline Vector2D(const Vector2D &v)						{ x = v.x; y = v.y;				}
	inline Vector2D(float rgfl[2])							{ x = rgfl[0]; y = rgfl[1];		}

	// operators
	inline Vector2D &operator=(const Vector2D &v)		{ x=v.x; y=v.y; return *this;		}
	inline bool operator==(const Vector2D &v) const		{ return (x==v.x && y==v.y);		}
	inline bool operator!=(const Vector2D &v) const		{ return (x!=v.x || y!=v.y);		}
	inline Vector2D operator-(void) const				{ return Vector2D(-x,-y);			}

	inline Vector2D operator+(const Vector2D &v) const	{ return Vector2D(x+v.x, y+v.y);	}
	inline Vector2D operator-(const Vector2D &v) const	{ return Vector2D(x-v.x, y-v.y);	}
	inline Vector2D operator*(const float &fl) const	{ return Vector2D(x*fl, y*fl);		}
	inline Vector2D operator/(const float &fl) const	{ return Vector2D(x/fl, y/fl);		}
//	inline Vector2D operator*(const double &fl) const	{ return Vector2D(x*fl, y*fl);		}
//	inline Vector2D operator/(const double &fl) const	{ return Vector2D(x/fl, y/fl);		}

	// arithmetic operations
	FORCEINLINE Vector2D &operator+=(const Vector2D &v);
	FORCEINLINE Vector2D &operator-=(const Vector2D &v);
	FORCEINLINE Vector2D &operator*=(const Vector2D &v);
	FORCEINLINE Vector2D &operator*=(const float &s);
//	FORCEINLINE Vector2D &operator*=(const float s);
//	FORCEINLINE Vector2D &operator*=(float &s);
	FORCEINLINE Vector2D &operator/=(const Vector2D &v);
	FORCEINLINE Vector2D &operator/=(const float &s);
//	FORCEINLINE Vector2D &operator/=(const float s);
//	FORCEINLINE Vector2D &operator/=(float &s);

	// for modifications
	float &operator[](int index)
	{
#ifdef _DEBUG
		if (index < 0 || index >= 2)// ASSERT
			index = 0;
#endif
#if defined (ALLOW_ANONYMOUS_STRUCT)
		return array[index];
#else
		return ((vec_t*)this)[index];
#endif
	}
	// for reading/comparing only
	const float &operator[](int index) const
	{
#ifdef _DEBUG
		if (index < 0 || index >= 2)// ASSERT
			index = 0;
#endif
#if defined (ALLOW_ANONYMOUS_STRUCT)
		return array[index];
#else
		return ((vec_t*)this)[index];
#endif
	}

	float *As2f(void)								{ return &x; }
	operator float *()								{ return &x; }// automatically convert to (float *) when needed
	operator float *() const						{ return (float *)&x; }// Hack? Some old functions require (float *) as argument, but we need a way to supply (const Vector)s. Overrides const-protection
	operator const float *() const					{ return &x; }

	inline void CopyToArray(float *rgfl) const		{ rgfl[0] = x, rgfl[1] = y; }
	inline bool IsZero(void) const					{ return (x==0.0f && y==0.0f); }
#if defined (VALIDATE_VECTORS)
	inline bool IsValid(void) const					{ return IsFinite(x) && IsFinite(y); }
#endif
	inline void Negate(void)						{ x = -x; y = -y; }
	inline vec_t Length(void) const					{ return (vec_t)sqrt(x*x + y*y); }
	inline float Square(void) const					{ return x*y; }
	inline Vector2D Normalize(void) const
	{
		float flLen = Length();
		if (flLen == 0.0f) return Vector2D(0.0f,0.0f);
		flLen = 1.0f / flLen;
		return Vector2D(x * flLen, y * flLen);
	}
	inline vec_t NormalizeSelf(void)// XDM3037: this is how it shoud be
	{
		vec_t flLen = Length();
		if (flLen != 0.0f)
		{
			x /= flLen;
			y /= flLen;
		}
		return flLen;
	}
	// Base address
	vec_t *Base()					{ return (vec_t *)this; }
	vec_t const *Base() const		{ return (vec_t const *)this; }
};

#define vec2_t Vector2D

FORCEINLINE Vector2D &Vector2D::operator+=(const Vector2D &v)
{
//	CHECK_VALID(*this);
//	CHECK_VALID(v);
	x+=v.x; y+=v.y;
	return *this;
}

FORCEINLINE Vector2D &Vector2D::operator-=(const Vector2D &v)
{
//	CHECK_VALID(*this);
//	CHECK_VALID(v);
	x-=v.x; y-=v.y;
	return *this;
}

FORCEINLINE Vector2D &Vector2D::operator*=(const float &fl)
{
	x *= fl; y *= fl;
//	CHECK_VALID(*this);
	return *this;
}
/*
FORCEINLINE Vector2D &Vector2D::operator*=(float &fl)
{
	x *= fl; y *= fl;
//	CHECK_VALID(*this);
	return *this;
}*/

FORCEINLINE Vector2D &Vector2D::operator*=(const Vector2D &v)
{
//	CHECK_VALID(v);
	x *= v.x; y *= v.y;
//	CHECK_VALID(*this);
	return *this;
}

FORCEINLINE Vector2D &Vector2D::operator/=(const float &fl)
{
	ASSERT(fl != 0.0f);
	float oofl = 1.0f / fl;
	x *= oofl; y *= oofl;
//	CHECK_VALID(*this);
	return *this;
}
/*
FORCEINLINE Vector2D &Vector2D::operator/=(float &fl)
{
	ASSERT(fl != 0.0f);
	float oofl = 1.0f / fl;
	x *= oofl; y *= oofl; z *= oofl;
//	CHECK_VALID(*this);
	return *this;
}*/

FORCEINLINE Vector2D &Vector2D::operator/=(const Vector2D &v)
{
//	CHECK_VALID(v);
	ASSERT(v.x != 0.0f && v.y != 0.0f);
	x /= v.x; y /= v.y;
//	CHECK_VALID(*this);
	return *this;
}

// stand-alone
inline Vector2D operator*(float fl, const Vector2D &v)
{
	return v * fl;
}

inline float DotProduct(const Vector2D &a, const Vector2D &b)
{
	return(a.x*b.x + a.y*b.y);
}






/*=========================================================
 3D Vector - same data-layout as vec_t[3]
=========================================================*/
class Vector
{
public:
#if defined (ALLOW_ANONYMOUS_STRUCT)
	union
	{
		vec_t array[3];
		struct
		{
			vec_t x;
			vec_t y;
			vec_t z;
		};
	};
#else
	vec_t x;
	vec_t y;
	vec_t z;
#endif

	// construction/destruction
	inline Vector(void): x(0.0f), y(0.0f), z(0.0f)					{ }
	inline Vector(const float &X, const float &Y, const float &Z)	{ x = X; y = Y; z = Z; }
	inline Vector(const double &X, const double &Y, const double &Z){ x = (float)X; y = (float)Y; z = (float)Z;	}
	inline Vector(const int &X, const int &Y, const int &Z)			{ x = (float)X; y = (float)Y; z = (float)Z;	}
	inline Vector(const Vector &v)									{ x = v.x; y = v.y; z = v.z;				}
	inline Vector(float rgfl[3])									{ x = rgfl[0]; y = rgfl[1]; z = rgfl[2];	}
	inline Vector(const Vector2D &v)								{ x = v.x; y = v.y; z = 0.0f;				}

	// operators
	inline Vector &operator=(const Vector &v)		{ x=v.x; y=v.y; z=v.z; return *this;	}
	inline bool operator==(const Vector &v) const	{ return (x==v.x && y==v.y && z==v.z);	}
	inline bool operator!=(const Vector &v) const	{ return (x!=v.x || y!=v.y || z!=v.z);	}
	inline Vector operator-(void) const				{ return Vector(-x,-y,-z);				}

	inline Vector operator+(const Vector &v) const	{ return Vector(x+v.x, y+v.y, z+v.z);	}
	inline Vector operator-(const Vector &v) const	{ return Vector(x-v.x, y-v.y, z-v.z);	}
	inline Vector operator*(const float &fl) const	{ return Vector(x*fl, y*fl, z*fl);		}
	inline Vector operator/(const float &fl) const	{ return Vector(x/fl, y/fl, z/fl);		}
//	inline Vector operator*(const double &fl) const	{ return Vector(x*fl, y*fl, z*fl);		}
//	inline Vector operator/(const double &fl) const	{ return Vector(x/fl, y/fl, z/fl);		}

	// arithmetic operations
	FORCEINLINE Vector &operator+=(const Vector &v);
	FORCEINLINE Vector &operator-=(const Vector &v);
	FORCEINLINE Vector &operator*=(const Vector &v);
	FORCEINLINE Vector &operator*=(const float &s);
//	FORCEINLINE Vector &operator*=(const float s);
//	FORCEINLINE Vector &operator*=(float &s);
	FORCEINLINE Vector &operator/=(const Vector &v);
	FORCEINLINE Vector &operator/=(const float &s);
//	FORCEINLINE Vector &operator/=(const float s);
//	FORCEINLINE Vector &operator/=(float &s);

	// for modifications
	float &operator[](int index)
	{
#ifdef _DEBUG
		if (index < 0 || index >= 3)// ASSERT
			index = 0;
#endif
#if defined (ALLOW_ANONYMOUS_STRUCT)
		return array[index];
#else
		return ((vec_t *)this)[index];
#endif
	}
	// for reading/comparing only
	const float &operator[](int index) const
	{
#ifdef _DEBUG
		if (index < 0 || index >= 3)// ASSERT
			index = 0;
#endif
#if defined (ALLOW_ANONYMOUS_STRUCT)
		return array[index];
#else
		return ((vec_t *)this)[index];
#endif
	}

#if defined (ALLOW_ANONYMOUS_STRUCT)
	float *As3f(void)								{ return array; }
	operator float *()								{ return array; }// automatically convert to (float *) when needed
	operator float *() const						{ return (float *)array; }// Hack? Some old functions require (float *) as argument, but we need a way to supply (const Vector)s. Overrides const-protection
	operator const float *() const					{ return array; }
#else
	float *As3f(void)								{ return &x; }
	operator float *()								{ return &x; }// automatically convert to (float *) when needed
	operator float *() const						{ return (float *)&x; }// Hack? Some old functions require (float *) as argument, but we need a way to supply (const Vector)s. Overrides const-protection
	operator const float *() const					{ return &x; }
#endif
	inline void CopyToArray(float *rgfl) const		{ rgfl[0] = x, rgfl[1] = y, rgfl[2] = z; }
	inline bool IsZero(void) const					{ return (x==0.0f && y==0.0f && z==0.0f); }
#ifdef VALIDATE_VECTORS
	inline bool IsValid(void) const					{ return IsFinite(x) && IsFinite(y) && IsFinite(z); }
#endif
	inline void Clear(void)							{ x = 0.0f; y = 0.0f; z = 0.0f; }
	inline void Negate(void)						{ x = -x; y = -y; z = -z; }
	inline vec_t Length(void) const					{ return (vec_t)sqrt(x*x + y*y + z*z); }
	inline float Volume(void) const					{ return x*y*z; }
	inline Vector Normalize(void) const
	{
		float flLen = Length();
		if (flLen == 0.0f) return Vector(0.0f,0.0f,1.0f); /* ???? */
		flLen = 1.0f / flLen;
		return Vector(x * flLen, y * flLen, z * flLen);
	}
	inline vec_t NormalizeSelf(void)// XDM3037: this is how it shoud be
	{
		vec_t flLen = Length();
		if (flLen != 0.0f)
		{
			x /= flLen;
			y /= flLen;
			z /= flLen;
		}
		return flLen;
	}
	// 2D operations
	inline Vector2D Make2D(void) const
	{
		return Vector2D(x,y);
	}
	inline vec_t Length2D(void) const
	{
		return (vec_t)sqrt(x*x + y*y);
	}

	// Base address
	vec_t *Base()					{ return (vec_t *)this; }
	vec_t const *Base() const		{ return (vec_t const *)this; }
};

#define vec3_t Vector

FORCEINLINE Vector &Vector::operator+=(const Vector &v)
{
//	CHECK_VALID(*this);
//	CHECK_VALID(v);
	x+=v.x; y+=v.y; z += v.z;
	return *this;
}

FORCEINLINE Vector &Vector::operator-=(const Vector &v)
{
//	CHECK_VALID(*this);
//	CHECK_VALID(v);
	x-=v.x; y-=v.y; z -= v.z;
	return *this;
}

FORCEINLINE Vector &Vector::operator*=(const float &fl)
{
	x *= fl; y *= fl; z *= fl;
//	CHECK_VALID(*this);
	return *this;
}
/*
FORCEINLINE Vector &Vector::operator*=(float &fl)
{
	x *= fl; y *= fl; z *= fl;
//	CHECK_VALID(*this);
	return *this;
}*/

FORCEINLINE Vector &Vector::operator*=(const Vector &v)
{
//	CHECK_VALID(v);
	x *= v.x; y *= v.y; z *= v.z;
//	CHECK_VALID(*this);
	return *this;
}

FORCEINLINE Vector &Vector::operator/=(const float &fl)
{
	ASSERT(fl != 0.0f);
	float oofl = 1.0f / fl;
	x *= oofl; y *= oofl; z *= oofl;
//	CHECK_VALID(*this);
	return *this;
}
/*
FORCEINLINE Vector &Vector::operator/=(float &fl)
{
	ASSERT(fl != 0.0f);
	float oofl = 1.0f / fl;
	x *= oofl; y *= oofl; z *= oofl;
//	CHECK_VALID(*this);
	return *this;
}*/

FORCEINLINE Vector &Vector::operator/=(const Vector &v)
{
//	CHECK_VALID(v);
	ASSERT(v.x != 0.0f && v.y != 0.0f && v.z != 0.0f);
	x /= v.x; y /= v.y; z /= v.z;
//	CHECK_VALID(*this);
	return *this;
}

// stand-alone
inline Vector operator*(float fl, const Vector &v)
{
	return v * fl;
}

inline float DotProduct(const Vector &a, const Vector &b)
{
	return (a.x*b.x+a.y*b.y+a.z*b.z);
}

inline Vector CrossProduct(const Vector &a, const Vector &b)
{
	return Vector(a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x);
}



/*=========================================================
 4D Vector - for matrix operations
=========================================================*/
class Vector4D
{
public:
#if defined (ALLOW_ANONYMOUS_STRUCT)
	union
	{
		vec_t array[4];
		struct
		{
			vec_t x;
			vec_t y;
			vec_t z;
			vec_t w;
		};
	};
#else
	vec_t x;
	vec_t y;
	vec_t z;
	vec_t w;
#endif

	// construction/destruction
	inline Vector4D(void) {}
//	inline Vector4D(void): x(0.0f), y(0.0f), z(0.0f), w(0.0f)						{ }
	inline Vector4D(const float &X, const float &Y, const float &Z, const float &W)	{ x = X; y = Y; z = Z; w = W; }
	inline Vector4D(const double &X, const double &Y, const double &Z, const double &W){ x = (float)X; y = (float)Y; z = (float)Z; w = (float)W;}
	inline Vector4D(const int &X, const int &Y, const int &Z, const int &W)			{ x = (float)X; y = (float)Y; z = (float)Z; w = (float)W;	}
	inline Vector4D(const Vector4D &v)												{ x = v.x; y = v.y; z = v.z; w = v.w;						}
	inline Vector4D(float rgfl[4])													{ x = rgfl[0]; y = rgfl[1]; z = rgfl[2]; w = rgfl[3];		}
	inline Vector4D(const Vector &v)												{ x = v.x; y = v.y; z = v.z; w = 0.0f;						}

	// operators
	inline Vector4D &operator=(const Vector4D &v)		{ x=v.x; y=v.y; z=v.z; w=v.w; return *this;		}
	inline bool operator==(const Vector4D &v) const		{ return (x==v.x && y==v.y && z==v.z && w==v.w);}
	inline bool operator!=(const Vector4D &v) const		{ return (x!=v.x || y!=v.y || z!=v.z || w!=v.w);}
	inline Vector4D operator-(void) const				{ return Vector4D(-x,-y,-z,-w);					}
/*
	inline Vector4D operator+(const Vector4D &v) const	{ return Vector4D(x+v.x, y+v.y, z+v.z, w+v.w);	}
	inline Vector4D operator-(const Vector4D &v) const	{ return Vector4D(x-v.x, y-v.y, z-v.z, w-v.w);	}
	inline Vector4D operator*(const float &fl) const	{ return Vector4D(x*fl, y*fl, z*fl, w*fl);		}
	inline Vector4D operator/(const float &fl) const	{ return Vector4D(x/fl, y/fl, z/fl, w/fl);		}
//	inline Vector4D operator*(const double &fl) const	{ return Vector4D(x*fl, y*fl, z*fl, w*fl);		}
//	inline Vector4D operator/(const double &fl) const	{ return Vector4D(x/fl, y/fl, z/fl, w/fl);		}
*/
	// for modifications
	float &operator[](int index)
	{
#ifdef _DEBUG
		ASSERT(index >= 0 && index < 4);
//		if (index < 0 || index >= 4)// ASSERT
//			index = 0;
#endif
#if defined (ALLOW_ANONYMOUS_STRUCT)
		return array[index];
#else
		return ((vec_t*)this)[index];
#endif
	}
	// for reading/comparing only
	const float &operator[](int index) const
	{
#ifdef _DEBUG
		ASSERT(index >= 0 && index < 4);
//		if (index < 0 || index >= 4)// ASSERT
//			index = 0;
#endif
#if defined (ALLOW_ANONYMOUS_STRUCT)
		return array[index];
#else
		return ((vec_t*)this)[index];
#endif
	}

	float *As4f(void)								{ return &x; }
	operator float *()								{ return &x; }// automatically convert to (float *) when needed
	operator float *() const						{ return (float *)&x; }// Hack? Some old functions require (float *) as argument, but we need a way to supply (const Vector)s. Overrides const-protection
	operator const float *() const					{ return &x; }
	operator Vector()								{ return Vector(x,y,z); }
	operator const Vector() const					{ return Vector(x,y,z); } 

	// initialization
	void Init(float ix = 0.0f, float iy = 0.0f, float iz = 0.0f, float iw = 0.0f)
	{
		x = ix; y = iy; z = iz; w = iw;
	}

#ifdef VALIDATE_VECTORS
	inline bool IsValid(void) const					{ return IsFinite(x) && IsFinite(y) && IsFinite(z) && IsFinite(w); }
#endif
};

//NOT NOW! The code is not ready for this. #define vec4_t Vector4D
typedef vec_t vec4_t[4];


#else /* __cplusplus */


typedef vec_t vec2_t[2];
typedef vec_t vec3_t[3];
typedef vec_t vec4_t[4];


#endif /* __cplusplus */


extern const vec3_t g_vecZero;


#endif /* VECTOR_H */
