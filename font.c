#include "main.h"

Font font;
Font *boundFont=NULL;
float fontScale=FONT_SIZE;

static void NewTexture(Font *f,SDL_Surface *surface,bool smooth,bool alpha,int levels){ // set level=-1 for unlimited mipmap levels
    f->tex=0;
    glGenTextures(1,&f->tex);
    if(!f->tex)Exit("Unable to generate texture handle.");
    f->w=surface->w;
    f->h=surface->h;
    glBindTexture(GL_TEXTURE_2D,f->tex);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,smooth?GL_LINEAR:GL_NEAREST);
    if(levels)glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,smooth?GL_LINEAR_MIPMAP_LINEAR:GL_NEAREST_MIPMAP_LINEAR);
    else glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,smooth?GL_LINEAR:GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D,0,alpha?GL_RGBA8:GL_RGB8,surface->w,surface->h,0,alpha?GL_RGBA:GL_RGB,GL_UNSIGNED_BYTE,surface->pixels);
    if(glGetError()!=GL_NO_ERROR)Exit("Unable to create texture.");
    if(levels){
        if(levels>0)glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAX_LEVEL,levels);
        GL_GenerateMipmap(GL_TEXTURE_2D);
        if(glGetError()!=GL_NO_ERROR)Exit("Unable to generate texture mipmap.");
    }
}

static void FreeTexture(Font *f){
    glDeleteTextures(1,&f->tex);
    f->tex=0;
}

static SDL_Surface *LoadTGA(SDL_IOStream *stream){
    Uint8 length=ReadU8(stream);
    SDL_SeekIO(stream,1,SDL_IO_SEEK_CUR);
    bool rle=ReadU8(stream)==10;
    SDL_SeekIO(stream,9,SDL_IO_SEEK_CUR);
    int w=ReadU16LE(stream);
    int h=ReadU16LE(stream);
    int bpp=ReadU8(stream)>>3;
    SDL_SeekIO(stream,length+1,SDL_IO_SEEK_CUR);
    SDL_Surface *surface=SDL_CreateSurface(w,h,bpp==4?SDL_PIXELFORMAT_BGRA32:SDL_PIXELFORMAT_BGR24);
    if(!surface)Exit("Unable to create surface.");
    Uint8 pixel[4],*p=(Uint8*)surface->pixels+(h-1)*surface->pitch;
    int count=0,rep=0;
    for(int i=0;i<h;i++){
        if(rle){
            int j=0;
            for(;;){
                if(count){
                    int n=count;
                    if(n>w-j)n=w-j;
                    SDL_ReadIO(stream,p+j*bpp,bpp*n);
                    count-=n;
                    j+=n;
                }
                else if(rep){
                    int n=rep;
                    if(n>w-j)n=w-j;
                    rep-=n;
                    while(n--)SDL_memcpy(p+j++*bpp,pixel,bpp);
                }
                if(j==w)break;
                Uint8 c=ReadU8(stream);
                if(c&0x80){
                    SDL_ReadIO(stream,pixel,bpp);
                    rep=(c&0x7f)+1;
                }
                else count=c+1;
            }
        }
        else SDL_ReadIO(stream,p,w*bpp);
        p-=surface->pitch;
    }
    return surface;
}

static void LoadMainTexture(Font *f,const char *filename,bool smooth,bool alpha,int levels){
    SDL_IOStream *stream=NULL;
    SDL_Surface *surface=NULL;
    if(!(stream=SDL_IOFromFile(filename,"rb")))Exit("Unable to open font file.");
    if(!(surface=LoadTGA(stream)))Exit("Unable to load texture.");
    if(!SDL_CloseIO(stream))Exit("Unable to close font file.");
    NewTexture(f,surface,smooth,alpha,levels);
    SDL_DestroySurface(surface);
}

static void LoadFont(Font *f,const char *fontFile,const char *texFile){
    SDL_IOStream *stream=NULL;
    if(!(stream=SDL_IOFromFile(fontFile,"rb")))Exit("Unable to open font file.");
    f->iw=1.0f/ReadU16LE(stream);
    f->ih=1.0f/ReadU16LE(stream);
    f->base=ReadU16LE(stream);
    f->base=ReadU16LE(stream);
    f->kerning=ReadU8(stream)?calloc(1,96*96*sizeof(Sint8)):NULL;
    SDL_memset(f->glyph,0,96*sizeof(Glyph));
    Uint8 i,amount;
    for(;;){
        if(!(i=ReadU8(stream)))break;
        i-=32;
        f->glyph[i].x=(Sint16)ReadU16LE(stream);
        f->glyph[i].y=(Sint16)ReadU16LE(stream);
        f->glyph[i].w=(Sint16)ReadU16LE(stream);
        f->glyph[i].h=(Sint16)ReadU16LE(stream);
        f->glyph[i].ofsX=(Sint16)ReadU16LE(stream);
        f->glyph[i].ofsY=(Sint16)ReadU16LE(stream);
        f->glyph[i].advX=(Sint16)ReadU16LE(stream);
        for(;;){
            if(!(amount=ReadU8(stream)))break;
            Uint8 j=ReadU8(stream);
            f->kerning[i*96+j-32]=amount;
        }
    }
    if(!SDL_CloseIO(stream))Exit("Unable to close font file.");
    LoadMainTexture(&font,texFile,true,true,0);
}

void InitFont(void){
    LoadFont(&font,"font.fon","font.tga");
}

void QuitFont(void){
    FreeTexture(&font);
    free(font.kerning);
}

void BindFont(Font *f){
    boundFont=f;
    glBindTexture(GL_TEXTURE_2D,f->tex);
    GL_Uniform2f(unifTextTexInvSize,boundFont->iw,boundFont->ih);
}

void SetFontProperties(float thickness,float border,float edge){
    GL_Uniform3f(unifTextThick,thickness,thickness+border,edge);
}

void SetFontOutlineColor(float r,float g,float b){
    GL_Uniform3f(unifTextOutlineColor,r,g,b);
}

void SetFontSize(float scale){
    fontScale=scale/boundFont->base;
    GL_Uniform1f(unifTextScale,fontScale);
}

void SetFontColor(float r,float g,float b,float a){
    GL_Uniform4f(unifTextFaceColor,r,g,b,a);
}
