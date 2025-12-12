// green_corridor.cpp
// Compile: g++ -O2 green_corridor.cpp -o green_corridor
// Run: ./green_corridor
//
// Reads samarthaka_greencorridor.csv in current folder.

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <cstdlib>
using namespace std;

const int MAXROW = 10050;        // support 10000+ rows
const int MAXN   = 10050;        // nodes
const int MAXE   = 400000;       // edges (bidirectional)
const int INF    = 1000000000;

// CSV arrays
int NodeID[MAXROW];
double Lat[MAXROW], Lon[MAXROW];
int Conn1[MAXROW], Time1A[MAXROW];
int Conn2[MAXROW], Time2A[MAXROW];
int HospitalDist[MAXROW];
int SignalStatus[MAXROW];
int Blocked[MAXROW];
int ROWS = 0;

// Graph adjacency (arrays)
int head[MAXN], toE[MAXE], costE[MAXE], nxtE[MAXE], ecnt;

void initGraph(int n) {
    for (int i=0;i<=n;i++) head[i] = -1;
    ecnt = 0;
}

void addEdgeOne(int u, int v, int w) {
    if (ecnt >= MAXE) return;
    toE[ecnt] = v;
    costE[ecnt] = w;
    nxtE[ecnt] = head[u];
    head[u] = ecnt++;
}

void addEdgeUndir(int u, int v, int w) {
    if (u <= 0 || v <= 0) return;
    addEdgeOne(u,v,w);
    addEdgeOne(v,u,w);
}

// ------------------ CSV loader ------------------
void loadCSV(const char *fn) {
    ifstream fin(fn);
    if (!fin) {
        cerr << "Cannot open " << fn << "\n";
        exit(1);
    }
    string line;
    getline(fin, line); // header
    ROWS = 0;
    while (getline(fin, line) && ROWS < MAXROW-2) {
        stringstream ss(line);
        string tok;
        getline(ss, tok, ','); NodeID[ROWS] = atoi(tok.c_str());
        getline(ss, tok, ','); Lat[ROWS] = atof(tok.c_str());
        getline(ss, tok, ','); Lon[ROWS] = atof(tok.c_str());
        getline(ss, tok, ','); Conn1[ROWS] = atoi(tok.c_str());
        getline(ss, tok, ','); Time1A[ROWS] = atoi(tok.c_str());
        getline(ss, tok, ','); Conn2[ROWS] = atoi(tok.c_str());
        getline(ss, tok, ','); Time2A[ROWS] = atoi(tok.c_str());
        getline(ss, tok, ','); HospitalDist[ROWS] = atoi(tok.c_str());
        getline(ss, tok, ','); SignalStatus[ROWS] = atoi(tok.c_str());
        getline(ss, tok, ','); Blocked[ROWS] = atoi(tok.c_str());
        ROWS++;
    }
    fin.close();
}

// ------------------ Dijkstra (min-heap) ------------------
// arrays sized by MAXN
int distA[MAXN];
int parentA[MAXN];
bool seenA[MAXN];

// heap for (node,dist) -- 1-indexed
int heapNode[MAXN], heapDist[MAXN], heapSize;

void heapPush(int node, int dist) {
    int i = ++heapSize;
    heapNode[i] = node; heapDist[i] = dist;
    while (i > 1) {
        int p = i/2;
        if (heapDist[p] <= heapDist[i]) break;
        // swap
        int tn = heapNode[p], td = heapDist[p];
        heapNode[p] = heapNode[i]; heapDist[p] = heapDist[i];
        heapNode[i] = tn; heapDist[i] = td;
        i = p;
    }
}

int heapPopNode() {
    if (heapSize == 0) return -1;
    int ret = heapNode[1];
    heapNode[1] = heapNode[heapSize];
    heapDist[1] = heapDist[heapSize];
    heapSize--;
    int i = 1;
    while (true) {
        int l = i*2, r = l+1, best = i;
        if (l <= heapSize && heapDist[l] < heapDist[best]) best = l;
        if (r <= heapSize && heapDist[r] < heapDist[best]) best = r;
        if (best == i) break;
        int tn = heapNode[i], td = heapDist[i];
        heapNode[i] = heapNode[best]; heapDist[i] = heapDist[best];
        heapNode[best] = tn; heapDist[best] = td;
        i = best;
    }
    return ret;
}

void dijkstra(int src, int maxNode) {
    for (int i=1;i<=maxNode;i++) { distA[i] = INF; parentA[i] = -1; seenA[i] = false; }
    heapSize = 0;
    distA[src] = 0;
    heapPush(src, 0);
    while (heapSize) {
        int u = heapPopNode();
        if (u == -1) break;
        if (seenA[u]) continue;
        seenA[u] = true;
        // relax neighbors
        for (int e = head[u]; e != -1; e = nxtE[e]) {
            int v = toE[e], w = costE[e];
            if (distA[u] + w < distA[v]) {
                distA[v] = distA[u] + w;
                parentA[v] = u;
                heapPush(v, distA[v]);
            }
        }
    }
}

void printPath(int dest) {
    if (dest <= 0) return;
    if (distA[dest] >= INF) {
        cout << "  Node " << dest << " unreachable\n";
        return;
    }
    int stackArr[10240], sc=0;
    int cur = dest;
    while (cur != -1 && sc < 10240) { stackArr[sc++] = cur; cur = parentA[cur]; }
    cout << "  Node " << dest << " dist=" << distA[dest] << " min, path: ";
    for (int i=sc-1;i>=0;i--) {
        cout << stackArr[i];
        if (i) cout << " -> ";
    }
    cout << "\n";
}

// ------------------ Quicksort (resource allocation by HospitalDist) ------------------
int sortNodes[MAXROW]; // indices 0..ROWS-1
void quickswap(int i, int j) {
    int t = sortNodes[i]; sortNodes[i] = sortNodes[j]; sortNodes[j] = t;
}
int partition_arr(int l, int r) {
    int pivot = HospitalDist[ sortNodes[(l+r)/2] ];
    int i = l, j = r;
    while (i <= j) {
        while (HospitalDist[ sortNodes[i] ] < pivot) i++;
        while (HospitalDist[ sortNodes[j] ] > pivot) j--;
        if (i <= j) { quickswap(i,j); i++; j--; }
    }
    return i;
}
void quicksort_arr(int l, int r) {
    if (l >= r) return;
    int idx = partition_arr(l,r);
    if (l < idx-1) quicksort_arr(l, idx-1);
    if (idx < r) quicksort_arr(idx, r);
}

// ------------------ BFS (signal optimization / clearance shortest hops) ------------------
int qArr[MAXROW];
bool visitedB[MAXROW];
int parentB[MAXROW];

int bfs_shortest_by_signals(int start, int target, int maxNode) {
    for (int i=1;i<=maxNode;i++) { visitedB[i] = false; parentB[i] = -1; }
    int qh=0, qt=0;
    qArr[qt++] = start; visitedB[start] = true;
    while (qh < qt) {
        int u = qArr[qh++];
        if (u == target) return 1;
        for (int e = head[u]; e != -1; e = nxtE[e]) {
            int v = toE[e];
            // we only visit signals that are clear (SignalStatus==1)
            if (!visitedB[v] && SignalStatus[v-1] == 1 && Blocked[v-1] == 0) {
                visitedB[v] = true;
                parentB[v] = u;
                qArr[qt++] = v;
                if (v == target) return 1;
            }
        }
    }
    return 0;
}
void printBFSpath(int target) {
    if (!visitedB[target]) { cout << "  No BFS path to " << target << "\n"; return; }
    int st[10240], sp=0;
    int cur = target;
    while (cur != -1 && sp < 10240) { st[sp++] = cur; cur = parentB[cur]; }
    cout << "  BFS path to " << target << ": ";
    for (int i=sp-1;i>=0;i--) { cout << st[i]; if (i) cout << " -> "; }
    cout << "\n";
}

// ------------------ DFS (iterative) to find blocked regions ------------------
bool visitedD[MAXROW];
int stackD[MAXROW];

int countBlockedComponent(int start, int maxNode) {
    int top = 0;
    stackD[top++] = start;
    visitedD[start] = true;
    int count = 0;
    while (top) {
        int u = stackD[--top];
        count++;
        for (int e = head[u]; e != -1; e = nxtE[e]) {
            int v = toE[e];
            if (!visitedD[v] && Blocked[v-1] == 1) {
                visitedD[v] = true;
                stackD[top++] = v;
            }
        }
    }
    return count;
}

// ------------------ MAIN ------------------
int main() {
    cout << "Loading CSV (samarthaka_greencorridor.csv)...\n";
    loadCSV("samarthaka_greencorridor.csv");
    cout << "Rows loaded: " << ROWS << "\n";

    // Determine max node id present
    int maxNode = 0;
    for (int i=0;i<ROWS;i++) if (NodeID[i] > maxNode) maxNode = NodeID[i];
    if (maxNode >= MAXN) maxNode = MAXN-1;

    // Build graph (undirected). NodeIDs are assumed 1..maxNode
    initGraph(maxNode);
    for (int i=0;i<ROWS;i++) {
        int u = NodeID[i];
        if (Conn1[i] > 0) addEdgeUndir(u, Conn1[i], Time1A[i]);
        if (Conn2[i] > 0) addEdgeUndir(u, Conn2[i], Time2A[i]);
    }

    cout << "\n=== 1) ROUTE IDENTIFICATION & MAPPING (Dijkstra) ===\n";
    int src = 1;
    dijkstra(src, maxNode);

    // Find top 8 nearest reachable nodes
    // build small array of nodes and distances
    int picked = 0;
    for (int i=1;i<=maxNode;i++) {
        if (i == src) continue;
        if (distA[i] < INF) {
            cout << "  Node " << i << " dist=" << distA[i] << " mins; path: ";
            // reconstruct path quickly
            int buf[10240], bc=0, cur = i;
            while (cur != -1 && bc < 10240) { buf[bc++] = cur; cur = parentA[cur]; }
            for (int j=bc-1; j>=0; j--) {
                cout << buf[j];
                if (j) cout << " -> ";
            }
            cout << "\n";
            picked++;
            if (picked >= 8) break;
        }
    }
    if (picked == 0) cout << "  No reachable nodes from " << src << "\n";

    // ---------------- Sorting for resource allocation (closest hospitals)
    cout << "\n=== 2) RESOURCE ALLOCATION & SCHEDULING (Sorting by Hospital distance) ===\n";
    for (int i=0;i<ROWS;i++) sortNodes[i] = i;
    quicksort_arr(0, ROWS-1);

    cout << "Top 10 nodes nearest to hospitals (NodeID : HospitalDist):\n";
    int shown = 0;
    for (int i=0;i<ROWS && shown<10;i++) {
        int idx = sortNodes[i];
        cout << "  " << NodeID[idx] << " : " << HospitalDist[idx] << "\n";
        shown++;
    }

    // ---------------- BFS for signal optimization - find green-wave path to target hospital node
    cout << "\n=== 3) TRAFFIC SIGNAL OPTIMIZATION & CLEARANCE (BFS) ===\n";
    // pick a target: the nearest node with small HospitalDist (first quicksorted)
    int targetIdx = sortNodes[0];
    int targetNode = NodeID[targetIdx];
    cout << "Target hospital-node chosen: " << targetNode << " (HospitalDist=" << HospitalDist[targetIdx] << ")\n";
    // BFS but only traverse nodes with SignalStatus==1 and not blocked
    for (int i=1;i<=maxNode;i++) { visitedB[i] = false; parentB[i] = -1; }
    int reachable = bfs_shortest_by_signals(src, targetNode, maxNode);
    if (reachable) printBFSpath(targetNode);
    else cout << "  No clear green-wave path from " << src << " to " << targetNode << "\n";

    // ---------------- DFS for blocked-component detection
    cout << "\n=== 4) LIVE MONITORING & EMERGENCY HANDLING (DFS) ===\n";
    for (int i=1;i<=maxNode;i++) visitedD[i] = false;
    int components = 0;
    int biggestSize = 0;
    int biggestRoot = -1;
    for (int i=0;i<ROWS;i++) {
        int node = NodeID[i];
        if (Blocked[i] == 1 && !visitedD[node]) {
            int size = countBlockedComponent(node, maxNode);
            components++;
            if (size > biggestSize) { biggestSize = size; biggestRoot = node; }
        }
    }
    if (components == 0) cout << "  No blocked components detected.\n";
    else {
        cout << "  Blocked components found: " << components << "\n";
        cout << "  Largest blocked component root: " << biggestRoot << " (size=" << biggestSize << " nodes)\n";
        cout << "  Suggest alternative route by routing around largest blocked component.\n";
    }

    cout << "\n=== END OF REPORT ===\n";
    return 0;
}
