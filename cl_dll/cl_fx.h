#ifndef CL_FX_H
#define CL_FX_H
#ifdef _WIN32
#ifndef __MINGW32__
#pragma once
#endif /* not __MINGW32__ */
#endif

#define BUBBLE_SCALE	1.0f
#define BUBBLE_SPEED	32.0f
#define BUBBLE_LIFE		10.0f

extern int *g_iMuzzleFlashSprites[];

struct particle_s *DrawParticle(const Vector &origin, short color, float life);
void ParticlesCustom(const Vector &origin, float rnd_vel, int color, int color_range, int number, float life, bool normalize = 0);

#ifdef DLIGHTH

inline struct dlight_s *DynamicLight(const Vector &vecPos, float radius, byte r, byte g, byte b, float life, float decay)
{
	dlight_t *dl = gEngfuncs.pEfxAPI->CL_AllocDlight(0);\
	if (!dl) return NULL;\
	VectorCopy(vecPos, dl->origin);\
	dl->radius = radius;\
	dl->color.r = r;\
	dl->color.g = g;\
	dl->color.b = b;\
	dl->decay = decay;\
	dl->die = gEngfuncs.GetClientTime() + life;\
	return dl;\
}

inline struct dlight_s *EntityLight(const Vector &vecPos, float radius, byte r, byte g, byte b, float life, float decay)
{
	dlight_t *dl = gEngfuncs.pEfxAPI->CL_AllocElight(0);\
	if (!dl) return NULL;\
	VectorCopy(vecPos, dl->origin);\
	dl->radius = radius;\
	dl->color.r = r;\
	dl->color.g = g;\
	dl->color.b = b;\
	dl->decay = decay;\
	dl->die = gEngfuncs.GetClientTime() + life;\
	return dl;\
}

#endif

//#ifndef TEMPENTITY
typedef struct tempent_s TEMPENTITY;
//#endif

void HookFXMessages(void);

struct dlight_s *CL_UpdateFlashlight(cl_entity_t *pEnt);
float FX_GetBubbleSpeed(void);

TEMPENTITY *FX_Trail(vec3_t origin, int entindex, unsigned short type, float life);

void FX_SparkShower(const Vector &origin, int mdl_idx, int count, const Vector &velocity, bool random);
TEMPENTITY *FX_Smoke(const Vector &origin, int spriteindex, float scale, float framerate);

void FX_DecalTrace(int decalindex, struct pmtrace_s *pTrace);
void FX_DecalTrace(int decalindex, const Vector &start, const Vector &end);
void DecalTrace(int decal, struct pmtrace_s *pTrace);
void DecalTrace(char *decalname, struct pmtrace_s *pTrace);
void DecalTrace(int decal, const Vector &start, const Vector &end);
void DecalTrace(char *decalname, const Vector &start, const Vector &end);

int FX_StreakSplash(const Vector &pos, const Vector &dir, color24 color, int count, float velocity, bool gravity = true, bool clip = true, bool bounce = true);//speed, int velocityMin, int velocityMax);
void FX_MuzzleFlashSprite(const Vector &pos, int entindex, short attachment, int sprite_index, int frame, float framerate, float scale, bool rotation);
//void FX_Bubbles(const Vector &mins, const Vector &maxs, const Vector &direction, int modelIndex, int count, float speed);
void FX_BubblesPoint(const Vector &center, const Vector &spread, int modelIndex, int count, float speed);
void FX_BubblesSphere(const Vector &center, float radius, int modelIndex, int count, float speed);
void FX_BubblesBox(const Vector &center, const Vector &halfbox, int modelIndex, int count, float speed);
void FX_BubblesLine(const Vector &start, const Vector &end, int modelIndex, int count, float speed);


void RenderFog(byte r, byte g, byte b, float fStartDist, float fEndDist, bool updateonly);
void ResetFog(void);

#endif // CL_FX_H
