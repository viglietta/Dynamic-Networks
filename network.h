typedef struct{
    Entity *e1,*e2;
    int multiplicity;
    Texture *label;
}Interaction;

typedef struct{
    Vector *entities; // Vector of Entity
    Vector *rounds; // dynamic network; Vector of Vector of Interaction
}Network;

extern Network *network;
extern int currentRound;
extern HistoryTree *finalHistory; // history tree to be displayed in window 2
extern int selectedEntity;
extern int selectedNodeI,selectedNodeJ;
extern bool drawingEdge;
extern bool draggingEntity;
extern int algorithm;
extern int numSteps; // number of counting algorithm steps to perform (-1: infinity)

Entity *GetEntity(int i);
int GetEntityIndex(Entity *e);
void SortLeaders(void); // put all leaders at the beginning of the list of entities
void InitNetwork(int type,int n);
void ExecuteNetwork(void);
void DoneNetwork(void);
void InsertRound(int index,bool copy);
void DeleteInteractions(int index);
void DeleteRound(int index);
void AddEntity(int input,float x,float y);
void DeleteEntity(int index);
void AddInteraction(int round,int i,int j,int multiplicity); // interaction from entity i to entity j
void AddDoubleInteraction(int round,int i,int j,int multiplicity); // interaction between entity i and entity j
int SelectEntityXY(int x,int y);
void CountingAlgorithm(void);
bool LoadNetwork(const char *filename);
bool SaveNetwork(const char *filename);
