#include "stdafx.h"

Camera::Camera()
	{
	m_mxProjection = PVRTMat4::Identity();
	m_mxView       = PVRTMat4::Identity();
	};

/*!***********************************************************************
 @Function		SetProjection
 @Access		public 
 @Param			const PVRTMat4 & mxProj
 @Returns		void
 @Description	
*************************************************************************/
void Camera::SetProjection(const PVRTMat4& mxProj)
	{
	m_mxProjection = mxProj;
	}

/*!***********************************************************************
 @Function		Set
 @Access		public 
 @Param			const PVRTMat4 & mxView
 @Returns		void
 @Description	
*************************************************************************/
void Camera::Set(const PVRTMat4& mxView)
	{
	m_mxView = mxView;
	}

/*!***********************************************************************
 @Function		SetPosition
 @Access		public 
 @Param			const PVRTVec3 & vPos
 @Returns		void
 @Description	
*************************************************************************/
void Camera::SetPosition(const PVRTVec3& vPos)
	{
	m_mxView.f[12] = vPos.x;
	m_mxView.f[13] = vPos.y;
	m_mxView.f[14] = vPos.z;
	}

/*!***********************************************************************
 @Function		Translate
 @Access		public 
 @Param			const PVRTVec3 & vTrans
 @Returns		void
 @Description	
*************************************************************************/
void Camera::Translate(const PVRTVec3& vTrans)
	{
	m_mxView.f[12] += vTrans.x;
	m_mxView.f[13] += vTrans.y;
	m_mxView.f[14] += vTrans.z;
	}

/*!***********************************************************************
 @Function		Rotate
 @Access		public 
 @Param			const PVRTMat3 & mxRot
 @Returns		void
 @Description	
*************************************************************************/
void Camera::Rotate(const PVRTMat3&	mxRot)
	{

	}

/*!***********************************************************************
 @Function		GetView
 @Access		public 
 @Returns		const PVRTMat4&
 @Description	
*************************************************************************/
const PVRTMat4& Camera::GetView()
	{
	return m_mxView;
	}

/*!***********************************************************************
 @Function		GetProjection
 @Access		public 
 @Returns		const PVRTMat4&
 @Description	
*************************************************************************/
const PVRTMat4& Camera::GetProjection()
	{
	return m_mxProjection;
	}