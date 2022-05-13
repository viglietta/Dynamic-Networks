#include "main.h"

bool *locked=NULL; // levels containing a guessed non-counted node
int unguessableLevels=2; // the first level that may be guessable
int countSteps=0;

static bool Guessed(AuxData *node){ // is the node guessed?
    return node->guess!=-1 && !node->counted;
}

static int Guessable(AuxData *node){ // return the index of a guesser or -1 if the node is not guessable
    if(!node->visible)return -1; // the node must be visible
    for(int i=0;i<node->observations->tot;i++){ // scan all red edges
        int obs=node->observations->items[i]; // get observed node's index
        if(obs==node->parent)continue; // the node's parent cannot be its guesser
        if(GetAuxData(node->i-1,obs)->guesser)return i; // if the observed node is a guesser, return its index
    }
    return -1; // no guesser was found
}

static int CumulativeMultiplicities(AuxData *guesser,AuxData *guessable){ // estimated number of links between guesser and guessable's parent
    int parent=guessable->parent; // get guessable's parent's index
    int sum=0;
    for(int i=0;i<guesser->children->tot;i++){
        AuxData *child=GetAuxData(guesser->i+1,guesser->children->items[i]); // get the next child of guesser
        if(!child->visible)continue; // the child must be visible
        for(int j=0;j<child->observations->tot;j++){ // check if the child observed the guessable node's parent
            if(child->observations->items[j]!=parent)continue;
            sum+=child->anonymity*child->multiplicities->items[j]; // increase sum by the number of links from the child to the guessable node's parent
        }
    }
    return sum;
}

static AuxData *MarkCounted(AuxData *node){ // return the nearest counted ancestor of the node, or NULL if there are none
    if(!Guessed(node))return NULL;
    if(node->h->input){ // not a leader's node
        locked[node->i]=false; // unlock level, because the node is no longer guessed
        unguessableLevels=node->i; // this level may have become guessable
    }
    else node->guesser=true; // leaders are guessers
    node->counted=true; // mark node as counted
    node->weight=0;
    int delta=node->guess-node->cumulativeAnonymity; // variation of cumulative anonymity to be propagated to ancestors
    while(node->i>0){ // scan the black path from the counted node to the root
        node=GetAuxData(node->i-1,node->parent); // move to node's parent
        node->cumulativeAnonymity+=delta; // account for newly counted entities
        if(node->counted)return node; // if the node is counted, return it
    }
    return NULL; // no counted node was found on the black path to the root
}

static int ResolveCompleteIsleHelper(AuxData *node){ // return the number of guessed nodes that became counted
    int weightLoss=0;
    if(node->guess!=-1 && !node->counted){ // the node is guessed (and not counted)
        locked[node->i]=false; // unlock the level
        weightLoss=1; // the weight of its ancestors should decrease
    }
    node->guess=node->cumulativeAnonymity; // the node's anonymity is the sum of its (visible) children's anonymities
    node->counted=true; // mark node as counted
    node->weight=0;
    node->guesser=true; // the node is now a guesser
    for(int i=0;i<node->children->tot;i++){
        AuxData *child=GetAuxData(node->i+1,node->children->items[i]); // get node's child
        if(!child->counted)weightLoss+=ResolveCompleteIsleHelper(child); // resolve the (complete) isle rooted at the child
    }
    return weightLoss;
}

static void ResolveCompleteIsle(AuxData *node){ // count all the internal nodes of a complete isle given its root
    int weightLoss=ResolveCompleteIsleHelper(node); // mark all internal nodes as counted and get the weight loss
    if(unguessableLevels>node->i+1)unguessableLevels=node->i+1; // the modified levels may have become guessable
    while(node->i>0){ // scan the black path from the isle's root to the history tree's root
        node=GetAuxData(node->i-1,node->parent);
        if(node->weight>0)node->weight-=weightLoss; // decrease every guessed node's weight
    }
}

static int CountingCut(AuxData *node){ // return the deepest level in a cut for the node, or -1 if there is no counting cut
    if(node->counted)return node->h->level; // if the node is counted, return its level
    int deepest=-1;
    for(int i=0;i<node->children->tot;i++){
        AuxData *child=GetAuxData(node->i+1,node->children->items[i]); // get node's child
        if(!child->visible)continue; // skip nodes not in the view
        int deepestChild=CountingCut(child); // find the deepest level in a cut for the child
        if(deepestChild==-1)return -1; // if the child has no cut, there is no cut for the node
        if(deepestChild>deepest)deepest=deepestChild; // store the deepest level
    }
    return deepest;
}

static bool LastAlgorithmStep(void){
    if(numSteps==-1 || ++countSteps<numSteps)return false;
    if(locked)free(locked);
    locked=NULL;
    return true;
}

bool TerminatingAlgorithm(void){ // terminates with correct output by round 3n-2
    if(numSteps==0)return false;
    countSteps=0;
    int numLevels; // number of levels containing a visible leader node
    for(numLevels=1;numLevels<aux->tot;numLevels++){ // scan all levels excluding the root
        AuxData *first=GetAuxData(numLevels,0); // assuming the leader's node is always the first in each level
        if(!first->visible || first->h->input)break; // stop when a level does not contain a visible leader node
        first->guess=1; // the leader is unique; its node's anonymity is 1
        MarkCounted(first);
    }
    if(LastAlgorithmStep())return false;
    locked=malloc(sizeof(bool)*numLevels);
    for(int i=0;i<numLevels;i++)locked[i]=false; // initially, no levels contain a guessed non-counted node
    unguessableLevels=2; // initially, only the first two levels are certainly not guessable
    while(unguessableLevels<numLevels){ // keep looping as long as there are guessable levels
        if(locked[unguessableLevels]){ // the level already contains a guessed node; skip it
            unguessableLevels++;
            continue;
        }
        AuxData *guessable=NULL;
        int sumMultiplicities=0,multiplicity=0;
        Vector *v=GetLevel(unguessableLevels);
        for(int j=0;j<v->tot;j++){ // search for a guessable node in the current level
            AuxData *node=v->items[j]; // get next node in the level
            if(node->counted)continue; // counted nodes are not guessable
            int obs=Guessable(node); // find a guesser for the node
            if(obs==-1)continue; // no guesser was found; the node is not guessable
            AuxData *guesser=GetAuxData(unguessableLevels-1,node->observations->items[obs]); // get the guesser
            sumMultiplicities=CumulativeMultiplicities(guesser,node); // get number of times the the guesser's children observed the node's parent
            if(sumMultiplicities==0)continue; // no children of guesser observed the node's parent; skip it
            multiplicity=node->multiplicities->items[obs]; // get number of times the node observed the guesser
            guessable=node; // the node is guessable
            break; // a guessable node was found; stop searching
        }
        unguessableLevels++;
        if(!guessable)continue; // the level does not contain a guessable non-counted node; search next level
        guessable->guess=(sumMultiplicities-1)/multiplicity+1; // assign a guess to guessable
        if(LastAlgorithmStep())return false;
        locked[guessable->i]=true; // locked guessable's level
        AuxData *node=guessable;
        while(node->i>1){ // scan the black path from the guessed node to the root
            AuxData *parent=GetAuxData(node->i-1,node->parent); // get node's parent
            if(node->guess==-1){ node=parent; continue; } // the node is not guessed; skip it
            node->weight++; // increment the node's weight
            if(node->weight<node->guess){ node=parent; continue; } // the node is not heavy; proceed along the path
            AuxData *high=MarkCounted(node); // the node is heavy; mark it as counted
            if(high && high->cumulativeAnonymity==high->guess)ResolveCompleteIsle(high); // the node is a leaf of a complete isle
            if(node->cumulativeAnonymity==node->guess)ResolveCompleteIsle(node); // the node is the root of a complete isle
            break; // all other weights stay the same
        }
    }
    free(locked);
    locked=NULL;
    AuxData *root=GetAuxData(0,0); // get the root of the history tree
    int deepest=CountingCut(root); // get the level of a deepest node in a counting cut
    if(deepest==-1 || selectedNode->h->level<deepest+root->cumulativeAnonymity) // there are no counting cuts or the terminating condition is not satisfied
        return false; // the algorithm did not succeed
    root->guess=root->cumulativeAnonymity; // assign the correct count to the root
    for(int j=0;j<GetLevel(1)->tot;j++){
        AuxData *node=GetAuxData(1,j);
        if(node->visible)node->guess=node->cumulativeAnonymity; // assign the correct count to the root's children
    }
    return true; // the algorithm succeeded

}
