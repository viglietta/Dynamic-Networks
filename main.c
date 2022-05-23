#include "main.h"

WindowData *win1,*win2;
bool quit=false;
static Uint64 timeout;

void DisplayMessage(char *format,...){
    va_list args;
    va_start(args,format);
    vsprintf(infoMessage,format,args);
    va_end(args);
    renderMessage=true;
    win1->invalid=true;
    timeout=SDL_GetTicks64()+MESSAGE_TIME;
}

void UndisplayMessage(void){
    renderMessage=false;
    win1->invalid=true;
}

static void Update(void){
    Events();
    if(renderMessage && SDL_GetTicks64()>=timeout)UndisplayMessage();
    if(prompting)DisplayMessage("Save network to file \"%s\"? (Y/N)",FILENAME);
}

#ifdef WIN32
static void EnableHiDPI(void){
    typedef enum{
        PROCESS_DPI_UNAWARE=0,
        PROCESS_SYSTEM_DPI_AWARE=1,
        PROCESS_PER_MONITOR_DPI_AWARE=2
    }PROCESS_DPI_AWARENESS;
    void *shcoreDLL=SDL_LoadObject("SHCORE.DLL");
    if(shcoreDLL){
        HRESULT(WINAPI *SetProcessDpiAwareness)(PROCESS_DPI_AWARENESS dpiAwareness);
        SetProcessDpiAwareness=(HRESULT(WINAPI*)(PROCESS_DPI_AWARENESS))SDL_LoadFunction(shcoreDLL,"SetProcessDpiAwareness");
        if(SetProcessDpiAwareness){
            SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
            return;
        }
    }
    void *userDLL=SDL_LoadObject("USER32.DLL");
    if(userDLL){
        BOOL(WINAPI *SetProcessDPIAware)(void);
        SetProcessDPIAware=(BOOL(WINAPI*)(void))SDL_LoadFunction(userDLL,"SetProcessDPIAware");
        if(SetProcessDPIAware)SetProcessDPIAware();
    }
}
#endif // WIN32

static int WindowEventWatcher(void *data,SDL_Event *event){
    if(event->type==SDL_WINDOWEVENT&&(event->window.event==SDL_WINDOWEVENT_RESIZED||event->window.event==SDL_WINDOWEVENT_MOVED)){
        SDL_Window *window=SDL_GetWindowFromID(event->window.windowID);
        WindowData *win=NULL;
        if(window==win1->window)win=win1;
        else if(window==win2->window)win=win2;
        if(win){
            if(event->window.event==SDL_WINDOWEVENT_RESIZED){
                win->w=event->window.data1;
                win->h=event->window.data2;
                SetWindowViewport(win);
            }
            win->invalid=true;
            RenderWindow(win);
        }
    }
    return 0;
}

static void InitSystem(void){
    #ifdef WIN32
    EnableHiDPI();
    #endif // WIN32
    SDL_Init(SDL_INIT_VIDEO|SDL_INIT_EVENTS|SDL_INIT_TIMER);
    keyboardState=SDL_GetKeyboardState(NULL);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE,8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,0);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS,1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES,4);
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL,1);
    TTF_Init();
    font=TTF_OpenFont("font.ttf",(int)NODE_SIZE);
    SDL_Rect rect;
    SDL_GetDisplayUsableBounds(0,&rect);
    float mx=(1.0f-2.0f*WIN_WIDTH)/3.0f;
    float my=(1.0f-WIN_HEIGHT)/2.0f;
    win1=NewWindow("Network",rect.x+rect.w*mx,rect.y+rect.h*my,rect.w*WIN_WIDTH,rect.h*WIN_HEIGHT,RenderWindow1);
    win2=NewWindow("History Tree",rect.x+rect.w*(WIN_WIDTH+2.0f*mx),rect.y+rect.h*my,rect.w*WIN_WIDTH,rect.h*WIN_HEIGHT,RenderWindow2);
    SDL_AddEventWatch(WindowEventWatcher,NULL);
    SDL_RaiseWindow(win1->window);
}

static void QuitSystem(void){
    FreeWindow(win1);
    FreeWindow(win2);
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_Quit();
}

int main(int argc,char **argv){
    InitSystem();
    bool loaded=false;
    if(argc>1)loaded=LoadNetwork(argv[1]);
    if(!loaded)loaded=LoadNetwork(FILENAME);
    if(!loaded)InitNetwork(INITIAL_NETWORK_TYPE,INITIAL_NETWORK_NUM_ENTITIES);
    DisplayMessage("Press TAB to select a counting algorithm");
    FlushEvents();
    while(!quit){
        Update();
        RenderWindow(win1);
        RenderWindow(win2);
    }
    DoneNetwork();
    QuitSystem();
    return EXIT_SUCCESS;
}
