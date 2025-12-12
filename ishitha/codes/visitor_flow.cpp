#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <cstdlib>

using namespace std;

const int MAXR = 10050;
const int MAXZ = 210;
const int MAXN = 150;
const int MAXE = 20000;
const int INF   = 1000000000;

/* ==== CSV DATA ARRAYS ==== */
int VisitorID[MAXR];
int Zone[MAXR];
int TimeAt[MAXR];
int Density[MAXR];
int U[MAXR], V[MAXR], W[MAXR];
int TaskPriority[MAXR];
int ROWS = 0;

/* ==== Load CSV ==== */
void loadCSV(const char *file) {
    ifstream fin(file);
    if(!fin){ cout<<"CSV file missing.\n"; exit(1); }

    string line;
    getline(fin, line); // header

    while(getline(fin, line) && ROWS < MAXR-1) {
        stringstream ss(line);
        string t;

        getline(ss,t,','); VisitorID[ROWS] = atoi(t.c_str());
        getline(ss,t,','); Zone[ROWS] = atoi(t.c_str());
        getline(ss,t,','); TimeAt[ROWS] = atoi(t.c_str());
        getline(ss,t,','); Density[ROWS] = atoi(t.c_str());
        getline(ss,t,','); U[ROWS] = atoi(t.c_str());
        getline(ss,t,','); V[ROWS] = atoi(t.c_str());
        getline(ss,t,','); W[ROWS] = atoi(t.c_str());
        getline(ss,t,','); TaskPriority[ROWS] = atoi(t.c_str());

        ROWS++;
    }
    fin.close();
}

/* ======================================================
   =========== 1. HASH TABLE (Visitor Logging) ===========
   ====================================================== */

const int HSIZE = 20011;

struct Slot {
    int id;
    int zone;
    bool used;
    Slot(){ id=0; zone=0; used=false; }
} HT[HSIZE];

int hashF(int x){ return (x * 1315423911u) % HSIZE; }

void addHash(int id, int zone){
    int h = hashF(id);
    for(int i=0;i<HSIZE;i++){
        int p = (h+i) % HSIZE;
        if(!HT[p].used){
            HT[p].used=true;
            HT[p].id=id;
            HT[p].zone=zone;
            return;
        }
    }
}

int findHash(int id){
    int h = hashF(id);
    for(int i=0;i<HSIZE;i++){
        int p=(h+i)%HSIZE;
        if(!HT[p].used) return -1;
        if(HT[p].used && HT[p].id==id) return HT[p].zone;
    }
    return -1;
}

/* ======================================================
   =========== 2. SEGMENT TREE (Zone Density) ============
   ====================================================== */

int seg[4*MAXZ];

void buildSeg(int idx,int l,int r){
    if(l==r){
        seg[idx] = 0;
        return;
    }
    int mid=(l+r)/2;
    buildSeg(idx*2,l,mid);
    buildSeg(idx*2+1,mid+1,r);
    seg[idx] = 0;
}

void updateSeg(int idx,int l,int r,int pos,int val){
    if(l==r){
        seg[idx] += val;
        return;
    }
    int mid=(l+r)/2;
    if(pos<=mid) updateSeg(idx*2,l,mid,pos,val);
    else updateSeg(idx*2+1,mid+1,r,pos,val);

    seg[idx] = seg[idx*2] + seg[idx*2+1];
}

int querySeg(int idx,int l,int r,int ql,int qr){
    if(qr<l || ql>r) return 0;
    if(ql<=l && r<=qr){
        return seg[idx];
    }
    int mid=(l+r)/2;
    return querySeg(idx*2,l,mid,ql,qr)
         + querySeg(idx*2+1,mid+1,r,ql,qr);
}

/* ======================================================
   =========== 3. BELLMAN-FORD Crowd Routing ============
   ====================================================== */

int head[MAXN], toE[MAXE], wE[MAXE], nxt[MAXE], ECNT;
int distBF[MAXN];
int parentBF[MAXN];

void initGraph(){
    ECNT=0;
    for(int i=0;i<MAXN;i++) head[i]=-1;
}

void addEdge(int u,int v,int w){
    toE[ECNT]=v;
    wE[ECNT]=w;
    nxt[ECNT]=head[u];
    head[u]=ECNT++;
}

void bellmanFord(int src){
    for(int i=0;i<MAXN;i++){
        distBF[i]=INF;
        parentBF[i]=-1;
    }
    distBF[src]=0;

    for(int it=0;it<MAXN-1;it++){
        bool changed=false;
        for(int u=1;u<MAXN;u++){
            for(int e=head[u]; e!=-1; e=nxt[e]){
                int v=toE[e], w=wE[e];
                if(distBF[u]<INF && distBF[u]+w<distBF[v]){
                    distBF[v] = distBF[u]+w;
                    parentBF[v] = u;
                    changed = true;
                }
            }
        }
        if(!changed) break;
    }
}

/* ======================================================
   =========== 4. MIN-HEAP (Crowd Tasks Priority) ========
   ====================================================== */

struct HeapNode { int pr,id; };

HeapNode heapArr[MAXR];
int heapSz=0;

void swapH(int a,int b){
    HeapNode t=heapArr[a];
    heapArr[a]=heapArr[b];
    heapArr[b]=t;
}

void pushHeap(int pr,int id){
    heapArr[++heapSz].pr = pr;
    heapArr[heapSz].id = id;

    int i=heapSz;
    while(i>1 && heapArr[i].pr < heapArr[i/2].pr){
        swapH(i,i/2);
        i/=2;
    }
}

HeapNode popHeap(){
    HeapNode top = heapArr[1];
    heapArr[1] = heapArr[heapSz--];

    int i=1;
    while(true){
        int l=i*2, r=l+1, sm=i;
        if(l<=heapSz && heapArr[l].pr < heapArr[sm].pr) sm=l;
        if(r<=heapSz && heapArr[r].pr < heapArr[sm].pr) sm=r;
        if(sm==i) break;
        swapH(i,sm);
        i=sm;
    }
    return top;
}

/* ======================================================
   ======================== MAIN ========================
   ====================================================== */

int main(){

    loadCSV("samarthaka_visitor_flow.csv");
    cout<<"Loaded rows: "<<ROWS<<"\n";

    /* ---------------- HASHING ---------------- */
    cout<<"\n=== REAL-TIME VISITOR LOGGING (Hash) ===\n";
    for(int i=0;i<1000;i++){
        addHash(VisitorID[i], Zone[i]);
    }
    cout<<"Visitor 500 zone = "<<findHash(500)<<"\n";


    /* ---------------- SEGMENT TREE ---------------- */
    cout<<"\n=== ZONE DENSITY MONITORING (Segment Tree) ===\n";
    buildSeg(1,1,200);

    for(int i=0;i<2000;i++){
        updateSeg(1,1,200, Zone[i], Density[i]);
    }
    cout<<"Density in zones 50–100 = "<<querySeg(1,1,200,50,100)<<"\n";


    /* ---------------- BELLMAN-FORD ---------------- */
    cout<<"\n=== SAFE CROWD ROUTING (Bellman–Ford) ===\n";
    initGraph();
    for(int i=0;i<3000;i++){
        addEdge(U[i], V[i], W[i]);
    }

    bellmanFord(1);
    cout<<"Distance to node 10 = "<<distBF[10]<<"\n";


    /* ---------------- HEAP ---------------- */
    cout<<"\n=== PRIORITY CROWD MANAGEMENT (Heap) ===\n";
    for(int i=0;i<50;i++){
        pushHeap(TaskPriority[i], VisitorID[i]);
    }

    cout<<"Top 5 urgent tasks:\n";
    for(int i=0;i<5;i++){
        HeapNode x = popHeap();
        cout<<"Task Visitor "<<x.id<<" Priority "<<x.pr<<"\n";
    }

    cout<<"\n=== END OF REPORT ===\n";
    return 0;
}
