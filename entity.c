#include "main.h"

void UpdateEntityLabel(Entity *e){
    SetWindowContext(win1);
    if(e->label)FreeTexture(e->label);
    e->label=e->input?CreateText(font,"%d",e->input):CreateText(font,"L");
}

Entity *NewEntity(int input,float x,float y){
    Entity *e=malloc(sizeof(Entity));
    e->input=input;
    e->finalLeaf=e->current=e->history=NULL;
    e->mailbox=NewVector(4);
    e->x=x;
    e->y=y;
    e->label=NULL;
    UpdateEntityLabel(e);
    return e;
}

void FreeEntity(Entity *e){ // assumes mailbox to be empty
    FreeHistoryTree(e->history);
    FreeVector(e->mailbox);
    if(e->label){
        SetWindowContext(win1);
        FreeTexture(e->label);
    }
    free(e);
}

bool IsLeader(Entity *e){
    return !e->input;
}

Entity *CopyEntity(Entity *e){ // assumes mailbox to be empty
    Entity *e2=malloc(sizeof(Entity));
    e2->input=e->input;
    e2->history=CopyHistoryTree(e->history,&e2->current);
    e2->mailbox=NewVector(4);
    return e2;
}

void ExtendHistory(Entity *e){ // extend history of e by one level
    e->current=AddHistoryTreeChild(e->current,e->input);
}

int EndRound(Entity *e,bool count,bool naiveAlgorithm){ // returns guess on n
    ExtendHistory(e); // create new level
    for(int i=0;i<e->mailbox->tot;i++){ // read all messages
        Observation *m=e->mailbox->items[i];
        HistoryTree *h=MergeHistoryTrees(e->history,m->history,NULL); // update history tree of e
        AddRedEdge(e->current,h,m->multiplicity); // add red edge
        FreeHistoryTree(m->history); // destroy message
        free(m);
    }
    FreeVector(e->mailbox); // empty mailbox
    e->mailbox=NewVector(4);
    return -1;
}

void SendHistory(Entity *e1,Entity *e2,int multiplicity){ // sends the history from e1 to e2
    HistoryTree *h=CopyHistoryTree(e1->history,NULL);
    AddVector(e2->mailbox,NewObservation(h,multiplicity));
}

void Interact(Entity *e1,Entity *e2){ // two-way single link between e1 and e2
    SendHistory(e1,e2,1);
    SendHistory(e2,e1,1);
}
