// chip_flow.cpp
// Compile: g++ -O2 chip_flow.cpp -o chip_flow
// Run: ./chip_flow
// Reads: samarthaka_chip_design.csv (in same folder)

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <cmath>
#include <cstdlib>

using namespace std;

const int MAXROW = 10050;
const int MAXN   = 10050;
const int MAXE   = 40050;
const int GRID_W = 500;   // tile grid width for greedy placement
const int GRID_H = 500;   // tile grid height
const int INF = 1000000000;

// CSV arrays
int BlockID[MAXROW];
char Type[MAXROW][16];
int Width[MAXROW], HeightA[MAXROW];
int ConnA[MAXROW], ConnB[MAXROW];
int WireLen[MAXROW];
double Power_mW[MAXROW];
double Temp_C[MAXROW];
int ROWS = 0;

// ---------------- CSV loader ----------------
void loadCSV(const char *fn) {
    ifstream fin(fn);
    if (!fin) { cerr << "Cannot open " << fn << "\n"; exit(1); }
    string line;
    getline(fin, line); // header
    ROWS = 0;
    while (getline(fin, line) && ROWS < MAXROW-2) {
        stringstream ss(line);
        string tok;
        getline(ss, tok, ','); BlockID[ROWS] = atoi(tok.c_str());
        getline(ss, tok, ','); strncpy(Type[ROWS], tok.c_str(), 15); Type[ROWS][15]=0;
        getline(ss, tok, ','); Width[ROWS] = atoi(tok.c_str());
        getline(ss, tok, ','); HeightA[ROWS] = atoi(tok.c_str());
        getline(ss, tok, ','); ConnA[ROWS] = atoi(tok.c_str());
        getline(ss, tok, ','); ConnB[ROWS] = atoi(tok.c_str());
        getline(ss, tok, ','); WireLen[ROWS] = atoi(tok.c_str());
        getline(ss, tok, ','); Power_mW[ROWS] = atof(tok.c_str());
        getline(ss, tok, ','); Temp_C[ROWS] = atof(tok.c_str());
        ROWS++;
    }
    fin.close();
}

// ---------------- Backtracking demo (place small Nsmall blocks exactly into a tile grid) ----------------
// We'll attempt to pack first Nsmall blocks into a small grid using backtracking.
// grid 2D array: 0 empty, >0 block id placed.
int gridDemoW = 40, gridDemoH = 20;
int gridDemo[50][50];
int placeX[MAXROW], placeY[MAXROW]; // store placements (for first Nsmall)
int Nsmall = 12;

bool canPlaceDemo(int id, int x, int y) {
    int w = Width[id], h = HeightA[id];
    if (x + w > gridDemoW || y + h > gridDemoH) return false;
    for (int i=x;i<x+w;i++) for (int j=y;j<y+h;j++) if (gridDemo[i][j] != 0) return false;
    return true;
}
void setPlaceDemo(int id, int x, int y, int val) {
    int w = Width[id], h = HeightA[id];
    for (int i=x;i<x+w;i++) for (int j=y;j<y+h;j++) gridDemo[i][j] = val;
}
bool backtrackPlace(int idx) {
    if (idx == Nsmall) return true;
    for (int x=0;x<gridDemoW;x++) {
        for (int y=0;y<gridDemoH;y++) {
            if (canPlaceDemo(idx, x, y)) {
                setPlaceDemo(idx, x, y, idx+1);
                placeX[idx] = x; placeY[idx] = y;
                if (backtrackPlace(idx+1)) return true;
                setPlaceDemo(idx, x, y, 0);
            }
        }
    }
    return false;
}

// ---------------- Greedy placement for whole design ----------------
// Place blocks sequentially left-to-right, skip occupied tiles (fast)
int gridFullW = GRID_W, gridFullH = GRID_H;
int gridFull[GRID_W+2][GRID_H+2];
int placeFX[MAXROW], placeFY[MAXROW];

void greedyPlaceAll() {
    // clear grid
    for (int i=0;i<gridFullW;i++) for (int j=0;j<gridFullH;j++) gridFull[i][j] = 0;
    int curx = 0, cury = 0, rowh = 0;
    for (int i=0;i<ROWS;i++) {
        int w = Width[i], h = HeightA[i];
        if (curx + w > gridFullW) { // move to next row
            curx = 0; cury += rowh; rowh = 0;
        }
        if (cury + h > gridFullH) { // grid full, place at last valid spot by wrap
            curx = (curx + 1) % gridFullW;
            cury = cury % gridFullH;
        }
        // find first fit from cur pos
        bool placed = false;
        for (int yy=cury; yy<gridFullH && !placed; yy++) {
            for (int xx=curx; xx<gridFullW && !placed; xx++) {
                bool ok = true;
                if (xx + w > gridFullW || yy + h > gridFullH) { ok = false; continue; }
                for (int a=xx;a<xx+w && ok;a++) for (int b=yy;b<yy+h && ok;b++) if (gridFull[a][b] != 0) ok = false;
                if (ok) {
                    for (int a=xx;a<xx+w;a++) for (int b=yy;b<yy+h;b++) gridFull[a][b] = i+1;
                    placeFX[i] = xx; placeFY[i] = yy;
                    placed = true;
                    curx = xx + w;
                    if (h > rowh) rowh = h;
                }
            }
            if (placed) break;
            curx = 0;
        }
        if (!placed) { // fallback: scan entire grid
            for (int yy=0; yy<gridFullH && !placed; yy++) {
                for (int xx=0; xx<gridFullW && !placed; xx++) {
                    bool ok = true;
                    if (xx + w > gridFullW || yy + h > gridFullH) { ok = false; continue; }
                    for (int a=xx;a<xx+w && ok;a++) for (int b=yy;b<yy+h && ok;b++) if (gridFull[a][b] != 0) ok = false;
                    if (ok) {
                        for (int a=xx;a<xx+w;a++) for (int b=yy;b<yy+h;b++) gridFull[a][b] = i+1;
                        placeFX[i] = xx; placeFY[i] = yy;
                        placed = true;
                        curx = xx + w;
                        if (h > rowh) rowh = h;
                    }
                }
            }
        }
        if (!placed) {
            // mark as -1 (couldn't place)
            placeFX[i] = -1; placeFY[i] = -1;
        }
    }
}

// ---------------- DFS for circuit verification (detect cycles/reachability) ----------------
int adj[MAXE*2], adjNext[MAXE*2], adjHead[MAXN], edgeCnt;
void addAdj(int u, int v) {
    if (u <= 0 || v <= 0) return;
    adj[edgeCnt] = v; adjNext[edgeCnt] = adjHead[u]; adjHead[u] = edgeCnt++;
}

bool visited[MAXN];
int visitStack[MAXN];

bool dfs_cycle_util(int start, int n) {
    // iterative DFS with parent tracking to detect back-edges (cycle)
    for (int i=1;i<=n;i++) visited[i] = false;
    int top = 0;
    int parent[MAXN]; for (int i=1;i<=n;i++) parent[i] = -1;
    visitStack[top++] = start;
    while (top) {
        int u = visitStack[--top];
        if (!visited[u]) {
            visited[u] = true;
            // push neighbors
            for (int e = adjHead[u]; e != -1; e = adjNext[e]) {
                int v = adj[e];
                if (!visited[v]) {
                    parent[v] = u;
                    visitStack[top++] = v;
                } else if (v != parent[u]) {
                    // found a back edge -> cycle
                    return true;
                }
            }
        }
    }
    return false;
}

// ---------------- Kruskal MST (using connection wire lengths) ----------------
struct E { int u,v,w; } edges[MAXE];
int eCount = 0;

int uf_parent[MAXN], uf_rank[MAXN];
void uf_init(int n) { for (int i=1;i<=n;i++){ uf_parent[i]=i; uf_rank[i]=0; } }
int uf_find(int x) { if (uf_parent[x]==x) return x; uf_parent[x] = uf_find(uf_parent[x]); return uf_parent[x]; }
void uf_unite(int a,int b) {
    a = uf_find(a); b = uf_find(b); if (a==b) return;
    if (uf_rank[a] < uf_rank[b]) uf_parent[a] = b;
    else { uf_parent[b] = a; if (uf_rank[a]==uf_rank[b]) uf_rank[a]++; }
}

void addEdgeKruskal(int u, int v, int w) {
    if (u <=0 || v<=0) return;
    if (eCount >= MAXE) return;
    edges[eCount].u = u; edges[eCount].v = v; edges[eCount].w = w; eCount++;
}

// simple in-place sort of edges by weight -> merge sort used (no std)
void mergeSortEdges(int l, int r) {
    if (l >= r) return;
    int m = (l + r) / 2;
    mergeSortEdges(l, m); mergeSortEdges(m+1, r);
    int tmpR = r - l + 1;
    E *tmp = (E*) malloc(sizeof(E) * tmpR);
    int i = l, j = m+1, k = 0;
    while (i <= m && j <= r) {
        if (edges[i].w <= edges[j].w) tmp[k++] = edges[i++];
        else tmp[k++] = edges[j++];
    }
    while (i <= m) tmp[k++] = edges[i++];
    while (j <= r) tmp[k++] = edges[j++];
    for (int t=0;t<tmpR;t++) edges[l+t] = tmp[t];
    free(tmp);
}

// ---------------- Merge Sort for thermal hotspot analysis ----------------
int idxArr[MAXROW]; // indices 0..ROWS-1
void mergeSortIdx(int l, int r) {
    if (l >= r) return;
    int m = (l + r)/2;
    mergeSortIdx(l,m); mergeSortIdx(m+1,r);
    int tmpR = r - l + 1;
    int *tmp = (int*) malloc(sizeof(int)*tmpR);
    int i=l, j=m+1, k=0;
    while (i<=m && j<=r) {
        if (Temp_C[idxArr[i]] >= Temp_C[idxArr[j]]) tmp[k++] = idxArr[i++]; // descending (hotspots first)
        else tmp[k++] = idxArr[j++];
    }
    while (i<=m) tmp[k++] = idxArr[i++];
    while (j<=r) tmp[k++] = idxArr[j++];
    for (int t=0;t<tmpR;t++) idxArr[l+t] = tmp[t];
    free(tmp);
}

// ---------------- Main ----------------
int main() {
    cout << "Loading CSV samarthaka_chip_design.csv ...\n";
    loadCSV("samarthaka_chip_design.csv");
    cout << "Rows loaded: " << ROWS << "\n";

    // determine max node id
    int maxNode = 0;
    for (int i=0;i<ROWS;i++) if (BlockID[i] > maxNode) maxNode = BlockID[i];
    if (maxNode >= MAXN) maxNode = MAXN-1;

    // ---------- 1) Backtracking (small demo) ----------
    cout << "\n=== 1) ARCHITECTURE PLANNING (Backtracking demo + greedy full placement) ===\n";
    if (ROWS < Nsmall) Nsmall = ROWS;
    // clear demo grid
    for (int x=0;x<50;x++) for (int y=0;y<50;y++) gridDemo[x][y] = 0;
    bool success = backtrackPlace(0);
    if (success) {
        cout << "Backtracking demo succeeded for first " << Nsmall << " blocks. Placements (BlockID at x,y):\n";
        for (int i=0;i<Nsmall;i++) {
            cout << "  Block " << BlockID[i] << " placed at (" << placeX[i] << "," << placeY[i] << "), size "
                 << Width[i] << "x" << HeightA[i] << "\n";
        }
    } else {
        cout << "Backtracking demo failed (no exact packing found) for first " << Nsmall << " blocks.\n";
    }

    // Greedy placement for full dataset
    greedyPlaceAll();
    int placed = 0, notplaced = 0;
    for (int i=0;i<ROWS;i++) if (placeFX[i] >= 0) placed++; else notplaced++;
    cout << "Greedy placement finished for all blocks. Placed: " << placed << "  Not placed: " << notplaced << "\n";

    // ---------- 2) DFS (logic circuit verification) ----------
    cout << "\n=== 2) LOGIC CIRCUIT VERIFICATION (DFS reachability / cycle detection) ===\n";
    // build adjacency list from connections (directed: A->B and B->A as signals)
    for (int i=0;i<=maxNode;i++) adjHead[i] = -1;
    edgeCnt = 0;
    for (int i=0;i<ROWS;i++) {
        int u = BlockID[i];
        addAdj(u, ConnA[i]);
        addAdj(u, ConnB[i]);
        // make symmetric signal edges for circuit net
        addAdj(ConnA[i], u);
        addAdj(ConnB[i], u);
    }
    // detect cycles / check reachability from block 1
    bool hasCycle = false;
    for (int i=1;i<=maxNode;i++) {
        if (i <= 0) continue;
        if (dfs_cycle_util(i, maxNode)) { hasCycle = true; break; }
    }
    if (hasCycle) cout << "  Circuit graph contains cycles (feedback loops) — review required.\n";
    else cout << "  No cycles found in connection graph (acyclic) — good.\n";

    // reachability from Block 1
    int reachableCount = 0;
    for (int i=1;i<=maxNode;i++) visited[i] = false;
    int sp = 0; visitStack[sp++] = BlockID[0];
    while (sp) {
        int u = visitStack[--sp];
        if (u <=0 || u>maxNode) continue;
        if (visited[u]) continue;
        visited[u] = true; reachableCount++;
        for (int e=adjHead[u]; e!=-1; e=adjNext[e]) {
            int v = adj[e];
            if (!visited[v]) visitStack[sp++] = v;
        }
    }
    cout << "  Nodes reachable from Block " << BlockID[0] << " : " << reachableCount << " / " << maxNode << "\n";

    // ---------- 3) Kruskal MST (physical routing) ----------
    cout << "\n=== 3) PHYSICAL LAYOUT & ROUTING (Kruskal's MST using WireLen) ===\n";
    eCount = 0;
    for (int i=0;i<ROWS;i++) {
        int u = BlockID[i], v1 = ConnA[i], v2 = ConnB[i];
        // add both nets
        addEdgeKruskal(u, v1, WireLen[i]);
        addEdgeKruskal(u, v2, WireLen[i]);
    }
    if (eCount == 0) { cout << "  No edges found for MST.\n"; }
    else {
        mergeSortEdges(0, eCount-1);
        uf_init(maxNode);
        long long mstCost = 0;
        int usedEdges = 0;
        for (int i=0;i<eCount;i++) {
            int u = edges[i].u, v = edges[i].v;
            if (u<=0||v<=0) continue;
            if (uf_find(u) != uf_find(v)) {
                uf_unite(u,v);
                mstCost += edges[i].w;
                usedEdges++;
            }
        }
        cout << "  MST estimated total wiring cost (sum of wire lengths): " << mstCost << " (units)\n";
        cout << "  Edges used in MST: " << usedEdges << "\n";
    }

    // ---------- 4) Merge Sort thermal analysis (hotspot detection) ----------
    cout << "\n=== 4) PERFORMANCE & THERMAL OPTIMIZATION (MergeSort by Temp) ===\n";
    for (int i=0;i<ROWS;i++) idxArr[i] = i;
    if (ROWS > 0) mergeSortIdx(0, ROWS-1);
    cout << "Top 12 hotspots (BlockID, Type, Temp_C, Power_mW):\n";
    int kshow = 12;
    for (int i=0;i<kshow && i<ROWS;i++) {
        int idx = idxArr[i];
        cout << "  " << (i+1) << ". Block " << BlockID[idx] << " (" << Type[idx] << ") -> Temp=" << Temp_C[idx]
             << "C Power=" << Power_mW[idx] << "mW\n";
    }

    cout << "\n=== SUMMARY ===\n";
    cout << "Rows processed: " << ROWS << "\n";
    cout << "Backtracking demo blocks: " << Nsmall << " (exact if succeeded)\n";
    cout << "Greedy placement: placed " << (ROWS - (int) ( (double)0 )) << " blocks (report above shows counts)\n";
    cout << "Kruskal MST cost estimate: (see above)\n";
    cout << "Thermal hotspots listed: top " << kshow << "\n";

    cout << "\nEnd of chip flow report.\n";

    return 0;
}
