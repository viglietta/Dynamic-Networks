#include "main.h"

static void FanInteract(int round,int a,int b1,int b2,int m){
    for(int i=b1;i<=b2;i++)AddDoubleInteraction(round,a,i,m);
}

static void PathInteract(int round,int a,int b,int m){
    for(int i=a;i<b;i++)AddDoubleInteraction(round,i,i+1,m);
}

static void CycleInteract(int round,int a,int b,int m){
    PathInteract(round,a,b,m);
    AddDoubleInteraction(round,a,b,m);
}

void ExampleNetwork1(int n){ // terminating algorithm's worst case
    for(int i=0;i<n;i++){
        float alpha=i*2.0f*M_PI/n;
        AddEntity(i?1:0,0.75f*sin(alpha),-0.75f*cos(alpha));
    }
    for(int r=1;r<=3*n-3;r++){
        InsertRound(r-1,false);
        if(r<=n-2){
            FanInteract(r-1,0,1,n-r,1);
            AddDoubleInteraction(r-1,0,n-r,1);
            PathInteract(r-1,n-r,n-1,1);
        }
        else PathInteract(r-1,0,n-1,1);
    }
}

void ExampleNetwork2(int n){ // lower bound of 2n-c rounds
    for(int i=0;i<n;i++){
        float alpha=i*2.0f*M_PI/n;
        AddEntity(i?1:0,0.75f*sin(alpha),-0.75f*cos(alpha));
    }
    for(int r=1;r<=2*n-1;r++){
        InsertRound(r-1,false);
        PathInteract(r-1,0,n-1,1);
        if(r<n-1)AddDoubleInteraction(r-1,r,n-1,1);
        else AddInteraction(r-1,n-1,n-1,2);
    }
}

void ExampleNetwork3(int n){ // lower bound of 1.5n-2 rounds
    for(int i=0;i<n;i++){
        float alpha=i*2.0f*M_PI/n;
        AddEntity(i?1:0,0.75f*sin(alpha),-0.75f*cos(alpha));
    }
    int m=n/2;
    for(int r=1;r<=3*m-2;r++){
        InsertRound(r-1,false);
        if(r<m)CycleInteract(r-1,0,n-1,1);
        else{
            PathInteract(r-1,0,m-1,1);
            AddDoubleInteraction(r-1,m-1,n-1,1);
            PathInteract(r-1,m,n-1,1);
        }
    }
}

void ExampleNetwork4(int n){ // the stabilizing approach fails for an arbitrarily long time
    int l=0,a=n-3,a1=1,a2=a,b1=a+1,b2=a+2;
    for(int i=0;i<n;i++){
        float alpha=((i==b2?0:i==0?b2:i)+1)*2.0f*M_PI/n;
        AddEntity(i?i<=a?1:2:0,0.75f*sin(alpha),-0.75f*cos(alpha));
    }
    int t=n%2==0?n/2+1:n/2+2;
    for(int r=1;r<=t;r++){
        InsertRound(r-1,false);
        if(r==1){
            FanInteract(r-1,l,b1,b2,1);
            AddDoubleInteraction(r-1,b2,a1,1);
            FanInteract(r-1,b1,a1+1,a2,1);
        }
        else if(r==2 || r>a){
            AddDoubleInteraction(r-1,l,b2,1);
            AddDoubleInteraction(r-1,b2,a1,1);
            PathInteract(r-1,a1,a2,1);
            AddDoubleInteraction(r-1,a2,b1,1);
        }
        else{
            AddDoubleInteraction(r-1,l,b2,1);
            AddDoubleInteraction(r-1,b2,a1,1);
            AddDoubleInteraction(r-1,a1,a1+r-1,1);
            PathInteract(r-1,a1+r-1,a2,1);
            AddDoubleInteraction(r-1,a2,b1,1);
            FanInteract(r-1,b1,a1+1,a1+r-2,1);
        }
    }
}

void ExampleNetwork5(void){ // boldi-codenotti bipartite static anonymous network
    AddEntity(1,-0.5f,-2.0f/3);
    AddEntity(1,0.0f,-2.0f/3);
    AddEntity(1,0.5f,-2.0f/3);
    AddEntity(1,-0.5f,-1.0f/3);
    AddEntity(1,0.0f,-1.0f/3);
    AddEntity(1,0.5f,-1.0f/3);
    AddEntity(1,-0.5f,0.0f/3);
    AddEntity(1,0.0f,0.0f/3);
    AddEntity(1,0.5f,0.0f/3);
    AddEntity(1,-0.5f,1.0f/3);
    AddEntity(1,0.0f,1.0f/3);
    AddEntity(1,0.5f,1.0f/3);
    AddEntity(1,-0.5f,2.0f/3);
    AddEntity(1,0.0f,2.0f/3);
    AddEntity(1,0.5f,2.0f/3);

    for(int r=1;r<=10;r++){
        InsertRound(r-1,false);
        AddDoubleInteraction(r-1,0,1,1);
        AddDoubleInteraction(r-1,1,2,1);
        AddDoubleInteraction(r-1,3,4,1);
        AddDoubleInteraction(r-1,4,5,1);
        AddDoubleInteraction(r-1,6,7,1);
        AddDoubleInteraction(r-1,7,8,1);
        AddDoubleInteraction(r-1,9,10,1);
        AddDoubleInteraction(r-1,10,11,1);
        AddDoubleInteraction(r-1,12,13,1);
        AddDoubleInteraction(r-1,13,14,1);
        AddDoubleInteraction(r-1,0,4,1);
        AddDoubleInteraction(r-1,1,3,1);
        AddDoubleInteraction(r-1,1,5,1);
        AddDoubleInteraction(r-1,2,7,1);
        AddDoubleInteraction(r-1,4,8,1);
        AddDoubleInteraction(r-1,6,10,1);
        AddDoubleInteraction(r-1,7,12,1);
        AddDoubleInteraction(r-1,9,13,1);
        AddDoubleInteraction(r-1,10,14,1);
        AddDoubleInteraction(r-1,11,13,1);
    }
}

void ExampleNetwork6(void){ // regular static anonymous network
    int n=10;
    for(int i=0;i<n;i++){
        float alpha=i*2.0f*M_PI/n;
        AddEntity(1,0.75f*sin(alpha),-0.75f*cos(alpha));
    }
    for(int r=1;r<=10;r++){
        InsertRound(r-1,false);
        AddDoubleInteraction(r-1,0,1,1);
        AddDoubleInteraction(r-1,0,4,1);
        AddDoubleInteraction(r-1,0,7,1);
        AddDoubleInteraction(r-1,1,2,1);
        AddDoubleInteraction(r-1,2,3,1);
        AddDoubleInteraction(r-1,3,1,1);
        AddDoubleInteraction(r-1,4,5,1);
        AddDoubleInteraction(r-1,5,6,1);
        AddDoubleInteraction(r-1,6,4,1);
        AddDoubleInteraction(r-1,7,8,1);
        AddDoubleInteraction(r-1,8,9,1);
        AddDoubleInteraction(r-1,9,7,1);
        AddDoubleInteraction(r-1,3,5,1);
        AddDoubleInteraction(r-1,6,8,1);
        AddDoubleInteraction(r-1,9,2,1);
    }
}
