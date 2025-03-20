typedef struct WindowData{
    SDL_Window *window;
    SDL_GLContext context;
    void(*renderFunction)(struct WindowData*);
    int w,h;
    float sx,sy;
    float aspect,aspect2;
    bool invalid;
}WindowData;

#ifdef __EMSCRIPTEN__
#define GL_ActiveTexture glActiveTexture
#define GL_AttachShader glAttachShader
#define GL_BindBuffer glBindBuffer
#define GL_BindVertexArray glBindVertexArray
#define GL_BufferData glBufferData
#define GL_BufferSubData glBufferSubData
//#define GL_ClearDepthf glClearDepthf
#define GL_CompileShader glCompileShader
#define GL_CreateProgram glCreateProgram
#define GL_CreateShader glCreateShader
#define GL_DeleteBuffers glDeleteBuffers
#define GL_DeleteProgram glDeleteProgram
#define GL_DeleteShader glDeleteShader
#define GL_DeleteVertexArrays glDeleteVertexArrays
#define GL_DetachShader glDetachShader
#define GL_DrawArraysInstanced glDrawArraysInstanced
//#define GL_DrawElementsInstanced glDrawElementsInstanced
#define GL_EnableVertexAttribArray glEnableVertexAttribArray
#define GL_GenBuffers glGenBuffers
#define GL_GenerateMipmap glGenerateMipmap
#define GL_GenVertexArrays glGenVertexArrays
//#define GL_GetFramebufferAttachmentParameteriv glGetFramebufferAttachmentParameteriv
#define GL_GetProgramInfoLog glGetProgramInfoLog
#define GL_GetProgramiv glGetProgramiv
#define GL_GetShaderInfoLog glGetShaderInfoLog
#define GL_GetShaderiv glGetShaderiv
#define GL_GetUniformLocation glGetUniformLocation
#define GL_LinkProgram glLinkProgram
#define GL_ShaderSource glShaderSource
#define GL_Uniform1f glUniform1f
#define GL_Uniform1i glUniform1i
#define GL_Uniform2f glUniform2f
#define GL_Uniform2i glUniform2i
#define GL_Uniform3f glUniform3f
#define GL_Uniform4f glUniform4f
#define GL_Uniform4fv glUniform4fv
#define GL_UniformMatrix3x2fv glUniformMatrix3x2fv
#define GL_UniformMatrix4fv glUniformMatrix4fv
#define GL_UseProgram glUseProgram
#define GL_VertexAttribDivisor glVertexAttribDivisor
#define GL_VertexAttribPointer glVertexAttribPointer
#else
extern void (*GL_ActiveTexture)(GLenum);
extern void (*GL_AttachShader)(GLuint,GLuint);
extern void (*GL_BindBuffer)(GLenum,GLuint);
extern void (*GL_BindVertexArray)(GLuint);
extern void (*GL_BufferData)(GLenum,GLsizeiptr,const void*,GLenum);
extern void (*GL_BufferSubData)(GLenum,GLintptr,GLsizeiptr,const void*);
//extern void (*GL_ClearDepthf)(GLfloat);
extern void (*GL_CompileShader)(GLuint);
extern GLuint (*GL_CreateProgram)(void);
extern GLuint (*GL_CreateShader)(GLenum);
extern void (*GL_DeleteBuffers)(GLsizei,const GLuint*);
extern void (*GL_DeleteProgram)(GLuint);
extern void (*GL_DeleteShader)(GLuint);
extern void (*GL_DeleteVertexArrays)(GLsizei,const GLuint*);
extern void (*GL_DetachShader)(GLuint,GLuint);
extern void (*GL_DrawArraysInstanced)(GLenum,GLint,GLsizei,GLsizei);
//extern void (*GL_DrawElementsInstanced)(GLenum,GLsizei,GLenum,const void*,GLsizei);
extern void (*GL_EnableVertexAttribArray)(GLuint);
extern void (*GL_GenBuffers)(GLsizei,GLuint*);
extern void (*GL_GenerateMipmap)(GLenum);
extern void (*GL_GenVertexArrays)(GLsizei,GLuint*);
//extern void (*GL_GetFramebufferAttachmentParameteriv)(GLenum,GLenum,GLenum,GLint*);
extern void (*GL_GetProgramInfoLog)(GLuint,GLsizei,GLsizei*,GLchar*);
extern void (*GL_GetProgramiv)(GLuint,GLenum,GLint*);
extern void (*GL_GetShaderInfoLog)(GLuint,GLsizei,GLsizei*,GLchar*);
extern void (*GL_GetShaderiv)(GLuint,GLenum,GLint*);
extern GLint (*GL_GetUniformLocation)(GLuint,const GLchar*);
extern void (*GL_LinkProgram)(GLuint);
extern void (*GL_ShaderSource)(GLuint,GLsizei,const GLchar*const*,const GLint*);
extern void (*GL_Uniform1f)(GLint,GLfloat);
extern void (*GL_Uniform1i)(GLint,GLint);
extern void (*GL_Uniform2f)(GLint,GLfloat,GLfloat);
extern void (*GL_Uniform2i)(GLint,GLint,GLint);
extern void (*GL_Uniform3f)(GLint,GLfloat,GLfloat,GLfloat);
extern void (*GL_Uniform4f)(GLint,GLfloat,GLfloat,GLfloat,GLfloat);
extern void (*GL_Uniform4fv)(GLint,GLsizei,const GLfloat*);
extern void (*GL_UniformMatrix3x2fv)(GLint,GLsizei,GLboolean,const GLfloat*);
extern void (*GL_UniformMatrix4fv)(GLint,GLsizei,GLboolean,const GLfloat*);
extern void (*GL_UseProgram)(GLuint);
extern void (*GL_VertexAttribDivisor)(GLuint,GLuint);
extern void (*GL_VertexAttribPointer)(GLuint,GLint,GLenum,GLboolean,GLsizei,const void*);
#endif

float ToWorldX(WindowData *win,int x);
float ToWorldY(WindowData *win,int y);
float ToScreenX1(WindowData *win,float x);
float ToScreenX2(WindowData *win,float x);
float ToScreenY(WindowData *win,float y);
float DiscretizeX(WindowData *win,float x);
float DiscretizeY(WindowData *win,float y);
float SeparatorX(void);
WindowData *NewWindow(const char *title,void(*renderFunction)(struct WindowData*));
void FreeWindow(WindowData *win);
void SetWindowContext(WindowData *win);
void SetWindowViewport(WindowData *win);
void RenderWindow(WindowData *win);
void SetColor(int r,int g,int b);
void SetWidth(float line);
void PrepareRenderLine(void);
void DrawLine(WindowData *win,float x1,float y1,float x2,float y2,bool discretize);
void DrawArrow(WindowData *win,float x1,float y1,float x2,float y2,bool arrow,bool margin,bool bothWays,bool discretize);
void DrawSelfArrow(WindowData *win,float x,float y,float rx,float ry,float ofs,bool arrow,bool discretize);
void DrawRectangle(WindowData *win,float x,float y,float rx,float ry,bool fill,int border,bool discretize);
void DrawPolygon(WindowData *win,int sides,float x,float y,float rx,float ry,bool fill,int border,bool discretize);
void DrawEllipse(WindowData *win,float x,float y,float rx,float ry,bool fill,int border,bool discretize);
void DrawLabel(WindowData *win,float x1,float y1,float x2,float y2,int label,float position,float rx,float ry,bool dark,bool col,bool discretize);
