#ifndef GRAPHICSIMPL_H
#define GRAPHICSIMPL_H

class GraphicsSystemImpl : public GraphicsSystem
	{
	private:
		HDC							m_hDC;
		HGLRC						m_hRC;
		HWND						m_hWnd;
		HINSTANCE					m_hInstance;
		char						m_szWindowTitle[256];

		static bool					m_bWindowActive;
		static bool					m_bKeys[256];
		static bool					m_bMouseDown[2];
		static Touch				m_Touches[2];					

	public:
		GraphicsSystemImpl();
		~GraphicsSystemImpl();

		// --- Inherited methods
		virtual bool Initialise(Uint32 uiWidth, Uint32 uiHeight, Float32 fVWidth, Float32 fVHeight, bool bRotated, const char* c_pszWindowTitle = NULL);
		virtual void SwapBuffers();
		virtual void Destroy();
		
		// -- Implementation methods
		static void Resize(unsigned int width, unsigned int height);
		static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		bool IsActive();
		
		void SetOGLContext(HGLRC hContext);
		HGLRC CreateOGLContext();
		bool ShareOGLContext(HGLRC secContext);
		void MakeCurrent();
	};

#endif
