typedef struct QueueElement{
    void *item;
    struct QueueElement *next;
}QueueElement;

typedef struct{
    QueueElement *head;
    QueueElement *tail;
    int tot;
}Queue;

Queue *NewQueue(void);
void FreeQueue(Queue *q);
bool IsQueueEmpty(Queue *q);
void AppendQueue(Queue *q,void *item);
void *PopQueue(Queue *q);
