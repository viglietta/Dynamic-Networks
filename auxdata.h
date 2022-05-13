typedef struct{ // inherits the structure of a history tree and adds more data that is used for rendering and computing
    int i,j; // indices in aux
    HistoryTree *h;
    int parent; // index of parent in previous level
    IntVector *children; // black edges; indices of children in next level
    IntVector *observations; // red edges; indices of nodes in previous level
    IntVector *multiplicities; // multiplicities of red edges
    int width; // used for drawing the history tree
    float x,y,dx,dy; // used for drawing the history tree
    Texture *label; // label to be displayed in the node
    Vector *linkLabels; // Vector of Texture; corresponds to multiplicities of upward red edges
    int anonymity; // computed knowing the network information; not used by counting algorithms
    bool visible; // the node is part of the selected view
    int guess; // estimated anonymity; -1 if no guess has been made
    bool counted; // the guess has been proved correct (only used by the terminating algorithm)
    int weight; // number of guessed nodes in subtree (only used by the terminating algorithm)
    int cumulativeAnonymity; // sum of known anonymities of children (only used by the terminating algorithm)
    bool guesser; // this node and all its visible children are counted (only used by the terminating algorithm)
}AuxData;

extern Vector *aux; // Vector of Vector of AuxData
extern AuxData *selectedNode;

Vector *GetLevel(int i);
AuxData *GetAuxData(int i,int j);
void ResetAuxDataVariables(void);
void ComputeAuxData(HistoryTree *h);
void FreeAuxData(void);
void UpdateAuxDataLabels(void);
void SelectView(void);
void SelectNodeXY(int x,int y,int *si,int *sj);
bool CorrespondsToSelectedNode(Entity *e);
Entity *FirstEntityCorrespondingToSelectedNode(void);
void SelectNodeFromEntity(Entity *e);
void IncrementSelectedNodeI(void);
void DecrementSelectedNodeI(void);
bool IncrementSelectedNodeJ(void);
bool DecrementSelectedNodeJ(void);
