#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

/* ================= CONSTANTS ================= */
const int MAX_NODES = 10000;
const int HASH_SIZE = 20011;
const int INF = 1000000000;
const int FW_LIMIT = 200;   // SAFE LIMIT for Floyd–Warshall

/* ================= GLOBALS ================= */
int N = 0;
int graphMat[MAX_NODES][MAX_NODES];
int distFW[FW_LIMIT][FW_LIMIT];
int nextFW[FW_LIMIT][FW_LIMIT];

/* ================= HASH TABLE ================= */
struct BackupRoute {
    int towerId;
    int count;
    int backups[5];
    bool used;
};

BackupRoute tableH[HASH_SIZE];

int hashFunc(int k) { return (k < 0 ? -k : k) % HASH_SIZE; }

void initHash() {
    for (int i = 0; i < HASH_SIZE; i++) {
        tableH[i].used = false;
        tableH[i].count = 0;
    }
}

void insertBackup(int towerID, int count, int arr[]) {
    int h = hashFunc(towerID);
    for (int i = 0; i < HASH_SIZE; i++) {
        int p = (h + i) % HASH_SIZE;
        if (!tableH[p].used) {
            tableH[p].used = true;
            tableH[p].towerId = towerID;
            tableH[p].count = count;
            for (int j = 0; j < count; j++)
                tableH[p].backups[j] = arr[j];
            return;
        }
    }
}

BackupRoute* getBackup(int towerID) {
    int h = hashFunc(towerID);
    for (int i = 0; i < HASH_SIZE; i++) {
        int p = (h + i) % HASH_SIZE;
        if (!tableH[p].used) return nullptr;
        if (tableH[p].towerId == towerID) return &tableH[p];
    }
    return nullptr;
}

/* ================= BST ================= */
struct SessionNode {
    int userID, bw;
    SessionNode *left, *right;
};

SessionNode* Root = nullptr;

SessionNode* newNode(int u, int b) {
    SessionNode* n = new SessionNode;
    n->userID = u; n->bw = b;
    n->left = n->right = nullptr;
    return n;
}

SessionNode* insertSession(SessionNode* r, int u, int bw) {
    if (!r) return newNode(u, bw);
    if (bw < r->bw) r->left = insertSession(r->left, u, bw);
    else r->right = insertSession(r->right, u, bw);
    return r;
}

int topUsers[10][2], topCount = 0;

void collectTop(SessionNode* r) {
    if (!r || topCount >= 10) return;
    collectTop(r->left);
    if (topCount < 10) {
        topUsers[topCount][0] = r->userID;
        topUsers[topCount][1] = r->bw;
        topCount++;
    }
    collectTop(r->right);
}

void findMax(SessionNode* r, int &bw, int &u) {
    if (!r) return;
    if (r->bw > bw) { bw = r->bw; u = r->userID; }
    findMax(r->left, bw, u);
    findMax(r->right, bw, u);
}

/* ================= CSV LOADER ================= */
void loadCSV(const char* file) {
    ifstream fin(file);
    if (!fin) { cout << "CSV open failed\n"; exit(0); }

    string line; getline(fin, line);
    int count = 0;

    while (getline(fin, line) && count < MAX_NODES) {
        stringstream ss(line);
        string cell;
        int node, to, dist, arr[5] = {0}, bcount = 0, user, bw;

        getline(ss, cell, ','); node = stoi(cell);
        getline(ss, cell, ','); // zone
        getline(ss, cell, ','); to = stoi(cell);
        getline(ss, cell, ','); dist = stoi(cell);

        for (int i = 0; i < 5; i++) {
            getline(ss, cell, ',');
            if (!cell.empty()) arr[bcount++] = stoi(cell);
        }

        getline(ss, cell, ','); user = stoi(cell);
        getline(ss, cell, ','); bw = stoi(cell);

        graphMat[node][to] = graphMat[to][node] = dist;
        insertBackup(node, bcount, arr);
        Root = insertSession(Root, user, bw);
        count++;
    }
    N = count;
}

/* ================= PRIM MST ================= */
void runPrim() {
    bool used[MAX_NODES];
    int key[MAX_NODES], parent[MAX_NODES];

    for (int i = 0; i < N; i++)
        used[i] = false, key[i] = INF, parent[i] = -1;

    key[0] = 0;

    for (int k = 0; k < N; k++) {
        int u = -1, best = INF;
        for (int i = 0; i < N; i++)
            if (!used[i] && key[i] < best)
                best = key[i], u = i;

        if (u == -1) break;
        used[u] = true;

        for (int v = 0; v < N; v++)
            if (!used[v] && graphMat[u][v] < key[v])
                key[v] = graphMat[u][v], parent[v] = u;
    }

    cout << "\n--- COMMUNICATION BACKBONE (PRIM MST) ---\n";
    long long total = 0; int shown = 0;

    for (int i = 1; i < N; i++)
        if (parent[i] != -1) {
            total += graphMat[parent[i]][i];
            if (shown++ < 5)
                cout << parent[i] << " -> " << i
                     << " (cost " << graphMat[parent[i]][i] << ")\n";
        }
    cout << "Total Backbone Cost = " << total << "\n";
}

/* ================= FLOYD WARSHALL (LIMITED) ================= */

void runFWLimited() {
    int M = min(N, FW_LIMIT);

    cout << "\n--- ROUTING ENGINE (FLOYD WARSHALL) ---\n";
    cout << "NOTE:\n";
    cout << "Floyd-Warshall has O(N^3) time complexity.\n";
    cout << "For large datasets (N = " << N << "), running it fully is impractical.\n";
    cout << "Hence, Floyd-Warshall is demonstrated on first "
         << M << " nodes only.\n\n";

    for (int i = 0; i < M; i++)
        for (int j = 0; j < M; j++) {
            distFW[i][j] = graphMat[i][j];
            nextFW[i][j] = (graphMat[i][j] < INF ? j : -1);
        }

    for (int k = 0; k < M; k++)
        for (int i = 0; i < M; i++)
            for (int j = 0; j < M; j++)
                if (distFW[i][k] + distFW[k][j] < distFW[i][j]) {
                    distFW[i][j] = distFW[i][k] + distFW[k][j];
                    nextFW[i][j] = nextFW[i][k];
                }

    cout << "Sample shortest path (within limited nodes):\n";
    cout << "Path 0 -> 50 | Cost = " << distFW[0][50] << "\n";
}


/* ================= FAILURE DEMO ================= */
void failureDemo() {
    cout << "\n--- FAILURE RECOVERY (HASH TABLE) ---\n";
    int fail[3] = {10, 25, 50};

    for (int f : fail) {
        cout << "Tower " << f << " FAILED\n";
        BackupRoute* br = getBackup(f);
        if (!br) cout << " No backup\n";
        else {
            cout << " Backup: ";
            for (int i = 0; i < br->count; i++) {
                cout << br->backups[i];
                if (i + 1 < br->count) cout << " -> ";
            }
            cout << "\n";
        }
    }
}

/* ================= MAIN ================= */
int main() {
    for (int i = 0; i < MAX_NODES; i++)
        for (int j = 0; j < MAX_NODES; j++)
            graphMat[i][j] = (i == j ? 0 : INF);

    initHash();

    cout << "SAMARTHAKA SMART GRID SYSTEM\n";
    cout << "Loading CSV...\n";
    loadCSV("samarthaka_network.csv");
    cout << "Loaded Nodes: " << N << "\n";

    runPrim();
    runFWLimited();
    failureDemo();

    cout << "\n--- LOAD BALANCING (BST) ---\n";
    topCount = 0; collectTop(Root);

    for (int i = 0; i < topCount; i++)
        cout << "User " << topUsers[i][0]
             << " -> " << topUsers[i][1] << " Mbps\n";

    int u = -1, bw = -1;
    findMax(Root, bw, u);
    cout << "Highest Consumer: User " << u
         << " (" << bw << " Mbps)\n";

    cout << "\nDONE.\n";
    return 0;
}
