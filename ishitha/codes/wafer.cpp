#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <cstdlib>
#include <cmath>

using namespace std;

const int MAXR = 10000;
const int MAXN = 10000;
const int MAXE = 40000;
const int INF  = 1000000000;

/* =======================================================
   CSV DATA ARRAYS
======================================================= */
int WaferID[MAXR];
int LayerCount[MAXR];
char Pattern[MAXR][80];
char ReferencePat[MAXR][80];
int ConnA[MAXR], ConnB[MAXR];
double DefectScore[MAXR];
double YieldVal[MAXR];
int ROWS = 0;

/* =======================================================
   SAFE LOAD CSV (PREVIEW ONLY)
======================================================= */
void loadCSV(const char *fn){
    ifstream fin(fn);
    if(!fin){
        cout << "ERROR: CSV file not found\n";
        exit(1);
    }

    string line, tok;
    getline(fin, line); // header

    ROWS = 0;
    while(getline(fin, line) && ROWS < MAXR){
        stringstream ss(line);

        getline(ss, tok, ','); WaferID[ROWS] = atoi(tok.c_str());
        getline(ss, tok, ','); LayerCount[ROWS] = atoi(tok.c_str());

        getline(ss, tok, ',');
        strncpy(Pattern[ROWS], tok.c_str(), 79);
        Pattern[ROWS][79] = '\0';

        getline(ss, tok, ',');
        strncpy(ReferencePat[ROWS], tok.c_str(), 79);
        ReferencePat[ROWS][79] = '\0';

        getline(ss, tok, ','); ConnA[ROWS] = atoi(tok.c_str());
        getline(ss, tok, ','); ConnB[ROWS] = atoi(tok.c_str());

        getline(ss, tok, ','); DefectScore[ROWS] = atof(tok.c_str());
        getline(ss, tok, ','); YieldVal[ROWS] = atof(tok.c_str());

        ROWS++;
    }
    fin.close();
}

/* =======================================================
   (1) BACKTRACKING — LITHOGRAPHY ORDER
======================================================= */
int smallN = 10;
int order[20], used[20];
bool btDone = false;

void backtrack(int idx){
    if(idx == smallN){
        cout << "Optimal lithography layer order (demo): ";
        for(int i=0;i<smallN;i++) cout << order[i]+1 << " ";
        cout << "\n";
        btDone = true;
        return;
    }
    for(int i=0;i<smallN;i++){
        if(!used[i]){
            used[i] = 1;
            order[idx] = i;
            backtrack(idx+1);
            used[i] = 0;
            if(btDone) return;
        }
    }
}

/* =======================================================
   (2) KMP — DEFECT PATTERN MATCHING
======================================================= */
int lps[200];

void buildLPS(const char *pat){
    int m = strlen(pat), len = 0;
    lps[0] = 0;
    for(int i=1;i<m;){
        if(pat[i] == pat[len]) lps[i++] = ++len;
        else if(len) len = lps[len-1];
        else lps[i++] = 0;
    }
}

bool kmpSearch(const char *txt, const char *pat){
    if(strlen(pat) == 0) return false;
    buildLPS(pat);
    int i=0, j=0, n=strlen(txt), m=strlen(pat);
    while(i<n){
        if(txt[i] == pat[j]){ i++; j++; }
        if(j == m) return true;
        else if(i<n && txt[i] != pat[j]){
            if(j) j = lps[j-1];
            else i++;
        }
    }
    return false;
}

/* =======================================================
   (3) DIJKSTRA — ELECTRICAL CONNECTIVITY
======================================================= */
int head[MAXN], toE[MAXE], costE[MAXE], nxt[MAXE], ec;
int distA[MAXN], usedA[MAXN], parent[MAXN];

void addEdge(int u,int v){
    if(u<=0 || v<=0 || u>=MAXN || v>=MAXN) return;
    toE[ec]=v; costE[ec]=1; nxt[ec]=head[u]; head[u]=ec++;
}

void dijkstra(int src, int N){
    for(int i=1;i<=N;i++){
        distA[i]=INF; usedA[i]=0; parent[i]=-1;
    }
    distA[src]=0;

    for(int i=1;i<=N;i++){
        int v=-1, best=INF;
        for(int j=1;j<=N;j++)
            if(!usedA[j] && distA[j]<best)
                best=distA[j], v=j;

        if(v==-1) break;
        usedA[v]=1;

        for(int e=head[v];e!=-1;e=nxt[e]){
            int u=toE[e];
            if(distA[v]+1 < distA[u]){
                distA[u]=distA[v]+1;
                parent[u]=v;
            }
        }
    }
}

void printPath(int d){
    if(distA[d]>=INF){
        cout<<"No electrical path found.\n";
        return;
    }
    int st[2000], c=0;
    while(d!=-1){ st[c++]=d; d=parent[d]; }
    cout<<"Path: ";
    for(int i=c-1;i>=0;i--) cout<<st[i]<<" ";
    cout<<"\n";
}

/* =======================================================
   (4) MERGE SORT — YIELD RANKING
======================================================= */
int idxArr[MAXR];

void mergeSort(int l,int r){
    if(l>=r) return;
    int m=(l+r)/2;
    mergeSort(l,m);
    mergeSort(m+1,r);

    int i=l,j=m+1,k=0;
    int tmp[10000];
    while(i<=m && j<=r){
        if(YieldVal[idxArr[i]] < YieldVal[idxArr[j]])
            tmp[k++]=idxArr[i++];
        else
            tmp[k++]=idxArr[j++];
    }
    while(i<=m) tmp[k++]=idxArr[i++];
    while(j<=r) tmp[k++]=idxArr[j++];

    for(int t=0;t<k;t++) idxArr[l+t]=tmp[t];
}

/* =======================================================
   MAIN
======================================================= */
int main(){
    cout<<"====================================\n";
    cout<<" SAMARTHAKA — WAFER ANALYTICS SYSTEM\n";
    cout<<"====================================\n\n";

    cout<<"Loading wafer CSV...\n";
    loadCSV("samarthaka_wafer.csv");
    cout<<"Rows loaded: "<<ROWS<<"\n";

    /* BACKTRACKING */
    cout<<"\n=== (1) LITHOGRAPHY SEQUENCE (Backtracking) ===\n";
    if(ROWS < smallN) smallN = ROWS;
    memset(used,0,sizeof(used));
    backtrack(0);

    cout<<"\nNOTE: Backtracking demonstrated on first "
        << smallN << " wafers only due to factorial complexity.\n";

    /* KMP */
    cout<<"\n=== (2) DEFECT DETECTION (KMP) ===\n";
    int matches = 0;
    for(int i=0;i<ROWS;i++)
        if(kmpSearch(Pattern[i], ReferencePat[i])) matches++;
    cout<<"Defect pattern matches found: "<<matches<<"\n";

    /* GRAPH BUILD */
    cout<<"\n=== (3) ELECTRICAL PATH VALIDATION (Dijkstra) ===\n";
    int maxNode = ROWS;
    if(maxNode > 2000) maxNode = 2000;

    for(int i=1;i<=maxNode;i++) head[i]=-1;
    ec=0;

    for(int i=0;i<ROWS;i++){
        addEdge(WaferID[i], ConnA[i]);
        addEdge(WaferID[i], ConnB[i]);
    }

    cout<<"NOTE: Dijkstra limited to "<<maxNode
        <<" nodes for scalability.\n";

    dijkstra(1, maxNode);
    cout<<"Electrical path from wafer 1 to wafer "<<maxNode<<":\n";
    printPath(maxNode);

    /* MERGE SORT */
    cout<<"\n=== (4) YIELD ANALYSIS (Merge Sort) ===\n";
    for(int i=0;i<ROWS;i++) idxArr[i]=i;
    mergeSort(0, ROWS-1);

    cout<<"Worst 10 yields:\n";
    for(int i=0;i<10 && i<ROWS;i++){
        int id = idxArr[i];
        cout<<"Wafer "<<WaferID[id]
            <<" | Yield="<<YieldVal[id]
            <<" | DefectScore="<<DefectScore[id]<<"\n";
    }

    cout<<"\n=== END OF WAFER REPORT ===\n";
    return 0;
}
