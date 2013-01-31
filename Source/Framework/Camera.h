#ifndef CAMERA_H
#define CAMERA_H

class Camera
	{
	private:
		PVRTMat4		m_mxProjection;
		PVRTMat4		m_mxView;

	public:
		Camera();

		void SetProjection(const PVRTMat4& mxProj);
		void Set(const PVRTMat4& mxView);
		void SetPosition(const PVRTVec3& vPos);

		const PVRTMat4& GetView();
		const PVRTMat4& GetProjection();
		
		void Translate(const PVRTVec3& vTrans);
		void Rotate(const PVRTMat3&	mxRot);

		// --- Implementation methods
		void Render();
	};

#endif
