#ifndef GLEXT_H
#define GLEXT_H

// A list of functions not supported by the shitty MS library.

class GLExtensions
	{
	public:
		// VBO
		PFNGLGENBUFFERSPROC glGenBuffersProc;
		PFNGLBINDBUFFERPROC glBindBufferProc;
		PFNGLBUFFERDATAPROC glBufferDataProc;

		// Shader
		PFNGLCREATESHADERPROC glCreateShaderProc;
		PFNGLSHADERSOURCEPROC glShaderSourceProc;
		PFNGLCOMPILESHADERPROC glCompileShaderProc;
		PFNGLGETSHADERIVPROC glGetShaderivProc;
		PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLogProc;
		PFNGLDELETEPROGRAMPROC glDeleteProgramProc;
		PFNGLDELETESHADERPROC glDeleteShaderProc;
		PFNGLATTACHSHADERPROC glAttachShaderProc;
		PFNGLBINDATTRIBLOCATIONPROC glBindAttribLocationProc;
		PFNGLCREATEPROGRAMPROC glCreateProgramProc;
		PFNGLLINKPROGRAMARBPROC glLinkProgramProc;
		PFNGLGETPROGRAMIVPROC glGetProgramivProc;
		PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLogProc;
		PFNGLUSEPROGRAMPROC glUseProgramProc;
		
		PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocationProc;
		PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArrayProc;
		PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointerProc;
		PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArrayProc;
		PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fvProc;
		PFNGLUNIFORM3FVPROC glUniform3fvProc;
		PFNGLUNIFORM1FPROC glUniform1fProc;

		void Load();
	};

extern void glGenBuffers(GLsizei, GLuint *);
extern void glBindBuffer(GLenum, GLuint);
extern void glBufferData(GLenum, GLsizeiptr, const GLvoid *, GLenum);

extern GLuint glCreateShader(GLenum);
extern void glShaderSource(GLuint, GLsizei, const GLchar* *, const GLint *);
extern void glCompileShader(GLuint);
extern void glGetShaderiv(GLuint, GLenum, GLint *);
extern void glGetShaderInfoLog(GLuint, GLsizei, GLsizei *, GLchar *);
extern void glDeleteProgram(GLuint);
extern void glDeleteShader(GLuint);
extern void glAttachShader(GLuint, GLuint);
extern void glBindAttribLocation(GLuint, GLuint, const GLchar *);
extern GLuint glCreateProgram(void);
extern void glLinkProgram(GLuint);
extern void glGetProgramiv(GLuint, GLenum, GLint *);
extern void glGetProgramInfoLog(GLuint, GLsizei, GLsizei *, GLchar *);
extern void glUseProgram(GLuint);

extern GLint glGetUniformLocation(GLuint, const GLchar *);
extern void glEnableVertexAttribArray(GLuint);
extern void glVertexAttribPointer(GLuint, GLint, GLenum, GLboolean, GLsizei, const GLvoid *);
extern void glDisableVertexAttribArray(GLuint);
extern void glUniformMatrix4fv(GLint, GLsizei, GLboolean, const GLfloat *);
extern void glUniform3fv(GLint, GLsizei, const GLfloat *);
extern void glUniform1f(GLint, GLfloat);

#endif
