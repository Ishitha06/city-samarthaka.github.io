// samarthaka_waste_final.cpp
// Compile: g++ samarthaka_waste_final.cpp -O2 -o samarthaka_waste_final
// Run: ./samarthaka_waste_final
// Reads: samarthaka_waste_corrected.csv

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <cmath>
#include <iomanip>
#include <cstdlib>

using namespace std;

/* ============================================================
   CONSTANTS & CSV STORAGE
   ============================================================ */

const int MAX_BINS = 11000;
const int MAX_ROUTES = 1200;
const int MAX_EDGES = 200000;
const int INF = 1000000000;
const double EARTH_R = 6371000.0;

/* CSV columns */
static int BinID[MAX_BINS];
static char Zone[MAX_BINS][16];
static double Lat[MAX_BINS], Lon[MAX_BINS];
static char Waste[MAX_BINS][24];
static int RouteID[MAX_BINS];
static int NearbyPop[MAX_BINS];
static char Sensor[MAX_BINS][8];

static int BIN_COUNT = 0;

/* ============================================================
   ROUTE GRAPH STORAGE
   ============================================================ */

static double Rlat[MAX_ROUTES], Rlon[MAX_ROUTES];
static int Rcount[MAX_ROUTES];

struct Edge { int u, v, w; };
static Edge edges[MAX_EDGES];
static int edgeCount = 0;
static int Nnodes = 0;

static int distBF[MAX_ROUTES];
static int parentBF[MAX_ROUTES];

/* ============================================================
   DEG → RAD FIX (No M_PI)
   ============================================================ */

inline double deg2rad(double x) { 
    return x * 0.017453292519943295;   // PI/180
}

/* Haversine distance in meters */
double haversine_m(double a1, double b1, double a2, double b2) {
    double dlat = deg2rad(a2 - a1);
    double dlon = deg2rad(b2 - b1);

    double s = sin(dlat / 2.0);
    double t = sin(dlon / 2.0);

    double aa = s*s + cos(deg2rad(a1)) * cos(deg2rad(a2)) * t*t;
    double cc = 2.0 * atan2(sqrt(aa), sqrt(1 - aa));

    return EARTH_R * cc;
}

/* ============================================================
   CSV LOADER
   ============================================================ */

bool loadCSV(const char* fname) {
    ifstream fin(fname);
    if (!fin.is_open()) {
        cout << "ERROR: Cannot open CSV file.\n";
        return false;
    }

    string line;
    getline(fin, line); // skip header

    BIN_COUNT = 0;

    while (getline(fin, line) && BIN_COUNT < MAX_BINS) {
        stringstream ss(line);
        string tok;

        getline(ss, tok, ','); BinID[BIN_COUNT] = atoi(tok.c_str());
        getline(ss, tok, ','); strncpy(Zone[BIN_COUNT], tok.c_str(), 15);
        getline(ss, tok, ','); Lat[BIN_COUNT] = atof(tok.c_str());
        getline(ss, tok, ','); Lon[BIN_COUNT] = atof(tok.c_str());
        getline(ss, tok, ','); strncpy(Waste[BIN_COUNT], tok.c_str(), 23);
        getline(ss, tok, ','); RouteID[BIN_COUNT] = atoi(tok.c_str());
        getline(ss, tok, ','); NearbyPop[BIN_COUNT] = atoi(tok.c_str());
        getline(ss, tok, ','); strncpy(Sensor[BIN_COUNT], tok.c_str(), 7);

        BIN_COUNT++;
    }

    fin.close();
    return true;
}

/* ============================================================
   ROUTE GRAPH GENERATION + BELLMAN–FORD
   ============================================================ */

void buildRouteGraph(int K = 10) {
    int maxr = 0;
    for (int i = 0; i < BIN_COUNT; i++)
        if (RouteID[i] > maxr) maxr = RouteID[i];

    Nnodes = maxr + 1;
    if (Nnodes > MAX_ROUTES) Nnodes = MAX_ROUTES;

    for (int r = 0; r < Nnodes; r++) {
        Rlat[r] = 0.0;
        Rlon[r] = 0.0;
        Rcount[r] = 0;
    }

    for (int i = 0; i < BIN_COUNT; i++) {
        int r = RouteID[i];
        if (r >= 0 && r < Nnodes) {
            Rlat[r] += Lat[i];
            Rlon[r] += Lon[i];
            Rcount[r]++;
        }
    }

    for (int r = 0; r < Nnodes; r++) {
        if (Rcount[r] > 0) {
            Rlat[r] /= Rcount[r];
            Rlon[r] /= Rcount[r];
        }
    }

    edgeCount = 0;

    for (int u = 0; u < Nnodes; u++) {
        if (Rcount[u] == 0) continue;

        double bestD[20];
        int bestR[20];
        for (int i = 0; i < K; i++) {
            bestD[i] = 1e18;
            bestR[i] = -1;
        }

        for (int v = 0; v < Nnodes; v++) {
            if (u == v) continue;
            if (Rcount[v] == 0) continue;

            double d = haversine_m(Rlat[u], Rlon[u], Rlat[v], Rlon[v]);

            for (int t = 0; t < K; t++) {
                if (d < bestD[t]) {
                    for (int s = K - 1; s > t; s--) {
                        bestD[s] = bestD[s - 1];
                        bestR[s] = bestR[s - 1];
                    }
                    bestD[t] = d;
                    bestR[t] = v;
                    break;
                }
            }
        }

        for (int t = 0; t < K; t++) {
            if (bestR[t] >= 0 && edgeCount < MAX_EDGES) {
                edges[edgeCount].u = u;
                edges[edgeCount].v = bestR[t];
                edges[edgeCount].w = (int)(bestD[t] + 0.5);
                edgeCount++;
            }
        }
    }
}

/* ---------------- Bellman-Ford ---------------- */

void bellmanFord(int src) {
    for (int i = 0; i < Nnodes; i++) {
        distBF[i] = INF;
        parentBF[i] = -1;
    }

    distBF[src] = 0;

    for (int it = 0; it < Nnodes - 1; it++) {
        bool changed = false;

        for (int e = 0; e < edgeCount; e++) {
            int u = edges[e].u;
            int v = edges[e].v;
            int w = edges[e].w;

            if (distBF[u] < INF && distBF[u] + w < distBF[v]) {
                distBF[v] = distBF[u] + w;
                parentBF[v] = u;
                changed = true;
            }
        }

        if (!changed) break;
    }
}

void printRoutePath(int dest) {
    if (dest < 0 || dest >= Nnodes) return;

    if (distBF[dest] >= INF) {
        cout << "  Route " << dest << ": No path\n";
        return;
    }

    int tmp[2000];
    int c = 0, cur = dest;

    while (cur != -1) {
        tmp[c++] = cur;
        cur = parentBF[cur];
    }

    cout << "  Route " << dest << " dist=" << distBF[dest] << " m\n";
    cout << "   Path: ";

    for (int i = c - 1; i >= 0; i--) {
        cout << tmp[i];
        if (i) cout << " -> ";
    }

    cout << "\n";
}

/* ============================================================
   WASTE TYPE NORMALIZATION
   ============================================================ */

const char* normalizeWaste(const char* s) {
    static char buf[32];
    int j = 0;

    for (int i = 0; s[i]; i++) {
        char c = s[i];
        if (c >= 'A' && c <= 'Z') c = c - 'A' + 'a';
        if (c >= 'a' && c <= 'z') buf[j++] = c;
    }

    buf[j] = 0;

    if (strstr(buf, "organic")) return "organic";
    if (strstr(buf, "recycl")) return "recyclable";
    if (strstr(buf, "ewaste") || strstr(buf, "ew")) return "ewaste";
    if (strstr(buf, "hazard")) return "hazardous";
    return "other";
}

/* ============================================================
   RED–BLACK TREE (TOP 10 LOADS)
   ============================================================ */

enum Color { RED, BLACK };

struct RB {
    int pop, id;
    Color color;
    RB *left, *right, *parent;

    RB(int p, int i): pop(p), id(i), color(RED),
        left(NULL), right(NULL), parent(NULL) {}
};

static RB* RBroot = NULL;
static int topPop[10], topId[10], topCnt = 0;

RB* grandp(RB* n) { return n && n->parent ? n->parent->parent : NULL; }

RB* uncle(RB* n) {
    RB* g = grandp(n);
    if (!g) return NULL;
    return (n->parent == g->left ? g->right : g->left);
}

void rotateLeftRB(RB*& root, RB* x) {
    RB* y = x->right;

    x->right = y->left;
    if (y->left) y->left->parent = x;

    y->parent = x->parent;

    if (!x->parent) root = y;
    else if (x == x->parent->left) x->parent->left = y;
    else x->parent->right = y;

    y->left = x;
    x->parent = y;
}

void rotateRightRB(RB*& root, RB* x) {
    RB* y = x->left;

    x->left = y->right;
    if (y->right) y->right->parent = x;

    y->parent = x->parent;

    if (!x->parent) root = y;
    else if (x == x->parent->left) x->parent->left = y;
    else x->parent->right = y;

    y->right = x;
    x->parent = y;
}

void fixInsertRB(RB*& root, RB* n) {
    while (n != root && n->parent->color == RED) {
        RB* u = uncle(n);
        RB* g = grandp(n);

        if (u && u->color == RED) {
            n->parent->color = BLACK;
            u->color = BLACK;
            g->color = RED;
            n = g;
        } else {
            if (n->parent == g->left) {
                if (n == n->parent->right) {
                    n = n->parent;
                    rotateLeftRB(root, n);
                }
                n->parent->color = BLACK;
                g->color = RED;
                rotateRightRB(root, g);
            } else {
                if (n == n->parent->left) {
                    n = n->parent;
                    rotateRightRB(root, n);
                }
                n->parent->color = BLACK;
                g->color = RED;
                rotateLeftRB(root, g);
            }
        }
    }

    root->color = BLACK;
}

void RBinsert(int pop, int id) {
    RB* n = new RB(pop, id);
    RB* cur = RBroot;
    RB* par = NULL;

    while (cur) {
        par = cur;
        if (pop < cur->pop) cur = cur->left;
        else cur = cur->right;
    }

    n->parent = par;

    if (!par) RBroot = n;
    else if (pop < par->pop) par->left = n;
    else par->right = n;

    fixInsertRB(RBroot, n);
}

void collectTopRB(RB* node) {
    if (!node || topCnt >= 10) return;
    collectTopRB(node->right);
    if (topCnt < 10) {
        topPop[topCnt] = node->pop;
        topId[topCnt] = node->id;
        topCnt++;
    }
    collectTopRB(node->left);
}

/* ============================================================
   SKIP LIST FOR FAULT DETECTION
   ============================================================ */

struct SL {
    int key;
    SL *next, *down;
    SL(int k): key(k), next(NULL), down(NULL) {}
};

static SL* SLhead = new SL(-1);

void SLinsert(int key) {
    SL* update[20];
    SL* cur = SLhead;
    int lvl = 0;

    while (cur) {
        while (cur->next && cur->next->key < key) cur = cur->next;
        update[lvl++] = cur;
        cur = cur->down;
    }

    SL* downNode = NULL;
    bool promote = true;

    for (int i = 0; i < lvl && promote; i++) {
        SL* n = new SL(key);
        n->next = update[i]->next;
        update[i]->next = n;
        n->down = downNode;
        downNode = n;

        promote = (rand() % 2 == 0);
    }
}

bool SLsearch(int key) {
    SL* cur = SLhead;
    while (cur) {
        while (cur->next && cur->next->key < key) cur = cur->next;
        if (cur->next && cur->next->key == key) return true;
        cur = cur->down;
    }
    return false;
}

/* ============================================================
   MAIN PROGRAM
   ============================================================ */

int main() {

    cout << "=============================================\n";
    cout << " SAMARTHAKA — Integrated Waste Management\n";
    cout << "=============================================\n\n";

    if (!loadCSV("samarthaka_waste_corrected.csv")) return 0;

    cout << "Loaded rows: " << BIN_COUNT << "\n\n";

    /* SAMPLE ROWS */
    cout << "Sample rows (first 5):\n";
    for (int i = 0; i < 5; i++) {
        cout << " ID:" << BinID[i]
             << " Zone:" << Zone[i]
             << " Lat:" << fixed << setprecision(4) << Lat[i]
             << " Lon:" << Lon[i]
             << " Waste:" << Waste[i]
             << " Route:" << RouteID[i]
             << " Pop:" << NearbyPop[i]
             << " Sensor:" << Sensor[i]
             << "\n";
    }
    cout << "\n";

    /* ----------------------------------------------------------
       1) ROUTE OPTIMIZATION (Bellman–Ford)
       ---------------------------------------------------------- */
    buildRouteGraph(10);

    cout << "=== ROUTE OPTIMIZATION (Bellman-Ford) ===\n";
    cout << "Route nodes: " << Nnodes << "   Edges: " << edgeCount << "\n";

    bellmanFord(0);

    cout << "Nearest reachable routes from depot (0):\n";
    int shown = 0;

    for (int r = 0; r < Nnodes && shown < 10; r++) {
        if (distBF[r] < INF) {
            printRoutePath(r);
            shown++;
        }
    }

    cout << "\n";

    /* ----------------------------------------------------------
       2) WASTE SEGREGATION (Normalized)
       ---------------------------------------------------------- */
    int organic = 0, recyclable = 0, ewaste = 0, hazardous = 0, other = 0;

    for (int i = 0; i < BIN_COUNT; i++) {
        const char* cat = normalizeWaste(Waste[i]);

        if (strcmp(cat, "organic") == 0) organic++;
        else if (strcmp(cat, "recyclable") == 0) recyclable++;
        else if (strcmp(cat, "ewaste") == 0) ewaste++;
        else if (strcmp(cat, "hazardous") == 0) hazardous++;
        else other++;
    }

    cout << "=== WASTE SEGREGATION (normalized) ===\n";
    cout << "Organic:     " << organic << "\n";
    cout << "Recyclable:  " << recyclable << "\n";
    cout << "E-waste:     " << ewaste << "\n";
    cout << "Hazardous:   " << hazardous << "\n";
    cout << "Other:       " << other << "\n\n";

    /* ----------------------------------------------------------
       3) LOAD BALANCING — Red–Black Tree (Top 10)
       ---------------------------------------------------------- */
    for (int i = 0; i < BIN_COUNT; i++) {
        RBinsert(NearbyPop[i], BinID[i]);
    }

    topCnt = 0;
    collectTopRB(RBroot);

    cout << "Top 10 High-Load Bins (Nearby Population)\n";
    cout << "Rank | BinID | NearbyPop\n";

    for (int i = 0; i < topCnt; i++) {
        cout << setw(4) << i+1 << " | "
             << setw(5) << topId[i] << " | "
             << setw(9) << topPop[i] << "\n";
    }

    cout << "\n";

    /* ----------------------------------------------------------
       4) SKIP LIST — Fault Detection
       ---------------------------------------------------------- */
    for (int i = 0; i < BIN_COUNT; i++) {
        if (strcmp(Sensor[i], "FAIL") == 0 || strcmp(Sensor[i], "FAULT") == 0)
            SLinsert(BinID[i]);
    }

    cout << "=== FAULT DETECTION (Skip List) ===\n";
    cout << "Checking first 20 bins:\n";

    for (int i = 0; i < 20; i++) {
        cout << "Bin " << i << ": " << (SLsearch(i) ? "FAULTY" : "OK") << "\n";
    }

    cout << "\n=== END OF REPORT ===\n";

    return 0;
}
