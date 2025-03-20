#include "main.h"

static const char *helpMessage[]={
    "MOUSE COMMANDS",
    "",
    "Left-click: select/deselect agent or view.",
    "Mouse wheel up/down: decrement/increment current round.",
    "",
    "NETWORK PANEL",
    "",
    "Left-click on an agent and drag to draw a new link; release mouse button to confirm.",
    "Hold CTRL while creating a link to delete the link instead.",
    "Hold SHIFT while creating/deleting a link to make it two-way.",
    "Hold ALT while creating/deleting a link to affect all rounds.",
    "Right-click on an agent and drag to move it.",
    "Right-click away from agents to create a new agent.",
    "",
    "HOTKEYS",
    "",
    "TAB: select counting algorithm.",
    "SPACE: execute counting algorithm step by step.",
    "ESC: deselect agents and view.",
    "UP/DOWN: decrement/increment current round.",
    "LEFT/RIGHT: select different anonymity class.",
    "BACKSPACE: delete all links in current round.",
    "DEL: delete selected agents.",
    "+/-: insert/delete round.",
    "O: toggle outdegree awareness of agents.",
    "A: toggle arrowsheads.",
    "D: change red edge and outdegree drawing mode.",
    "B: draw round/square nodes.",
    "C: toggle highlight current level.",
    "G: snap agents to grid.",
    "H: show command list.",
    "0..9: change input of selected agents; 0 is leader.",
    "CAPS-LOCK: create two-way links by default.",
    "L: load network.",
    "S: save network.",
    "",
    "See https://arxiv.org/abs/2404.02673 for more information."
};

bool roundNodes=true;
int renderLinks=0;
bool renderBar=true;
bool renderArrows=true;
bool renderMessage=false;
char infoMessage[512];

static float Patch(float x,bool left){
    return left?(1.0f+x)*(separator-1.0f)*0.5f+x:(1.0f-x)*(separator+1.0f)*0.5f+x;
}

static void RenderNetwork(WindowData *win,float rx,float ry){
    bool selfLoopOfs=false;
    if(currentRound>=0){
        Vector *v=network->rounds->items[currentRound];
        SetWidth(5.0f);
        SetColor(255,0,0);
        for(int i=0;i<v->tot;i++){
            Interaction *interaction=v->items[i];
            Entity *e1=interaction->e1;
            Entity *e2=interaction->e2;
            if(e1==e2)DrawSelfArrow(win,e1->x,e1->y,rx,ry,0.0f,renderArrows,DISCRETIZE_RENDERING);
            else DrawArrow(win,e1->x,e1->y,e2->x,e2->y,renderArrows,true,false,DISCRETIZE_RENDERING);
        }
        SetWidth(2.0f);
        PrepareRenderText();
        SetFontProperties(0.35f,0.0f,FONT_EDGE*5);
        SetFontSize(ry*1.5f*LINK_LABEL_SIZE);
        PrepareRenderLine();
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
                x=Patch(x,true);
                cx*=rx*SELF_LOOP_RADIUS; cy*=ry*SELF_LOOP_RADIUS;
                x+=2.0f*cx; y+=2.0f*cy;
                if(interaction->multiplicity>1)DrawLabel(win,x,y,x,y,interaction->multiplicity,LINK_LABEL_POSITION,rx*LINK_LABEL_SIZE,ry*LINK_LABEL_SIZE,true,true,DISCRETIZE_RENDERING);
                if(drawingEdge && GetEntityIndex(e1)==selectedEntity)selfLoopOfs=true;
            }
            else if(interaction->multiplicity>1)DrawLabel(win,Patch(e1->x,true),e1->y,Patch(e2->x,true),e2->y,interaction->multiplicity,LINK_LABEL_POSITION,rx*LINK_LABEL_SIZE,ry*LINK_LABEL_SIZE,true,true,DISCRETIZE_RENDERING);
        }
    }
    if(selectedEntity!=-1 && drawingEdge){
        Entity *e1=GetEntity(selectedEntity);
        float x,y;
        SDL_GetMouseState(&x,&y);
        x*=win->sx; y*=win->sy;
        SetWidth(5.0f);
        SetColor(255,0,0);
        int s=SelectEntityXY(x,y);
        if(s==selectedEntity)DrawSelfArrow(win,e1->x,e1->y,rx,ry,selfLoopOfs?0.55f:0.0f,true,DISCRETIZE_RENDERING);
        else{
            bool bothWays=keyboardState[SDL_SCANCODE_LSHIFT] || keyboardState[SDL_SCANCODE_RSHIFT];
            if(SDL_GetModState()&SDL_KMOD_CAPS)bothWays=!bothWays;
            if(s!=-1){
                Entity *e2=GetEntity(s);
                DrawArrow(win,e1->x,e1->y,e2->x,e2->y,true,true,bothWays,DISCRETIZE_RENDERING);
            }
            else DrawArrow(win,e1->x,e1->y,ToWorldX(win,x*2.0f/(1.0f+separator)),ToWorldY(win,y),true,false,bothWays,DISCRETIZE_RENDERING);
        }
    }
    SetWidth(3.0f);
    PrepareRenderText();
    SetFontProperties(0.35f,0.0f,FONT_EDGE*5);
    SetFontSize(ry*1.5f);
    SetFontOutlineColor(0.0f,0.0f,0.0f);
    SetFontColor(0.0f,0.0f,0.0f,1.0f);
    PrepareRenderLine();
    for(int i=network->entities->tot-1;i>=0;i--){
        Entity *e=GetEntity(i);
        if(i==selectedEntity || CorrespondsToSelectedNode(e))SetColor(255,255,0);
        else SetColor(255,255,255);
        DrawEllipse(win,Patch(e->x,true),e->y,rx,ry,true,2,DISCRETIZE_RENDERING);
        if(currentRound<-1)continue;
        if(!e->input)DrawEllipse(win,Patch(e->x,true),e->y,rx*LINK_LABEL_SIZE,ry*LINK_LABEL_SIZE,false,2,DISCRETIZE_RENDERING);
        PrepareRenderText();
        switch(e->input){
            case 0: PrintString(Patch(e->x,true)*win1->aspect+rx*LABEL_OFS_X,e->y+ry*LABEL_OFS_Y,true,"L"); break;
            case 1: PrintString(Patch(e->x,true)*win1->aspect+rx*LABEL_OFS_X1,e->y+ry*LABEL_OFS_Y,true,"1"); break;
            default: PrintString(Patch(e->x,true)*win1->aspect+rx*LABEL_OFS_X,e->y+ry*LABEL_OFS_Y,true,"%d",e->input); break;
        }
        PrepareRenderLine();
    }
}

static void RenderHistoryTree(WindowData *win,float rx,float ry){
    SetWidth(3.0f);
    if(renderBar){ // render gray bar on current level
        SetColor(208,208,208);
        AuxData *data=GetAuxData(currentRound+2,0);
        DrawRectangle(win,Patch(0,false),data->y,-Patch(0,true),ry,true,false,false);
    }
    for(int i=1;i<aux->tot;i++) // render black edges
        for(int j=0;j<GetLevel(i)->tot;j++){
            AuxData *data=GetAuxData(i,j);
            AuxData *parent=GetAuxData(i-1,data->parent);
            if(!selectedNode || data->visible)SetColor(0,0,0);
            else SetColor(128,128,128);
            SetWidth(!selectedNode || data->visible?8.0f:3.0f);
            DrawLine(win,Patch(data->x,false),data->y,Patch(parent->x,false),parent->y,DISCRETIZE_RENDERING);
        }
    if(renderLinks<2){
        SetColor(255,0,0);
        for(int i=0;i<aux->tot;i++) // render red edges
            for(int j=0;j<GetLevel(i)->tot;j++){
                AuxData *data=GetAuxData(i,j);
                if(renderLinks && !data->visible)continue;
                SetWidth(!selectedNode || data->visible?4.0f:1.5f);
                for(int k=0;k<data->observations->tot;k++){
                    AuxData *obs=GetAuxData(i-1,data->observations->items[k]);
                    DrawLine(win,Patch(data->x,false),data->y,Patch(obs->x,false),obs->y,DISCRETIZE_RENDERING);
                }
            }
        SetWidth(2.0f);
        PrepareRenderText();
        SetFontProperties(0.35f,0.0f,FONT_EDGE*5);
        SetFontSize(ry*1.5f*BLACK_EDGE_LABEL_SIZE);
        PrepareRenderLine();
        for(int i=aux->tot-1;i>1;i--) // render outdegrees
            for(int j=0;j<GetLevel(i)->tot;j++){
                AuxData *data=GetAuxData(i,j);
                if(data->outdegree<0)continue;
                if(renderLinks && !data->visible)continue;
                AuxData *parent=GetAuxData(i-1,data->parent);
                DrawLabel(win,Patch(data->x,false),data->y,Patch(parent->x,false),parent->y,data->outdegree,BLACK_EDGE_LABEL_POSITION,rx*BLACK_EDGE_LABEL_SIZE,ry*BLACK_EDGE_LABEL_SIZE,!selectedNode || data->visible,false,DISCRETIZE_RENDERING);
            }
        PrepareRenderText();
        SetFontSize(ry*1.5f*RED_EDGE_LABEL_SIZE);
        PrepareRenderLine();
        for(int i=aux->tot-1;i>=0;i--) // render multiplicities
            for(int j=0;j<GetLevel(i)->tot;j++){
                AuxData *data=GetAuxData(i,j);
                if(renderLinks && !data->visible)continue;
                for(int k=0;k<data->observations->tot;k++){
                    Observation *obs1=data->h->observations->items[k];
                    if(obs1->multiplicity<=1)continue;
                    AuxData *obs2=GetAuxData(i-1,data->observations->items[k]);
                    DrawLabel(win,Patch(data->x,false),data->y,Patch(obs2->x,false),obs2->y,obs1->multiplicity,RED_EDGE_LABEL_POSITION,rx*RED_EDGE_LABEL_SIZE,ry*RED_EDGE_LABEL_SIZE,!selectedNode || data->visible,true,DISCRETIZE_RENDERING);
                }
            }
    }
    PrepareRenderText();
    SetFontProperties(0.35f,0.0f,FONT_EDGE*5);
    SetFontSize(ry*1.5f);
    PrepareRenderLine();
    for(int i=aux->tot-1;i>=0;i--) // render nodes
        for(int j=0;j<GetLevel(i)->tot;j++){
            AuxData *data=GetAuxData(i,j);
            if(data->visible){
                if(algorithm==1){
                    if(data->guess!=-1){
                        if(data->guess==data->anonymity)SetColor(128,255,128);
                        else SetColor(255,128,128);
                    }
                    else SetColor(255,255,0);
                }
                else if(algorithm==2){
                    if(data->i<=1 && data->guess!=-1)SetColor(0,255,255);
                    else if(data->counted)SetColor(128,255,128);
                    else if(data->guess!=-1)SetColor(255,192,64);
                    else SetColor(255,255,0);
                }
                else SetColor(255,255,0);
            }
            else SetColor(255,255,255);
            SetWidth(3.0f);
            int border=!selectedNode || data->visible?2:1;
            if(roundNodes){
                DrawEllipse(win,Patch(data->x,false),data->y,rx,ry,true,border,DISCRETIZE_RENDERING);
                if(!data->h->input)DrawEllipse(win,Patch(data->x,false),data->y,rx*0.8f,ry*0.8f,false,border,DISCRETIZE_RENDERING);
            }
            else{
                DrawRectangle(win,Patch(data->x,false),data->y,rx,ry,true,border,DISCRETIZE_RENDERING);
                if(!data->h->input)DrawRectangle(win,Patch(data->x,false),data->y,rx*0.8f,ry*0.8f,false,border,DISCRETIZE_RENDERING);
            }
            int label=-1;
            if(algorithm && (selectedEntity!=-1 || selectedNodeI!=-1))label=data->guess;
            else if(data->i==1){
                if(data->h->input)label=data->h->input;
                else label=0;
            }
            if(label>=0){
                PrepareRenderText();
                if(border==1){
                    SetFontOutlineColor(0.5f,0.5f,0.5f);
                    SetFontColor(0.5f,0.5f,0.5f,1.0f);
                }
                else{
                    SetFontOutlineColor(0.0f,0.0f,0.0f);
                    SetFontColor(0.0f,0.0f,0.0f,1.0f);
                }
                switch(label){
                    case 0: PrintString(Patch(data->x,false)*win1->aspect+rx*LABEL_OFS_X,data->y+ry*LABEL_OFS_Y,true,"L"); break;
                    case 1: PrintString(Patch(data->x,false)*win1->aspect+rx*LABEL_OFS_X1,data->y+ry*LABEL_OFS_Y,true,"1"); break;
                    default:
                        if(label>=10 && label<20)PrintString(Patch(data->x,false)*win1->aspect+rx*LABEL_OFS_X1,data->y+ry*LABEL_OFS_Y,true,"%d",label);
                        else PrintString(Patch(data->x,false)*win1->aspect+rx*LABEL_OFS_X,data->y+ry*LABEL_OFS_Y,true,"%d",label);
                        break;
                }
                PrepareRenderLine();
            }
        }
}

static void RenderHelp(void){
    SetFontProperties(0.35f,0.0f,FONT_EDGE*5);
    SetFontSize(FONT_SIZE*1.0f);
    SetFontOutlineColor(0.0f,0.0f,0.0f);
    SetFontColor(0.0f,0.0f,0.0f,1.0f);

    int lines=sizeof(helpMessage)/sizeof(helpMessage[0]);
    for(int i=0;i<lines;i++)PrintString(-win1->aspect+0.05f,-1.0f+0.05f*(i+1)+((i==0||i==5||i==14||i==35)?0.025f:0.0f),false,"%s",helpMessage[i]);
}

void RenderWindow1(WindowData *win){
    int w=win->w;
    int h=win->h;
    if(!w || !h)return;
    float rx=NODE_SIZE/w;
    float ry=NODE_SIZE/h;
    PrepareRenderText();
    BindFont(&font);
    GL_Uniform1f(unifTextAspect,win1->aspect2);
    if(helping){
        RenderHelp();
        return;
    }
    PrepareRenderLine();
    SetColor(0,0,255);
    SetWidth(1.0f);
    if(SeparatorX()>1.0f){
        RenderNetwork(win,rx,ry);
        SetColor(255,255,255);
        DrawRectangle(win,Patch(0,false),0.0f,-Patch(0,true),1.0f,true,false,false);
    }
    float m=1.0f/GetAuxData(0,0)->width;
    if(rx>m)rx=m;
    ry=rx*w/h;
    m=1.0f/aux->tot;
    if(ry>m){
        ry=m;
        rx=ry*h/w;
    }
    if(SeparatorX()<w-2.0f)RenderHistoryTree(win,rx,ry);
    SetColor(0,0,0);
    if(resizeHover)SetWidth(5.0f);
    else SetWidth(1.0f);
    DrawLine(win1,separator,-1.0f,separator,1.0f,true);
    PrepareRenderText();
    SetFontProperties(0.15f,0.4f,FONT_EDGE*4);
    SetFontSize(FONT_SIZE*1.0f);
    SetFontOutlineColor(0.0f,0.0f,0.0f);
    if(network){
        SetFontColor(0.9f,0.4f,0.0f,1.0f);
        PrintString(-win->aspect+0.025f,-1.0f+0.01f,false,"Round %d of %d",currentRound+1,network->rounds->tot);
        if(network->entities->tot==1)PrintString(-win->aspect+0.025f,-1.0f+0.07f,false,"1 agent");
        else PrintString(-win->aspect+0.025f,-1.0f+0.07f,false,"%d agents",network->entities->tot);

    }
    if(renderMessage){
        SetFontColor(0.0f,0.8f,0.8f,1.0f);
        PrintString(-win->aspect+0.025f,1.0f-0.075f,false,"%s",infoMessage);
    }
}
