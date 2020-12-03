/*
randomrange.h - simple class of implementation random values
Copyright (C) 2011 Uncle Mike

  XDM: NOTE: not checked
*/
#ifndef RANDOMRANGE_H
#define RANDOMRANGE_H

#include "vector.h"

// default constructor must set all balues to zero (particle code relies on this)
class RandomRange
{
public:
	RandomRange(void) { m_flMin = m_flMax = 0.0f; m_bDefined = false; }
	RandomRange(float fValue) { m_flMin = m_flMax = fValue; m_bDefined = true; }
	RandomRange(float fMin, float fMax) { m_flMin = fMin; m_flMax = fMax; m_bDefined = true; }
	RandomRange(char *szToken)// SHL compatibility
	{
		char *cOneDot = NULL;
		m_bDefined = true;
		for (char *c = szToken; *c; c++)
		{
			if (*c == '.')
			{
				if (cOneDot != NULL)
				{
					// found two dots in a row - it's a range
					*cOneDot = 0; // null terminate the first number
					m_flMin = (float)atof(szToken); // parse the first number
					*cOneDot = '.'; // change it back, just in case
					c++;
					m_flMax = (float)atof(c); // parse the second number
					return;
				}
				else
					cOneDot = c;
			}
			else
				cOneDot = NULL;
		}
		// no range, just record the number
		m_flMax = m_flMin = (float)atof(szToken);
	}

//	float Random() { return (((float)rand() / RAND_MAX) * (m_flMax - m_flMin) + m_flMin); }// a simple implementation of RANDOM_FLOAT
	float Random() { return RANDOM_FLOAT(m_flMin, m_flMax); }// better call engine function

	float Get(void) { return Random(); }
	float GetOffset(float fBasis) { return Random() - fBasis; }

	bool IsDefined(void) { return m_bDefined; } 

	// array access
	operator float *() { return &m_flMin; }
	operator const float *() const { return &m_flMin; } 

	float m_flMin, m_flMax;// NOTE: should be first so array acess is working correctly
	bool m_bDefined;
};

#endif // RANDOMRANGE_H
