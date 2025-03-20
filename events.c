#include "main.h"

const bool *keyboardState;
bool resizeHover=false,resizing=false,helping=false;

static bool CloseToSeparator(float x){
    return x>=SeparatorX()-5.0f && x<=SeparatorX()+5.0f;
}

void FlushEvents(void){
    SDL_Event event;
    while(SDL_PollEvent(&event));
}

static void IncrementCurrentRound(void){
    if(currentRound<network->rounds->tot-1){
        currentRound++;
        if(selectedNodeI!=-1)IncrementSelectedNodeI();
        numSteps=-1;
        CountingAlgorithm();
        win1->invalid=true;
    }
}

static void DecrementCurrentRound(void){
    if(currentRound>=-1){
        currentRound--;
        if(selectedNodeI!=-1)DecrementSelectedNodeI();
    }
    else if(selectedNodeI>0)DecrementSelectedNodeI();
    numSteps=-1;
    CountingAlgorithm();
    win1->invalid=true;
}

static void KeyPressed(SDL_Keycode key){
    Entity *e;
    int temp1,temp2,temp3;
    switch(key){
        case SDLK_ESCAPE:
            if(selectedEntity!=-1 || selectedNodeI!=-1){
                selectedEntity=-1;
                selectedNodeI=selectedNodeJ=-1;
                numSteps=-1;
                drawingEdge=false;
                CountingAlgorithm();
                win1->invalid=true;
            }
            break;
        case SDLK_UP: DecrementCurrentRound(); break;
        case SDLK_DOWN: IncrementCurrentRound(); break;
        case SDLK_LEFT:
            temp1=selectedEntity;
            temp2=selectedNodeI;
            temp3=selectedNodeJ;
            if(selectedEntity>=0){
                selectedNodeI=currentRound+2;
                SelectNodeFromEntity(GetEntity(selectedEntity));
                selectedEntity=-1;
            }
            if(DecrementSelectedNodeJ()){
                numSteps=-1;
                CountingAlgorithm();
                win1->invalid=true;
            }
            else{
                selectedEntity=temp1;
                selectedNodeI=temp2;
                selectedNodeJ=temp3;
            }
            break;
        case SDLK_RIGHT:
            temp1=selectedEntity;
            temp2=selectedNodeI;
            temp3=selectedNodeJ;
            if(selectedEntity>=0){
                selectedNodeI=currentRound+2;
                SelectNodeFromEntity(GetEntity(selectedEntity));
                selectedEntity=-1;
            }
            if(IncrementSelectedNodeJ()){
                numSteps=-1;
                CountingAlgorithm();
                win1->invalid=true;
            }
            else{
                selectedEntity=temp1;
                selectedNodeI=temp2;
                selectedNodeJ=temp3;
            }
            break;
        case SDLK_BACKSPACE:
            if(currentRound>=0){
                Entity *en=FirstEntityCorrespondingToSelectedNode();
                DeleteInteractions(currentRound);
                ExecuteNetwork();
                if(en)SelectNodeFromEntity(en);
                numSteps=-1;
                CountingAlgorithm();
                win1->invalid=true;
                DisplayMessage("Delete all links in current round");
            }
            break;
        case SDLK_DELETE:
            selectedNode=NULL;
            if(selectedEntity!=-1&&network->entities->tot>1){
                DeleteEntity(selectedEntity);
                selectedEntity=-1;
                drawingEdge=false;
                ExecuteNetwork();
                win1->invalid=true;
                DisplayMessage("Delete selected agent");
            }
            if(selectedNodeI!=-1 && selectedNodeJ!=-1){
                bool changed=false;
                for(int i=network->entities->tot-1;i>=0&&network->entities->tot>1;i--){
                    Entity *en=GetEntity(i);
                    if(CorrespondsToSelectedNode(en)){
                        changed=true;
                        DeleteEntity(i);
                    }
                }
                if(changed){
                    selectedNodeI=selectedNodeJ=-1;
                    ExecuteNetwork();
                    win1->invalid=true;
                    DisplayMessage("Delete selected agents");
                }
            }
            break;
        case SDLK_EQUALS:
        case SDLK_KP_PLUS:
            if(currentRound>=-1){
                e=FirstEntityCorrespondingToSelectedNode();
                InsertRound(++currentRound,true);
                if(selectedNodeI!=-1)selectedNodeI++;
                ExecuteNetwork();
                if(e)SelectNodeFromEntity(e);
                numSteps=-1;
                CountingAlgorithm();
                win1->invalid=true;
                DisplayMessage("Insert new round");
            }
            break;
        case SDLK_MINUS:
        case SDLK_KP_MINUS:
            if(currentRound>=0){
                e=FirstEntityCorrespondingToSelectedNode();
                DeleteRound(currentRound);
                if(currentRound==network->rounds->tot){
                    currentRound--;
                    if(selectedNodeI!=-1)selectedNodeI--;
                }
                ExecuteNetwork();
                if(e)SelectNodeFromEntity(e);
                numSteps=-1;
                CountingAlgorithm();
                win1->invalid=true;
                DisplayMessage("Delete current round");
            }
            break;
        case SDLK_LCTRL:
        case SDLK_RCTRL:
            if(drawingEdge)DisplayMessage("Delete link");
            break;
        case SDLK_LALT:
        case SDLK_RALT:
            if(drawingEdge)DisplayMessage("Affect all rounds");
            break;
        case SDLK_LSHIFT:
        case SDLK_RSHIFT:
            if(drawingEdge){
                if(SDL_GetModState()&SDL_KMOD_CAPS)DisplayMessage("One-way link");
                else DisplayMessage("Two-way link");
            }
            break;
        case SDLK_CAPSLOCK:
            if(SDL_GetModState()&SDL_KMOD_CAPS)DisplayMessage("Create two-way links by default");
            else DisplayMessage("Create one-way links by default");
            break;
        case SDLK_0:
        case SDLK_1:
        case SDLK_2:
        case SDLK_3:
        case SDLK_4:
        case SDLK_5:
        case SDLK_6:
        case SDLK_7:
        case SDLK_8:
        case SDLK_9:
            if(selectedEntity!=-1){
                if(currentRound<-1){
                    DisplayMessage("Cannot assign input before round 0");
                    break;
                }
                int input=key-SDLK_0;
                Entity *en=GetEntity(selectedEntity);
                if(input!=en->input){
                    en->input=input;
                    SortLeaders();
                    ExecuteNetwork();
                    numSteps=-1;
                    CountingAlgorithm();
                    win1->invalid=true;
                    DisplayMessage("Change input of selected agent");
                }
            }
            if(selectedNodeI!=-1 && selectedNodeJ!=-1){
                if(currentRound<-1){
                    DisplayMessage("Cannot assign input before round 0");
                    break;
                }
                int input=key-SDLK_0;
                Entity *changed=NULL;
                for(int i=0;i<network->entities->tot;i++){
                    Entity *en=GetEntity(i);
                    if(CorrespondsToSelectedNode(en) && input!=en->input){
                        changed=en;
                        en->input=input;
                    }
                }
                if(changed){
                    SortLeaders();
                    ExecuteNetwork();
                    SelectNodeFromEntity(changed);
                    numSteps=-1;
                    CountingAlgorithm();
                    win1->invalid=true;
                    DisplayMessage("Change input of selected agents");
                }
            }
            break;
        case SDLK_TAB:
            algorithm++;
            if(algorithm>2)algorithm=0;
            numSteps=-1;
            CountingAlgorithm();
            win1->invalid=true;
            switch(algorithm){
                case 0: DisplayMessage("Execute no counting algorithm"); break;
                case 1: DisplayMessage("Execute stabilizing counting algorithm"); break;
                case 2: DisplayMessage("Execute terminating counting algorithm"); break;
                default: break;
            }
            break;
        case SDLK_O:
            outAware=!outAware;
            ExecuteNetwork();
            numSteps=-1;
            CountingAlgorithm();
            win1->invalid=true;
            if(outAware)DisplayMessage("Agents are outdegree-aware");
            else DisplayMessage("Agents are not outdegree-aware");
            break;
        case SDLK_A:
            renderArrows=!renderArrows;
            win1->invalid=true;
            if(renderArrows)DisplayMessage("Display arrowheads");
            else DisplayMessage("Do not display arrowheads");
            break;
        case SDLK_D:
            renderLinks++;
            if(renderLinks>2)renderLinks=0;
            win1->invalid=true;
            if(outAware)
                switch(renderLinks){
                    case 0: DisplayMessage("Display all red edges and outdegrees"); break;
                    case 1: DisplayMessage("Display red edges and outdegrees in selected view"); break;
                    case 2: DisplayMessage("Do not display red edges and outdegrees"); break;
                    default: break;
                }
            else
                switch(renderLinks){
                    case 0: DisplayMessage("Display all red edges"); break;
                    case 1: DisplayMessage("Display red edges in selected view"); break;
                    case 2: DisplayMessage("Do not display red edges"); break;
                    default: break;
                }

            break;
        case SDLK_B:
            roundNodes=!roundNodes;
            win1->invalid=true;
            if(roundNodes)DisplayMessage("Round nodes");
            else DisplayMessage("Square nodes");
            break;
        case SDLK_C:
            renderBar=!renderBar;
            win1->invalid=true;
            if(renderBar)DisplayMessage("Highlight current level");
            else DisplayMessage("Do not highlight current level");
            break;
        case SDLK_G:
            for(int i=0;i<network->entities->tot;i++){
                Entity *en=GetEntity(i);
                en->x=round(en->x*ENTITY_GRID)/ENTITY_GRID;
                en->y=round(en->y*ENTITY_GRID)/ENTITY_GRID;
                win1->invalid=true;
                DisplayMessage("Snap agents to grid");
            }
            break;
        case SDLK_H:
            helping=true;
            resizeHover=resizing=false;
            win1->invalid=true;
            break;
        case SDLK_SPACE:
            if(!algorithm || !selectedNode)break;
            numSteps++;
            if(numSteps)DisplayMessage("Execute step %d",numSteps);
            else DisplayMessage("Execute counting algorithm step by step");
            CountingAlgorithm();
            win1->invalid=true;
            break;
        case SDLK_L:
            if(drawingEdge || draggingEntity)break;
            LoadNetwork();
            break;
        case SDLK_S:
            if(drawingEdge || draggingEntity)break;
            SaveNetwork();
            break;
        default: break;
    }
}

static void KeyReleased(SDL_Keycode key){
    switch(key){
        case SDLK_LCTRL:
        case SDLK_RCTRL:
            if(drawingEdge)DisplayMessage("Create link");
            break;
        case SDLK_LALT:
        case SDLK_RALT:
            if(drawingEdge)DisplayMessage("Affect current round only");
            break;
        case SDLK_LSHIFT:
        case SDLK_RSHIFT:
            if(drawingEdge){
                if(SDL_GetModState()&SDL_KMOD_CAPS)DisplayMessage("Double link");
                else DisplayMessage("Single link");
            }
            break;
        default: break;
    }
}

static void MousePressed1(SDL_MouseButtonEvent *button){ // specific to left panel
    if(selectedNodeI!=-1 || selectedNodeJ!=-1){
        selectedNodeI=selectedNodeJ=-1;
        numSteps=-1;
        CountingAlgorithm();
        win1->invalid=true;
    }
    if(button->button==SDL_BUTTON_LEFT && !draggingEntity){
        int s=SelectEntityXY(button->x*win1->sx,button->y*win1->sy);
        if(s!=selectedEntity){
            selectedEntity=s;
            numSteps=-1;
            CountingAlgorithm();
            win1->invalid=true;
        }
    }
    if(button->button==SDL_BUTTON_RIGHT && !drawingEdge){
        int s=SelectEntityXY(button->x*win1->sx,button->y*win1->sy);
        if(s==-1){
            AddEntity(1,ToWorldX(win1,button->x*win1->sx*2.0f/(1.0f+separator)),ToWorldY(win1,button->y*win1->sy));
            selectedEntity=network->entities->tot-1;
            ExecuteNetwork();
            numSteps=-1;
            CountingAlgorithm();
            win1->invalid=true;
            DisplayMessage("Create new agent");
        }
        else{
            draggingEntity=true;
            if(s!=selectedEntity){
                selectedEntity=s;
                Entity *e=GetEntity(selectedEntity);
                e->x=ToWorldX(win1,button->x*win1->sx*2.0f/(1.0f+separator));
                e->y=ToWorldY(win1,button->y*win1->sy);
                if(e->x<-1.0f)e->x=-1.0f;
                if(e->y<-1.0f)e->y=-1.0f;
                if(e->x>1.0f)e->x=1.0f;
                if(e->y>1.0f)e->y=1.0f;
                numSteps=-1;
                CountingAlgorithm();
                win1->invalid=true;
            }
        }
    }
}

static void MousePressed2(SDL_MouseButtonEvent *button){ // specific to right panel
    if(selectedEntity!=-1){
        selectedEntity=-1;
        numSteps=-1;
        CountingAlgorithm();
        win1->invalid=true;
    }
    if(button->button==SDL_BUTTON_LEFT){
        int si,sj;
        SelectNodeXY(button->x*win1->sx,button->y*win1->sy,&si,&sj);
        if(si!=selectedNodeI || sj!=selectedNodeJ){
            selectedNodeI=si; selectedNodeJ=sj;
            numSteps=-1;
            CountingAlgorithm();
            if(selectedNodeI!=-1)currentRound=selectedNodeI-2;
            win1->invalid=true;
        }
    }
}

static void MousePressed3(SDL_MouseButtonEvent *button){ // specific to separator
    if(button->button==SDL_BUTTON_LEFT){
        resizing=true;
        separator=ToWorldX(win1,button->x*win1->sx);
        if(separator<-1.0f)separator=-1.0f;
        if(separator>1.0f)separator=1.0f;
        win1->invalid=true;
    }
    else if(button->button==SDL_BUTTON_RIGHT){
        resizeHover=resizing=false;
        separator=0.0f;
        win1->invalid=true;
    }
}

static void MouseReleased(SDL_MouseButtonEvent *button){
    if(button->button==SDL_BUTTON_LEFT){
        if(drawingEdge){
            drawingEdge=false;
            int mult=(keyboardState[SDL_SCANCODE_LCTRL] || keyboardState[SDL_SCANCODE_RCTRL])?-1:1;
            bool allRounds=keyboardState[SDL_SCANCODE_LALT] || keyboardState[SDL_SCANCODE_RALT];
            bool bothWays=keyboardState[SDL_SCANCODE_LSHIFT] || keyboardState[SDL_SCANCODE_RSHIFT];
            if(SDL_GetModState()&SDL_KMOD_CAPS)bothWays=!bothWays;
            win1->invalid=true;
            int s=SelectEntityXY(button->x*win1->sx,button->y*win1->sy);
            if(selectedEntity!=-1 && s!=-1){
                if(currentRound<0)DisplayMessage("Cannot modify links before round 1");
                else{
                    if(allRounds)
                        for(int r=0;r<network->rounds->tot;r++){
                            AddInteraction(r,selectedEntity,s,mult);
                            if(bothWays && selectedEntity!=s)AddInteraction(r,s,selectedEntity,mult);
                        }
                    else{
                        AddInteraction(currentRound,selectedEntity,s,mult);
                        if(bothWays && selectedEntity!=s)AddInteraction(currentRound,s,selectedEntity,mult);
                    }
                    ExecuteNetwork();
                    numSteps=-1;
                    CountingAlgorithm();
                    win1->invalid=true;
                    if(mult>0)DisplayMessage("Create new link");
                    else DisplayMessage("Delete link");
                }
            }
        }
        resizing=false;
    }
    if(button->button==SDL_BUTTON_RIGHT){
        if(draggingEntity){
            draggingEntity=false;
            Entity *e=GetEntity(selectedEntity);
            e->x=ToWorldX(win1,button->x*win1->sx*2.0f/(1.0f+separator));
            e->y=ToWorldY(win1,button->y*win1->sy);
            if(e->x<-1.0f)e->x=-1.0f;
            if(e->y<-1.0f)e->y=-1.0f;
            if(e->x>1.0f)e->x=1.0f;
            if(e->y>1.0f)e->y=1.0f;
            win1->invalid=true;
        }
    }
}

static void MouseMoved(SDL_MouseMotionEvent *motion){
    if(selectedEntity!=-1 && !drawingEdge && !resizing && (motion->state & SDL_BUTTON_LMASK)){
        int mx=motion->x*win1->sx;
        int my=motion->y*win1->sy;
        Entity *e=GetEntity(selectedEntity);
        float sx=ToScreenX1(win1,e->x);
        float sy=ToScreenY(win1,e->y);
        float dist=(sx-mx)*(sx-mx)+(sy-my)*(sy-my);
        if(dist>NODE_SIZE*NODE_SIZE*0.25f){
            drawingEdge=true;
            //else
        }
    }
    if(drawingEdge)win1->invalid=true;
    if(draggingEntity){
        Entity *e=GetEntity(selectedEntity);
        e->x=ToWorldX(win1,motion->x*win1->sx*2.0f/(1.0f+separator));
        e->y=ToWorldY(win1,motion->y*win1->sy);
        if(e->x<-1.0f)e->x=-1.0f;
        if(e->y<-1.0f)e->y=-1.0f;
        if(e->x>1.0f)e->x=1.0f;
        if(e->y>1.0f)e->y=1.0f;
        win1->invalid=true;
    }
    if(resizing && (motion->state & SDL_BUTTON_LMASK)){
        separator=ToWorldX(win1,motion->x*win1->sx);
        if(separator<-1.0f)separator=-1.0f;
        if(separator>1.0f)separator=1.0f;
        win1->invalid=true;
    }
    if(resizeHover && !resizing && !CloseToSeparator(motion->x*win1->sx)){
        resizeHover=false;
        win1->invalid=true;
    }
    else if(!resizeHover && !drawingEdge && !draggingEntity && CloseToSeparator(motion->x*win1->sx)){
        resizeHover=true;
        win1->invalid=true;
    }
}

void Events(void){
    SDL_Event e;
    SDL_Window *win;
    while(SDL_PollEvent(&e)){
        switch(e.type){
            case SDL_EVENT_QUIT:
                quit=true;
                break;
            case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
                quit=true;
                break;
            #ifdef __EMSCRIPTEN__
            case SDL_EVENT_WINDOW_RESIZED:
                win1->invalid=true;
                RenderWindow(win1);
                break;
            #endif
            case SDL_EVENT_KEY_DOWN:
                if(helping){
                    helping=false;
                    win1->invalid=true;
                    break;
                }
                KeyPressed(e.key.key);
                break;
            case SDL_EVENT_KEY_UP:
                if(helping)break;
                KeyReleased(e.key.key);
                break;
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
                if(helping){
                    helping=false;
                    win1->invalid=true;
                    break;
                }
                win=SDL_GetWindowFromID(e.window.windowID);
                if(win!=win1->window)break;
                if(resizeHover)MousePressed3(&e.button);
                else if(e.button.x*win1->sx<SeparatorX())MousePressed1(&e.button);
                else MousePressed2(&e.button);
                break;
            case SDL_EVENT_MOUSE_BUTTON_UP:
                if(helping)break;
                win=SDL_GetWindowFromID(e.window.windowID);
                if(win!=win1->window)break;
                MouseReleased(&e.button);
                break;
            case SDL_EVENT_MOUSE_MOTION:
                if(helping)break;
                win=SDL_GetWindowFromID(e.window.windowID);
                if(win!=win1->window)break;
                MouseMoved(&e.motion);
                break;
            case SDL_EVENT_MOUSE_WHEEL:
                if(helping)break;
                if(e.wheel.y<0)IncrementCurrentRound();
                else if(e.wheel.y>0)DecrementCurrentRound();
                break;
            default:
                break;
        }
    }
}
