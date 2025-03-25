#include "main.h"

WindowData *win1=NULL;
float separator=0.0f;
bool quit=false;
static Uint64 timeout;

void Exit(const char *m){
    if(m)SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,"Critical error",m,NULL);
    #ifdef __EMSCRIPTEN__
    emscripten_force_exit(1);
    #else
    exit(EXIT_FAILURE);
    #endif
}

Uint8 ReadU8(SDL_IOStream *stream){
    Uint8 x;
    if(!SDL_ReadU8(stream,&x))Exit("Error reading stream.");
    return x;
}

Uint16 ReadU16LE(SDL_IOStream *stream){
    Uint16 x;
    if(!SDL_ReadU16LE(stream,&x))Exit("Error reading stream.");
    return x;
}

Uint32 ReadU32LE(SDL_IOStream *stream){
    Uint32 x;
    if(!SDL_ReadU32LE(stream,&x))Exit("Error reading stream.");
    return x;
}

float ReadFloat(SDL_IOStream *stream){
    union{float f;Uint32 x;}u;
    u.x=ReadU32LE(stream);
    return u.f;
}

void DisplayMessage(const char *format,...){
    va_list args;
    va_start(args,format);
    vsprintf(infoMessage,format,args);
    va_end(args);
    renderMessage=true;
    win1->invalid=true;
    timeout=SDL_GetTicks()+MESSAGE_TIME;
}

void UndisplayMessage(void){
    renderMessage=false;
    win1->invalid=true;
}

static void Update(void){
    Events();
    if(renderMessage && SDL_GetTicks()>=timeout)UndisplayMessage();
}

#ifndef __EMSCRIPTEN__
static bool WindowEventWatcher(void *data,SDL_Event *event){
    (void)data;
    if(event->type==SDL_EVENT_WINDOW_RESIZED||event->type==SDL_EVENT_WINDOW_MOVED){
        SDL_Window *window=SDL_GetWindowFromID(event->window.windowID);
        if(window==win1->window){
            if(event->type==SDL_EVENT_WINDOW_RESIZED){
                win1->w=event->window.data1;
                win1->h=event->window.data2;
                SetWindowViewport(win1);
            }
            win1->invalid=true;
            RenderWindow(win1);
        }
    }
    return false;
}
#endif

static void QuitSystem(void){
    QuitText();
    if(win1)FreeWindow(win1);
    SDL_Quit();
}

static void InitSystem(void){
    SDL_Init(SDL_INIT_VIDEO|SDL_INIT_EVENTS);
    keyboardState=SDL_GetKeyboardState(NULL);
    #ifdef __EMSCRIPTEN__
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,CONTEXT_PROFILE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION,CONTEXT_MAJOR);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION,CONTEXT_MINOR);
    #endif
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE,8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE,0);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,0);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS,1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES,4);
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL,1);
    win1=NewWindow(WINDOW_TITLE,RenderWindow1);
    InitText();
    #ifndef __EMSCRIPTEN__
    SDL_AddEventWatch(WindowEventWatcher,NULL);
    #endif
}

static void MainLoop(void){
    Update();
    RenderWindow(win1);
}

int main(int argc,char **argv){
    (void)argc; (void)argv;
    InitSystem();
    Uint64 time;
    SDL_GetCurrentTime((SDL_Time*)&time);
    SDL_srand(time);
    InitNetwork(SDL_rand(6)+1,INITIAL_NETWORK_NUM_ENTITIES);
    FlushEvents();
    DisplayMessage("Press H for help");
    #ifdef __EMSCRIPTEN__
    emscripten_set_keydown_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW,NULL,EM_TRUE,KeyPressedCallback);
    emscripten_set_keyup_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW,NULL,EM_TRUE,KeyReleasedCallback);
    emscripten_set_main_loop(MainLoop,0,1);
    #else
    while(!quit)MainLoop();
    #endif
    DoneNetwork();
    QuitSystem();
    return EXIT_SUCCESS;
}
