#include "main.h"

static float lineWidth=1.0f;
static float vertices[26*4];

#ifdef __EMSCRIPTEN__
static bool LoadOpenGLFunctions(void){
    return true;
}
#else
void (*GL_ActiveTexture)(GLenum);
void (*GL_AttachShader)(GLuint,GLuint);
void (*GL_BindBuffer)(GLenum,GLuint);
void (*GL_BindVertexArray)(GLuint);
void (*GL_BufferData)(GLenum,GLsizeiptr,const void*,GLenum);
void (*GL_BufferSubData)(GLenum,GLintptr,GLsizeiptr,const void*);
//void (*GL_ClearDepthf)(GLfloat);
void (*GL_CompileShader)(GLuint);
GLuint (*GL_CreateProgram)(void);
GLuint (*GL_CreateShader)(GLenum);
void (*GL_DeleteBuffers)(GLsizei,const GLuint*);
void (*GL_DeleteProgram)(GLuint);
void (*GL_DeleteShader)(GLuint);
void (*GL_DeleteVertexArrays)(GLsizei,const GLuint*);
void (*GL_DetachShader)(GLuint,GLuint);
void (*GL_DrawArraysInstanced)(GLenum,GLint,GLsizei,GLsizei);
//void (*GL_DrawElementsInstanced)(GLenum,GLsizei,GLenum,const void*,GLsizei);
void (*GL_EnableVertexAttribArray)(GLuint);
void (*GL_GenBuffers)(GLsizei,GLuint*);
void (*GL_GenerateMipmap)(GLenum);
void (*GL_GenVertexArrays)(GLsizei,GLuint*);
//void (*GL_GetFramebufferAttachmentParameteriv)(GLenum,GLenum,GLenum,GLint*);
void (*GL_GetProgramInfoLog)(GLuint,GLsizei,GLsizei*,GLchar*);
void (*GL_GetProgramiv)(GLuint,GLenum,GLint*);
void (*GL_GetShaderInfoLog)(GLuint,GLsizei,GLsizei*,GLchar*);
void (*GL_GetShaderiv)(GLuint,GLenum,GLint*);
GLint (*GL_GetUniformLocation)(GLuint,const GLchar*);
void (*GL_LinkProgram)(GLuint);
void (*GL_ShaderSource)(GLuint,GLsizei,const GLchar*const*,const GLint*);
void (*GL_Uniform1f)(GLint,GLfloat);
void (*GL_Uniform1i)(GLint,GLint);
void (*GL_Uniform2f)(GLint,GLfloat,GLfloat);
void (*GL_Uniform2i)(GLint,GLint,GLint);
void (*GL_Uniform3f)(GLint,GLfloat,GLfloat,GLfloat);
void (*GL_Uniform4f)(GLint,GLfloat,GLfloat,GLfloat,GLfloat);
void (*GL_Uniform4fv)(GLint,GLsizei,const GLfloat*);
void (*GL_UniformMatrix3x2fv)(GLint,GLsizei,GLboolean,const GLfloat*);
void (*GL_UniformMatrix4fv)(GLint,GLsizei,GLboolean,const GLfloat*);
void (*GL_UseProgram)(GLuint);
void (*GL_VertexAttribDivisor)(GLuint,GLuint);
void (*GL_VertexAttribPointer)(GLuint,GLint,GLenum,GLboolean,GLsizei,const void*);

static bool LoadOpenGLFunctions(void){
    if(!(GL_ActiveTexture=(void(*)(GLenum))SDL_GL_GetProcAddress("glActiveTexture")))return false;
    if(!(GL_AttachShader=(void(*)(GLuint,GLuint))SDL_GL_GetProcAddress("glAttachShader")))return false;
    if(!(GL_BindBuffer=(void(*)(GLenum,GLuint))SDL_GL_GetProcAddress("glBindBuffer")))return false;
    if(!(GL_BindVertexArray=(void(*)(GLuint))SDL_GL_GetProcAddress("glBindVertexArray")))return false;
    if(!(GL_BufferData=(void(*)(GLenum,GLsizeiptr,const void*,GLenum))SDL_GL_GetProcAddress("glBufferData")))return false;
    if(!(GL_BufferSubData=(void(*)(GLenum,GLintptr,GLsizeiptr,const void*))SDL_GL_GetProcAddress("glBufferSubData")))return false;
//    if(!(GL_ClearDepthf=(void(*)(GLfloat))SDL_GL_GetProcAddress("glClearDepthf")))return false;
    if(!(GL_CompileShader=(void(*)(GLuint))SDL_GL_GetProcAddress("glCompileShader")))return false;
    if(!(GL_CreateProgram=(GLuint(*)(void))SDL_GL_GetProcAddress("glCreateProgram")))return false;
    if(!(GL_CreateShader=(GLuint(*)(GLenum))SDL_GL_GetProcAddress("glCreateShader")))return false;
    if(!(GL_DeleteBuffers=(void(*)(GLsizei,const GLuint*))SDL_GL_GetProcAddress("glDeleteBuffers")))return false;
    if(!(GL_DeleteProgram=(void(*)(GLuint))SDL_GL_GetProcAddress("glDeleteProgram")))return false;
    if(!(GL_DeleteShader=(void(*)(GLuint))SDL_GL_GetProcAddress("glDeleteShader")))return false;
    if(!(GL_DeleteVertexArrays=(void(*)(GLsizei,const GLuint*))SDL_GL_GetProcAddress("glDeleteVertexArrays")))return false;
    if(!(GL_DetachShader=(void(*)(GLuint,GLuint))SDL_GL_GetProcAddress("glDetachShader")))return false;
    if(!(GL_DrawArraysInstanced=(void(*)(GLenum,GLint,GLsizei,GLsizei))SDL_GL_GetProcAddress("glDrawArraysInstanced")))return false;
//    if(!(GL_DrawElementsInstanced=(void(*)(GLenum,GLsizei,GLenum,const void*,GLsizei))SDL_GL_GetProcAddress("glDrawElementsInstanced")))return false;
    if(!(GL_EnableVertexAttribArray=(void(*)(GLuint))SDL_GL_GetProcAddress("glEnableVertexAttribArray")))return false;
    if(!(GL_GenBuffers=(void(*)(GLsizei,GLuint*))SDL_GL_GetProcAddress("glGenBuffers")))return false;
    if(!(GL_GenerateMipmap=(void(*)(GLenum))SDL_GL_GetProcAddress("glGenerateMipmap")))return false;
    if(!(GL_GenVertexArrays=(void(*)(GLsizei,GLuint*))SDL_GL_GetProcAddress("glGenVertexArrays")))return false;
//    if(!(GL_GetFramebufferAttachmentParameteriv=(void(*)(GLenum,GLenum,GLenum,GLint*))SDL_GL_GetProcAddress("glGetFramebufferAttachmentParameteriv")))return false;
    if(!(GL_GetProgramInfoLog=(void(*)(GLuint,GLsizei,GLsizei*,GLchar*))SDL_GL_GetProcAddress("glGetProgramInfoLog")))return false;
    if(!(GL_GetProgramiv=(void(*)(GLuint,GLenum,GLint*))SDL_GL_GetProcAddress("glGetProgramiv")))return false;
    if(!(GL_GetShaderInfoLog=(void(*)(GLuint,GLsizei,GLsizei*,GLchar*))SDL_GL_GetProcAddress("glGetShaderInfoLog")))return false;
    if(!(GL_GetShaderiv=(void(*)(GLuint,GLenum,GLint*))SDL_GL_GetProcAddress("glGetShaderiv")))return false;
    if(!(GL_GetUniformLocation=(GLint(*)(GLuint,const GLchar*))SDL_GL_GetProcAddress("glGetUniformLocation")))return false;
    if(!(GL_LinkProgram=(void(*)(GLuint))SDL_GL_GetProcAddress("glLinkProgram")))return false;
    if(!(GL_ShaderSource=(void(*)(GLuint,GLsizei,const GLchar*const*,const GLint*))SDL_GL_GetProcAddress("glShaderSource")))return false;
    if(!(GL_Uniform1f=(void(*)(GLint,GLfloat))SDL_GL_GetProcAddress("glUniform1f")))return false;
    if(!(GL_Uniform1i=(void(*)(GLint,GLint))SDL_GL_GetProcAddress("glUniform1i")))return false;
    if(!(GL_Uniform2f=(void(*)(GLint,GLfloat,GLfloat))SDL_GL_GetProcAddress("glUniform2f")))return false;
    if(!(GL_Uniform2i=(void(*)(GLint,GLint,GLint))SDL_GL_GetProcAddress("glUniform2i")))return false;
    if(!(GL_Uniform3f=(void(*)(GLint,GLfloat,GLfloat,GLfloat))SDL_GL_GetProcAddress("glUniform3f")))return false;
    if(!(GL_Uniform4f=(void(*)(GLint,GLfloat,GLfloat,GLfloat,GLfloat))SDL_GL_GetProcAddress("glUniform4f")))return false;
    if(!(GL_Uniform4fv=(void(*)(GLint,GLsizei,const GLfloat*))SDL_GL_GetProcAddress("glUniform4fv")))return false;
    if(!(GL_UniformMatrix3x2fv=(void(*)(GLint,GLsizei,GLboolean,const GLfloat*))SDL_GL_GetProcAddress("glUniformMatrix3x2fv")))return false;
    if(!(GL_UniformMatrix4fv=(void(*)(GLint,GLsizei,GLboolean,const GLfloat*))SDL_GL_GetProcAddress("glUniformMatrix4fv")))return false;
    if(!(GL_UseProgram=(void(*)(GLuint))SDL_GL_GetProcAddress("glUseProgram")))return false;
    if(!(GL_VertexAttribDivisor=(void(*)(GLuint,GLuint))SDL_GL_GetProcAddress("glVertexAttribDivisor")))return false;
    if(!(GL_VertexAttribPointer=(void(*)(GLuint,GLint,GLenum,GLboolean,GLsizei,const void*))SDL_GL_GetProcAddress("glVertexAttribPointer")))return false;
    return true;
}
#endif

float ToWorldX(WindowData *win,int x){
    return 2.0f*x/win->w-1.0f;
}

float ToWorldY(WindowData *win,int y){
    return 2.0f*y/win->h-1.0f;
}

float ToScreenX1(WindowData *win,float x){
    return (x+1.0f)*win->w*(1.0f+separator)*0.25f;
}

float ToScreenX2(WindowData *win,float x){
    return ((1.0f-x)*(1.0f+separator)*0.5f+x+1.0f)*win->w*0.5f;
}

float ToScreenY(WindowData *win,float y){
    return (y+1.0f)*win->h*0.5f;
}

float DiscretizeX(WindowData *win,float x){
    return ToWorldX(win,round((x+1.0f)*win->w*0.5f));
}

float DiscretizeY(WindowData *win,float y){
    return ToWorldY(win,round((y+1.0f)*win->h*0.5f));
}

float SeparatorX(void){
    return win1->w*(1.0f+separator)*0.5f;
}

WindowData *NewWindow(const char *title,void(*renderFunction)(struct WindowData*)){
    WindowData *win=malloc(sizeof(WindowData));
    Uint32 winFlags=SDL_WINDOW_OPENGL|SDL_WINDOW_HIGH_PIXEL_DENSITY|SDL_WINDOW_RESIZABLE|SDL_WINDOW_MAXIMIZED;
    #ifdef __EMSCRIPTEN__
    emscripten_get_canvas_element_size("#canvas",&win->w,&win->h);
    #else
    int winSize=100;
    win->w=winSize; win->h=winSize;
    #endif
    if(!(win->window=SDL_CreateWindow(title,win->w,win->h,winFlags)))Exit("Unable to create window.");
    if(!(win->context=SDL_GL_CreateContext(win->window)))Exit("Unable to create OpenGL context.");
    if(!LoadOpenGLFunctions())Exit("Unable to load OpenGL functions.");
    win->renderFunction=renderFunction;
    SDL_GetWindowSize(win->window,&win->w,&win->h);
    SetWindowViewport(win);
    InitShader();

    glClearColor(1.0f,1.0f,1.0f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    SDL_GL_SwapWindow(win->window);

    win->invalid=true;
    return win;
}

void FreeWindow(WindowData *win){
    QuitShader();
    SDL_GL_DestroyContext(win->context);
    SDL_DestroyWindow(win->window);
}

void SetWindowContext(WindowData *win){
    SDL_GL_MakeCurrent(win->window,win->context);
}

void SetWindowViewport(WindowData *win){
    SetWindowContext(win);
    #ifdef __EMSCRIPTEN__
    emscripten_get_canvas_element_size("#canvas",&win->w,&win->h);
    #endif
    glViewport(0,0,win->w,win->h);
    win->aspect=(float)win->w/win->h;
    win->aspect2=(float)win->h/win->w;
    int win_w,win_h,draw_w,draw_h;
    SDL_GetWindowSize(win->window,&win_w,&win_h);
    SDL_GetWindowSizeInPixels(win->window,&draw_w,&draw_h);
    win->sx=(float)draw_w/win_w;
    win->sy=(float)draw_h/win_h;
}

void RenderWindow(WindowData *win){
    if(!win->invalid)return;
    SetWindowViewport(win);
    glClearColor(1.0f,1.0f,1.0f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    GL_ActiveTexture(GL_TEXTURE0);
    win->renderFunction(win);
    SDL_GL_SwapWindow(win->window);
    win->invalid=false;
}

void SetColor(int r,int g,int b){
    GL_Uniform4f(unifLineColor,r/255.0f,g/255.0f,b/255.0f,1.0f);
}

void SetWidth(float line){
    lineWidth=line;
}

void PrepareRenderLine(void){
    glDisable(GL_BLEND);
    GL_BindVertexArray(lineVAO);
    GL_BindBuffer(GL_ARRAY_BUFFER,lineVBO);
    GL_UseProgram(progLine);
}

void DrawLine(WindowData *win,float x1,float y1,float x2,float y2,bool discretize){
    if(discretize){
        x1=DiscretizeX(win,x1); y1=DiscretizeY(win,y1);
        x2=DiscretizeX(win,x2); y2=DiscretizeY(win,y2);
    }
    float dx=x2-x1,dy=y2-y1;
    float d=dx*dx+dy*dy;
    if(d<EPSILON)return;
    d=lineWidth/sqrtf(d);
    dx=dx*d/win->h;
    dy=dy*d/win->w;
    vertices[0]=x1-dy; vertices[1]=-y1-dx;
    vertices[2]=x1+dy; vertices[3]=-y1+dx;
    vertices[4]=x2-dy; vertices[5]=-y2-dx;
    vertices[6]=x2+dy; vertices[7]=-y2+dx;
    GL_BufferSubData(GL_ARRAY_BUFFER,0,sizeof(float)*8,vertices);
    glDrawArrays(GL_TRIANGLE_STRIP,0,4);
}

static float Patch(float x){
    return (1.0f+x)*(separator-1.0f)*0.5f+x;
}

void DrawArrow(WindowData *win,float x1,float y1,float x2,float y2,bool arrow,bool margin,bool bothWays,bool discretize){
    if(discretize){
        x1=DiscretizeX(win,x1); y1=DiscretizeY(win,y1);
        x2=DiscretizeX(win,x2); y2=DiscretizeY(win,y2);
    }
    float sx1=ToScreenX1(win,x1),sy1=ToScreenY(win,y1);
    float sx2=ToScreenX1(win,x2),sy2=ToScreenY(win,y2);
    float d1=sqrt((sx2-sx1)*(sx2-sx1)+(sy2-sy1)*(sy2-sy1));
    float d2=margin?d1-NODE_SIZE*0.35f:d1;
    if(d2<=0.0f)return;
    float d3=d2-ARROW_HEAD_LENGTH;
    if(d3<0.0f)d3=0.0f;
    float nx=(sx2-sx1)/d1,ny=(sy2-sy1)/d1;
    float sx3=sx1+nx*d2,sy3=sy1+ny*d2;
    float sx4=sx1+nx*d3,sy4=sy1+ny*d3;
    float d4=ARROW_HEAD_WIDTH*0.5f;
    float sx5=sx4+ny*d4,sy5=sy4-nx*d4;
    float sx6=sx4+sx4-sx5,sy6=sy4+sy4-sy5;
    float x3=ToWorldX(win,sx3),y3=ToWorldY(win,sy3);
    float x5=ToWorldX(win,sx5),y5=ToWorldY(win,sy5);
    float x6=ToWorldX(win,sx6),y6=ToWorldY(win,sy6);
    float d5=(d2+d3)*0.5f;
    float x7=ToWorldX(win,sx1+nx*d5),y7=ToWorldY(win,sy1+ny*d5);
    if(margin||!arrow)DrawLine(win,Patch(x1),y1,Patch(x2),y2,false);
    else DrawLine(win,Patch(x1),y1,x7,y7,false);
    if(!arrow)return;
    vertices[0]=x3; vertices[1]=-y3;
    vertices[2]=x5; vertices[3]=-y5;
    vertices[4]=x6; vertices[5]=-y6;
    d2=d1-NODE_SIZE*0.35f;
    if(!bothWays || d2<=0.0f){
        GL_BufferSubData(GL_ARRAY_BUFFER,0,sizeof(float)*6,vertices);
        glDrawArrays(GL_TRIANGLES,0,3);
        return;
    }
    d3=d2-ARROW_HEAD_LENGTH;
    if(d3<0.0f)d3=0.0f;
    nx=-nx; ny=-ny;
    sx3=sx2+nx*d2; sy3=sy2+ny*d2;
    sx4=sx2+nx*d3; sy4=sy2+ny*d3;
    sx5=sx4+ny*d4; sy5=sy4-nx*d4;
    sx6=sx4+sx4-sx5; sy6=sy4+sy4-sy5;
    vertices[6]=ToWorldX(win,sx3); vertices[7]=-ToWorldY(win,sy3);
    vertices[8]=ToWorldX(win,sx5); vertices[9]=-ToWorldY(win,sy5);
    vertices[10]=ToWorldX(win,sx6); vertices[11]=-ToWorldY(win,sy6);
    GL_BufferSubData(GL_ARRAY_BUFFER,0,sizeof(float)*12,vertices);
    glDrawArrays(GL_TRIANGLES,0,6);
}

void DrawSelfArrow(WindowData *win,float x,float y,float rx,float ry,float ofs,bool arrow,bool discretize){
    if(discretize){
        x=DiscretizeX(win,x);
        y=DiscretizeY(win,y);
    }
    float dist=sqrt(x*x+y*y);
    float cx,cy;
    if(dist>0.0f){ cx=x/dist; cy=y/dist; }
    else{ cx=0.0f; cy=-1.0f; }
    float angle=atan2(cy,cx);
    rx*=SELF_LOOP_RADIUS;
    ry*=SELF_LOOP_RADIUS;
    if(ofs>0.0f || ofs<0.0f){
        angle+=ofs;
        cx=cosf(angle);
        cy=sinf(angle);
    }
    x=Patch(x);
    cx*=rx; cy*=ry;
    x+=cx; y+=cy;
    int sides=24;
    int n=arrow?19:sides;
    float dx=lineWidth/win->w;
    float dy=lineWidth/win->h;
    for(int i=0;i<=n;i++){
        float a=angle+i*2.0f*M_PI/sides;
        vertices[4*i+0]=x-(rx+dx)*cosf(a);
        vertices[4*i+1]=-y+(ry+dy)*sinf(a);
        vertices[4*i+2]=x-(rx-dx)*cosf(a);
        vertices[4*i+3]=-y+(ry-dy)*sinf(a);
    }
    GL_BufferSubData(GL_ARRAY_BUFFER,0,sizeof(float)*(4*n+4),vertices);
    glDrawArrays(GL_TRIANGLE_STRIP,0,2*n+2);
    if(!arrow)return;
    float a=angle+1.8f*M_PI;
    vertices[0]=x-rx*cosf(a);
    vertices[1]=-y+ry*sinf(a);
    a-=0.55f*M_PI;
    float b=a+0.16f*M_PI;
    float r=0.7f;
    vertices[2]=x-rx*r*cosf(b);
    vertices[3]=-y+ry*r*sinf(b);
    b=a+0.25f*M_PI;
    r=1.325f;
    vertices[4]=x-rx*r*cosf(b);
    vertices[5]=-y+ry*r*sinf(b);
    GL_BufferSubData(GL_ARRAY_BUFFER,0,sizeof(float)*6,vertices);
    glDrawArrays(GL_TRIANGLES,0,3);
}

void DrawRectangle(WindowData *win,float x,float y,float rx,float ry,bool fill,int border,bool discretize){
    if(discretize){
        x=DiscretizeX(win,x);
        y=DiscretizeY(win,y);
    }
    if(fill){
        vertices[0]=x-rx; vertices[1]=-y+ry;
        vertices[2]=x+rx; vertices[3]=-y+ry;
        vertices[4]=x-rx; vertices[5]=-y-ry;
        vertices[6]=x+rx; vertices[7]=-y-ry;
        GL_BufferSubData(GL_ARRAY_BUFFER,0,sizeof(float)*8,vertices);
        glDrawArrays(GL_TRIANGLE_STRIP,0,4);
    }
    if(!border)return;
    if(border==1)SetColor(128,128,128);
    else SetColor(0,0,0);
    float dx=lineWidth/win->w;
    float dy=lineWidth/win->h;
    vertices[0]=x-rx-dx; vertices[1]=-y+ry+dy;
    vertices[2]=x-rx+dx; vertices[3]=-y+ry-dy;
    vertices[4]=x+rx+dx; vertices[5]=-y+ry+dy;
    vertices[6]=x+rx-dx; vertices[7]=-y+ry-dy;
    vertices[8]=x+rx+dx; vertices[9]=-y-ry-dy;
    vertices[10]=x+rx-dx; vertices[11]=-y-ry+dy;
    vertices[12]=x-rx-dx; vertices[13]=-y-ry-dy;
    vertices[14]=x-rx+dx; vertices[15]=-y-ry+dy;
    vertices[16]=vertices[0]; vertices[17]=vertices[1];
    vertices[18]=vertices[2]; vertices[19]=vertices[3];
    GL_BufferSubData(GL_ARRAY_BUFFER,0,sizeof(float)*20,vertices);
    glDrawArrays(GL_TRIANGLE_STRIP,0,10);
}

void DrawPolygon(WindowData *win,int sides,float x,float y,float rx,float ry,bool fill,int border,bool discretize){
    if(discretize){
        x=DiscretizeX(win,x);
        y=DiscretizeY(win,y);
    }
    if(fill){
        vertices[0]=x;
        vertices[1]=-y;
        for(int i=0;i<=sides;i++){
            float a=i*2.0f*M_PI/sides;
            vertices[2+i*2]=x+rx*sinf(a);
            vertices[2+i*2+1]=-y+ry*cosf(a);
        }
        GL_BufferSubData(GL_ARRAY_BUFFER,0,sizeof(float)*(sides+2)*2,vertices);
        glDrawArrays(GL_TRIANGLE_FAN,0,sides+2);
    }
    if(!border)return;
    if(border==1)SetColor(128,128,128);
    else if(border==2)SetColor(0,0,0);
    float dx=lineWidth/win->w;
    float dy=lineWidth/win->h;
    for(int i=0;i<sides;i++){
        float a=i*2.0f*M_PI/sides;
        vertices[4*i+0]=x+(rx+dx)*sinf(a);
        vertices[4*i+1]=-y+(ry+dy)*cosf(a);
        vertices[4*i+2]=x+(rx-dx)*sinf(a);
        vertices[4*i+3]=-y+(ry-dy)*cosf(a);
    }
    vertices[4*sides+0]=vertices[0];
    vertices[4*sides+1]=vertices[1];
    vertices[4*sides+2]=vertices[2];
    vertices[4*sides+3]=vertices[3];
    GL_BufferSubData(GL_ARRAY_BUFFER,0,sizeof(float)*(4*sides+4),vertices);
    glDrawArrays(GL_TRIANGLE_STRIP,0,2*sides+2);
}

void DrawEllipse(WindowData *win,float x,float y,float rx,float ry,bool fill,int border,bool discretize){
    DrawPolygon(win,24,x,y,rx,ry,fill,border,discretize);
}

void DrawLabel(WindowData *win,float x1,float y1,float x2,float y2,int label,float position,float rx,float ry,bool dark,bool col,bool discretize){
    float x3=x1+(x2-x1)*position;
    float y3=y1+(y2-y1)*position;
    if(discretize){
        x3=DiscretizeX(win,x3);
        y3=DiscretizeY(win,y3);
    }
    if(col)SetColor(255,220,220);
    else SetColor(224,224,224);
    DrawEllipse(win,x3,y3,rx,ry,true,dark?2:1,discretize);
    PrepareRenderText();
    if(dark){
        SetFontOutlineColor(0.0f,0.0f,0.0f);
        SetFontColor(0.0f,0.0f,0.0f,1.0f);
    }
    else{
        SetFontOutlineColor(0.5f,0.5f,0.5f);
        SetFontColor(0.5f,0.5f,0.5f,1.0f);
    }
    if(label==1||(label>=10&&label<20))PrintString(x3*win1->aspect+rx*LABEL_OFS_X1,y3+ry*LABEL_OFS_Y,true,"%d",label);
    else PrintString(x3*win1->aspect+rx*LABEL_OFS_X,y3+ry*LABEL_OFS_Y,true,"%d",label);
    PrepareRenderLine();
}
