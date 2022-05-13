typedef struct{
    void **items;
    int mincap,cap,tot;
}Vector;

typedef struct{
    int *items;
    int mincap,cap,tot;
}IntVector;

Vector *NewVector(int m);
int VectorTotal(Vector *v);
int AddVector(Vector *v,void *item);
void SetVector(Vector *v,int i,void *item);
void InsertVector(Vector *v,int i,void *item); // shifts items without changing their order
void *GetVector(Vector *v,int i);
void *DeleteVector(Vector *v,int i);
void *DeinsertVector(Vector *v,int i); // shifts items without changing their order
void FreeVector(Vector *v);

IntVector *NewVectorI(int m);
int VectorTotalI(IntVector *v);
int AddVectorI(IntVector *v,int item);
void SetVectorI(IntVector *v,int i,int item);
int GetVectorI(IntVector *v,int i);
void DeleteVectorI(IntVector *v,int i);
void FreeVectorI(IntVector *v);
