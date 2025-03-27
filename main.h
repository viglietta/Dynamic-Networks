#ifndef __MAIN_H
#define __MAIN_H

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#ifdef __EMSCRIPTEN__
#include <SDL3/SDL.h>
#include <emscripten.h>
#include <emscripten/html5.h>
#include <GLES3/gl3.h>
#else
#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>
#endif

#define WINDOW_TITLE ("Anonymous Dynamic Networks")
#define CONTEXT_PROFILE (SDL_GL_CONTEXT_PROFILE_ES)
#define CONTEXT_MAJOR (3)
#define CONTEXT_MINOR (0)

#define FILENAME "Network.txt"
#define INITIAL_NETWORK_NUM_ENTITIES 10
#define WIN_WIDTH 0.95f
#define WIN_HEIGHT 0.95f
#define MESSAGE_SIZE 50.0f
#define MESSAGE_TIME 5000
#define MAX_STRING_LENGTH 512
#define NODE_SIZE 50.0f
#define FONT_SIZE 0.05f
#define FONT_THICKNESS 0.5f
#define FONT_OUTLINE 0.2f
#define FONT_EDGE 0.1f
#define LABEL_OFS_X 0.2f
#define LABEL_OFS_X1 -0.05f
#define LABEL_OFS_Y -0.2f
#define ARROW_HEAD_LENGTH NODE_SIZE*0.8f
#define ARROW_HEAD_WIDTH 30.0f*0.8f
#define SELF_LOOP_RADIUS 1.5f
#define LINK_LABEL_SIZE 0.8f
#define LINK_LABEL_POSITION 0.75f
#define RED_EDGE_LABEL_SIZE 0.8f
#define RED_EDGE_LABEL_POSITION 0.25f
#define BLACK_EDGE_LABEL_SIZE 0.7f
#define BLACK_EDGE_LABEL_POSITION 0.5f
#define DISCRETIZE_RENDERING false
#define ENTITY_GRID 10.0f
#define EPSILON 0.000001f
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef __EMSCRIPTEN__
#define SDL_EVENT_LOAD_NETWORK (SDL_EVENT_USER+1)
#define SDL_EVENT_SAVE_NETWORK (SDL_EVENT_USER+2)
#endif

#include "queue.h"
#include "vector.h"
#include "buffer.h"
#include "shader.h"
#include "font.h"
#include "text.h"
#include "graphics.h"
#include "history_tree.h"
#include "entity.h"
#include "examples.h"
#include "network.h"
#include "auxdata.h"
#include "stabilizing_algo.h"
#include "terminating_algo.h"
#include "render.h"
#include "events.h"

extern WindowData *win1;
extern float separator;
extern bool quit;

void Exit(const char *m);
Uint8 ReadU8(SDL_IOStream *stream);
Uint16 ReadU16LE(SDL_IOStream *stream);
Uint32 ReadU32LE(SDL_IOStream *stream);
float ReadFloat(SDL_IOStream *stream);
void DisplayMessage(const char *format,...);
void UndisplayMessage(void);

#endif
