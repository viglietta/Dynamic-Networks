#include "main.h"

Queue *NewQueue(void){
    Queue *q=malloc(sizeof(Queue));
    q->tot=0;
    q->head=NULL;
    q->tail=NULL;
    return q;
}

void FreeQueue(Queue *q){
    while(!IsQueueEmpty(q))PopQueue(q);
    free(q);
}

bool IsQueueEmpty(Queue *q){
    return !q->tot;
}

void AppendQueue(Queue *q,void *item){
    QueueElement *e=malloc(sizeof(QueueElement));
    e->item=item;
    e->next=NULL;
    if(q->tail)q->tail->next=e;
    q->tail=e;
    if(!q->head)q->head=q->tail;
    q->tot++;
}

void *PopQueue(Queue *q){
    if(IsQueueEmpty(q))exit(EXIT_FAILURE);
    void *item=q->head->item;
    QueueElement *next=q->head->next;
    free(q->head);
    q->head=next;
    if(!q->head)q->tail=NULL;
    q->tot--;
    return item;
}
