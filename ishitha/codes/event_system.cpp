#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <cstdlib>

using namespace std;

const int MAXR = 10050;
const int MAXE = 20050;
const int MAXV = 600;
const int INF  = 1000000000;

/* ---------- CSV DATA ARRAYS ---------- */
int EventID[MAXR];
int StartT[MAXR];
int EndT[MAXR];
int ResourceID[MAXR];
int VenueA[MAXR];
int VenueB[MAXR];
int Cost[MAXR];
int ROWS = 0;

/* ---------- CSV Loader ---------- */
void loadCSV(const char *fn){
    ifstream fin(fn);
    if(!fin){ cout<<"CSV not found!\n"; exit(1); }

    string line;
    getline(fin, line); // header

    while(getline(fin, line) && ROWS < MAXR-2){
        stringstream ss(line);
        string t;

        getline(ss,t,','); EventID[ROWS] = atoi(t.c_str());
        getline(ss,t,','); StartT[ROWS] = atoi(t.c_str());
        getline(ss,t,','); EndT[ROWS]   = atoi(t.c_str());
        getline(ss,t,','); ResourceID[ROWS] = atoi(t.c_str());
        getline(ss,t,','); VenueA[ROWS] = atoi(t.c_str());
        getline(ss,t,','); VenueB[ROWS] = atoi(t.c_str());
        getline(ss,t,','); Cost[ROWS]   = atoi(t.c_str());

        ROWS++;
    }
    fin.close();
}

/* ===========================================================
   ========== 1. BINARY SEARCH TREE FOR EVENT MATCHING ==========
   =========================================================== */

struct BST {
    int id;
    int start;
    int end;
    BST *left, *right;
};

BST *newNode(int id, int st, int en){
    BST *n = (BST*)malloc(sizeof(BST));
    n->id=id; n->start=st; n->end=en;
    n->left=n->right=NULL;
    return n;
}

BST* insertBST(BST *root, int id, int st, int en){
    if(!root) return newNode(id, st, en);
    if(st < root->start) root->left = insertBST(root->left, id, st, en);
    else root->right = insertBST(root->right, id, st, en);
    return root;
}

BST* searchNearest(BST *root, int st){
    BST *ans = NULL;
    while(root){
        if(root->start >= st){
            ans = root;
            root = root->left;
        } else root = root->right;
    }
    return ans;
}

/* ===========================================================
   ========== 2. UNION-FIND FOR RESOURCE CONFLICTS ==========
   =========================================================== */

int parentUF[400];
int sizeUF[400];

void uf_init(){
    for(int i=0;i<400;i++){
        parentUF[i]=i;
        sizeUF[i]=1;
    }
}

int uf_find(int x){
    while(x != parentUF[x]) x = parentUF[x];
    return x;
}

void uf_union(int a,int b){
    a = uf_find(a); b = uf_find(b);
    if(a==b) return;
    if(sizeUF[a] < sizeUF[b]){ parentUF[a]=b; sizeUF[b]+=sizeUF[a]; }
    else { parentUF[b]=a; sizeUF[a]+=sizeUF[b]; }
}

/* ===========================================================
   ========== 3. KRUSKAL MST FOR VENUE OPTIMIZATION ==========
   =========================================================== */

struct Edge {
    int u,v,w;
} edges[MAXE];

int ec = 0;
int mst_cost = 0;

void sortEdges(int l,int r){
    if(l>=r) return;
    int m=(l+r)/2;
    sortEdges(l,m);
    sortEdges(m+1,r);

    int i=l, j=m+1, k=0;
    Edge *tmp = (Edge*)malloc(sizeof(Edge)*(r-l+1));

    while(i<=m && j<=r){
        if(edges[i].w < edges[j].w) tmp[k++]=edges[i++];
        else tmp[k++]=edges[j++];
    }
    while(i<=m) tmp[k++]=edges[i++];
    while(j<=r) tmp[k++]=edges[j++];

    for(int a=0;a<k;a++) edges[l+a]=tmp[a];
    free(tmp);
}

/* ===========================================================
   ========== 4. MERGE SORT FINAL SCHEDULE ==========
   =========================================================== */

int idxArr[MAXR];

void mergeSort(int l,int r){
    if(l>=r) return;
    int m=(l+r)/2;
    mergeSort(l,m);
    mergeSort(m+1,r);

    int i=l, j=m+1, k=0;
    int *tmp=(int*)malloc(sizeof(int)*(r-l+1));

    while(i<=m && j<=r){
        if(StartT[idxArr[i]] <= StartT[idxArr[j]])
            tmp[k++] = idxArr[i++];
        else
            tmp[k++] = idxArr[j++];
    }
    while(i<=m) tmp[k++] = idxArr[i++];
    while(j<=r) tmp[k++] = idxArr[j++];

    for(int a=0;a<k;a++) idxArr[l+a] = tmp[a];
    free(tmp);
}

/* ===========================================================
   ======================   MAIN   ============================
   =========================================================== */

int main(){

    loadCSV("samarthaka_event_data.csv");
    cout<<"Loaded rows: "<<ROWS<<"\n";

    /* ---------- BST ---------- */
    cout<<"\n=== EVENT MATCHING (BST) ===\n";
    BST *root = NULL;

    for(int i=0;i<500;i++)  // insert first 500 into BST
        root = insertBST(root, EventID[i], StartT[i], EndT[i]);

    BST *f = searchNearest(root, 3000);
    if(f) cout<<"Nearest event starting after 3000 min: "<<f->id<<" at "<<f->start<<"\n";
    else cout<<"No event found.\n";


    /* ---------- UNION-FIND ---------- */
    cout<<"\n=== CONFLICT DETECTION (Union-Find) ===\n";
    uf_init();

    int conflicts=0;
    for(int i=0;i<ROWS;i++){
        int r = ResourceID[i];
        if(uf_find(r) == uf_find(r+1)){
            conflicts++;
        }
        uf_union(r, (r%300)+1);
    }
    cout<<"Detected resource conflict groups: "<<conflicts<<"\n";


    /* ---------- KRUSKAL ---------- */
    cout<<"\n=== VENUE OPTIMIZATION (Kruskal MST) ===\n";
    ec=0;
    for(int i=0;i<ROWS;i++){
        edges[ec].u = VenueA[i];
        edges[ec].v = VenueB[i];
        edges[ec].w = Cost[i];
        ec++;
    }

    sortEdges(0,ec-1);

    uf_init();
    mst_cost = 0;

    int used=0;
    for(int i=0;i<ec;i++){
        int u=edges[i].u, v=edges[i].v, w=edges[i].w;
        if(uf_find(u) != uf_find(v)){
            uf_union(u,v);
            mst_cost += w;
            used++;
            if(used > 500) break; // limit for speed
        }
    }

    cout<<"Approx MST cost (first 500 links): "<<mst_cost<<"\n";


    /* ---------- MERGE SORT ---------- */
    cout<<"\n=== FINAL EVENT SCHEDULE (Merge Sort) ===\n";

    for(int i=0;i<ROWS;i++) idxArr[i]=i;

    mergeSort(0, ROWS-1);

    cout<<"First 10 scheduled events:\n";
    for(int i=0;i<10;i++){
        int id = idxArr[i];
        cout<<"Event "<<EventID[id]<<" Start="<<StartT[id]<<" End="<<EndT[id]<<"\n";
    }

    cout<<"\n=== END OF REPORT ===\n";
    return 0;
}
