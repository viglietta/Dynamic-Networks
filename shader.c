#include "main.h"

GLuint lineVAO,lineVBO;
GLuint progLine,progText;
GLint unifLineColor;
GLint unifTextPosition,unifTextScale,unifTextTexInvSize,unifTextFaceColor,unifTextOutlineColor,unifTextThick,unifTextAspect;
static char infoLog[512];

static const GLchar *vertSourceLine=
"#version 300 es\n\
layout(location=0) in vec2 inPos;\n\
void main(){\n\
    gl_Position=vec4(inPos,0.0,1.0);\n\
}\n";

static const GLchar *fragSourceLine=
"#version 300 es\n\
precision mediump float;\n\
out vec4 outColor;\n\
uniform vec4 color;\n\
void main(){\n\
    outColor=color;\n\
}\n";

static const GLchar *vertSourceText=
"#version 300 es\n\
layout(location=0) in vec2 inPos;\n\
layout(location=1) in vec2 inTex;\n\
out vec2 midTexCoord;\n\
uniform vec2 position;\n\
uniform float scale;\n\
uniform float aspect;\n\
uniform vec2 texInvSize;\n\
void main(){\n\
    midTexCoord=inTex*texInvSize;\n\
    vec2 transPos=inPos*scale+position;\n\
    gl_Position=vec4(transPos*vec2(aspect,-1.0),0.0,1.0);\n\
}\n";

static const GLchar *fragSourceText=
"#version 300 es\n\
precision mediump float;\n\
in vec2 midTexCoord;\n\
out vec4 outColor;\n\
uniform sampler2D tex;\n\
uniform vec4 faceColor;\n\
uniform vec3 outlineColor;\n\
uniform vec3 thick;\n\
void main(){\n\
    float distance=1.0-texture(tex,midTexCoord).a;\n\
    float edge=thick.z*0.5;\n\
    float gradient1=1.0-smoothstep(thick.x-edge,thick.x+edge,distance);\n\
    float gradient2=1.0-smoothstep(thick.y-edge,thick.y+edge,distance);\n\
    outColor=vec4(mix(outlineColor,faceColor.rgb,gradient1),faceColor.a*gradient2);\n\
}\n";

static GLuint NewShader(GLenum type,const GLchar *source){
    GLuint shader=GL_CreateShader(type);
    GL_ShaderSource(shader,1,&source,NULL);
    GL_CompileShader(shader);
    GLint success;
    GL_GetShaderiv(shader,GL_COMPILE_STATUS,&success);
    if(!success){
        GL_GetShaderInfoLog(shader,sizeof(infoLog),NULL,infoLog);
        Exit(infoLog);
    }
    return shader;
}

static GLuint NewProgram(GLuint vertex,GLuint fragment){
    GLuint program=GL_CreateProgram();
    GL_AttachShader(program,vertex);
    GL_AttachShader(program,fragment);
    GL_LinkProgram(program);
    GLint success;
    GL_GetProgramiv(program,GL_LINK_STATUS,&success);
    if(!success){
        GL_GetProgramInfoLog(program,sizeof(infoLog),NULL,infoLog);
        Exit(infoLog);
    }
    GL_DetachShader(program,vertex);
    GL_DetachShader(program,fragment);
    return program;
}

void InitShader(void){
    GL_GenVertexArrays(1,&lineVAO);
    GL_GenBuffers(1,&lineVBO);
    GL_BindVertexArray(lineVAO);
    GL_BindBuffer(GL_ARRAY_BUFFER,lineVBO);
    GL_BufferData(GL_ARRAY_BUFFER,26*4*sizeof(GLfloat),NULL,GL_DYNAMIC_DRAW);
    GL_VertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,2*sizeof(GLfloat),NULL);
    GL_EnableVertexAttribArray(0);
    GL_BindVertexArray(0);

    GLuint vertShaderLine=NewShader(GL_VERTEX_SHADER,vertSourceLine);
    GLuint fragShaderLine=NewShader(GL_FRAGMENT_SHADER,fragSourceLine);

    progLine=NewProgram(vertShaderLine,fragShaderLine);
    GL_UseProgram(progLine);
    unifLineColor=GL_GetUniformLocation(progLine,"color");

    GL_DeleteShader(vertShaderLine);
    GL_DeleteShader(fragShaderLine);

    GLuint vertShaderText=NewShader(GL_VERTEX_SHADER,vertSourceText);
    GLuint fragShaderText=NewShader(GL_FRAGMENT_SHADER,fragSourceText);

    progText=NewProgram(vertShaderText,fragShaderText);
    GL_UseProgram(progText);
    unifTextPosition=GL_GetUniformLocation(progText,"position");
    unifTextScale=GL_GetUniformLocation(progText,"scale");
    unifTextTexInvSize=GL_GetUniformLocation(progText,"texInvSize");
    unifTextFaceColor=GL_GetUniformLocation(progText,"faceColor");
    unifTextOutlineColor=GL_GetUniformLocation(progText,"outlineColor");
    unifTextThick=GL_GetUniformLocation(progText,"thick");
    unifTextAspect=GL_GetUniformLocation(progText,"aspect");
    GL_Uniform1i(GL_GetUniformLocation(progText,"tex"),0);

    GL_DeleteShader(vertShaderText);
    GL_DeleteShader(fragShaderText);
}

void QuitShader(void){
    GL_DeleteBuffers(1,&lineVBO);
    GL_DeleteVertexArrays(1,&lineVAO);
    GL_DeleteProgram(progLine);
    GL_DeleteProgram(progText);
}
