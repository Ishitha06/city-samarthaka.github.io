#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <cmath>
#include <cstdlib>

using namespace std;

const int MAXR = 10050;
const int MAXN = 10050;
const int MAXE = 40050;
const int INF = 1000000000;

/* ========= CSV DATA ARRAYS ========= */
int WaferID[MAXR];
int LayerCount[MAXR];
char Pattern[MAXR][80];
char ReferencePat[MAXR][80];
int ConnA[MAXR], ConnB[MAXR];
double DefectScore[MAXR];
double Yield[MAXR];
int ROWS = 0;

/* ========= CSV LOADER ========= */
void loadCSV(const char *fn){
    ifstream fin(fn);
    if(!fin){ cout<<"File not found!\n"; exit(1); }

    string line;
    getline(fin,line);

    while(getline(fin,line) && ROWS < MAXR-2){
        stringstream ss(line);
        string t;
        getline(ss,t,','); WaferID[ROWS]=atoi(t.c_str());
        getline(ss,t,','); LayerCount[ROWS]=atoi(t.c_str());
        getline(ss,t,','); strncpy(Pattern[ROWS],t.c_str(),79);
        getline(ss,t,','); strncpy(ReferencePat[ROWS],t.c_str(),79);
        getline(ss,t,','); ConnA[ROWS]=atoi(t.c_str());
        getline(ss,t,','); ConnB[ROWS]=atoi(t.c_str());
        getline(ss,t,','); DefectScore[ROWS]=atof(t.c_str());
        getline(ss,t,','); Yield[ROWS]=atof(t.c_str());
        ROWS++;
    }
    fin.close();
}

/* ========= 1) BACKTRACKING: LITHOGRAPHY LAYER ORDERING ========= */
int smallN = 10;
int order[20], used[20];
bool btFound = false;

void backtrack(int idx){
    if(idx == smallN){
        btFound = true;
        cout<<"Optimal Layer Ordering (first 10 wafers): ";
        for(int i=0;i<smallN;i++) cout<<order[i]+1<<" ";
        cout<<"\n";
        return;
    }
    for(int i=0;i<smallN;i++){
        if(!used[i]){
            used[i] = 1;
            order[idx] = i;
            backtrack(idx+1);
            used[i] = 0;
            if(btFound) return;
        }
    }
}

/* ========= 2) KMP PATTERN MATCHING FOR DEFECTS ========= */
int lps[200];
void buildLPS(const char *pat){
    int m = strlen(pat);
    int len = 0;
    lps[0]=0;
    int i=1;
    while(i<m){
        if(pat[i]==pat[len]){ len++; lps[i]=len; i++; }
        else{
            if(len!=0) len=lps[len-1];
            else { lps[i]=0; i++; }
        }
    }
}

bool kmpSearch(const char *text, const char *pat){
    buildLPS(pat);
    int n = strlen(text), m = strlen(pat);
    int i=0,j=0;
    while(i<n){
        if(text[i]==pat[j]){ i++; j++; }
        if(j==m) return true;
        else if(i<n && text[i]!=pat[j]){
            if(j!=0) j=lps[j-1];
            else i++;
        }
    }
    return false;
}

/* ========= 3) DIJKSTRA FOR ELECTRICAL PATH VALIDATION ========= */
int head[MAXN], toE[MAXE], costE[MAXE], nxt[MAXE], ec = 0;
int distA[MAXN], usedA[MAXN], par[MAXN];

void addEdge(int u,int v,int w){
    toE[ec]=v; costE[ec]=w; nxt[ec]=head[u]; head[u]=ec++; 
}

void dijkstra(int src, int N){
    for(int i=1;i<=N;i++){
        distA[i]=INF; usedA[i]=0; par[i]=-1;
    }
    distA[src]=0;

    for(int t=1;t<=N;t++){
        int v=-1, best=INF;
        for(int i=1;i<=N;i++)
            if(!usedA[i] && distA[i]<best){ best=distA[i]; v=i; }
        if(v==-1) break;
        usedA[v]=1;
        for(int e=head[v];e!=-1;e=nxt[e]){
            int u=toE[e];
            if(distA[v]+costE[e] < distA[u]){
                distA[u]=distA[v]+costE[e];
                par[u]=v;
            }
        }
    }
}

void printPath(int d){
    if(distA[d]>=INF){ cout<<"No path.\n"; return; }
    int st[2000], c=0, x=d;
    while(x!=-1){ st[c++]=x; x=par[x]; }
    cout<<"Path: ";
    for(int i=c-1;i>=0;i--) cout<<st[i]<<" ";
    cout<<" (Cost="<<distA[d]<<")\n";
}

/* ========= 4) MERGE SORT FOR YIELD RANKING ========= */
int idxArr[MAXR];

void mergeSort(int l,int r){
    if(l>=r) return;
    int m=(l+r)/2;
    mergeSort(l,m);
    mergeSort(m+1,r);
    int i=l,j=m+1,k=0;
    int *tmp=(int*)malloc(sizeof(int)*(r-l+1));
    while(i<=m && j<=r){
        if(Yield[idxArr[i]] <= Yield[idxArr[j]])
            tmp[k++] = idxArr[i++];
        else
            tmp[k++] = idxArr[j++];
    }
    while(i<=m) tmp[k++]=idxArr[i++];
    while(j<=r) tmp[k++]=idxArr[j++];
    for(int a=0;a<k;a++) idxArr[l+a]=tmp[a];
    free(tmp);
}

/* ========= MAIN ========= */
int main(){
    cout<<"Loading wafer CSV...\n";
    loadCSV("samarthaka_wafer_data.csv");
    cout<<"Rows loaded: "<<ROWS<<"\n";

    /* ---- BACKTRACKING ---- */
    cout<<"\n=== (1) BACKTRACKING: Lithography Sequence ===\n";
    if(ROWS < smallN) smallN = ROWS;
    memset(used,0,sizeof(used));
    backtrack(0);

    /* ---- KMP Defect Detection ---- */
    cout<<"\n=== (2) DEFECT DETECTION (KMP Pattern Matching) ===\n";
    int defectMatches = 0;
    for(int i=0;i<ROWS;i++){
        if(kmpSearch(Pattern[i], ReferencePat[i]))
            defectMatches++;
    }
    cout<<"Matched (reference pattern found in wafer pattern): "<<defectMatches<<" wafers\n";

    /* ---- BUILD GRAPH ---- */
    cout<<"\n=== (3) ELECTRICAL PATH VALIDATION (Dijkstra) ===\n";
    int maxNode = ROWS;
    for(int i=1;i<=maxNode;i++) head[i]=-1;
    ec = 0;
    for(int i=0;i<ROWS;i++){
        addEdge(WaferID[i], ConnA[i], 1);
        addEdge(WaferID[i], ConnB[i], 1);
    }

    dijkstra(1, maxNode);
    cout<<"Shortest electrical path from wafer 1 to wafer "<<ROWS<<":\n";
    printPath(ROWS);

    /* ---- MERGE SORT (YIELD ANALYSIS) ---- */
    cout<<"\n=== (4) YIELD ANALYSIS (Merge Sort) ===\n";
    for(int i=0;i<ROWS;i++) idxArr[i]=i;
    mergeSort(0, ROWS-1);

    cout<<"Worst 10 yields:\n";
    for(int i=0;i<10;i++){
        int id = idxArr[i];
        cout<<"  Wafer "<<WaferID[id]<<" Yield="<<Yield[id]<<"%  Defect="<<DefectScore[id]<<"\n";
    }

    cout<<"\n=== END OF WAFER REPORT ===\n";
    return 0;
}
