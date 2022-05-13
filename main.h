#ifndef __MAIN_H
#define __MAIN_H

#ifdef WIN32
#include <windows.h>
#endif // WIN32
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_ttf.h>

#define false 0
#define true 1
#define bool int

#define FILENAME "DefaultNetwork.txt"
#define INITIAL_NETWORK_TYPE 1
#define INITIAL_NETWORK_NUM_ENTITIES 10
#define WIN_WIDTH 0.4f
#define WIN_HEIGHT 0.8f
#define MESSAGE_SIZE 50.0f
#define MESSAGE_TIME 5000
#define NODE_SIZE 50.0f
#define LABEL_OFS_Y -0.06f
#define ARROW_HEAD_LENGTH NODE_SIZE
#define ARROW_HEAD_WIDTH 30.0f
#define SELF_LOOP_RADIUS 1.5f
#define LINK_LABEL_SIZE 0.8f
#define LINK_LABEL_POSITION 0.75f
#define RED_EDGE_LABEL_SIZE 0.8f
#define RED_EDGE_LABEL_POSITION 0.25f
#define DISCRETIZE_RENDERING true
#define ENTITY_GRID 10.0f

#include "queue.h"
#include "vector.h"
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

extern WindowData *win1,*win2;
extern bool quit;

void DisplayMessage(char *format,...);
void UndisplayMessage(void);

#endif
