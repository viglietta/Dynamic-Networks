#include "main.h"

TTF_Font *font;

float ToWorldX(WindowData *win,int x){
    return (2.0f*x+1.0f)/win->w-1.0f;
}

float ToWorldY(WindowData *win,int y){
    return (2.0f*y+1.0f)/win->h-1.0f;
}

float ToScreenX(WindowData *win,float x){
    return (x+1.0f)*win->w*0.5f-0.5f;
}

float ToScreenY(WindowData *win,float y){
    return (y+1.0f)*win->h*0.5f-0.5f;
}

float DiscretizeX(WindowData *win,float x){
    return ToWorldX(win,round((x+1.0f)*(win->w-1)*0.5f));
}

float DiscretizeY(WindowData *win,float y){
    return ToWorldY(win,round((y+1.0f)*(win->h-1)*0.5f));
}

WindowData *NewWindow(const char *title,int x,int y,int w,int h,void(*renderFunction)(struct WindowData*)){
    WindowData *win=malloc(sizeof(WindowData));
    win->window=SDL_CreateWindow(title,x,y,w,h,SDL_WINDOW_RESIZABLE|SDL_WINDOW_OPENGL);
    win->context=SDL_GL_CreateContext(win->window);
    win->renderFunction=renderFunction;
    SDL_GetWindowSize(win->window,&win->w,&win->h);
    SetWindowViewport(win);
    glClearColor(1.0f,1.0f,1.0f,1.0f);
    win->invalid=true;
    return win;
}

void FreeWindow(WindowData *win){
    SDL_GL_DeleteContext(win->context);
    SDL_DestroyWindow(win->window);
}

void SetWindowContext(WindowData *win){
    SDL_GL_MakeCurrent(win->window,win->context);
}

void SetWindowViewport(WindowData *win){
    SetWindowContext(win);
    glViewport(0,0,win->w,win->h);
}

void RenderWindow(WindowData *win){
    if(!win->invalid)return;
    SetWindowContext(win);
    glClear(GL_COLOR_BUFFER_BIT);
    win->renderFunction(win);
    SDL_GL_SwapWindow(win->window);
    win->invalid=false;
}

Texture *NewTexture(SDL_Surface *surface){
    Texture *texture=malloc(sizeof(Texture));
    SDL_Surface *newSurface=SDL_ConvertSurfaceFormat(surface,SDL_PIXELFORMAT_RGBA32,0);
    glGenTextures(1,&texture->tex);
    texture->w=newSurface->w;
    texture->h=newSurface->h;
    glBindTexture(GL_TEXTURE_2D,texture->tex);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,newSurface->w,newSurface->h,0,GL_RGBA,GL_UNSIGNED_BYTE,newSurface->pixels);
    SDL_FreeSurface(newSurface);
    return texture;
}

void FreeTexture(Texture *texture){
    glDeleteTextures(1,(const GLuint*)&texture->tex);
    free(texture);
}

static Texture *CreateTextHelper(TTF_Font *font,char *format,va_list args){
    char buffer[256];
    vsprintf(buffer,format,args);
    SDL_Color fgcolor={255,255,255};
    SDL_Surface *surface=TTF_RenderUTF8_Blended(font,buffer,fgcolor);
    Texture *texture=NewTexture(surface);
    SDL_FreeSurface(surface);
    return texture;
}

Texture *CreateText(TTF_Font *font,char *format,...){
    va_list args;
    va_start(args,format);
    Texture *texture=CreateTextHelper(font,format,args);
    va_end(args);
    return texture;
}

void SetWidth(float line,float point){
    glLineWidth(line);
    glPointSize(point);
}

void DrawLineI(WindowData *win,int x1,int y1,int x2,int y2){
    glVertex2f(ToWorldX(win,x1),ToWorldY(win,y1));
    glVertex2f(ToWorldX(win,x2),ToWorldY(win,y2));
}

void DrawLine(WindowData *win,float x1,float y1,float x2,float y2,bool discretize){
    if(discretize){
        x1=DiscretizeX(win,x1); y1=DiscretizeY(win,y1);
        x2=DiscretizeX(win,x2); y2=DiscretizeY(win,y2);
    }
    glBegin(GL_LINES);
    glVertex2f(x1,-y1);
    glVertex2f(x2,-y2);
    glEnd();
}

void DrawArrow(WindowData *win,float x1,float y1,float x2,float y2,bool margin,bool bothWays,bool discretize){
    if(discretize){
        x1=DiscretizeX(win,x1); y1=DiscretizeY(win,y1);
        x2=DiscretizeX(win,x2); y2=DiscretizeY(win,y2);
    }
    float sx1=ToScreenX(win,x1),sy1=ToScreenY(win,y1);
    float sx2=ToScreenX(win,x2),sy2=ToScreenY(win,y2);
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
    glBegin(GL_LINES);
    glVertex2f(x1,-y1);
    if(margin)glVertex2f(x2,-y2);
    else glVertex2f(x7,-y7);
    glEnd();
    glBegin(GL_TRIANGLES);
    glVertex2f(x3,-y3);
    glVertex2f(x5,-y5);
    glVertex2f(x6,-y6);
    glEnd();
    if(!bothWays)return;
    d2=d1-NODE_SIZE*0.35f;
    if(d2<=0.0f)return;
    d3=d2-ARROW_HEAD_LENGTH;
    if(d3<0.0f)d3=0.0f;
    nx=-nx; ny=-ny;
    sx3=sx2+nx*d2; sy3=sy2+ny*d2;
    sx4=sx2+nx*d3; sy4=sy2+ny*d3;
    sx5=sx4+ny*d4; sy5=sy4-nx*d4;
    sx6=sx4+sx4-sx5; sy6=sy4+sy4-sy5;
    x3=ToWorldX(win,sx3); y3=ToWorldY(win,sy3);
    x5=ToWorldX(win,sx5); y5=ToWorldY(win,sy5);
    x6=ToWorldX(win,sx6); y6=ToWorldY(win,sy6);
    glBegin(GL_TRIANGLES);
    glVertex2f(x3,-y3);
    glVertex2f(x5,-y5);
    glVertex2f(x6,-y6);
    glEnd();
}

void DrawSelfArrow(WindowData *win,float x,float y,float rx,float ry,float ofs,bool discretize){
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
    if(ofs!=0.0f){
        angle+=ofs;
        cx=cos(angle);
        cy=sin(angle);
    }
    cx*=rx; cy*=ry;
    x+=cx; y+=cy;
    int sides=24;
    int n=18;
    glBegin(GL_LINE_STRIP);
    for(int i=0;i<n;i++){
        float a=angle+i*2.0f*M_PI/sides;
        glVertex2f(x-rx*cos(a),-y+ry*sin(a));
    }
    glEnd();
    glBegin(GL_POINTS);
    for(int i=0;i<n;i++){
        float a=angle+i*2.0f*M_PI/sides;
        glVertex2f(x-rx*cos(a),-y+ry*sin(a));
    }
    glEnd();
    float a=angle+1.8f*M_PI;
    float x1=x-rx*cos(a);
    float y1=y-ry*sin(a);
    a-=0.55f*M_PI;
    float b=a-0.0f*M_PI;
    float r=0.7f;
    float x2=x-rx*r*cos(b);
    float y2=y-ry*r*sin(b);
    b=a+0.15f*M_PI;
    r=1.35f;
    float x3=x-rx*r*cos(b);
    float y3=y-ry*r*sin(b);
    glBegin(GL_TRIANGLES);
    glVertex2f(x1,-y1);
    glVertex2f(x2,-y2);
    glVertex2f(x3,-y3);
    glEnd();
}

void DrawLabel(WindowData *win,float x1,float y1,float x2,float y2,Texture *label,float position,float rx,float ry,bool dark,bool discretize){
    if(!label)return;
    float x3=x1+(x2-x1)*position;
    float y3=y1+(y2-y1)*position;
    if(discretize){
        x3=DiscretizeX(win,x3);
        y3=DiscretizeY(win,y3);
    }
    glColor3ub(255,220,220);
    DrawEllipse(win,x3,y3,rx,ry,true,dark?2:1,discretize);
    if(dark)glColor3ub(0,0,0);
    else glColor3ub(128,128,128);
    float ratio=0.9f*rx*label->h/label->w;
    DrawTexture(win,label,x3,y3,0,ry*LABEL_OFS_Y,rx>ratio?ratio:rx,ry,discretize);
}

void DrawRectangle(WindowData *win,float x,float y,float rx,float ry,bool fill,int border,bool discretize){
    if(discretize){
        x=DiscretizeX(win,x);
        y=DiscretizeY(win,y);
    }
    if(fill){
        glBegin(GL_QUADS);
        glVertex2f(x-rx,-y+ry);
        glVertex2f(x+rx,-y+ry);
        glVertex2f(x+rx,-y-ry);
        glVertex2f(x-rx,-y-ry);
        glEnd();
    }
    if(!border)return;
    if(border==1)glColor3ub(128,128,128);
    else glColor3ub(0,0,0);
    glBegin(GL_LINE_LOOP);
    glVertex2f(x-rx,-y+ry);
    glVertex2f(x+rx,-y+ry);
    glVertex2f(x+rx,-y-ry);
    glVertex2f(x-rx,-y-ry);
    glEnd();
    glBegin(GL_POINTS);
    glVertex2f(x-rx,-y+ry);
    glVertex2f(x+rx,-y+ry);
    glVertex2f(x+rx,-y-ry);
    glVertex2f(x-rx,-y-ry);
    glEnd();
}

void DrawPolygon(WindowData *win,int sides,float x,float y,float rx,float ry,bool fill,int border,bool discretize){
    if(discretize){
        x=DiscretizeX(win,x);
        y=DiscretizeY(win,y);
    }
    if(fill){
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(x,-y);
        for(int i=0;i<=sides;i++){
            float a=i*2.0f*M_PI/sides;
            glVertex2f(x+rx*sin(a),-y+ry*cos(a));
        }
        glEnd();
    }
    if(!border)return;
    if(border==1)glColor3ub(128,128,128);
    else if(border==2)glColor3ub(0,0,0);
    glBegin(GL_LINE_LOOP);
    for(int i=0;i<sides;i++){
        float a=i*2.0f*M_PI/sides;
        glVertex2f(x+rx*sin(a),-y+ry*cos(a));
    }
    glEnd();
    glBegin(GL_POINTS);
    for(int i=0;i<sides;i++){
        float a=i*2.0f*M_PI/sides;
        glVertex2f(x+rx*sin(a),-y+ry*cos(a));
    }
    glEnd();
}

void DrawEllipse(WindowData *win,float x,float y,float rx,float ry,bool fill,int border,bool discretize){
    DrawPolygon(win,24,x,y,rx,ry,fill,border,discretize);
}

void DrawTexture(WindowData *win,Texture *texture,float x,float y,float ofsx,float ofsy,float dx,float dy,bool discretize){
    if(discretize){
        x=DiscretizeX(win,x);
        y=DiscretizeY(win,y);
    }
    x+=ofsx;
    y+=ofsy;
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glBindTexture(GL_TEXTURE_2D,texture->tex);
    dx=dx*texture->w/texture->h;
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f,0.0f); glVertex2f(x-dx,-y+dy);
    glTexCoord2f(1.0f,0.0f); glVertex2f(x+dx,-y+dy);
    glTexCoord2f(1.0f,1.0f); glVertex2f(x+dx,-y-dy);
    glTexCoord2f(0.0f,1.0f); glVertex2f(x-dx,-y-dy);
    glEnd();
    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
}

void DrawString(WindowData *win,TTF_Font *font,float x,float y,float size,char *format,...){
    va_list args;
    va_start(args,format);
    Texture *texture=CreateTextHelper(font,format,args);
    va_end(args);
    x=x*2.0f/win->w-1.0f;
    y=y*2.0f/win->h-1.0f;
    float sizex=size/win->w;
    float sizey=size/win->h;
    DrawTexture(win,texture,x,y,sizex*texture->w/texture->h,-sizey,sizex,sizey,false);
    FreeTexture(texture);
}
