typedef struct WindowData{
    SDL_Window *window;
    SDL_GLContext context;
    void(*renderFunction)(struct WindowData*);
    int w,h;
    bool invalid;
}WindowData;

typedef struct{
    GLuint tex;
    int w,h;
}Texture;

extern TTF_Font *font;

float ToWorldX(WindowData *win,int x);
float ToWorldY(WindowData *win,int y);
float ToScreenX(WindowData *win,float x);
float ToScreenY(WindowData *win,float y);
float DiscretizeX(WindowData *win,float x);
float DiscretizeY(WindowData *win,float y);
WindowData *NewWindow(const char *title,int x,int y,int w,int h,void(*renderFunction)(struct WindowData*));
void FreeWindow(WindowData *win);
void SetWindowContext(WindowData *win);
void SetWindowViewport(WindowData *win);
void RenderWindow(WindowData *win);
Texture *NewTexture(SDL_Surface *surface);
void FreeTexture(Texture *texture);
Texture *CreateText(TTF_Font *font,char *format,...);
void SetWidth(float line,float point);
void DrawLineI(WindowData *win,int x1,int y1,int x2,int y2);
void DrawLine(WindowData *win,float x1,float y1,float x2,float y2,bool discretize);
void DrawArrow(WindowData *win,float x1,float y1,float x2,float y2,bool margin,bool bothWays,bool discretize);
void DrawSelfArrow(WindowData *win,float x,float y,float rx,float ry,float ofs,bool discretize);
void DrawLabel(WindowData *win,float x1,float y1,float x2,float y2,Texture *label,float position,float rx,float ry,bool dark,bool discretize);
void DrawRectangle(WindowData *win,float x,float y,float rx,float ry,bool fill,int border,bool discretize);
void DrawPolygon(WindowData *win,int sides,float x,float y,float rx,float ry,bool fill,int border,bool discretize);
void DrawEllipse(WindowData *win,float x,float y,float rx,float ry,bool fill,int border,bool discretize);
void DrawTexture(WindowData *win,Texture *texture,float x,float y,float ofsx,float ofsy,float dx,float dy,bool discretize);
void DrawString(WindowData *win,TTF_Font *font,float x,float y,float size,char *format,...);
