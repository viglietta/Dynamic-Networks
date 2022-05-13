#include "main.h"

HistoryTree *NewHistoryTree(void){ // creates new root
    HistoryTree *h=malloc(sizeof(HistoryTree));
    h->input=-1;
    h->level=-1;
    h->parent=NULL;
    h->children=NewVector(4);
    h->observations=NewVector(4);
    h->reference=NULL;
    h->data=NULL;
    return h;
}

void FreeHistoryTree(HistoryTree *h){
    for(int i=0;i<h->observations->tot;i++) // free red edges
        free(h->observations->items[i]);
    FreeVector(h->observations);
    for(int i=0;i<h->children->tot;i++) // free children
        FreeHistoryTree(h->children->items[i]);
    FreeVector(h->children);
    free(h);
}

Observation *NewObservation(HistoryTree *history,int multiplicity){
    Observation *o=malloc(sizeof(Observation));
    o->history=history;
    o->multiplicity=multiplicity;
    return o;
}

static Observation *FindRedEdge(HistoryTree *h1,HistoryTree *h2){ // find red edge from h1 to h2; return NULL if not found
    for(int i=0;i<h1->observations->tot;i++){
        Observation *o=h1->observations->items[i];
        if(o->history==h2)return o;
    }
    return NULL;
}

static void AddNewRedEdge(HistoryTree *h1,HistoryTree *h2,int multiplicity){ // assumes red edge between h1 and h2 does not exist
    AddVector(h1->observations,NewObservation(h2,multiplicity));
}

void AddRedEdge(HistoryTree *h1,HistoryTree *h2,int multiplicity){ // add red edge between h1 and h2, assuming h1 is in lower level
    Observation *o1=FindRedEdge(h1,h2);
    if(o1)o1->multiplicity+=multiplicity;
    else AddNewRedEdge(h1,h2,multiplicity); // edge does not exist; create it
}

HistoryTree *AddHistoryTreeChild(HistoryTree *h,int input){ // adds a child node to h with given input and returns it
    HistoryTree *h2=NewHistoryTree();
    h2->parent=h;
    h2->input=input;
    h2->level=h->level+1;
    AddVector(h->children,h2);
    return h2;
}

static bool EquivalentNodes(HistoryTree *h1,HistoryTree *h2){ // used when constructing homomorphisms
    if(!h1 || !h2)return false;
    if(!h1->parent){ if(h2->parent)return false; }
    else if(!h2->parent || h1->parent->reference!=h2->parent)return false; // check if parents are homomorphic
    if(h1->level!=h2->level)return false;
    if(h1->input!=h2->input)return false;
    if(h1->observations->tot!=h2->observations->tot)return false;
    for(int i=0;i<h1->observations->tot;i++){ // check if all red edges lead to homomorphic nodes
        Observation *o1=h1->observations->items[i];
        Observation *o2=FindRedEdge(h2,o1->history->reference);
        if(!o2 || o1->multiplicity!=o2->multiplicity)return false;
    }
    return true;
}

static void ResetReferences(HistoryTree *h){
    h->reference=NULL;
    for(int i=0;i<h->children->tot;i++)
        ResetReferences(h->children->items[i]);
}

HistoryTree *MergeHistoryTrees(HistoryTree *h1,HistoryTree *h2,bool *added){ // modifies h1; returns endpoint in new tree and whether any nodes were added
    HistoryTree *deepest=h2;
    if(added)*added=false;
    Queue *q=NewQueue();
    h2->reference=h1; // map root of h2 to root of h1
    AppendQueue(q,h2); // enqueue root of h2
    while(!IsQueueEmpty(q)){
        HistoryTree *a=PopQueue(q); // the children of node a of h2 have to be mapped to h1
        if(a->level>deepest->level)deepest=a; // update deepest
        HistoryTree *b=a->reference; // node b of h1 is homeomorphic to a
        for(int i=0;i<a->children->tot;i++){ // scan children of a
            HistoryTree *x=a->children->items[i];
            HistoryTree *y=NULL;
            AppendQueue(q,x); // enqueue child x of a
            for(int j=0;j<b->children->tot;j++){ // search for child y of b homeomorphic to x
                HistoryTree *z=b->children->items[j];
                if(EquivalentNodes(x,z)){ y=z; break; }
            }
            if(!y){ // y has not been found
                if(added)*added=true;
                y=AddHistoryTreeChild(b,x->input); // add new child y to b
                for(int j=0;j<x->observations->tot;j++){ // scan red edges from x
                    Observation *o=x->observations->items[j];
                    AddNewRedEdge(y,o->history->reference,o->multiplicity); // create homeomorphic red edge from y
                }
            }
            x->reference=y; // map x to y
        }
    }
    FreeQueue(q);
    deepest=deepest->reference;
    ResetReferences(h2);
    return deepest;
}

HistoryTree *CopyHistoryTree(HistoryTree *h,HistoryTree **deepest){ // returns copied tree and deepest node
    HistoryTree *h2=NewHistoryTree();
    HistoryTree *aux=MergeHistoryTrees(h2,h,NULL);
    if(deepest)*deepest=aux;
    return h2;
}

bool HistoryTreeContains(HistoryTree *h1,HistoryTree *h2){ // does h1 contain a homeomorphic copy of h2?
    HistoryTree *t=CopyHistoryTree(h1,NULL);
    bool added;
    MergeHistoryTrees(t,h2,&added);
    FreeHistoryTree(t);
    return !added;
}

bool HistoryTreeEquals(HistoryTree *h1,HistoryTree *h2){ // is h1 homeomorphic to h2?
    return HistoryTreeContains(h1,h2) && HistoryTreeContains(h2,h1);
}

void PrintHistoryTree(HistoryTree *h){
    for(int i=-1;i<h->level;i++)printf("  ");
    printf("L: %d, I: %d, C: %d\n",h->level,h->input,h->children->tot);
    for(int i=0;i<h->children->tot;i++)
        PrintHistoryTree(h->children->items[i]);
}
