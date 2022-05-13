#include "main.h"

bool roundNodes=true;
int renderLinks=0;
bool renderMessage=false;
char infoMessage[512];

static void RenderHistoryTree(WindowData *win,float rx,float ry){
    for(int i=0;i<aux->tot;i++) // render black edges
        for(int j=0;j<GetLevel(i)->tot;j++){
            AuxData *data=GetAuxData(i,j);
            if(data->parent!=-1){
                AuxData *parent=GetAuxData(i-1,data->parent);
                if(!selectedNode || data->visible)glColor3ub(0,0,0);
                else glColor3ub(128,128,128);
                glLineWidth(!selectedNode || data->visible?8.0f:3.0f);
                DrawLine(win,data->x,data->y,parent->x,parent->y,DISCRETIZE_RENDERING);
            }
        }
    if(renderLinks<2){
        glColor3ub(255,0,0);
        for(int i=0;i<aux->tot;i++) // render red edges
            for(int j=0;j<GetLevel(i)->tot;j++){
                AuxData *data=GetAuxData(i,j);
                if(!renderLinks || data->visible){
                    glLineWidth(!selectedNode || data->visible?4.0f:1.5f);
                    for(int k=0;k<data->observations->tot;k++){
                        AuxData *obs=GetAuxData(i-1,data->observations->items[k]);
                        DrawLine(win,data->x,data->y,obs->x,obs->y,DISCRETIZE_RENDERING);
                    }
                }
            }
        SetWidth(2.0f,1.0f);
        for(int i=aux->tot-1;i>=0;i--) // render multiplicities
            for(int j=0;j<GetLevel(i)->tot;j++){
                AuxData *data=GetAuxData(i,j);
                if(!renderLinks || data->visible){
                    for(int k=0;k<data->observations->tot;k++){
                        Texture *label=data->linkLabels->items[k];
                        if(!label)continue;
                        AuxData *obs=GetAuxData(i-1,data->observations->items[k]);
                        DrawLabel(win,data->x,data->y,obs->x,obs->y,label,RED_EDGE_LABEL_POSITION,rx*RED_EDGE_LABEL_SIZE,ry*RED_EDGE_LABEL_SIZE,!selectedNode || data->visible,DISCRETIZE_RENDERING);
                    }
                }
            }
    }
    for(int i=aux->tot-1;i>=0;i--) // render nodes
        for(int j=0;j<GetLevel(i)->tot;j++){
            AuxData *data=GetAuxData(i,j);
            if(data->visible){
                if(algorithm==1){
                    if(data->guess!=-1){
                        if(data->guess==data->anonymity)glColor3ub(128,255,128);
                        else glColor3ub(255,128,128);
                    }
                    else glColor3ub(255,255,0);
                }
                else if(algorithm==2){
                    if(data->i<=1 && data->guess!=-1)glColor3ub(0,255,255);
                    else if(data->counted)glColor3ub(128,255,128);
                    else if(data->guess!=-1)glColor3ub(255,192,64);
                    else glColor3ub(255,255,0);
                }
                else glColor3ub(255,255,0);
            }
            else glColor3ub(255,255,255);
            SetWidth(3.0f,2.0f);
            int border=!selectedNode || data->visible?2:1;
            if(roundNodes){
                DrawEllipse(win,data->x,data->y,rx,ry,true,border,DISCRETIZE_RENDERING);
                if(data->h->input==0)DrawEllipse(win,data->x,data->y,rx*0.8f,ry*0.8f,false,border,DISCRETIZE_RENDERING);
            }
            else{
                DrawRectangle(win,data->x,data->y,rx,ry,true,border,DISCRETIZE_RENDERING);
                if(data->h->input==0)DrawRectangle(win,data->x,data->y,rx*0.8f,ry*0.8f,false,border,DISCRETIZE_RENDERING);
            }
            if(border==1)glColor3ub(128,128,128);
            else glColor3ub(0,0,0);

            if(data->label){
                float ratio=0.9f*rx*data->label->h/data->label->w;
                DrawTexture(win,data->label,data->x,data->y,0,ry*LABEL_OFS_Y,rx>ratio?ratio:rx,ry,DISCRETIZE_RENDERING);
            }
        }
}

void RenderWindow1(WindowData *win){
    int w=win->w;
    int h=win->h;
    if(!w || !h)return;
    float rx=NODE_SIZE/w;
    float ry=NODE_SIZE/h;
    float selfLoopOfs=false;
    if(currentRound>=0){
        Vector *v=network->rounds->items[currentRound];
        SetWidth(5.0f,5.0f);
        glColor3ub(255,0,0);
        for(int i=0;i<v->tot;i++){
            Interaction *interaction=v->items[i];
            Entity *e1=interaction->e1;
            Entity *e2=interaction->e2;
            if(e1==e2)DrawSelfArrow(win,e1->x,e1->y,rx,ry,0.0f,DISCRETIZE_RENDERING);
            else DrawArrow(win,e1->x,e1->y,e2->x,e2->y,true,false,DISCRETIZE_RENDERING);
        }
        SetWidth(2.0f,1.0f);
        for(int i=0;i<v->tot;i++){
            Interaction *interaction=v->items[i];
            Entity *e1=interaction->e1;
            Entity *e2=interaction->e2;
            if(e1==e2){
                float x=e1->x;
                float y=e1->y;
                float dist=sqrt(x*x+y*y);
                float cx,cy;
                if(dist>0.0f){ cx=x/dist; cy=y/dist; }
                else{ cx=0.0f; cy=-1.0f; }
                cx*=rx*SELF_LOOP_RADIUS; cy*=ry*SELF_LOOP_RADIUS;
                x+=2.0f*cx; y+=2.0f*cy;
                DrawLabel(win,x,y,x,y,interaction->label,LINK_LABEL_POSITION,rx*LINK_LABEL_SIZE,ry*LINK_LABEL_SIZE,true,DISCRETIZE_RENDERING);
                if(drawingEdge && GetEntityIndex(e1)==selectedEntity)selfLoopOfs=true;
            }
            else DrawLabel(win,e1->x,e1->y,e2->x,e2->y,interaction->label,LINK_LABEL_POSITION,rx*LINK_LABEL_SIZE,ry*LINK_LABEL_SIZE,true,DISCRETIZE_RENDERING);
        }
    }
    if(selectedEntity!=-1 && drawingEdge){
        Entity *e1=GetEntity(selectedEntity);
        int x,y;
        SDL_GetMouseState(&x,&y);
        SetWidth(5.0f,5.0f);
        glColor3ub(255,0,0);
        int s=SelectEntityXY(x,y);
        if(s==selectedEntity)DrawSelfArrow(win,e1->x,e1->y,rx,ry,selfLoopOfs?0.55f:0.0f,DISCRETIZE_RENDERING);
        else{
            bool bothWays=keyboardState[SDL_SCANCODE_LSHIFT] || keyboardState[SDL_SCANCODE_RSHIFT];
            if(SDL_GetModState()&KMOD_CAPS)bothWays=!bothWays;
            if(s!=-1){
                Entity *e2=GetEntity(s);
                DrawArrow(win,e1->x,e1->y,e2->x,e2->y,true,bothWays,DISCRETIZE_RENDERING);
            }
            else DrawArrow(win,e1->x,e1->y,ToWorldX(win,x),ToWorldY(win,y),false,bothWays,DISCRETIZE_RENDERING);
        }
    }
    SetWidth(3.0f,2.0f);
    for(int i=network->entities->tot-1;i>=0;i--){
        Entity *e=GetEntity(i);
        if(i==selectedEntity || CorrespondsToSelectedNode(e))glColor3ub(255,255,0);
        else glColor3ub(255,255,255);
        DrawEllipse(win,e->x,e->y,rx,ry,true,2,DISCRETIZE_RENDERING);
        if(e->input==0)DrawEllipse(win,e->x,e->y,rx*LINK_LABEL_SIZE,ry*LINK_LABEL_SIZE,false,2,DISCRETIZE_RENDERING);
        glColor3ub(0,0,0);
        float ratio=0.9f*rx*e->label->h/e->label->w;
        DrawTexture(win,e->label,e->x,e->y,0,ry*LABEL_OFS_Y,rx>ratio?ratio:rx,ry,DISCRETIZE_RENDERING);
    }
    glColor3ub(0,0,255);
    if(network)DrawString(win,font,0,MESSAGE_SIZE,MESSAGE_SIZE,"Round %d of %d",currentRound+1,network->rounds->tot);
    if(renderMessage)DrawString(win,font,0,win->h,MESSAGE_SIZE,infoMessage);
}

void RenderWindow2(WindowData *win){
    int w=win->w;
    int h=win->h;
    if(!w || !h)return;
    float rx=NODE_SIZE/w;
    float m=1.0f/GetAuxData(0,0)->width;
    if(rx>m)rx=m;
    float ry=rx*w/h;
    m=1.0f/aux->tot;
    if(ry>m){
        ry=m;
        rx=ry*h/w;
    }
    RenderHistoryTree(win,rx,ry);
}
