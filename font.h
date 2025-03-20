typedef struct{
    int x,y,w,h,ofsX,ofsY,advX;
}Glyph;

typedef struct{
    GLuint tex;
    int w,h;
    float iw,ih; // inverse width and height of texture
    int base;
    Glyph glyph[96];
    Sint8 *kerning; // NULL if font has no kerning
}Font;

extern Font font;
extern Font *boundFont;
extern float fontScale;

void InitFont(void);
void QuitFont(void);
void BindFont(Font *f);
void SetFontProperties(float thickness,float border,float edge);
void SetFontOutlineColor(float r,float g,float b);
void SetFontSize(float scale);
void SetFontColor(float r,float g,float b,float a);
