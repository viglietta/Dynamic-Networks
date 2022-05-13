#include "main.h"

static int NumChildren(AuxData *data){ // return number of visible children
    int num=0;
    for(int i=0;i<data->children->tot;i++){
        int c=data->children->items[i];
        AuxData *child=GetAuxData(data->i+1,c);
        if(child->visible)num++; // only consider visible children
    }
    return num;
}

static AuxData *GetFirstChild(AuxData *data){ // return first visible child; NULL if none
    for(int i=0;i<data->children->tot;i++){
        int c=data->children->items[i];
        AuxData *child=GetAuxData(data->i+1,c);
        if(child->visible)return child; // only consider visible children
    }
    return NULL;
}

bool StabilizingAlgorithm(void){ // stabilizes on correct output by round 2n-2
    if(numSteps==0)return false;
    int countSteps=0;
    bool guessedLevel=-1; // level where all visible nodes have been guessed; -1 if none
    for(int i=1;i<aux->tot;i++){ // scan all levels excluding the root
        Vector *v=GetLevel(i);
        bool branching=false; // are some visible nodes in this level branching?
        int visibleNodes=0; // number of visible nodes in this level
        int leader=-1; // index of leader's node; -1 if none
        for(int j=0;j<v->tot;j++){
            AuxData *data=GetAuxData(i,j);
            if(!data->visible)continue; // node is not visible; skip it
            visibleNodes++;
            int num=NumChildren(data);
            if(num==0)return false; // the next level in the view is incomplete; the algorithm fails
            if(num>1){ branching=true; break; } // the node is branching
            if(!data->h->input)leader=j; // the leader's node has been found
        }
        if(leader==-1)return false; // this level has no leader node; the algorithm fails
        if(branching)continue; // this level has a branching node; skip this level
        Queue *q=NewQueue();
        AuxData *leaderNode=GetAuxData(i,leader);
        leaderNode->guess=GetAuxData(leaderNode->i+1,leaderNode->children->items[0])->guess=1; // the leader is unique; its node's anonymity is 1
        AppendQueue(q,leaderNode); // put leader's node in the queue
        int guessed=1; // number of guessed nodes
        while(!IsQueueEmpty(q)){
            if(numSteps!=-1 && ++countSteps>=numSteps){
                FreeQueue(q);
                return false;
            }
            AuxData *node1=PopQueue(q); // get next (non-branching) node from the queue
            AuxData *child1=GetFirstChild(node1); // get node1's unique visible child
            for(int j=0;j<child1->observations->tot;j++){
                int obs1=child1->observations->items[j]; // get index of node observed by child1
                AuxData *node2=GetAuxData(node1->i,obs1); // get observed node
                if(!node2->visible || node2->guess!=-1)continue; // node2 is not visible or has already been guessed; skip it
                AuxData *child2=GetFirstChild(node2); // get node2's unique visible child
                int mult2=0;
                for(int k=0;k<child2->observations->tot;k++){ // search for node1 among nodes observed by child2
                    int obs2=child2->observations->items[k]; // get index of node observed by child2
                    if(obs2!=node1->j)continue; // the observed node is not node1; skip it
                    mult2=child2->multiplicities->items[k]; // get multiplicity of child2's observation
                }
                if(!mult2)continue; // node1 and node2 do not form an exposed pair; skip node2
                int mult1=child1->multiplicities->items[j]; // get multiplicity of child1's observation
                node2->guess=child2->guess=node1->guess*mult1/mult2; // guess node2's anonymity
                AppendQueue(q,node2); // add node2 to the queue
                guessed++;
            }
        }
        FreeQueue(q);
        if(guessed==visibleNodes){ // all visible nodes in the level have been guessed; stop scanning levels
            guessedLevel=i;
            break;
        }
        for(int j=0;j<v->tot;j++)((AuxData*)v->items[j])->guess=-1; // undo all guesses and scan next level
    }
    if(guessedLevel==-1)return false; // no level has been completely guessed
    for(int i=guessedLevel-1;i>=0;i--){ // propagate guesses to other visible nodes up to the root
        Vector *v=GetLevel(i);
        for(int j=0;j<v->tot;j++){
            AuxData *node=GetAuxData(i,j);
            if(!node->visible)continue;
            node->guess=0;
            for(int k=0;k<node->children->tot;k++){
                int c=node->children->items[k];
                AuxData *child=GetAuxData(i+1,c);
                if(!child->visible || child->guess==-1)continue;
                node->guess+=child->guess; // add together guesses of visible children
            }
        }
    }
    return true;
}
