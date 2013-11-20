#ifndef VIEW_H
#define VIEW_H

class View
	{
	private:
		Camera*		m_pCamera;

	public:
		View() : m_pCamera(NULL) {}
		virtual void Render() {};
		virtual void Update(double /*fDT*/) {};

		virtual void OnForeground() {};
		virtual void OnTouchDown(Touch* /*pTouches*/, Uint32 /*uiNum*/) {}
		virtual void OnTouchMoved(Touch* /*pTouches*/, Uint32 /*uiNum*/) {}
		virtual void OnTouchUp(Touch* /*pTouches*/, Uint32 /*uiNum*/) {}
        virtual void OnKeyPress(Uint32 /*eKey*/) {}

		void SetCamera(Camera* pCamera)
			{
			m_pCamera = pCamera;
			}

		Camera* GetCamera()
			{
			return m_pCamera;
			}
	};

#endif
