#include "main.h"

bool outAware=true;

Entity *NewEntity(int input,float x,float y){
    Entity *e=malloc(sizeof(Entity));
    e->input=input;
    e->finalLeaf=e->current=e->history=NULL;
    e->mailbox=NewVector(4);
    e->outdegree=outAware?0:-1;
    e->x=x;
    e->y=y;
    return e;
}

void FreeEntity(Entity *e){ // assumes mailbox to be empty
    FreeHistoryTree(e->history);
    FreeVector(e->mailbox);
    free(e);
}

bool IsLeader(Entity *e){
    return !e->input;
}

void ExtendHistory(Entity *e){ // extend history of e by one level
    e->current=AddHistoryTreeChild(e->current,e->input);
    e->current->outdegree=e->outdegree;
    e->outdegree=outAware?0:-1;
}

void EndRound(Entity *e){
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
}

void SendHistory(Entity *e1,Entity *e2,int multiplicity){ // sends the history from e1 to e2
    HistoryTree *h=CopyHistoryTree(e1->history,NULL);
    AddVector(e2->mailbox,NewObservation(h,multiplicity));
    if(outAware)e1->outdegree+=multiplicity;
}

void Interact(Entity *e1,Entity *e2){ // two-way single link between e1 and e2
    SendHistory(e1,e2,1);
    SendHistory(e2,e1,1);
}
