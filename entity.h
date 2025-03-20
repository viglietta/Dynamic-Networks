typedef struct{
    int input; // 0: leader
    HistoryTree *history; // view of the history tree
    HistoryTree *current; // current node of the history tree (i.e., deepest node)
    Vector *mailbox; // messages received in current round; Vector of Observation
    int outdegree; // messages sent in previous round; -1 if not outAware
    float x,y; // used for drawing
    HistoryTree *finalLeaf; // leaf in the final history tree representing entity
}Entity;

extern bool outAware;

Entity *NewEntity(int input,float x,float y);
void FreeEntity(Entity *e); // assumes mailbox to be empty
bool IsLeader(Entity *e);
void ExtendHistory(Entity *e); // extend history of e by one level
void EndRound(Entity *e);
void SendHistory(Entity *e1,Entity *e2,int multiplicity); // sends the history from e1 to e2
void Interact(Entity *e1,Entity *e2); // two-way single link between e1 and e2
