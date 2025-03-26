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

static Interaction *NewInteraction(int i,int j,int multiplicity){
    Interaction *interaction=malloc(sizeof(Interaction));
    interaction->e1=GetEntity(i);
    interaction->e2=GetEntity(j);
    interaction->multiplicity=multiplicity;
    return interaction;
}

static Interaction *CopyInteraction(Interaction *interaction){
    Interaction *interaction2=malloc(sizeof(Interaction));
    interaction2->e1=interaction->e1;
    interaction2->e2=interaction->e2;
    interaction2->multiplicity=interaction->multiplicity;
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
    if(network->rounds->tot)currentRound=0;
    ExecuteNetwork();
    win1->invalid=true;
}

void ExecuteNetwork(void){
    for(int i=0;i<network->entities->tot;i++){
        Entity *e=GetEntity(i);
        if(e->history)FreeHistoryTree(e->history);
        e->outdegree=outAware?0:-1;
        e->current=e->history=NewHistoryTree();
        ExtendHistory(e);
    }
    for(int r=0;r<network->rounds->tot;r++){
        Vector *v=network->rounds->items[r];
        for(int i=0;i<v->tot;i++)
            ExecuteInteraction(v->items[i]);
        for(int i=0;i<network->entities->tot;i++)
            EndRound(GetEntity(i));
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
        float cx=ToScreenX1(win1,e->x);
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
}

static char *EnsureSuffix(const char *string,const char *suffix){
    if(!string)return NULL;
    if(!suffix)return SDL_strdup(string);
    size_t stringLen=SDL_strlen(string);
    size_t suffixLen=SDL_strlen(suffix);
    if(stringLen>=suffixLen && !SDL_strcasecmp(string+stringLen-suffixLen,suffix))return SDL_strdup(string);
    size_t newStringLen=stringLen+suffixLen+1;
    char *newString=SDL_malloc(newStringLen);
    if(!newString)return NULL;
    SDL_strlcpy(newString,string,newStringLen);
    SDL_strlcat(newString,suffix,newStringLen);
    return newString;
}

static bool LoadNetworkHelper(SDL_IOStream *stream){
    #define BUF_SIZE 1024
    char buffer[BUF_SIZE];
    int input,round=-1,r,e1,e2,m,n=0;
    float x,y;
    size_t length=0,readBytes=0;
    char ch;
    Network *backup=network;
    network=malloc(sizeof(Network));
    network->entities=NewVector(8);
    network->rounds=NewVector(8);
    for(;;){
        readBytes=SDL_ReadIO(stream,&ch,1);
        if(!readBytes)break;
        if(length<BUF_SIZE-1)buffer[length++]=ch;
        if(ch=='\n'||length>=BUF_SIZE-1){
            buffer[length]='\0';
            if(sscanf(buffer,"entity (%d, %f, %f)",&input,&x,&y)==3){
                if(x<-1.0f)x=-1.0f;
                if(x>1.0f)x=1.0f;
                if(y<-1.0f)y=-1.0f;
                if(y>1.0f)y=1.0f;
                if(input<0)input=0;
                AddEntity(input,x,y);
                n++;
            }
            else if(sscanf(buffer,"round %d",&r)==1)InsertRound(++round,false);
            else if(sscanf(buffer,"inter (%d, %d, %d)",&e1,&e2,&m)==3){
                if(0<=e1 && e1<n && 0<=e2 && e2<n && m>0)AddInteraction(round,e1,e2,m);
            }
            length=0;
        }
    }
    if(n==0){
        for(int i=0;i<network->entities->tot;i++)FreeEntity(GetEntity(i));
        for(int j=network->rounds->tot-1;j>=0;j--)DeleteRound(j);
        FreeVector(network->entities);
        FreeVector(network->rounds);
        free(network);
        network=backup;
        return false;
    }
    selectedEntity=selectedNodeI=selectedNodeJ=-1;
    selectedNode=NULL;
    currentRound=round==-1?-1:0;
    Network *temp=network;
    network=backup;
    DoneNetwork();
    network=temp;
    ExecuteNetwork();
    win1->invalid=true;
    return true;
}

static bool SaveNetworkHelper(SDL_IOStream *stream){
    for(int i=0;i<network->entities->tot;i++){
        Entity *e=GetEntity(i);
        if(!SDL_IOprintf(stream,"entity (%d, %f, %f)\n",e->input,e->x,e->y))return false;
    }
    for(int i=0;i<network->rounds->tot;i++){
        if(!SDL_IOprintf(stream,"\nround %d\n",i+1))return false;
        Vector *v=network->rounds->items[i];
        for(int j=0;j<v->tot;j++){
            Interaction *interaction=v->items[j];
            int e1=GetEntityIndex(interaction->e1);
            int e2=GetEntityIndex(interaction->e2);
            if(e1==-1 || e2==-1)continue;
            if(!SDL_IOprintf(stream,"inter (%d, %d, %d)\n",e1,e2,interaction->multiplicity))return false;
        }
    }
    return true;
}

#ifdef __EMSCRIPTEN__
EMSCRIPTEN_KEEPALIVE
void HandleLoadedFile(const char *data,int length){
    SDL_IOStream *stream=NULL;
    if(!(stream=SDL_IOFromMem((void*)data,length))){
        free((void*)data);
        DisplayMessage("Failed to load network");
        return;
    }
    if(LoadNetworkHelper(stream) && SDL_CloseIO(stream))DisplayMessage("Network loaded");
    else DisplayMessage("Failed to load network");
    free((void*)data);
}

static void LoadFileHelper(void){
    EM_ASM_({
        const input=document.createElement('input');
        input.type='file';
        input.accept='.txt,*/*';
        input.style.display='none';
        input.onchange=(event)=>{
            const file=event.target.files[0];
            if(!file)return;
            const reader=new FileReader();
            reader.onload=function(e){
                const arrayBuffer=e.target.result;
                const byteArray=new Uint8Array(arrayBuffer);
                const ptr=Module._malloc(byteArray.length);
                Module.HEAPU8.set(byteArray,ptr);
                Module.ccall('HandleLoadedFile',null,['number','number'],[ptr,byteArray.length]);
            };
            reader.readAsArrayBuffer(file);
        };
        document.body.appendChild(input);
        input.click();
        document.body.removeChild(input);
    });
}

static void SaveFileHelper(const char *filename,const char *data,int length){
    char *newFilename=EnsureSuffix(filename,".txt");
    EM_ASM_({
        const filename=UTF8ToString($0);
        const dataPtr=$1;
        const dataLen=$2;
        const data=new Uint8Array(Module.HEAPU8.buffer,dataPtr,dataLen);
        const blob=new Blob([data],{type:'application/octet-stream'});
        const a=document.createElement('a');
        a.href=URL.createObjectURL(blob);
        a.download=filename;
        a.click();
        URL.revokeObjectURL(a.href);
    },newFilename,data,length);
    SDL_free(newFilename);
}

void LoadNetwork(void){
    resizing=drawingEdge=draggingEntity=false;
    DisplayMessage("Loading network");
    LoadFileHelper();
}

void SaveNetwork(void){
    resizing=drawingEdge=draggingEntity=false;
    char *buffer=NULL;
    size_t size=network->entities->tot;
    for(int i=0;i<network->rounds->tot;i++){
        Vector *v=network->rounds->items[i];
        size+=v->tot+1;
    }
    size*=64;
    buffer=malloc(size);
    SDL_IOStream *stream=NULL;
    if(!(stream=SDL_IOFromMem(buffer,size))){
        DisplayMessage("Failed to save network");
        free(buffer);
        return;
    }
    if(!SaveNetworkHelper(stream) || !SDL_FlushIO(stream)){
        SDL_CloseIO(stream);
        DisplayMessage("Failed to save network");
        free(buffer);
        return;
    }
    SaveFileHelper(FILENAME,buffer,SDL_TellIO(stream));
    if(SDL_CloseIO(stream))DisplayMessage("Saving network");
    else DisplayMessage("Failed to save network");
    free(buffer);
}
#else
static const SDL_DialogFileFilter filters[]={
    {"TXT files","txt"},
    {"All files","*"}
};

static void SDLCALL LoadFileCallback(void *userdata,const char *const *filelist,int filter){
    (void)userdata; (void)filter;
    if(!filelist || !*filelist)return;
    SDL_Event event;
    SDL_zero(event);
    event.type=SDL_EVENT_LOAD_NETWORK;
    event.user.data1=SDL_strdup(*filelist);
    SDL_PushEvent(&event);
}

static void SDLCALL SaveFileCallback(void *userdata,const char *const *filelist,int filter){
    (void)userdata; (void)filter;
    if(!filelist || !*filelist)return;
    SDL_Event event;
    SDL_zero(event);
    event.type=SDL_EVENT_SAVE_NETWORK;
    event.user.data1=EnsureSuffix(*filelist,".txt");
    SDL_PushEvent(&event);
}

void LoadNetwork(void){
    SDL_ShowOpenFileDialog(LoadFileCallback,NULL,win1->window,filters,2,SDL_GetBasePath(),false);
}

void SaveNetwork(void){
    SDL_ShowSaveFileDialog(SaveFileCallback,NULL,win1->window,filters,2,SDL_GetBasePath());
}

void LoadNetworkRun(const char *filename){
    resizing=drawingEdge=draggingEntity=false;
    SDL_IOStream *stream=NULL;
    if(!(stream=SDL_IOFromFile(filename,"rb"))){
        DisplayMessage("Failed to load network");
        SDL_free((char*)filename);
        return;
    }
    if(LoadNetworkHelper(stream) && SDL_CloseIO(stream))DisplayMessage("Network loaded");
    else DisplayMessage("Failed to load network");
    SDL_free((char*)filename);
}

void SaveNetworkRun(const char *filename){
    resizing=drawingEdge=draggingEntity=false;
    SDL_IOStream *stream=NULL;
    if(!(stream=SDL_IOFromFile(filename,"wb"))){
        DisplayMessage("Failed to save network");
        SDL_free((char*)filename);
        return;
    }
    if(SaveNetworkHelper(stream) && SDL_CloseIO(stream))DisplayMessage("Network saved");
    else DisplayMessage("Failed to save network");
    SDL_free((char*)filename);
}
#endif
