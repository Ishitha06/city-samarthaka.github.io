#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
using namespace std;

/* =======================================================
   CONSTANTS
======================================================= */
const int MAXROW = 10000;
const int MAXN   = 300;        // routing demo limit
const int MAXE   = 40000;
const int INF    = 1000000000;

/* =======================================================
   SAFE STRING TO INT
======================================================= */
int safe_stoi(const string &s, int def = 0) {
    if (s.empty()) return def;
    try { return stoi(s); }
    catch(...) { return def; }
}

/* =======================================================
   CSV ARRAYS
======================================================= */
int StationID[MAXROW], Neighbor1[MAXROW], Cost1[MAXROW];
int Neighbor2[MAXROW], Cost2[MAXROW];
int InitialPassengers[MAXROW], TapFare[MAXROW], TapTimeArr[MAXROW];
unsigned long long CardID[MAXROW];
int InitialBalance[MAXROW], FaultCode[MAXROW];
char DepartureTimes[MAXROW][50];
int rowCount = 0;

/* =======================================================
   LOAD CSV
======================================================= */
void loadCSV(const char *file)
{
    ifstream fin(file);
    if (!fin) { cout << "ERROR: Cannot open CSV\n"; exit(0); }

    string line, tmp;
    getline(fin, line); // header

    rowCount = 0;
    while (getline(fin, line) && rowCount < MAXROW)
    {
        stringstream ss(line);

        getline(ss, tmp, ','); StationID[rowCount] = safe_stoi(tmp);
        getline(ss, tmp, ','); // zone

        getline(ss, tmp, ','); Neighbor1[rowCount] = safe_stoi(tmp);
        getline(ss, tmp, ','); Cost1[rowCount]     = safe_stoi(tmp);

        getline(ss, tmp, ','); Neighbor2[rowCount] = safe_stoi(tmp);
        getline(ss, tmp, ','); Cost2[rowCount]     = safe_stoi(tmp);

        getline(ss, tmp, ','); strncpy(DepartureTimes[rowCount], tmp.c_str(), 49);

        getline(ss, tmp, ','); InitialPassengers[rowCount] = safe_stoi(tmp);
        getline(ss, tmp, ','); CardID[rowCount]            = safe_stoi(tmp);
        getline(ss, tmp, ','); InitialBalance[rowCount]    = safe_stoi(tmp);
        getline(ss, tmp, ','); TapFare[rowCount]           = safe_stoi(tmp);
        getline(ss, tmp, ','); TapTimeArr[rowCount]        = safe_stoi(tmp);
        getline(ss, tmp, ','); FaultCode[rowCount]         = safe_stoi(tmp);

        rowCount++;
    }
    fin.close();
}

/* =======================================================
   GRAPH (DIJKSTRA)
======================================================= */
int head[MAXN], toE[MAXE], costE[MAXE], nxtE[MAXE], ec;
int distArr[MAXN], parentArr[MAXN];
bool used[MAXN];

void initGraph() {
    ec = 0;
    for (int i=0;i<MAXN;i++) head[i] = -1;
}

void addEdge(int u, int v, int w) {
    if (u<=0 || v<=0 || u>=MAXN || v>=MAXN) return;
    toE[ec]=v; costE[ec]=w; nxtE[ec]=head[u]; head[u]=ec++;
}

void buildGraph() {
    initGraph();
    for (int i=0;i<rowCount;i++) {
        addEdge(StationID[i], Neighbor1[i], Cost1[i]);
        addEdge(Neighbor1[i], StationID[i], Cost1[i]);
        addEdge(StationID[i], Neighbor2[i], Cost2[i]);
        addEdge(Neighbor2[i], StationID[i], Cost2[i]);
    }
}

void dijkstra(int src) {
    for (int i=1;i<MAXN;i++) {
        distArr[i]=INF; parentArr[i]=-1; used[i]=false;
    }
    distArr[src]=0;

    for (int i=1;i<MAXN;i++) {
        int v=-1,best=INF;
        for (int j=1;j<MAXN;j++)
            if(!used[j] && distArr[j]<best) best=distArr[j],v=j;
        if(v==-1) break;
        used[v]=true;

        for(int e=head[v];e!=-1;e=nxtE[e]){
            int to=toE[e], w=costE[e];
            if(distArr[v]+w<distArr[to]){
                distArr[to]=distArr[v]+w;
                parentArr[to]=v;
            }
        }
    }
}

void printPath(int d){
    int p[MAXN],c=0;
    while(d!=-1){p[c++]=d; d=parentArr[d];}
    for(int i=c-1;i>=0;i--){
        cout<<p[i]; if(i) cout<<" -> ";
    }
    cout<<"\n";
}

/* =======================================================
   PASSENGER ANALYSIS
======================================================= */
void passengerAnalysis() {
    cout << "\n--- PASSENGER LOAD ANALYSIS ---\n";
    int total=0, maxP=0, maxS=-1;
    for(int i=0;i<rowCount;i++){
        total+=InitialPassengers[i];
        if(InitialPassengers[i]>maxP){
            maxP=InitialPassengers[i];
            maxS=StationID[i];
        }
    }
    cout<<"Total passengers recorded: "<<total<<"\n";
    cout<<"Highest load at Station "<<maxS<<" ("<<maxP<<" passengers)\n";
}

/* =======================================================
   TICKET VALIDATION
======================================================= */
void ticketValidation() {
    cout << "\n--- TICKET VALIDATION (HASH LOGIC) ---\n";
    for(int i=0,shown=0;i<rowCount && shown<5;i++){
        cout<<"Card "<<CardID[i]<<" : ";
        if(InitialBalance[i]>=TapFare[i])
            cout<<"VALID\n";
        else
            cout<<"INSUFFICIENT BALANCE\n";
        shown++;
    }
}

/* =======================================================
   FAULT DETECTION
======================================================= */
void faultReport() {
    cout << "\n--- FAULT DETECTION ---\n";
    int cnt=0;
    for(int i=0;i<rowCount;i++){
        if(FaultCode[i]!=0){
            cout<<"Station "<<StationID[i]<<" fault code "<<FaultCode[i]<<"\n";
            cnt++;
            if(cnt==5) break;
        }
    }
    if(cnt==0) cout<<"No faults detected\n";
}

/* =======================================================
   SCHEDULE PREVIEW
======================================================= */
void schedulePreview() {
    cout << "\n--- SCHEDULE PREVIEW ---\n";
    for(int i=0;i<5 && i<rowCount;i++)
        cout<<"Station "<<StationID[i]<<" : "<<DepartureTimes[i]<<"\n";
}

/* =======================================================
   MAIN
======================================================= */
int main()
{
    cout<<"=== SAMARTHAKA METRO TRANSIT SYSTEM ===\n";
    cout<<"Loading CSV...\n";

    loadCSV("samarthaka_metro.csv");
    cout<<"Rows loaded: "<<rowCount<<"\n";

    buildGraph();

    cout<<"\nNOTE:\n";
    cout<<"Routing uses only first "<<MAXN-1
        <<" stations due to O(V^2) Dijkstra complexity.\n";
    cout<<"Other analyses use full dataset.\n";

    cout<<"\n--- ROUTE PLANNING (DIJKSTRA) ---\n";
    dijkstra(1);
    for(int i=2,shown=0;i<MAXN && shown<5;i++){
        if(distArr[i]<INF){
            cout<<"Station "<<i<<" | Time "<<distArr[i]<<" mins\n Path: ";
            printPath(i);
            shown++;
        }
    }

    passengerAnalysis();
    ticketValidation();
    faultReport();
    schedulePreview();

    cout<<"\n=== END OF METRO REPORT ===\n";
    return 0;
}
