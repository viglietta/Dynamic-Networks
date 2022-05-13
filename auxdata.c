#include "main.h"

Vector *aux=NULL; // Vector of Vector of AuxData
AuxData *selectedNode=NULL;

Vector *GetLevel(int i){
    return aux->items[i];
}

AuxData *GetAuxData(int i,int j){
    return GetLevel(i)->items[j];
}

static int ComputeAuxDataWidth(HistoryTree *h){
    int index=h->level+1;
    while(aux->tot<=index)AddVector(aux,NewVector(8));
    AuxData *data=malloc(sizeof(AuxData));
    data->i=index;
    data->j=AddVector(GetLevel(index),data);
    h->data=data;
    data->h=h;
    if(index>0)data->parent=GetLevel(index-1)->tot-1;
    else data->parent=-1;
    data->children=NewVectorI(4);
    data->observations=NewVectorI(4);
    data->multiplicities=NewVectorI(4);
    data->label=NULL;
    data->linkLabels=NewVector(4);
    data->anonymity=0;
    data->visible=false;
    data->width=0;
    for(int i=0;i<h->children->tot;i++){
        data->width+=ComputeAuxDataWidth(h->children->items[i]);
        AddVectorI(data->children,GetLevel(index+1)->tot-1);
    }
    if(!data->width)data->width=1;
    return data->width;
}

static void ComputeAuxDataCoordinates(int i,int j,float x1,float y1,float x2,float y2){
    AuxData *data=GetAuxData(i,j);
    int w=data->width;
    int h=aux->tot;
    data->y=y1+(y2-y1)*(i+0.5f)/h;
    if(!data->children->tot)data->x=(x1+x2)*0.5f;
    else{
        float nx1=x1;
        for(int k=0;k<data->children->tot;k++){
            int c=data->children->items[k];
            float nx2=nx1+(x2-x1)*GetAuxData(i+1,c)->width/w;
            ComputeAuxDataCoordinates(i+1,c,nx1,y1,nx2,y2);
            nx1=nx2;
        }
        data->x=(GetAuxData(i+1,data->children->items[0])->x+GetAuxData(i+1,data->children->items[data->children->tot-1])->x)*0.5f;
    }
}

static void ComputeAuxDataRedEdges(void){
    SetWindowContext(win2);
    for(int i=0;i<aux->tot;i++)
        for(int j=0;j<GetLevel(i)->tot;j++){
            AuxData *data=GetAuxData(i,j);
            for(int l=0;l<data->h->observations->tot;l++)
                for(int k=0;k<GetLevel(i-1)->tot;k++){
                    Observation *obs=data->h->observations->items[l];
                    if(obs->history==GetAuxData(i-1,k)->h){
                        AddVectorI(data->observations,k);
                        AddVectorI(data->multiplicities,obs->multiplicity);
                        if(obs->multiplicity>1)AddVector(data->linkLabels,CreateText(font,"%d",obs->multiplicity));
                        else AddVector(data->linkLabels,NULL);
                    }
                }
        }
}

static void ComputeAuxDataAnonymities(void){
    for(int i=0;i<network->entities->tot;i++){
        Entity *e=GetEntity(i);
        ((AuxData*)e->finalLeaf->data)->anonymity++;
    }
    for(int i=aux->tot-2;i>=0;i--)
        for(int j=0;j<GetLevel(i)->tot;j++){
            AuxData *data=GetAuxData(i,j);
            for(int k=0;k<data->children->tot;k++){
                AuxData *child=GetAuxData(i+1,data->children->items[k]);
                data->anonymity+=child->anonymity;
            }
        }
}

void ResetAuxDataVariables(void){
    for(int i=0;i<aux->tot;i++){
        Vector *v=GetLevel(i);
        for(int j=0;j<v->tot;j++){
            AuxData *data=v->items[j];
            data->guess=-1; // no guess has been made
            data->counted=false;
            data->weight=0;
            data->cumulativeAnonymity=0;
            data->guesser=false;
        }
    }
}

void ComputeAuxData(HistoryTree *h){
    FreeAuxData();
    aux=NewVector(8);
    ComputeAuxDataWidth(h);
    ComputeAuxDataCoordinates(0,0,-1.0f,-1.0f,1.0f,1.0f);
    ComputeAuxDataRedEdges();
    ComputeAuxDataAnonymities();
    ResetAuxDataVariables();
    UpdateAuxDataLabels();
}

void FreeAuxData(void){
    if(!aux)return;
    SetWindowContext(win2);
    for(int i=0;i<aux->tot;i++){
        Vector *v=GetLevel(i);
        for(int j=0;j<v->tot;j++){
            AuxData *data=v->items[j];
            FreeVectorI(data->children);
            FreeVectorI(data->observations);
            FreeVectorI(data->multiplicities);
            if(data->label)FreeTexture(data->label);
            for(int k=0;k<data->linkLabels->tot;k++){
                Texture *texture=data->linkLabels->items[k];
                if(texture)FreeTexture(texture);
            }
            FreeVector(data->linkLabels);
            free(data);
        }
        FreeVector(v);
    }
    FreeVector(aux);
    aux=NULL;
}

void UpdateAuxDataLabels(void){
    SetWindowContext(win2);
    for(int i=0;i<aux->tot;i++)
        for(int j=0;j<GetLevel(i)->tot;j++){
            AuxData *data=GetAuxData(i,j);
            if(data->label)FreeTexture(data->label);
            data->label=NULL;
            if(algorithm && (selectedEntity!=-1 || selectedNodeI!=-1)){
                if(data->guess!=-1)data->label=CreateText(font,"%d",data->guess);
            }
            else if(data->i==1){
                if(data->h->input)data->label=CreateText(font,"%d",data->h->input);
                else data->label=CreateText(font,"L");
            }
        }
}

static void SelectNode(AuxData *data,bool select){
    data->visible=select;
}

static void SelectViewHelper(int si,int sj){
    for(int i=aux->tot-1;i>=0;i--){
        Vector *v1=GetLevel(i);
        for(int j=0;j<v1->tot;j++)SelectNode(v1->items[j],false);
        if(i==si)SelectNode(v1->items[sj],true);
        else if(i<si){
            Vector *v2=GetLevel(i+1);
            for(int j=0;j<v2->tot;j++){
                AuxData *data=v2->items[j];
                if(!data->visible)continue;
                SelectNode(v1->items[data->parent],true);
                for(int k=0;k<data->observations->tot;k++){
                    int obs=data->observations->items[k];
                    SelectNode(v1->items[obs],true);
                }
            }
        }
    }
}

void SelectView(void){
    if(selectedEntity!=-1){
        Entity *e=GetEntity(selectedEntity);
        HistoryTree *h=e->finalLeaf;
        for(int i=aux->tot-1;i>currentRound+2;i--)h=h->parent;
        selectedNode=h->data;
        SelectViewHelper(selectedNode->i,selectedNode->j);
    }
    else{
        selectedNode=selectedNodeI==-1?NULL:GetAuxData(selectedNodeI,selectedNodeJ);
        SelectViewHelper(selectedNodeI,selectedNodeJ);
    }
}

void SelectNodeXY(int x,int y,int *si,int *sj){
    float minDist=-1.0f;
    *si=-1; *sj=-1;
    for(int i=0;i<aux->tot;i++){
        Vector *v=GetLevel(i);
        float cy=ToScreenY(win2,((AuxData*)v->items[0])->y);
        if(cy+NODE_SIZE*0.5f<y)continue;
        if(cy-NODE_SIZE*0.5f>y)break;
        for(int j=0;j<v->tot;j++){
            float cx=ToScreenX(win2,((AuxData*)v->items[j])->x);
            float dist=(cx-x)*(cx-x)+(cy-y)*(cy-y);
            if(*si==-1 || dist<minDist){ minDist=dist; *si=i; *sj=j; }
        }
        break;
    }
    if(*si!=-1 && minDist<=NODE_SIZE*NODE_SIZE*0.25f)return;
    *si=-1; *sj=-1;
}

bool CorrespondsToSelectedNode(Entity *e){
    if(selectedNodeI==-1 || selectedNodeJ==-1)return false;
    AuxData *data=GetAuxData(selectedNodeI,selectedNodeJ);
    HistoryTree *h=e->finalLeaf;
    for(int i=aux->tot-1;i>selectedNodeI;i--)h=h->parent;
    return h==data->h;
}

Entity *FirstEntityCorrespondingToSelectedNode(void){
    if(selectedNodeI==-1 || selectedNodeJ==-1)return NULL;
    for(int i=0;i<network->entities->tot;i++){
        Entity *e=GetEntity(i);
        if(CorrespondsToSelectedNode(e))return e;
    }
    return NULL;
}

void SelectNodeFromEntity(Entity *e){
    HistoryTree *h=e->finalLeaf;
    for(int i=aux->tot-1;i>selectedNodeI;i--)h=h->parent;
    AuxData *data=h->data;
    selectedNodeJ=data->j;
}

void IncrementSelectedNodeI(void){
    AuxData *data=GetAuxData(selectedNodeI,selectedNodeJ);
    if(data->children->tot>0){
        selectedNodeI++;
        selectedNodeJ=data->children->items[0];
    }
    else selectedNodeI=selectedNodeJ=-1;
}

void DecrementSelectedNodeI(void){
    AuxData *data=GetAuxData(selectedNodeI,selectedNodeJ);
    if(data->parent!=-1){
        selectedNodeI--;
        selectedNodeJ=data->parent;
    }
    else selectedNodeI=selectedNodeJ=-1;
}

bool IncrementSelectedNodeJ(void){
    if(selectedNodeJ==-1)return false;
    Vector *v=GetLevel(selectedNodeI);
    if(selectedNodeJ>=v->tot-1)return false;
    selectedNodeJ++;
    return true;
}

bool DecrementSelectedNodeJ(void){
    if(selectedNodeJ<=0)return false;
    selectedNodeJ--;
    return true;
}
