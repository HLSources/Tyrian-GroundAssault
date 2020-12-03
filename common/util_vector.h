/*

  Purpose: vector manipulation functions 

  This header must be C compliant!
*/
/*
  WARNING!
 All arguments must be passed as (float *) and not vec3_t!
 Old shitty code depends on it!
*/

#ifndef UTIL_VECTOR_H
#define UTIL_VECTOR_H
#ifdef _WIN32
#ifndef __MINGW32__
#pragma once
#endif /* not __MINGW32__ */
#endif


#ifndef M_PI
#define M_PI		3.14159265358979323846	/* matches value in gcc v2 math.h */
#endif

const float d180pi = 180.0f / (float)M_PI;
const float pi180 = (float)M_PI/180.0f;/* XDM3035 */

// should this really be inline?
FORCEINLINE void SinCos(float rad, float *sinf, float *cosf)
{
#ifdef _WIN32
	_asm
	{
		fld DWORD PTR [rad]
		fsincos
		mov edx, DWORD PTR [cosf]
		mov eax, DWORD PTR [sinf]
		fstp DWORD PTR [edx]
		fstp DWORD PTR [eax]
	}
//#elif __linux))
#else
	register double __cosr, __sinr;
 	__asm __volatile__ ("fsincos" : "=t" (__cosr), "=u" (__sinr) : "0" (rad));
  	*sinf = __sinr;
  	*cosf = __cosr;
#endif
}

//#define FDotProduct(a, b) (fabs((a[0])*(b[0])) + fabs((a[1])*(b[1])) + fabs((a[2])*(b[2])))
float anglemod(const float &a);
float AngleDiff(const float &destAngle, const float &srcAngle);

/* vector routines */
//int VectorCompare(const float *v1, const float *v2);
void VectorCopy(const float *src, float *dst);
void VectorAdd(const float *a, const float *b, float *dst);
void VectorSubtract(const float *a, const float *b, float *dst);
//void VectorMultiply(const float *a, const float *b, float *dst);
void VectorScale(const float *in, const float &scale, float *out);
void VectorMA(const float *veca, const float &scale, const float *vecb, float *vecc);

float Length(const float *v);
//float Distance(const float *v1, const float *v2);
//float DotProduct(const float *x, const float *y);
void CrossProduct(const float *v1, const float *v2, float *out);

#define FDotProduct(a, b) (fabs((a[0])*(b[0])) + fabs((a[1])*(b[1])) + fabs((a[2])*(b[2])))

float VectorNormalize(float *v);
void VectorInverse(float *v);

#ifdef __cplusplus
void VectorRandom(float *v, float min = -1.0f, float max = 1.0f);
inline void VectorClear(float *a) { a[0]=0.0f; a[1]=0.0f; a[2]=0.0f; }
#else
//#error UTIL_VECTOR_H included in C file!
void VectorRandom(float *v, float min, float max);
void VectorClear(float *a) { a[0]=0.0f; a[1]=0.0f; a[2]=0.0f; }
#endif

void VectorRandom(float *v, const float *halfvolume);
void VectorRandom(float *v, const float *mins, const float *maxs);
vec3_t VectorRandom(void);
/*void VectorMatrix(const float *forward, float *right, float *up);*/

void NormalizeAngle(float *angle);
void NormalizeAngle360(float *angle);
void NormalizeAngle(float &angle);
float NormalizeAngle(float angle);
void NormalizeAngles(float *angles);
void VectorAngles(const float *forward, float *angles);
void AngleVectors(const float *angles, float *forward, float *right, float *up);
//void AngleVectors(const float *angles, float *forward, float *right, float *up);
void AngleVectorsTranspose(const float *angles, float *forward, float *right, float *up);

void InterpolateAngles(float *start, float *end, float *output, float frac);
float AngleBetweenVectors(const vec3_t &v1, const vec3_t &v2);

//void AngleMatrix(const float *angles, float (*matrix)[4]);
void AngleMatrix(const float *origin, const float *angles, float (*matrix)[4]);
void AngleIMatrix(const float *origin, const float *angles, float (*matrix)[4]);

void VectorTransform(const vec3_t &in1, float in2[3][4], vec3_t &out);
void ConcatTransforms(float in1[3][4], float in2[3][4], float out[3][4]);
void MatrixCopy(float in[3][4], float out[3][4]);

void AngleQuaternion(float *angles, vec4_t quaternion);
void QuaternionSlerp(const vec4_t &p, vec4_t q, const float &t, vec4_t qt);
void QuaternionMatrix(vec4_t quaternion, float (*matrix)[4]);

void V_SmoothInterpolateAngles(float *startAngle, float *endAngle, float *finalAngle, float degreesPerSec, float &frametime);

//#define BOX_ON_PLANE_SIDE(emins, emaxs, p) (((p)->type < 3)?(((p)->dist <= (emins)[(p)->type])? 1 : (((p)->dist >= (emaxs)[(p)->type])? 2 : 3)):BoxOnPlaneSide((emins), (emaxs), (p)))

#define PlaneDist(point,plane) (plane->type < 3 ? point[(int)plane->type] : DotProduct(point, plane->normal))
#define PlaneDiff(point,plane) ((plane->type < 3 ? point[(int)plane->type] : DotProduct(point, plane->normal)) - plane->dist)

#endif /* UTIL_VECTOR_H */
