#include "main.h"

const Uint8 *keyboardState;
bool prompting=false;

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
        win2->invalid=true;
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
    win2->invalid=true;
}

static void KeyPressed(SDL_Keycode key){
    if(prompting){
        if(key==SDLK_y){
            if(SaveNetwork(FILENAME))DisplayMessage("Network saved to file \"%s\"",FILENAME);
            else DisplayMessage("Failed to save network to file \"%s\"",FILENAME);
            prompting=false;
        }
        else if(key==SDLK_n){
            UndisplayMessage();
            prompting=false;
        }
        return;
    }
    Entity *e;
    switch(key){
        case SDLK_ESCAPE:
            if(selectedEntity!=-1 || selectedNodeI!=-1){
                selectedEntity=-1;
                selectedNodeI=selectedNodeJ=-1;
                numSteps=-1;
                drawingEdge=false;
                CountingAlgorithm();
                win1->invalid=true;
                win2->invalid=true;
            }
            break;
        case SDLK_UP: DecrementCurrentRound(); break;
        case SDLK_DOWN: IncrementCurrentRound(); break;
        case SDLK_LEFT:
            if(DecrementSelectedNodeJ()){
                numSteps=-1;
                CountingAlgorithm();
                win1->invalid=true;
                win2->invalid=true;
            }
            break;
        case SDLK_RIGHT:
            if(IncrementSelectedNodeJ()){
                numSteps=-1;
                CountingAlgorithm();
                win1->invalid=true;
                win2->invalid=true;
            }
            break;
        case SDLK_BACKSPACE:
            if(currentRound>=0){
                Entity *e=FirstEntityCorrespondingToSelectedNode();
                DeleteInteractions(currentRound);
                ExecuteNetwork();
                if(e)SelectNodeFromEntity(e);
                numSteps=-1;
                CountingAlgorithm();
                win1->invalid=true;
                win2->invalid=true;
                DisplayMessage("Delete all links in current round");
            }
            break;
        case SDLK_DELETE:
            selectedNode=NULL;
            if(selectedEntity!=-1){
                DeleteEntity(selectedEntity);
                selectedEntity=-1;
                drawingEdge=false;
                ExecuteNetwork();
                win1->invalid=true;
                win2->invalid=true;
                DisplayMessage("Delete selected entity");
            }
            if(selectedNodeI!=-1 && selectedNodeJ!=-1){
                bool changed=false;
                for(int i=0;i<network->entities->tot;i++){
                    Entity *e=GetEntity(i);
                    if(CorrespondsToSelectedNode(e)){
                        changed=true;
                        DeleteEntity(i--);
                    }
                }
                if(changed){
                    selectedNodeI=selectedNodeJ=-1;
                    ExecuteNetwork();
                    win1->invalid=true;
                    win2->invalid=true;
                    DisplayMessage("Delete selected entities");
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
                win2->invalid=true;
                DisplayMessage("Create new round");
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
                win2->invalid=true;
                DisplayMessage("Delete current round");
            }
            break;
        case SDLK_LCTRL:
        case SDLK_RCTRL:
            if(drawingEdge)DisplayMessage("Remove link");
            break;
        case SDLK_LALT:
        case SDLK_RALT:
            if(drawingEdge)DisplayMessage("Affect all rounds");
            break;
        case SDLK_LSHIFT:
        case SDLK_RSHIFT:
            if(drawingEdge){
                if(SDL_GetModState()&KMOD_CAPS)DisplayMessage("Single link");
                else DisplayMessage("Double link");
            }
            break;
        case SDLK_CAPSLOCK:
            if(SDL_GetModState()&KMOD_CAPS)DisplayMessage("Create double links by default");
            else DisplayMessage("Create single links by default");
            break;
        case SDLK_0...SDLK_9:
            if(selectedEntity!=-1){
                int input=key-SDLK_0;
                Entity *e=GetEntity(selectedEntity);
                if(input!=e->input){
                    e->input=input;
                    UpdateEntityLabel(e);
                    SortLeaders();
                    ExecuteNetwork();
                    numSteps=-1;
                    CountingAlgorithm();
                    win1->invalid=true;
                    win2->invalid=true;
                    DisplayMessage("Change input of selected entity");
                }
            }
            if(selectedNodeI!=-1 && selectedNodeJ!=-1){
                int input=key-SDLK_0;
                Entity *changed=NULL;
                for(int i=0;i<network->entities->tot;i++){
                    Entity *e=GetEntity(i);
                    if(CorrespondsToSelectedNode(e) && input!=e->input){
                        changed=e;
                        e->input=input;
                        UpdateEntityLabel(e);
                    }
                }
                if(changed){
                    SortLeaders();
                    ExecuteNetwork();
                    SelectNodeFromEntity(changed);
                    numSteps=-1;
                    CountingAlgorithm();
                    win1->invalid=true;
                    win2->invalid=true;
                    DisplayMessage("Change input of selected entities");
                }
            }
            break;
        case SDLK_TAB:
            algorithm++;
            if(algorithm>2)algorithm=0;
            numSteps=-1;
            CountingAlgorithm();
            win2->invalid=true;
            switch(algorithm){
                case 0: DisplayMessage("Execute no counting algorithm"); break;
                case 1: DisplayMessage("Execute stabilizing counting algorithm"); break;
                case 2: DisplayMessage("Execute terminating counting algorithm"); break;
                default: break;
            }
            break;
        case SDLK_l:
            renderLinks++;
            if(renderLinks>2)renderLinks=0;
            win2->invalid=true;
            switch(renderLinks){
                case 0: DisplayMessage("Display all red edges"); break;
                case 1: DisplayMessage("Display red edges in current view only"); break;
                case 2: DisplayMessage("Do not display red edges"); break;
                default: break;
            }
            break;
        case SDLK_b:
            roundNodes=!roundNodes;
            win2->invalid=true;
            if(roundNodes)DisplayMessage("Round nodes");
            else DisplayMessage("Square nodes");
            break;
        case SDLK_d:
            for(int i=0;i<network->entities->tot;i++){
                Entity *e=GetEntity(i);
                e->x=round(e->x*ENTITY_GRID)/ENTITY_GRID;
                e->y=round(e->y*ENTITY_GRID)/ENTITY_GRID;
                win1->invalid=true;
                DisplayMessage("Snap entities to grid");
            }
            break;
        case SDLK_SPACE:
            if(!algorithm || !selectedNode)break;
            numSteps++;
            if(numSteps)DisplayMessage("Execute step %d",numSteps);
            else DisplayMessage("Execute counting algorithm step by step");
            CountingAlgorithm();
            win2->invalid=true;
            break;
        case SDLK_F1:
            if(LoadNetwork(FILENAME))DisplayMessage("Network loaded from file \"%s\"",FILENAME);
            else DisplayMessage("Failed to load network from file \"%s\"",FILENAME);
            break;
        case SDLK_F2:
            if(drawingEdge || draggingEntity)break;
            prompting=true;
            break;
        default: break;
    }
}

static void KeyReleased(SDL_Keycode key){
    switch(key){
        case SDLK_LCTRL:
        case SDLK_RCTRL:
            if(drawingEdge)DisplayMessage("Add link");
            break;
        case SDLK_LALT:
        case SDLK_RALT:
            if(drawingEdge)DisplayMessage("Affect current round only");
            break;
        case SDLK_LSHIFT:
        case SDLK_RSHIFT:
            if(drawingEdge){
                if(SDL_GetModState()&KMOD_CAPS)DisplayMessage("Double link");
                else DisplayMessage("Single link");
            }
            break;
        default: break;
    }
}

static void MousePressed1(SDL_MouseButtonEvent *button){ // specific to window 1
    if(selectedNodeI!=-1 || selectedNodeJ!=-1){
        selectedNodeI=selectedNodeJ=-1;
        numSteps=-1;
        CountingAlgorithm();
        win1->invalid=true;
        win2->invalid=true;
    }
    if(button->button==SDL_BUTTON_LEFT && !draggingEntity){
        int s=SelectEntityXY(button->x,button->y);
        if(s!=selectedEntity){
            selectedEntity=s;
            numSteps=-1;
            CountingAlgorithm();
            win1->invalid=true;
            win2->invalid=true;
        }
    }
    if(button->button==SDL_BUTTON_RIGHT && !drawingEdge){
        int s=SelectEntityXY(button->x,button->y);
        if(s==-1){
            AddEntity(1,ToWorldX(win1,button->x),ToWorldY(win1,button->y));
            selectedEntity=network->entities->tot-1;
            ExecuteNetwork();
            numSteps=-1;
            CountingAlgorithm();
            win1->invalid=true;
            win2->invalid=true;
            DisplayMessage("Create new entity");
        }
        else{
            draggingEntity=true;
            if(s!=selectedEntity){
                selectedEntity=s;
                Entity *e=GetEntity(selectedEntity);
                e->x=ToWorldX(win1,button->x);
                e->y=ToWorldY(win1,button->y);
                if(e->x<-1.0f)e->x=-1.0f;
                if(e->y<-1.0f)e->y=-1.0f;
                if(e->x>1.0f)e->x=1.0f;
                if(e->y>1.0f)e->y=1.0f;
                numSteps=-1;
                CountingAlgorithm();
                win1->invalid=true;
                win2->invalid=true;
            }
        }
    }
}

static void MousePressed2(SDL_MouseButtonEvent *button){ // specific to window 2
    if(selectedEntity!=-1){
        selectedEntity=-1;
        numSteps=-1;
        CountingAlgorithm();
        win1->invalid=true;
        win2->invalid=true;
    }
    if(button->button==SDL_BUTTON_LEFT){
        int si,sj;
        SelectNodeXY(button->x,button->y,&si,&sj);
        if(si!=selectedNodeI || sj!=selectedNodeJ){
            selectedNodeI=si; selectedNodeJ=sj;
            numSteps=-1;
            CountingAlgorithm();
            if(selectedNodeI!=-1)currentRound=selectedNodeI-2;
            win1->invalid=true;
            win2->invalid=true;
        }
    }
}

static void MouseReleased1(SDL_MouseButtonEvent *button){ // specific to window 1
    if(button->button==SDL_BUTTON_LEFT){
        if(drawingEdge){
            int mult=(keyboardState[SDL_SCANCODE_LCTRL] || keyboardState[SDL_SCANCODE_RCTRL])?-1:1;
            bool allRounds=keyboardState[SDL_SCANCODE_LALT] || keyboardState[SDL_SCANCODE_RALT];
            bool bothWays=keyboardState[SDL_SCANCODE_LSHIFT] || keyboardState[SDL_SCANCODE_RSHIFT];
            if(SDL_GetModState()&KMOD_CAPS)bothWays=!bothWays;
            drawingEdge=false;
            win1->invalid=true;
            int s=SelectEntityXY(button->x,button->y);
            if(selectedEntity!=-1 && s!=-1){
                if(allRounds)
                    for(int r=0;r<network->rounds->tot;r++){
                        AddInteraction(r,selectedEntity,s,mult);
                        if(bothWays && selectedEntity!=s)AddInteraction(r,s,selectedEntity,mult);
                    }
                else if(currentRound>=0){
                    AddInteraction(currentRound,selectedEntity,s,mult);
                    if(bothWays && selectedEntity!=s)AddInteraction(currentRound,s,selectedEntity,mult);
                }
                ExecuteNetwork();
                numSteps=-1;
                CountingAlgorithm();
                win2->invalid=true;
            }
        }
    }
    if(button->button==SDL_BUTTON_RIGHT){
        if(draggingEntity){
            draggingEntity=false;
            Entity *e=GetEntity(selectedEntity);
            e->x=ToWorldX(win1,button->x);
            e->y=ToWorldY(win1,button->y);
            if(e->x<-1.0f)e->x=-1.0f;
            if(e->y<-1.0f)e->y=-1.0f;
            if(e->x>1.0f)e->x=1.0f;
            if(e->y>1.0f)e->y=1.0f;
            win1->invalid=true;
        }
    }
}

static void MouseMoved1(SDL_MouseMotionEvent *motion){ // specific to window 1
    if(selectedEntity!=-1 && !drawingEdge && (motion->state & SDL_BUTTON_LMASK)){
        int mx=motion->x;
        int my=motion->y;
        Entity *e=GetEntity(selectedEntity);
        float sx=ToScreenX(win1,e->x);
        float sy=ToScreenY(win1,e->y);
        float dist=(sx-mx)*(sx-mx)+(sy-my)*(sy-my);
        if(dist>NODE_SIZE*NODE_SIZE*0.25f)drawingEdge=true;
    }
    if(drawingEdge)win1->invalid=true;
    if(draggingEntity){
        Entity *e=GetEntity(selectedEntity);
        e->x=ToWorldX(win1,motion->x);
        e->y=ToWorldY(win1,motion->y);
        if(e->x<-1.0f)e->x=-1.0f;
        if(e->y<-1.0f)e->y=-1.0f;
        if(e->x>1.0f)e->x=1.0f;
        if(e->y>1.0f)e->y=1.0f;
        win1->invalid=true;
    }
}

void Events(void){
    SDL_Event e;
    SDL_Window *win;
    while(SDL_PollEvent(&e)){
        switch(e.type){
            case SDL_QUIT: quit=true; break;
            case SDL_WINDOWEVENT:
                switch(e.window.event){
                    case SDL_WINDOWEVENT_CLOSE: quit=true; break;
                    case SDL_WINDOWEVENT_ENTER: SDL_RaiseWindow(SDL_GetWindowFromID(e.window.windowID)); break;
                    default: break;
                }
            case SDL_KEYDOWN:
                KeyPressed(e.key.keysym.sym);
                break;
            case SDL_KEYUP:
                if(prompting)break;
                KeyReleased(e.key.keysym.sym);
                break;
            case SDL_MOUSEBUTTONDOWN:
                if(prompting)break;
                win=SDL_GetWindowFromID(e.window.windowID);
                if(win==win1->window)MousePressed1(&e.button);
                else if(win==win2->window)MousePressed2(&e.button);
                break;
            case SDL_MOUSEBUTTONUP:
                if(prompting)break;
                win=SDL_GetWindowFromID(e.window.windowID);
                if(win==win1->window)MouseReleased1(&e.button);
                break;
            case SDL_MOUSEMOTION:
                if(prompting)break;
                win=SDL_GetWindowFromID(e.window.windowID);
                if(win==win1->window)MouseMoved1(&e.motion);
                break;
            case SDL_MOUSEWHEEL:
                if(prompting)break;
                if(e.wheel.y<0)IncrementCurrentRound();
                else if(e.wheel.y>0)DecrementCurrentRound();
                break;
        }
    }
}
