//-----------------------------------------------------------------------------
// X-Half-Life: Deathmatch code
// Copyright (c) 2001-2012
// NOTE: most methods are useful only for float[] arrays,
// don't use these with class vector.
//-----------------------------------------------------------------------------
#include <math.h>
#include "vector.h"
#include "util_vector.h"
#ifdef CLIENT_DLL
#include "hud.h"
#include "cl_util.h"// RANDOM_FLOAT
#else
#include "extdll.h"
#include "util.h"// RANDOM_FLOAT
#endif

// XDM3035: from mathlib
float anglemod(const float &a)
{
	return (360.0f/65536.0f) * ((int)(a*(65536.0f/360.0f)) & 65535);
}

float AngleDiff(const float &destAngle, const float &srcAngle)
{
	float delta = destAngle - srcAngle;
	NormalizeAngle(&delta);
/*	byte c = 0;
	if (destAngle > srcAngle)
	{
		while (delta >= 180.0f && c < 255)
		{
			delta -= 360.0f;
			c++;
		}
	}
	else
	{
		while (delta <= -180.0f && c < 255)
		{
			delta += 360.0f;
			c++;
		}
	}
	ASERT(c < 128);// XDM: this will reveal bogus numbers!
*/
	return delta;
}

/*int VectorCompare(const float *v1, const float *v2)
{
	int i;
	for (i=0 ; i<3 ; ++i)
		if (v1[i] != v2[i])
			return 0;

	return 1;
}*/

void VectorCopy(const float *src, float *dst)
{
	dst[0] = src[0];
	dst[1] = src[1];
	dst[2] = src[2];
}

void VectorAdd(const float *a, const float *b, float *dst)
{
	dst[0]=a[0]+b[0];
	dst[1]=a[1]+b[1];
	dst[2]=a[2]+b[2];
}

void VectorSubtract(const float *a, const float *b, float *dst)
{
	dst[0]=a[0]-b[0];
	dst[1]=a[1]-b[1];
	dst[2]=a[2]-b[2];
}

/*void VectorMultiply(const float *a, const float *b, float *dst)
{
	dst[0]=a[0]*b[0];
	dst[1]=a[1]*b[1];
	dst[2]=a[2]*b[2];
}*/

void VectorScale(const float *in, const float &scale, float *out)
{
	out[0] = in[0]*scale;
	out[1] = in[1]*scale;
	out[2] = in[2]*scale;
}

void VectorMA(const float *veca, const float &scale, const float *vecb, float *vecc)
{
	vecc[0] = veca[0] + scale*vecb[0];
	vecc[1] = veca[1] + scale*vecb[1];
	vecc[2] = veca[2] + scale*vecb[2];
}

float Length(const float *v)
{
	float length = v[0]*v[0] + v[1]*v[1] + v[2]*v[2];
	return (float)sqrt(length);// FIXME
}

/*float Distance(const float *v1, const float *v2)
{
	vec3_t d;
	VectorSubtract(v2,v1,d);
	return Length(d);
}

float DotProduct(const float *x, const float *y)
{
	return (x[0]*y[0] + x[1]*y[1] + x[2]*y[2]);
}*/

void CrossProduct(const float *v1, const float *v2, float *out)
{
	out[0] = v1[1]*v2[2] - v1[2]*v2[1];
	out[1] = v1[2]*v2[0] - v1[0]*v2[2];
	out[2] = v1[0]*v2[1] - v1[1]*v2[0];
}

float VectorNormalize(float *v)
{
	float	length, ilength;

	length = v[0]*v[0] + v[1]*v[1] + v[2]*v[2];
	length = sqrt(length);		// FIXME

	if (length)
	{
		ilength = 1/length;
		v[0] *= ilength;
		v[1] *= ilength;
		v[2] *= ilength;
	}
	return length;
}

void VectorInverse(float *v)
{
	v[0] = -v[0];
	v[1] = -v[1];
	v[2] = -v[2];
}

// -1 ... 1 by default
void VectorRandom(float *v, float min, float max)
{
//	vec3_t v;
//	ASSERT(v != NULL);
//	if (v)
	{
		v[0] = RANDOM_FLOAT(min, max);
		v[1] = RANDOM_FLOAT(min, max);
		v[2] = RANDOM_FLOAT(min, max);
	}
//	return v;
}

void VectorRandom(float *v, const float *halfvolume)
{
	v[0] = RANDOM_FLOAT(-halfvolume[0], halfvolume[0]);
	v[1] = RANDOM_FLOAT(-halfvolume[1], halfvolume[1]);
	v[2] = RANDOM_FLOAT(-halfvolume[2], halfvolume[2]);
}

void VectorRandom(float *v, const float *mins, const float *maxs)
{
	v[0] = RANDOM_FLOAT(mins[0], maxs[0]);
	v[1] = RANDOM_FLOAT(mins[1], maxs[1]);
	v[2] = RANDOM_FLOAT(mins[2], maxs[2]);
}

// -1 ... 1
vec3_t VectorRandom(void)
{
	vec3_t v;
	VectorRandom(v);
	return v;
}

// 1D, modifies original
// -180...180 version
void NormalizeAngle(float *angle)
{
	byte c = 0;
	while (*angle > 180.0f && c < 255)// XDM: support for BIG angles :)
	{
		*angle -= 360.0f;
		++c;
	}
	while (*angle < -180.0f && c < 255)
	{
		*angle += 360.0f;
		++c;
	}
	ASSERT(c < 10);// XDM: this will reveal bogus numbers!
	if (c >= 10)
		*angle = 0.0f;// XDM: SUPRALEGAL MEASURES!! (tm)
}

// 0...360 version
void NormalizeAngle360(float *angle)
{
	byte c = 0;
	while (*angle > 360.0f && c < 255)// XDM: support for BIG angles :)
	{
		*angle -= 360.0f;
		++c;
	}
	while (*angle < 0.0f && c < 255)
	{
		*angle += 360.0f;
		++c;
	}
	ASSERT(c < 10);// XDM: this will reveal bogus numbers!
	if (c >= 10)
		*angle = 0.0f;// XDM: SUPRALEGAL MEASURES!! (tm)
}

// 1D, modifies original
void NormalizeAngle(float &angle)
{
	NormalizeAngle(&angle);
}

// 1D, returns modified copy
float NormalizeAngle(float angle)
{
	NormalizeAngle(&angle);
	return angle;
}

// 3D, modifies original
void NormalizeAngles(float *angles)
{
	byte i;// is this better than int?
	for (i = 0; i < 3; ++i)
		NormalizeAngle(&angles[i]);// XDM3035c: I love to reuse code
}

// XDM3035: optimized versions

void VectorAngles(const float *forward, float *angles)
{
	float tmp, yaw, pitch;
	
	if (forward[1] == 0 && forward[0] == 0)
	{
		yaw = 0.0f;
		if (forward[2] > 0.0f)
			pitch = 90.0f;
		else
			pitch = 270.0f;
	}
	else
	{
		yaw = (atan2(forward[1], forward[0]) * d180pi);
		if (yaw < 0.0f)
			yaw += 360.0f;

		tmp = sqrt(forward[0]*forward[0] + forward[1]*forward[1]);
		pitch = (atan2(forward[2], tmp) * d180pi);
		if (pitch < 0.0f)
			pitch += 360.0f;
	}
	
	angles[0] = pitch;
	angles[1] = yaw;
	angles[2] = 0.0f;
}

void AngleVectors(const float *angles, float *forward, float *right, float *up)
{
	float sr, sp, sy, cr, cp, cy;
	// XDM3035c: X_DEG2RAD
	SinCos(DEG2RAD(angles[PITCH]), &sp, &cp);
	SinCos(DEG2RAD(angles[YAW]), &sy, &cy);
	SinCos(DEG2RAD(angles[ROLL]), &sr, &cr);

	if (forward)
	{
		forward[0] = cp*cy;
		forward[1] = cp*sy;
		forward[2] = -sp;
	}
	if (right)
	{
		right[0] = (-1.0f*sr*sp*cy + -1.0f*cr*-sy);
		right[1] = (-1.0f*sr*sp*sy + -1.0f*cr*cy);
		right[2] = -1.0f*sr*cp;
	}
	if (up)
	{
		up[0] = (cr*sp*cy+-sr*-sy);
		up[1] = (cr*sp*sy+-sr*cy);
		up[2] = cr*cp;
	}
}

void AngleVectorsTranspose(const float *angles, float *forward, float *right, float *up)
{
	float sr, sp, sy, cr, cp, cy;

	SinCos(DEG2RAD(angles[PITCH]), &sp, &cp);
	SinCos(DEG2RAD(angles[YAW]), &sy, &cy);
	SinCos(DEG2RAD(angles[ROLL]), &sr, &cr);

	if (forward)
	{
		forward[0] = cp*cy;
		forward[1] = (sr*sp*cy+cr*-sy);
		forward[2] = (cr*sp*cy+-sr*-sy);
	}
	if (right)
	{
		right[0] = cp*sy;
		right[1] = (sr*sp*sy+cr*cy);
		right[2] = (cr*sp*sy+-sr*cy);
	}
	if (up)
	{
		up[0] = -sp;
		up[1] = sr*cp;
		up[2] = cr*cp;
	}
}

// old and buggy
void InterpolateAngles(float *start, float *end, float *output, float frac)
{
	int i;
	float ang1, ang2;
	float d;

	NormalizeAngles(start);
	NormalizeAngles(end);

	for (i = 0; i < 3; ++i)
	{
		ang1 = start[i];
		ang2 = end[i];

		d = ang2 - ang1;
		if (d > 180.0f)
		{
			d -= 360.0f;
		}
		else if (d < -180.0f)
		{
			d += 360.0f;
		}
		output[i] = ang1 + d * frac;
	}
	NormalizeAngles(output);
}

// You'd better provide normalized vectors
float AngleBetweenVectors(const vec3_t &v1, const vec3_t &v2)
{
	float l1 = Length(v1);
	float l2 = Length(v2);

	if (l1 == 0.0f || l2 == 0.0f)
		return 0.0f;

	return d180pi * ((float)acos(DotProduct(v1, v2)) / (l1*l2));
}

void AngleMatrix(const float *origin, const float *angles, float (*matrix)[4])
{
	float sr, sp, sy, cp, cy, cr;
	SinCos(DEG2RAD(angles[PITCH]), &sp, &cp);
	SinCos(DEG2RAD(angles[YAW]), &sy, &cy);
	SinCos(DEG2RAD(angles[ROLL]), &sr, &cr);
	matrix[0][0] = cp*cy;
	matrix[1][0] = cp*sy;
	matrix[2][0] = -sp;
	matrix[0][1] = sr*sp*cy+cr*-sy;
	matrix[1][1] = sr*sp*sy+cr*cy;
	matrix[2][1] = sr*cp;
	matrix[0][2] = (cr*sp*cy+-sr*-sy);
	matrix[1][2] = (cr*sp*sy+-sr*cy);
	matrix[2][2] = cr*cp;
	matrix[0][3] = origin[0];
	matrix[1][3] = origin[1];
	matrix[2][3] = origin[2];
}

void AngleIMatrix(const float *origin, const float *angles, float matrix[3][4])
{
	float sr, sp, sy, cp, cy, cr;// sin/cos pitch/yaw/roll
	SinCos(DEG2RAD(angles[PITCH]), &sp, &cp);
	SinCos(DEG2RAD(angles[YAW]), &sy, &cy);
	SinCos(DEG2RAD(angles[ROLL]), &sr, &cr);
	// matrix = (YAW * PITCH) * ROLL
	matrix[0][0] = cp*cy;
	matrix[0][1] = cp*sy;
	matrix[0][2] = -sp;
	matrix[1][0] = sr*sp*cy+cr*-sy;
	matrix[1][1] = sr*sp*sy+cr*cy;
	matrix[1][2] = sr*cp;
	matrix[2][0] = (cr*sp*cy+-sr*-sy);
	matrix[2][1] = (cr*sp*sy+-sr*cy);
	matrix[2][2] = cr*cp;
	matrix[0][3] = origin[0];
	matrix[1][3] = origin[1];
	matrix[2][3] = origin[2];
}

void VectorTransform(const vec3_t &in1, float in2[3][4], vec3_t &out)
{
	out[0] = DotProduct(in1, (vec3_t)in2[0]) + in2[0][3];
	out[1] = DotProduct(in1, (vec3_t)in2[1]) + in2[1][3];
	out[2] = DotProduct(in1, (vec3_t)in2[2]) + in2[2][3];
}

void ConcatTransforms(float in1[3][4], float in2[3][4], float out[3][4])
{
	out[0][0] = in1[0][0] * in2[0][0] + in1[0][1] * in2[1][0] +
				in1[0][2] * in2[2][0];
	out[0][1] = in1[0][0] * in2[0][1] + in1[0][1] * in2[1][1] +
				in1[0][2] * in2[2][1];
	out[0][2] = in1[0][0] * in2[0][2] + in1[0][1] * in2[1][2] +
				in1[0][2] * in2[2][2];
	out[0][3] = in1[0][0] * in2[0][3] + in1[0][1] * in2[1][3] +
				in1[0][2] * in2[2][3] + in1[0][3];
	out[1][0] = in1[1][0] * in2[0][0] + in1[1][1] * in2[1][0] +
				in1[1][2] * in2[2][0];
	out[1][1] = in1[1][0] * in2[0][1] + in1[1][1] * in2[1][1] +
				in1[1][2] * in2[2][1];
	out[1][2] = in1[1][0] * in2[0][2] + in1[1][1] * in2[1][2] +
				in1[1][2] * in2[2][2];
	out[1][3] = in1[1][0] * in2[0][3] + in1[1][1] * in2[1][3] +
				in1[1][2] * in2[2][3] + in1[1][3];
	out[2][0] = in1[2][0] * in2[0][0] + in1[2][1] * in2[1][0] +
				in1[2][2] * in2[2][0];
	out[2][1] = in1[2][0] * in2[0][1] + in1[2][1] * in2[1][1] +
				in1[2][2] * in2[2][1];
	out[2][2] = in1[2][0] * in2[0][2] + in1[2][1] * in2[1][2] +
				in1[2][2] * in2[2][2];
	out[2][3] = in1[2][0] * in2[0][3] + in1[2][1] * in2[1][3] +
				in1[2][2] * in2[2][3] + in1[2][3];
}

// angles index are not the same as ROLL, PITCH, YAW

void MatrixCopy(float in[3][4], float out[3][4])
{
	memcpy(out, in, sizeof(float) * 12);//3.0 * 4.0 );
}

void AngleQuaternion(float *angles, vec4_t quaternion)
{
	float sr, sp, sy, cr, cp, cy;

	// FIXME: rescale the inputs to 1/2 angle
	SinCos(angles[0] * 0.5f, &sr, &cr);
	SinCos(angles[1] * 0.5f, &sp, &cp);
	SinCos(angles[2] * 0.5f, &sy, &cy);

	quaternion[0] = sr*cp*cy-cr*sp*sy; // X
	quaternion[1] = cr*sp*cy+sr*cp*sy; // Y
	quaternion[2] = cr*cp*sy-sr*sp*cy; // Z
	quaternion[3] = cr*cp*cy+sr*sp*sy; // W
}

void QuaternionSlerp(const vec4_t &p, vec4_t q, const float &t, vec4_t qt)
{
	int i;
	float	omega, cosom, sinom, sclp, sclq;

	// decide if one of the quaternions is backwards
	float a = 0;
	float b = 0;

	for (i = 0; i < 4; ++i)
	{
		a += (p[i]-q[i])*(p[i]-q[i]);
		b += (p[i]+q[i])*(p[i]+q[i]);
	}
	if (a > b)
	{
		for (i = 0; i < 4; ++i)
			q[i] = -q[i];
	}

	cosom = p[0]*q[0] + p[1]*q[1] + p[2]*q[2] + p[3]*q[3];

	if ((1.0f + cosom) > 0.000001)
	{
		if ((1.0f - cosom) > 0.000001)
		{
			omega = (float)acos(cosom);
			sinom = (float)sin(omega);
			sclp = sin((1.0f - t)*omega) / sinom;
			sclq = sin(t*omega) / sinom;
		}
		else
		{
			sclp = 1.0f - t;
			sclq = t;
		}
		for (i = 0; i < 4; i++)
			qt[i] = sclp * p[i] + sclq * q[i];
	}
	else
	{
		qt[0] = -q[1];
		qt[1] = q[0];
		qt[2] = -q[3];
		qt[3] = q[2];

		sclp = (float)sin((1.0f - t) * (0.5f * (float)M_PI));
		sclq = (float)sin(t * (0.5 * M_PI));

		for (i = 0; i < 3; ++i)
			qt[i] = sclp * p[i] + sclq * qt[i];
	}
}

void QuaternionMatrix(vec4_t quaternion, float (*matrix)[4])
{
	matrix[0][0] = 1.0f - 2.0f * quaternion[1] * quaternion[1] - 2.0f * quaternion[2] * quaternion[2];
	matrix[1][0] = 2.0f * quaternion[0] * quaternion[1] + 2.0f * quaternion[3] * quaternion[2];
	matrix[2][0] = 2.0f * quaternion[0] * quaternion[2] - 2.0f * quaternion[3] * quaternion[1];

	matrix[0][1] = 2.0f * quaternion[0] * quaternion[1] - 2.0f * quaternion[3] * quaternion[2];
	matrix[1][1] = 1.0f - 2.0f * quaternion[0] * quaternion[0] - 2.0f * quaternion[2] * quaternion[2];
	matrix[2][1] = 2.0f * quaternion[1] * quaternion[2] + 2.0f * quaternion[3] * quaternion[0];

	matrix[0][2] = 2.0f * quaternion[0] * quaternion[2] + 2.0f * quaternion[3] * quaternion[1];
	matrix[1][2] = 2.0f * quaternion[1] * quaternion[2] - 2.0f * quaternion[3] * quaternion[0];
	matrix[2][2] = 1.0f - 2.0f * quaternion[0] * quaternion[0] - 2.0f * quaternion[1] * quaternion[1];
}

void V_SmoothInterpolateAngles(float *startAngle, float *endAngle, float *finalAngle, float degreesPerSec, float &frametime)
{
	float absd,frac,d,threshhold;

	NormalizeAngles(startAngle);
	NormalizeAngles(endAngle);

	for (int i = 0; i < 3; ++i)
	{
		d = endAngle[i] - startAngle[i];

		if (d > 180.0f)
			d -= 360.0f;
		else if (d < -180.0f)
			d += 360.0f;

//TODO		NormalizeAngle(&d);

		absd = (float)fabs(d);

		if (absd > 0.01f)
		{
			frac = degreesPerSec * frametime;
			threshhold = degreesPerSec / 4.0f;

			if (absd < threshhold)
			{
				float h = absd / threshhold;
				h *= h;
				frac *= h;  // slow down last degrees
			}

			if (frac > absd)
			{
				finalAngle[i] = endAngle[i];
			}
			else
			{
				if (d>0)
					finalAngle[i] = startAngle[i] + frac;
				else
					finalAngle[i] = startAngle[i] - frac;
			}
		}
		else
			finalAngle[i] = endAngle[i];
	}
	NormalizeAngles(finalAngle);
}
