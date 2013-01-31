#ifndef GRAPHICSIMPL_H
#define GRAPHICSIMPL_H

class GraphicsSystemImpl : public GraphicsSystem
	{
	private:

	public:
		GraphicsSystemImpl();
		~GraphicsSystemImpl();

		// --- Inherited methods
		virtual bool Initialise(Uint32 uiWidth, Uint32 uiHeight, Float32 fVWidth, Float32 fVHeight, bool bRotated, const char* c_pszWindowTitle = NULL);
		virtual void SwapBuffers();
		virtual void Destroy();
	};

#endif
