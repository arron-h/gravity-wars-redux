#include "stdafx.h"

/*!***********************************************************************
 @Function		Render
 @Access		public 
 @Returns		void
 @Description	
*************************************************************************/
void Emitter::Render()
	{
#ifdef _DEBUG
	GLint nActiveProg;
	glGetIntegerv(GL_CURRENT_PROGRAM, &nActiveProg);
	ASSERT(nActiveProg != 0);
#endif

	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	switch(m_eBlendMode)
		{
		default:
		case enumBLENDMODE_Additive:	glBlendFunc(GL_SRC_ALPHA, GL_ONE);	break;
		case enumBLENDMODE_Standard:	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	break;
		}
	

	glEnableVertexAttribArray(enumATTRIBUTE_POSITION);
	glEnableVertexAttribArray(enumATTRIBUTE_TEXCOORD0);
	glEnableVertexAttribArray(enumATTRIBUTE_RGBA);

	glBindTexture(GL_TEXTURE_2D, m_tex->GetHandle());

	// This is just a shitty PC implementation and will be better on iPhone.
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

			// Don't bother optimising this as this code isn't used on iPhone.
			glVertexAttribPointer(enumATTRIBUTE_POSITION,  3, GL_FLOAT, GL_FALSE,    0, fV);
			glVertexAttribPointer(enumATTRIBUTE_RGBA,      4, GL_UNSIGNED_BYTE, GL_TRUE, 0, u8RGBA);
			glVertexAttribPointer(enumATTRIBUTE_TEXCOORD0, 2, GL_FLOAT, GL_FALSE,   0, fUV);

			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			}
		}
	else if(m_eParticleType == enumPARTICLETYPE_Quads)
		{
		for(Uint32 uiQuads = 0; uiQuads < m_uiActiveParticles; uiQuads++)
			{
			m_pParticles[uiQuads].BuildGeometry();
			PrimQuad& quad = m_pQuads[uiQuads];		
	
			glVertexAttribPointer(enumATTRIBUTE_POSITION,  3, GL_FLOAT, GL_FALSE, sizeof(PrimVertex), &quad.vert[0].v);
			glVertexAttribPointer(enumATTRIBUTE_RGBA,      4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(PrimVertex), &quad.vert[0].rgba);
			glVertexAttribPointer(enumATTRIBUTE_TEXCOORD0, 2, GL_FLOAT, GL_FALSE, sizeof(PrimVertex), &quad.vert[0].uv);

			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			}
		}
	else
		{
		ASSERT(!"Support");
		}

	glDisableVertexAttribArray(enumATTRIBUTE_POSITION);
	glDisableVertexAttribArray(enumATTRIBUTE_TEXCOORD0);
	glDisableVertexAttribArray(enumATTRIBUTE_RGBA);

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// --- Render children
	if(m_pEmitters)
		{
		for(Uint32 uiIndex = 0; uiIndex < m_uiMaxEmitterChildren; uiIndex++)
			if(m_pEmitters[uiIndex].IsValid())
				m_pEmitters[uiIndex].Render();
		}
	}