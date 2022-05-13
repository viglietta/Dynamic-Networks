#include "main.h"

Network *network=NULL;
int currentRound=-1;
HistoryTree *finalHistory=NULL;
int selectedEntity=-1;
int selectedNodeI=-1,selectedNodeJ=-1;
bool drawingEdge=false;
bool draggingEntity=false;
int algorithm=0;
int numSteps=-1;

Entity *GetEntity(int i){
    return network->entities->items[i];
}

int GetEntityIndex(Entity *e){
    for(int i=0;i<network->entities->tot;i++)
        if(e==GetEntity(i))return i;
    return -1;
}

static void UpdateInteractionLabel(Interaction *interaction){
    SetWindowContext(win1);
    if(interaction->label){
        FreeTexture(interaction->label);
        interaction->label=NULL;
    }
    if(interaction->multiplicity>1)
        interaction->label=CreateText(font,"%d",interaction->multiplicity);
}

static Interaction *NewInteraction(int i,int j,int multiplicity){
    Interaction *interaction=malloc(sizeof(Interaction));
    interaction->e1=GetEntity(i);
    interaction->e2=GetEntity(j);
    interaction->multiplicity=multiplicity;
    interaction->label=NULL;
    UpdateInteractionLabel(interaction);
    return interaction;
}

static Interaction *CopyInteraction(Interaction *interaction){
    Interaction *interaction2=malloc(sizeof(Interaction));
    interaction2->e1=interaction->e1;
    interaction2->e2=interaction->e2;
    interaction2->multiplicity=interaction->multiplicity;
    interaction2->label=NULL;
    UpdateInteractionLabel(interaction2);
    return interaction2;
}

static void ExecuteInteraction(Interaction *interaction){
    SendHistory(interaction->e1,interaction->e2,interaction->multiplicity);
}

void SortLeaders(void){ // put all leaders at the beginning of the list of entities
    int l=0;
    for(int i=0;i<network->entities->tot;i++){
        Entity *e=GetEntity(i);
        if(e->input==0){
            if(i>l){
                network->entities->items[i]=GetEntity(l);
                network->entities->items[l]=e;
                if(selectedEntity==i)selectedEntity=l;
                else if(selectedEntity==l)selectedEntity=i;
            }
            l++;
        }
    }
}

void InitNetwork(int type,int n){
    drawingEdge=draggingEntity=false;
    selectedEntity=selectedNodeI=selectedNodeJ=-1;
    selectedNode=NULL;
    currentRound=-1;
    DoneNetwork();
    network=malloc(sizeof(Network));
    network->entities=NewVector(8);
    network->rounds=NewVector(8);
    switch(type){
        case 1: ExampleNetwork1(n); break;
        case 2: ExampleNetwork2(n); break;
        case 3: ExampleNetwork3(n); break;
        case 4: ExampleNetwork4(n); break;
        case 5: ExampleNetwork5(); break;
        case 6: ExampleNetwork6(); break;
        default: break;
    }
    ExecuteNetwork();
    win1->invalid=true;
    win2->invalid=true;
}

void ExecuteNetwork(void){
    for(int i=0;i<network->entities->tot;i++){
        Entity *e=GetEntity(i);
        if(e->history)FreeHistoryTree(e->history);
        e->current=e->history=NewHistoryTree();
        ExtendHistory(e);
    }
    for(int r=0;r<network->rounds->tot;r++){
        Vector *v=network->rounds->items[r];
        for(int i=0;i<v->tot;i++)
            ExecuteInteraction(v->items[i]);
        for(int i=0;i<network->entities->tot;i++)
            EndRound(GetEntity(i),false,true);
    }
    if(finalHistory)FreeHistoryTree(finalHistory);
    finalHistory=NewHistoryTree();
    for(int i=0;i<network->entities->tot;i++){
        Entity *e=GetEntity(i);
        e->finalLeaf=MergeHistoryTrees(finalHistory,e->history,NULL);
    }
    ComputeAuxData(finalHistory);
}

void DoneNetwork(void){
    if(!network)return;
    FreeAuxData();
    if(finalHistory)FreeHistoryTree(finalHistory);
    finalHistory=NULL;
    for(int i=0;i<network->entities->tot;i++)
        FreeEntity(GetEntity(i));
    for(int r=network->rounds->tot-1;r>=0;r--)
        DeleteRound(r);
    FreeVector(network->entities);
    FreeVector(network->rounds);
    free(network);
    network=NULL;
}

void InsertRound(int index,bool copy){ // copy interactions from previous round?
    InsertVector(network->rounds,index,NewVector(16));
    if(!copy || !index)return;
    Vector *v1=network->rounds->items[index-1];
    Vector *v2=network->rounds->items[index];
    for(int i=0;i<v1->tot;i++){
        Interaction *interaction=v1->items[i];
        AddVector(v2,CopyInteraction(interaction));
    }
}

static void FreeInteraction(Interaction *interaction){
    SetWindowContext(win1);
    if(interaction->label)FreeTexture(interaction->label);
    free(interaction);
}

void DeleteInteractions(int index){
    Vector *v=network->rounds->items[index];
    for(int i=0;i<v->tot;i++)FreeInteraction(v->items[i]);
    while(v->tot)DeleteVector(v,0);
}

void DeleteRound(int index){
    Vector *v=DeinsertVector(network->rounds,index);
    for(int i=0;i<v->tot;i++)FreeInteraction(v->items[i]);
    FreeVector(v);
}

void AddEntity(int input,float x,float y){
    AddVector(network->entities,NewEntity(input,x,y));
    if(input==0)SortLeaders();
}

void DeleteEntity(int index){
    Entity *e=DeinsertVector(network->entities,index);
    for(int r=0;r<network->rounds->tot;r++){
        Vector *v=network->rounds->items[r];
        for(int i=0;i<v->tot;i++){
            Interaction *interaction=v->items[i];
            if(interaction->e1==e || interaction->e2==e){
                DeleteVector(v,i);
                FreeInteraction(interaction);
                i--;
            }
        }
    }
    FreeEntity(e);
}

static int GetInteraction(Vector *v,int i,int j){
    Entity *e1=GetEntity(i);
    Entity *e2=GetEntity(j);
    for(int k=0;k<v->tot;k++){
        Interaction *interaction=v->items[k];
        if(e1==interaction->e1 && e2==interaction->e2)return k;
    }
    return -1;
}

void AddInteraction(int round,int i,int j,int multiplicity){ // interaction from entity i to entity j
    Vector *v=network->rounds->items[round];
    int k=GetInteraction(v,i,j);
    if(k!=-1){
        Interaction *interaction=v->items[k];
        interaction->multiplicity+=multiplicity;
        if(interaction->multiplicity<=0){
            DeleteVector(v,k);
            FreeInteraction(interaction);
        }
        else UpdateInteractionLabel(interaction);
    }
    else if(multiplicity>0)AddVector(v,NewInteraction(i,j,multiplicity));
}

void AddDoubleInteraction(int round,int i,int j,int multiplicity){ // interaction between entity i and entity j
    AddInteraction(round,i,j,multiplicity);
    AddInteraction(round,j,i,multiplicity);
}

int SelectEntityXY(int x,int y){
    float minDist=-1.0f;
    int s=-1;
    for(int i=0;i<network->entities->tot;i++){
        Entity *e=GetEntity(i);
        float cx=ToScreenX(win1,e->x);
        float cy=ToScreenY(win1,e->y);
        float dist=(cx-x)*(cx-x)+(cy-y)*(cy-y);
        if(s==-1 || dist<minDist){ minDist=dist; s=i; }
    }
    if(s!=-1 && minDist<=NODE_SIZE*NODE_SIZE*0.25f)return s;
    else return -1;
}

void CountingAlgorithm(void){
    SelectView();
    ResetAuxDataVariables();
    if(selectedNode && selectedNode->h->level>=0)
        switch(algorithm){
            case 1: StabilizingAlgorithm(); break;
            case 2: TerminatingAlgorithm(); break;
            default: break;
        }
    UpdateAuxDataLabels();
}

bool LoadNetwork(const char *filename){
    FILE *f;
    char buf[1024];
    f=fopen(filename,"rb");
    if(!f)return false;
    drawingEdge=draggingEntity=false;
    selectedEntity=selectedNodeI=selectedNodeJ=-1;
    selectedNode=NULL;
    currentRound=-1;
    DoneNetwork();
    network=malloc(sizeof(Network));
    network->entities=NewVector(8);
    network->rounds=NewVector(8);
    int input,round=-1,r,e1,e2,m,n=0;
    float x,y;
    while(fgets(buf,sizeof(buf),f)){
        if(sscanf(buf,"entity (%d, %f, %f)",&input,&x,&y)==3){
            if(x<-1.0f)x=-1.0f;
            if(x>1.0f)x=1.0f;
            if(y<-1.0f)y=-1.0f;
            if(y>1.0f)y=1.0f;
            AddEntity(input,x,y);
            n++;
        }
        if(sscanf(buf,"round %d",&r)==1)InsertRound(++round,false);
        if(sscanf(buf,"inter (%d, %d, %d)",&e1,&e2,&m)==3){
            if(0<=e1 && e1<n && 0<=e2 && e2<n && m>0)AddInteraction(round,e1,e2,m);
        }
    }
    fclose(f);
    ExecuteNetwork();
    win1->invalid=true;
    win2->invalid=true;
    return true;
}

bool SaveNetwork(const char *filename){
    FILE *f;
    f=fopen(filename,"wb");
    if(!f)return false;
    for(int i=0;i<network->entities->tot;i++){
        Entity *e=GetEntity(i);
        fprintf(f,"entity (%d, %f, %f)\n",e->input,e->x,e->y);
    }
    for(int i=0;i<network->rounds->tot;i++){
        fprintf(f,"\nround %d\n",i+1);
        Vector *v=network->rounds->items[i];
        for(int j=0;j<v->tot;j++){
            Interaction *interaction=v->items[j];
            int e1=GetEntityIndex(interaction->e1);
            int e2=GetEntityIndex(interaction->e2);
            if(e1==-1 || e2==-1)continue;
            fprintf(f,"inter (%d, %d, %d)\n",e1,e2,interaction->multiplicity);
        }
    }
    fclose(f);
    return true;
}
