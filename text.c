#include "main.h"

static GLuint textVAO,textVBO,textEBO;
static GLfloat *textVertices;
static int minX,maxX,numTri;
static char stringBuffer[MAX_STRING_LENGTH+1];

static int MinInt(int a,int b){
    return a<b?a:b;
}

static int MaxInt(int a,int b){
    return a>b?a:b;
}

void InitText(void){
    GLuint *textIndices=malloc(MAX_STRING_LENGTH*6*sizeof(GLuint));
    for(int i=0,j=0;i<MAX_STRING_LENGTH*6;i+=6){
        textIndices[i]=textIndices[i+5]=j++;
        textIndices[i+1]=j++;
        textIndices[i+2]=textIndices[i+3]=j++;
        textIndices[i+4]=j++;
    }
    GL_GenVertexArrays(1,&textVAO);
    GL_BindVertexArray(textVAO);
    GL_GenBuffers(1,&textVBO);
    GL_BindBuffer(GL_ARRAY_BUFFER,textVBO);
    GL_BufferData(GL_ARRAY_BUFFER,MAX_STRING_LENGTH*16*sizeof(GLfloat),NULL,GL_STREAM_DRAW);
    GL_GenBuffers(1,&textEBO);
    GL_BindBuffer(GL_ELEMENT_ARRAY_BUFFER,textEBO);
    GL_BufferData(GL_ELEMENT_ARRAY_BUFFER,MAX_STRING_LENGTH*6*sizeof(GLuint),textIndices,GL_STATIC_DRAW);
    GL_VertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,4*sizeof(GLfloat),NULL);
    GL_EnableVertexAttribArray(0);
    GL_VertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,4*sizeof(GLfloat),(void*)(2*sizeof(GLfloat)));
    GL_EnableVertexAttribArray(1);
    GL_BindVertexArray(0);
    free(textIndices);
    textVertices=malloc(MAX_STRING_LENGTH*16*sizeof(GLfloat));
    InitFont();
}

void QuitText(void){
    QuitFont();
    free(textVertices);
    GL_DeleteBuffers(1,&textEBO);
    GL_DeleteBuffers(1,&textVBO);
    GL_DeleteVertexArrays(1,&textVAO);
}

void PrepareRenderText(void){
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    GL_BindVertexArray(textVAO);
    GL_BindBuffer(GL_ARRAY_BUFFER,textVBO);
    GL_UseProgram(progText);
}

static void CreateText(void){
    char *s=stringBuffer;
    int j=0,x=0,n=MAX_STRING_LENGTH*16;
    char c,cc=-1;
    minX=maxX=0;
    for(;(c=*s);s++){
        if(c<32)continue;
        c-=32;
        if(boundFont->kerning && cc>=0)x+=boundFont->kerning[cc*96+c];
        Glyph *g=boundFont->glyph+c;
        if(g->w && g->h){
            textVertices[j]=textVertices[j+4]=x+g->ofsX;
            textVertices[j+1]=textVertices[j+13]=g->ofsY;
            textVertices[j+2]=textVertices[j+6]=g->x;
            textVertices[j+3]=textVertices[j+15]=g->y;
            textVertices[j+5]=textVertices[j+9]=g->ofsY+g->h;
            textVertices[j+7]=textVertices[j+11]=g->y+g->h;
            textVertices[j+8]=textVertices[j+12]=x+g->ofsX+g->w;
            textVertices[j+10]=textVertices[j+14]=g->x+g->w;
            if(g->w>0){
                minX=MinInt(minX,textVertices[j]);
                maxX=MaxInt(maxX,textVertices[j+8]);
            }
            else{
                minX=MinInt(minX,textVertices[j+8]);
                maxX=MaxInt(maxX,textVertices[j]);
            }
            if((j+=16)==n)break;
        }
        x+=g->advX;
        cc=c;
    }
    if((numTri=j*3/8))GL_BufferSubData(GL_ARRAY_BUFFER,0,j*sizeof(GLfloat),textVertices);
}

void PrintString(float x,float y,bool centered,const char *format,...){
    va_list args;
    va_start(args,format);
    vsprintf(stringBuffer,format,args);
    va_end(args);
    CreateText();
    if(numTri){
        float tx=centered?x+(minX-maxX)*fontScale*0.5f:x-minX*fontScale;
        float ty=centered?y-boundFont->base*fontScale*0.5f:y;
        GL_Uniform2f(unifTextPosition,tx,ty);
        glDrawElements(GL_TRIANGLES,numTri,GL_UNSIGNED_INT,0);
    }
}
