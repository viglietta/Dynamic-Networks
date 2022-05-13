typedef struct{ // used for messages between entities and for red edges
    struct HistoryTree *history; // history observed
    int multiplicity; // number of times observed
}Observation;

typedef struct HistoryTree{
    int input; // 0: leader
    int level; // level in the history tree (i.e., round number)
    struct HistoryTree *parent; // parent node
    Vector *children; // children nodes; Vector of HistoryTree
    Vector *observations; // red edges to the previous level; Vector of Observation
    struct HistoryTree *reference; // target of homomorphism (only used when merging history trees)
    void *data; // only used in finalHistory
}HistoryTree;

HistoryTree *NewHistoryTree(void); // creates new root
void FreeHistoryTree(HistoryTree *h);
Observation *NewObservation(HistoryTree *history,int multiplicity);
void AddRedEdge(HistoryTree *h1,HistoryTree *h2,int multiplicity); // create red edge between h1 and h2, assuming h1 is in lower level
HistoryTree *AddHistoryTreeChild(HistoryTree *h,int input); // adds a child node to h with given input and return it
HistoryTree *MergeHistoryTrees(HistoryTree *h1,HistoryTree *h2,bool *added); // result is in h1; returns endpoint in new tree and whether any nodes were added
HistoryTree *CopyHistoryTree(HistoryTree *h,HistoryTree **deepest); // returns copied tree and deepest node
bool HistoryTreeContains(HistoryTree *h1,HistoryTree *h2); // does h1 contain a homeomorphic copy of h2?
bool HistoryTreeEquals(HistoryTree *h1,HistoryTree *h2); // is h1 homeomorphic to h2?
void PrintHistoryTree(HistoryTree *h);
