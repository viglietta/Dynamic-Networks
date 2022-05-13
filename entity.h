typedef struct{
    int input; // 0: leader
    HistoryTree *history; // view of the history tree
    HistoryTree *current; // current node of the history tree (i.e., deepest node)
    Vector *mailbox; // messages received in current round; Vector of Observation
    float x,y; // used for drawing
    Texture *label;
    HistoryTree *finalLeaf; // leaf in the final history tree representing entity
}Entity;

void UpdateEntityLabel(Entity *e);
Entity *NewEntity(int input,float x,float y);
void FreeEntity(Entity *e); // assumes mailbox to be empty
bool IsLeader(Entity *e);
Entity *CopyEntity(Entity *e); // assumes mailbox to be empty
void ExtendHistory(Entity *e); // extend history of e by one level
int EndRound(Entity *e,bool count,bool careful);
void SendHistory(Entity *e1,Entity *e2,int multiplicity); // sends the history from e1 to e2
void Interact(Entity *e1,Entity *e2); // two-way single link between e1 and e2
