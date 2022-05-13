#include "main.h"

Vector *NewVector(int m){
    Vector *v=malloc(sizeof(Vector));
    v->mincap=v->cap=m;
    v->tot=0;
    v->items=malloc(sizeof(void*)*v->cap);
    return v;
}

int VectorTotal(Vector *v){
    return(v->tot);
}

static void ResizeVector(Vector *v,int c){
    void **items=realloc(v->items,sizeof(void*)*c);
    if(!items)exit(EXIT_FAILURE);
    v->items=items;
    v->cap=c;
}

int AddVector(Vector *v,void *item){
    if(v->tot==v->cap)ResizeVector(v,v->cap*2);
    v->items[v->tot++]=item;
    return v->tot-1;
}

void SetVector(Vector *v,int i,void *item){
    if(i>=0 && i<v->tot)v->items[i]=item;
    else exit(EXIT_FAILURE);
}

void InsertVector(Vector *v,int i,void *item){
    if(i<0 || i>v->tot)exit(EXIT_FAILURE);
    if(v->tot==v->cap)ResizeVector(v,v->cap*2);
    for(int j=v->tot++;j>i;j--)
        v->items[j]=v->items[j-1];
    v->items[i]=item;
}

void *GetVector(Vector *v,int i){
    if(i>=0 && i<v->tot)return(v->items[i]);
    exit(EXIT_FAILURE);
}

void *DeleteVector(Vector *v,int i){
    if(i<0 || i>=v->tot)exit(EXIT_FAILURE);
    void *r=v->items[i];
    v->tot--;
    v->items[i]=v->items[v->tot];
    if(v->tot==v->cap/4 && v->cap>=2*v->mincap)ResizeVector(v,v->cap/2);
    return r;
}

void *DeinsertVector(Vector *v,int i){
    if(i<0 || i>=v->tot)exit(EXIT_FAILURE);
    void *r=v->items[i];
    for(int j=i;j<v->tot-1;j++)
        v->items[j]=v->items[j+1];
    v->tot--;
    if(v->tot==v->cap/4 && v->cap>=2*v->mincap)ResizeVector(v,v->cap/2);
    return r;
}

void FreeVector(Vector *v){
    free(v->items);
    free(v);
}

IntVector *NewVectorI(int m){
    IntVector *v=malloc(sizeof(IntVector));
    v->mincap=v->cap=m;
    v->tot=0;
    v->items=malloc(sizeof(int)*v->cap);
    return v;
}

int VectorTotalI(IntVector *v){
    return(v->tot);
}

static void ResizeVectorI(IntVector *v,int c){
    int *items=realloc(v->items,sizeof(int)*c);
    if(!items)exit(EXIT_FAILURE);
    v->items=items;
    v->cap=c;
}

int AddVectorI(IntVector *v,int item){
    if(v->tot==v->cap)ResizeVectorI(v,v->cap*2);
    v->items[v->tot++]=item;
    return v->tot-1;
}

void SetVectorI(IntVector *v,int i,int item){
    if(i>=0 && i<v->tot)v->items[i]=item;
    else exit(EXIT_FAILURE);
}

int GetVectorI(IntVector *v,int i){
    if(i>=0 && i<v->tot)return(v->items[i]);
    exit(EXIT_FAILURE);
}

void DeleteVectorI(IntVector *v,int i){
    if(i<0 || i>=v->tot)exit(EXIT_FAILURE);
    v->tot--;
    v->items[i]=v->items[v->tot];
    if(v->tot==v->cap/4 && v->cap>=2*v->mincap)ResizeVectorI(v,v->cap/2);
}

void FreeVectorI(IntVector *v){
    free(v->items);
    free(v);
}
