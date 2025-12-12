// grid_full.cpp
// Reads samarthaka_grid.csv (10000 rows), builds graph and runs:
//  - Power-source max-heap selection
//  - Kruskal MST (Union-Find)
//  - Dijkstra shortest path (min-heap implemented with arrays)
//  - Segment tree (meter queries/updates)
//  - Outage detection (simulate disabling edges, use Union-Find)
//
// NO std::vector used — static arrays sized to support up to 10050 nodes and ~200k edges.

// #include <bits/stdc++.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include <limits>

using namespace std;

const int MAX_NODES = 10050;       // supports 10k comfortably
const int MAX_EDGES = 200050;      // edge storage (undirected edges will use 2*rows)
const int INF = 1000000000;

// ----------------------------- Data arrays -----------------------------
int NodeID[MAX_NODES];
char NodeType[MAX_NODES][32];
int ConnectedTo[MAX_NODES];
int LineResInt[MAX_NODES];        // rounded Resistance stored as int
long long MeterMonth[MAX_NODES];

int N = 0;   // number of nodes (rows)
int E = 0;   // number of directed edges added to adjacency (for Dijkstra)
int KR_E = 0; // number of edges for Kruskal (one per CSV row typically)

// ----------------------------- CSV parse helper -----------------------------
void splitCSVLine(const string &line, string out[], int &count) {
    count = 0;
    string cur;
    bool inquotes = false;
    for (size_t i = 0; i < line.size(); ++i) {
        char c = line[i];
        if (c == '"') {
            inquotes = !inquotes;
            continue;
        }
        if (c == ',' && !inquotes) {
            out[count++] = cur;
            cur.clear();
        } else cur.push_back(c);
    }
    out[count++] = cur;
}

// ----------------------------- Edge store (for Kruskal) -----------------------------
struct KEdge { int u,v,w; } KEdges[MAX_EDGES];
int kedgeCount = 0;

// comparator for sorting edges
bool cmpKEdge(const KEdge &a, const KEdge &b) {
    return a.w < b.w;
}

// ----------------------------- Union-Find -----------------------------
int uf_parent[MAX_NODES];
int uf_rankv[MAX_NODES];

void uf_make(int n) {
    for (int i = 0; i < n; ++i) { uf_parent[i] = i; uf_rankv[i] = 0; }
}
int uf_find(int x) {
    if (uf_parent[x] == x) return x;
    return uf_parent[x] = uf_find(uf_parent[x]);
}
void uf_union(int a,int b) {
    a = uf_find(a); b = uf_find(b);
    if (a == b) return;
    if (uf_rankv[a] < uf_rankv[b]) swap(a,b);
    uf_parent[b] = a;
    if (uf_rankv[a] == uf_rankv[b]) uf_rankv[a]++;
}

// ----------------------------- Adjacency for Dijkstra (static linked list) -----------------------------
int head[MAX_NODES];
int adj_to[MAX_EDGES];
int adj_w[MAX_EDGES];
int adj_next[MAX_EDGES];
int adj_ptr = 0;

void addEdgeAdj(int u,int v,int w) {
    if (adj_ptr >= MAX_EDGES-2) return;
    adj_to[adj_ptr] = v;
    adj_w[adj_ptr] = w;
    adj_next[adj_ptr] = head[u];
    head[u] = adj_ptr++;
}

// ----------------------------- Min-heap for Dijkstra (arrays) -----------------------------
int heap_node[MAX_NODES];
int heap_dist[MAX_NODES];
int heap_pos[MAX_NODES]; // position in heap (or -1)
int heap_size = 0;

void heapInit(int n) {
    heap_size = 0;
    for (int i = 0; i < n; ++i) heap_pos[i] = -1;
}
void heapSwap(int i,int j) {
    int ni = heap_node[i], nj = heap_node[j];
    swap(heap_node[i], heap_node[j]);
    swap(heap_dist[i], heap_dist[j]);
    heap_pos[heap_node[i]] = i;
    heap_pos[heap_node[j]] = j;
}
void heapSiftUp(int idx) {
    while (idx > 0) {
        int p = (idx - 1) / 2;
        if (heap_dist[idx] < heap_dist[p]) { heapSwap(idx, p); idx = p; }
        else break;
    }
}
void heapSiftDown(int idx) {
    while (true) {
        int l = idx*2+1, r = idx*2+2, best = idx;
        if (l < heap_size && heap_dist[l] < heap_dist[best]) best = l;
        if (r < heap_size && heap_dist[r] < heap_dist[best]) best = r;
        if (best != idx) { heapSwap(best, idx); idx = best; } else break;
    }
}
void heapPushOrUpdate(int node,int dist) {
    if (heap_pos[node] == -1) {
        heap_node[heap_size] = node;
        heap_dist[heap_size] = dist;
        heap_pos[node] = heap_size;
        heapSiftUp(heap_size);
        heap_size++;
    } else {
        int p = heap_pos[node];
        if (dist < heap_dist[p]) {
            heap_dist[p] = dist;
            heapSiftUp(p);
        }
    }
}
bool heapEmpty() { return heap_size == 0; }
pair<int,int> heapPop() {
    pair<int,int> res = {heap_node[0], heap_dist[0]};
    heap_size--;
    heapSwap(0, heap_size);
    heap_pos[heap_node[heap_size]] = heap_size;
    heap_pos[res.first] = -1; // popped
    heapSiftDown(0);
    return res;
}

// ----------------------------- Segment tree -----------------------------
long long seg[4 * MAX_NODES];

void segBuild(int idx,int l,int r) {
    if (l == r) { seg[idx] = MeterMonth[l]; return; }
    int mid=(l+r)/2;
    segBuild(idx*2, l, mid);
    segBuild(idx*2+1, mid+1, r);
    seg[idx] = seg[idx*2] + seg[idx*2+1];
}
void segUpdate(int idx,int l,int r,int pos,long long val) {
    if (l == r) { seg[idx] = val; return; }
    int mid=(l+r)/2;
    if (pos <= mid) segUpdate(idx*2, l, mid, pos, val);
    else segUpdate(idx*2+1, mid+1, r, pos, val);
    seg[idx] = seg[idx*2] + seg[idx*2+1];
}
long long segQuery(int idx,int l,int r,int ql,int qr) {
    if (qr < l || ql > r) return 0;
    if (ql <= l && r <= qr) return seg[idx];
    int mid=(l+r)/2;
    return segQuery(idx*2, l, mid, ql, qr) + segQuery(idx*2+1, mid+1, r, ql, qr);
}

// ----------------------------- Max-heap for Power Sources -----------------------------
struct Source { int id; char type[32]; int score; };
Source srcHeap[512];
int srcHeapSize = 0;

void srcSwap(int a,int b) { Source t=srcHeap[a]; srcHeap[a]=srcHeap[b]; srcHeap[b]=t; }
void srcHeapifyUp(int i) {
    while (i>0) {
        int p=(i-1)/2;
        if (srcHeap[i].score > srcHeap[p].score) { srcSwap(i,p); i=p; }
        else break;
    }
}
void srcPush(Source s) {
    srcHeap[srcHeapSize] = s;
    srcHeapifyUp(srcHeapSize);
    srcHeapSize++;
}
Source srcTop() {
    if (srcHeapSize==0) { Source s; s.id=-1; strcpy(s.type,"None"); s.score=-1; return s; }
    return srcHeap[0];
}

// ----------------------------- CSV loader -----------------------------
void loadCSV(const char *fname) {
    ifstream fin(fname);
    if (!fin.is_open()) {
        cerr << "Unable to open " << fname << "\n";
        exit(1);
    }
    string line;
    getline(fin, line); // header
    int idx = 0;
    string tokens[32];
    while (getline(fin, line)) {
        if ((int)line.size() < 2) continue;
        int cnt;
        splitCSVLine(line, tokens, cnt);
        // Expecting at least 12 columns (see guidance)
        // tokens[0]=NodeID, [1]=NodeType, [7]=ConnectedTo, [8]=LineResistance, [11]=MeterReadingMonthly
        int nodeid = 0;
        try { nodeid = stoi(tokens[0]); }
        catch(...) { nodeid = idx; }
        NodeID[idx] = nodeid;
        string ttype = (cnt>1?tokens[1]:"Unknown");
        strncpy(NodeType[idx], ttype.c_str(), sizeof(NodeType[idx])-1);
        NodeType[idx][sizeof(NodeType[idx])-1]=0;
        int conn = 0;
        if (cnt>7) {
            try { conn = stoi(tokens[7]); } catch(...) { conn = 0; }
        }
        ConnectedTo[idx] = conn;
        int rint = 1;
        if (cnt>8) {
            try {
                float rf = stof(tokens[8]);
                rint = max(1, (int)round(rf));
            } catch(...) { rint = 1; }
        }
        LineResInt[idx] = rint;
        long long month = 0;
        if (cnt>11) {
            try { month = stoll(tokens[11]); } catch(...) { month = 0; }
        }
        MeterMonth[idx] = month;
        idx++;
        if (idx >= MAX_NODES) break;
    }
    N = idx;
    fin.close();
}

// ----------------------------- Kruskal MST -----------------------------
long long runKruskalAndPrint() {
    // prepare KEdges from CSV rows (one undirected edge per row: node i <-> ConnectedTo[i])
    kedgeCount = 0;
    for (int i = 0; i < N; ++i) {
        int u = i;
        int v = ConnectedTo[i];
        if (v < 0 || v >= N) continue;
        int w = LineResInt[i];
        // to avoid duplicates in undirected MST, add only u < v representation OR add all but Kruskal's union-find will handle duplicates
        if (u <= v) {
            KEdges[kedgeCount].u = u;
            KEdges[kedgeCount].v = v;
            KEdges[kedgeCount].w = w;
            kedgeCount++;
            if (kedgeCount >= MAX_EDGES) break;
        }
    }
    sort(KEdges, KEdges + kedgeCount, cmpKEdge);
    uf_make(N);
    int used = 0;
    long long total = 0;
    for (int i = 0; i < kedgeCount && used < N-1; ++i) {
        int u = KEdges[i].u, v = KEdges[i].v, w = KEdges[i].w;
        if (uf_find(u) != uf_find(v)) {
            uf_union(u,v);
            used++;
            total += w;
        }
    }
    return total;
}

// ----------------------------- Dijkstra -----------------------------
long long runDijkstra(int src,int dest, int parent[]) {
    // initialize
    static int distArr[MAX_NODES];
    for (int i = 0; i < N; ++i) { distArr[i] = INF; parent[i] = -1; }
    heapInit(N);
    distArr[src] = 0;
    heapPushOrUpdate(src, 0);
    while (!heapEmpty()) {
        pair<int,int> p = heapPop();
        int u = p.first;
        int d = p.second;
        if (d != distArr[u]) continue;
        if (u == dest) break;
        for (int ei = head[u]; ei != -1; ei = adj_next[ei]) {
            int v = adj_to[ei], w = adj_w[ei];
            if (distArr[u] + w < distArr[v]) {
                distArr[v] = distArr[u] + w;
                parent[v] = u;
                heapPushOrUpdate(v, distArr[v]);
            }
        }
    }
    return (distArr[dest] >= INF ? -1 : distArr[dest]);
}

// ----------------------------- Build adjacency -----------------------------
void buildAdjacency() {
    for (int i = 0; i < N; ++i) head[i] = -1;
    adj_ptr = 0;
    for (int i = 0; i < N; ++i) {
        int u = i, v = ConnectedTo[i];
        if (v < 0 || v >= N) continue;
        int w = LineResInt[i];
        // add u->v and v->u (undirected)
        addEdgeAdj(u, v, w);
        addEdgeAdj(v, u, w);
        E += 2;
    }
}

// ----------------------------- main -----------------------------

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    cout << "Loading CSV 'samarthaka_grid.csv' ...\n";
    loadCSV("samarthaka_grid.csv");
    cout << "Loaded rows: " << N << "\n\n";

    // ------------------ AUTOMATIC HEAP ------------------
    cout << "=== POWER SOURCE SELECTION (HEAP) ===\n";

    srcHeapSize = 0;
    for (int i = 0; i < N && i < 512; ++i) {
        if (strcmp(NodeType[i], "SolarPlant") == 0 ||
            strcmp(NodeType[i], "WindFarm") == 0 ||
            strcmp(NodeType[i], "HydroPlant") == 0) {

            Source s;
            s.id = i;
            strcpy(s.type, NodeType[i]);
            s.score = 50 + (rand() % 51);  // 50–100
            srcPush(s);
        }
    }

    Source best = srcTop();
    cout << "Best Power Source Automatically Selected:\n";
    cout << "  ID = " << best.id << "\n";
    cout << "  Type = " << best.type << "\n";
    cout << "  Score = " << best.score << "\n\n";

    // ------------------ BUILD ADJACENCY ------------------
    for (int i = 0; i < N; i++) head[i] = -1;
    E = 0;
    buildAdjacency();
    cout << "Adjacency built. Directed edges = " << E << "\n\n";

    // ------------------ KRUSKAL MST ------------------
    cout << "=== KRUSKAL MINIMUM SPANNING TREE ===\n";
    long long mstCost = runKruskalAndPrint();
    cout << "Total MST Weight = " << mstCost << "\n";

    cout << "Sample MST edges (first 10):\n";
    for (int i = 0; i < min(kedgeCount, 10); i++) {
        cout << "  Edge " << i << ": "
             << KEdges[i].u << " - " << KEdges[i].v
             << " (w=" << KEdges[i].w << ")\n";
    }
    cout << "\n";

    // ------------------ SEGMENT TREE ------------------
    cout << "=== SEGMENT TREE (AUTO DEMO) ===\n";
    segBuild(1, 0, N - 1);

    long long fullSum = segQuery(1, 0, N - 1, 0, N - 1);
    cout << "Total Monthly Consumption of City = " << fullSum << "\n";

    cout << "Query: Sum of first 100 houses = "
         << segQuery(1, 0, N - 1, 0, 99) << "\n";

    cout << "Updating meter[10] += 500...\n";
    MeterMonth[10] += 500;
    segUpdate(1, 0, N - 1, 10, MeterMonth[10]);

    cout << "New sum of first 100 houses = "
         << segQuery(1, 0, N - 1, 0, 99) << "\n\n";

    // ------------------ DIJKSTRA AUTO ------------------
    cout << "=== DIJKSTRA SHORTEST PATH (AUTO) ===\n";

    int src = 0;
    int dest = ConnectedTo[0];  // natural city route

    int parent[MAX_NODES];
    long long dist = runDijkstra(src, dest, parent);

    if (dist >= 0) {
        cout << "Shortest Path from " << src << " to " << dest << " = " << dist << "\n";
        cout << "Path: ";
        int path[MAX_NODES], len = 0, cur = dest;
        while (cur != -1) {
            path[len++] = cur;
            cur = parent[cur];
        }
        for (int i = len - 1; i >= 0; i--)
            cout << path[i] << (i ? " -> " : "\n");
    } else {
        cout << "No path found.\n";
    }
    cout << "\n";

    // ------------------ OUTAGE DETECTION AUTO ------------------
  
    // ============================
// OUTAGE DETECTION (AUTO MODE)
// ============================
cout << "\n=== OUTAGE DETECTION (AUTO) ===\n";
cout << "Simulating failures on edges #2, #5, #10\n\n";

// Declare ONCE
static bool disabled[MAX_EDGES];

// Reset all to false
for (int i = 0; i < kedgeCount; i++)
    disabled[i] = false;

// Mark failures
disabled[2] = true;
disabled[5] = true;
disabled[10] = true;

// Recompute connectivity
uf_make(N);
for (int i = 0; i < kedgeCount; i++) {
    if (!disabled[i])
        uf_union(KEdges[i].u, KEdges[i].v);
}

// Determine disconnected nodes
int root = uf_find(0);
int groupCount = 0;

cout << "Disconnected Nodes (showing first 15 only):\n";

for (int i = 0; i < N; i++) {
    if (uf_find(i) != root) {

        if (groupCount < 15)
            cout << "  • Node " << i << " disconnected\n";

        groupCount++;
    }
}

if (groupCount == 0) {
    cout << "  No nodes were disconnected. Grid remains stable.\n";
} else {
    cout << "\nTotal disconnected nodes = " << groupCount << "\n";
    cout << "(Only first 15 shown to maintain clean output.)\n";
}

cout << "\n=== END OF OUTAGE REPORT ===\n\n";



    cout << "=== PROGRAM COMPLETE (AUTO MODE) ===\n";
    return 0;
}
