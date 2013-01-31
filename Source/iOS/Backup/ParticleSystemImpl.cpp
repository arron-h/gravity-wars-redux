#include "stdafx.h"

/*!***********************************************************************
 @Function		Render
 @Access		public 
 @Returns		void
 @Description	
*************************************************************************/
void Emitter::Render()
	{
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	glBindTexture(GL_TEXTURE_2D, m_tex->GetHandle());

	if(m_eParticleType == enumPARTICLETYPE_Points)
		{
		for(Uint32 uiPoint = 0; uiPoint < m_uiActiveParticles; uiPoint++)
			{
			m_pParticles[uiPoint].BuildGeometry();
			PrimPoint& point = m_pPoints[uiPoint];		
			Float32 fV[] = 
				{
				point.v.x - point.s*0.5f, point.v.y + point.s*0.5f, 0.0f,
				point.v.x - point.s*0.5f, point.v.y - point.s*0.5f, 0.0f,
				point.v.x + point.s*0.5f, point.v.y + point.s*0.5f, 0.0f,
				point.v.x + point.s*0.5f, point.v.y - point.s*0.5f, 0.0f,
				};
			Float32 fUV[] =
				{
				0.0f, 0.0f,
				0.0f, 1.0f,
				1.0f, 0.0f,
				1.0f, 1.0f,
				};
			Uint8 u8RGBA[] =
				{
				point.rgba[0], point.rgba[1], point.rgba[2], point.rgba[3],
				point.rgba[0], point.rgba[1], point.rgba[2], point.rgba[3],
				point.rgba[0], point.rgba[1], point.rgba[2], point.rgba[3],
				point.rgba[0], point.rgba[1], point.rgba[2], point.rgba[3],
				};

			glVertexPointer(3, GL_FLOAT, 0, fV);
			glColorPointer(4, GL_UNSIGNED_BYTE, 0, u8RGBA);
			glTexCoordPointer(2, GL_FLOAT, 0, fUV);

			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			}
		}
	else
		{
		ASSERT(!"Support");
		}

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}