#ifndef RSMODEL_H
#define RSMODEL_H

//#include "studio.h"// engine header for studiohdr_t

//-----------------------------------------------------------------------------
// A very convinient way to draw a studio model
//-----------------------------------------------------------------------------
class CRSModel : public CRenderSystem
{
	typedef CRenderSystem BaseClass;
public:
	CRSModel(void);
	CRSModel(const Vector &origin, const Vector &angles, const Vector &velocity, int entindex, int modelindex, int body, int skin, int sequence, int r_mode, int r_fx, byte r, byte g, byte b, float a, float adelta, float scale, float scaledelta, float framerate, float timetolive);
	virtual ~CRSModel(void);

	virtual void KillSystem(void);
	virtual void ResetParameters(void);
	virtual bool Update(const float &time, const double &elapsedTime);
	virtual void Render(void);
	virtual void CreateEntities(void);

	bool InitModel(int model_index);
	cl_entity_t *GetEntity(void) { return m_pEntity; };

	int m_iBSPLeaf;
private:
	cl_entity_t *m_pEntity;
	model_t *m_pModel;
//	studiohdr_t *m_pModelData;
	int m_iModelIndex;
};

#endif // RSMODEL_H
