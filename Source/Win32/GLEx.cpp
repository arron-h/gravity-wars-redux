#include "stdafx.h"
#include "glext.h"

GLExtensions ext;

/*!***********************************************************************
 @Function		Load
 @Access		public 
 @Returns		void
 @Description	
*************************************************************************/
void GLExtensions::Load()
	{
	PROC p = NULL;

	// --- VBO
	p = wglGetProcAddress("glGenBuffers");
	if(p) glGenBuffersProc = (PFNGLGENBUFFERSPROC)p;
	p = wglGetProcAddress("glBindBuffer");
	if(p) glBindBufferProc = (PFNGLBINDBUFFERPROC)p;
	p = wglGetProcAddress("glBufferData");
	if(p) glBufferDataProc = (PFNGLBUFFERDATAPROC)p;

	// --- Shaders
	p = wglGetProcAddress("glCreateShader");
	if(p) glCreateShaderProc = (PFNGLCREATESHADERPROC)p;
	p = wglGetProcAddress("glShaderSource");
	if(p) glShaderSourceProc = (PFNGLSHADERSOURCEPROC)p;
	p = wglGetProcAddress("glCompileShader");
	if(p) glCompileShaderProc = (PFNGLCOMPILESHADERPROC)p;
	p = wglGetProcAddress("glGetShaderiv");
	if(p) glGetShaderivProc = (PFNGLGETSHADERIVPROC)p;
	p = wglGetProcAddress("glGetShaderInfoLog");
	if(p) glGetShaderInfoLogProc = (PFNGLGETSHADERINFOLOGPROC)p;
	p = wglGetProcAddress("glDeleteProgram");
	if(p) glDeleteProgramProc = (PFNGLDELETEPROGRAMPROC)p;
	p = wglGetProcAddress("glDeleteShader");
	if(p) glDeleteShaderProc = (PFNGLDELETESHADERPROC)p;
	p = wglGetProcAddress("glAttachShader");
	if(p) glAttachShaderProc = (PFNGLATTACHSHADERPROC)p;
	p = wglGetProcAddress("glBindAttribLocation");
	if(p) glBindAttribLocationProc = (PFNGLBINDATTRIBLOCATIONPROC)p;
	p = wglGetProcAddress("glCreateProgram");
	if(p) glCreateProgramProc = (PFNGLCREATEPROGRAMPROC)p;
	p = wglGetProcAddress("glLinkProgram");
	if(p) glLinkProgramProc = (PFNGLLINKPROGRAMPROC)p;
	p = wglGetProcAddress("glGetProgramiv");
	if(p) glGetProgramivProc = (PFNGLGETPROGRAMIVPROC)p;
	p = wglGetProcAddress("glGetProgramInfoLog");
	if(p) glGetProgramInfoLogProc = (PFNGLGETPROGRAMINFOLOGPROC)p;
	p = wglGetProcAddress("glUseProgram");
	if(p) glUseProgramProc = (PFNGLUSEPROGRAMPROC)p;

	p = wglGetProcAddress("glGetUniformLocation");
	if(p) glGetUniformLocationProc = (PFNGLGETUNIFORMLOCATIONPROC)p;
	p = wglGetProcAddress("glEnableVertexAttribArray");
	if(p) glEnableVertexAttribArrayProc = (PFNGLENABLEVERTEXATTRIBARRAYPROC)p;
	p = wglGetProcAddress("glVertexAttribPointer");
	if(p) glVertexAttribPointerProc = (PFNGLVERTEXATTRIBPOINTERPROC)p;
	p = wglGetProcAddress("glDisableVertexAttribArray");
	if(p) glDisableVertexAttribArrayProc = (PFNGLDISABLEVERTEXATTRIBARRAYPROC)p;
	p = wglGetProcAddress("glUniformMatrix4fv");
	if(p) glUniformMatrix4fvProc = (PFNGLUNIFORMMATRIX4FVPROC)p;
	p = wglGetProcAddress("glUniform3fv");
	if(p) glUniform3fvProc = (PFNGLUNIFORM3FVPROC)p;
	p = wglGetProcAddress("glUniform1f");
	if(p) glUniform1fProc = (PFNGLUNIFORM1FPROC)p;
	}

// -------------- Aux functions
void glGenBuffers(GLsizei a, GLuint *b)
	{
	ext.glGenBuffersProc(a, b);
	}
void glBindBuffer(GLenum a, GLuint b)
	{
	ext.glBindBufferProc(a, b);
	}
void glBufferData(GLenum a, GLsizeiptr b, const GLvoid *c, GLenum d)
	{
	ext.glBufferDataProc(a, b, c, d);
	}
GLuint glCreateShader(GLenum a)
	{
	return ext.glCreateShaderProc(a);
	}
void glShaderSource(GLuint a, GLsizei b, const GLchar **c, const GLint *d)
	{
	ext.glShaderSourceProc(a,b,c,d);
	}
void glCompileShader(GLuint a)
	{
	ext.glCompileShaderProc(a);
	}
void glGetShaderiv(GLuint a, GLenum b, GLint *c)
	{
	ext.glGetShaderivProc(a, b, c);
	}
void glGetShaderInfoLog(GLuint a, GLsizei b, GLsizei *c, GLchar *d)
	{
	ext.glGetShaderInfoLogProc(a, b, c, d);
	}
void glDeleteProgram(GLuint a)
	{
	ext.glDeleteProgramProc(a);
	}
void glDeleteShader(GLuint a)
	{
	ext.glDeleteShaderProc(a);
	}
void glAttachShader(GLuint a, GLuint b)
	{
	ext.glAttachShaderProc(a, b);
	}
void glBindAttribLocation(GLuint a, GLuint b, const GLchar *c)
	{
	ext.glBindAttribLocationProc(a, b, c);
	}
GLuint glCreateProgram(void)
	{
	return ext.glCreateProgramProc();
	}
void glLinkProgram(GLuint a)
	{
	ext.glLinkProgramProc(a);
	}
void glGetProgramiv(GLuint a, GLenum b, GLint *c)
	{
	ext.glGetProgramivProc(a, b, c);
	}
void glGetProgramInfoLog(GLuint a, GLsizei b, GLsizei *c, GLchar *d)
	{
	ext.glGetProgramInfoLogProc(a, b, c, d);
	}
void glUseProgram(GLuint a)	
	{
	ext.glUseProgramProc(a);
	}
GLint glGetUniformLocation(GLuint a, const GLchar *b)	
	{
	return ext.glGetUniformLocationProc(a, b);
	}
void glEnableVertexAttribArray(GLuint a)	
	{
	ext.glEnableVertexAttribArrayProc(a);
	}
void glVertexAttribPointer(GLuint a, GLint b, GLenum c, GLboolean d, GLsizei e, const GLvoid *f)
	{
	ext.glVertexAttribPointerProc(a, b, c, d, e, f);
	}
void glDisableVertexAttribArray(GLuint a)
	{
	ext.glDisableVertexAttribArrayProc(a);
	}
void glUniformMatrix4fv(GLint a, GLsizei b, GLboolean c, const GLfloat *d)
	{
	ext.glUniformMatrix4fvProc(a, b, c, d);
	}
void glUniform3fv(GLint a, GLsizei b, const GLfloat *c)
	{
	ext.glUniform3fvProc(a, b, c);
	}
void glUniform1f(GLint a, GLfloat b)
	{
	ext.glUniform1fProc(a, b);
	}