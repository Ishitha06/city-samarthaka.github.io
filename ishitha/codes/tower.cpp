#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

// =======================================================
// CONSTANTS
// =======================================================
const int MAX_NODES = 10000;
const int HASH_SIZE = 20011;
const int INF = 1000000000;

// =======================================================
// GLOBALS
// =======================================================
int N = 0;
int graphMat[MAX_NODES][MAX_NODES];
int distFW[MAX_NODES][MAX_NODES];
int nextFW[MAX_NODES][MAX_NODES];

// =======================================================
// HASH TABLE FOR FAILURE HANDLING
// =======================================================
struct BackupRoute {
    int towerId;
    int count;
    int backups[5];
    bool used;
};

BackupRoute tableH[HASH_SIZE];

int hashFunc(int k) {
    if (k < 0) k = -k;
    return k % HASH_SIZE;
}

void initHash() {
    for (int i=0;i<HASH_SIZE;i++) {
        tableH[i].used = false;
        tableH[i].towerId = -1;
        tableH[i].count = 0;
    }
}

void insertBackup(int towerID, int count, int arr[]) {
    int h = hashFunc(towerID);
    for (int i=0;i<HASH_SIZE;i++) {
        int pos = (h + i) % HASH_SIZE;
        if (!tableH[pos].used) {
            tableH[pos].used = true;
            tableH[pos].towerId = towerID;
            tableH[pos].count = count;
            for (int j=0;j<count;j++)
                tableH[pos].backups[j] = arr[j];
            return;
        }
    }
}

BackupRoute* getBackup(int towerID) {
    int h = hashFunc(towerID);
    for (int i=0;i<HASH_SIZE;i++) {
        int pos = (h + i) % HASH_SIZE;
        if (!tableH[pos].used)
            return nullptr;
        if (tableH[pos].towerId == towerID)
            return &tableH[pos];
    }
    return nullptr;
}

// =======================================================
// BST FOR LOAD BALANCING
// =======================================================
struct SessionNode {
    int userID;
    int bw;
    SessionNode *left, *right;
};

SessionNode* Root = nullptr;

SessionNode* newNode(int u, int b) {
    SessionNode* n = new SessionNode;
    n->userID = u;
    n->bw = b;
    n->left = n->right = nullptr;
    return n;
}

SessionNode* insertSession(SessionNode* r, int user, int bw) {
    if (!r) return newNode(user, bw);
    if (bw < r->bw) r->left = insertSession(r->left, user, bw);
    else            r->right = insertSession(r->right, user, bw);
    return r;
}

int topUsers[10][2];
int topCount = 0;

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

void findMaxUser(SessionNode* r, int &maxBw, int &userID) {
    if (!r) return;
    if (r->bw > maxBw) {
        maxBw = r->bw;
        userID = r->userID;
    }
    findMaxUser(r->left, maxBw, userID);
    findMaxUser(r->right, maxBw, userID);
}

// =======================================================
// CSV LOADING (STRICT FORMAT)
// =======================================================
void loadCSV(const char* file) {
    ifstream fin(file);
    if (!fin.is_open()) {
        cout << "ERROR: Cannot open CSV\n";
        exit(0);
    }

    string line;
    getline(fin, line); // skip header

    int count = 0;

    while (getline(fin, line) && count < MAX_NODES) {
        stringstream ss(line);

        string cell;
        getline(ss, cell, ','); int node = stoi(cell);
        getline(ss, cell, ','); // zone
        getline(ss, cell, ','); // lat
        getline(ss, cell, ','); // lon

        getline(ss, cell, ','); int to = stoi(cell);
        getline(ss, cell, ','); int dist = stoi(cell);

        graphMat[node][to] = dist;
        graphMat[to][node] = dist;

        getline(ss, cell, ','); int bcount = stoi(cell);
        int arr[5] = {0};

        for (int j=0;j<5;j++) {
            getline(ss, cell, ',');
            arr[j] = stoi(cell);
        }

        insertBackup(node, bcount, arr);

        getline(ss, cell, ','); int user = stoi(cell);
        getline(ss, cell, ','); int bw = stoi(cell);

        Root = insertSession(Root, user, bw);

        count++;
    }

    N = count;
}

// =======================================================
// PRIM’S MST
// =======================================================
void runPrim() {
    bool used[MAX_NODES];
    int key[MAX_NODES];
    int parent[MAX_NODES];

    for (int i=0;i<N;i++) {
        used[i] = false;
        key[i] = INF;
        parent[i] = -1;
    }

    key[0] = 0;

    for (int k=0;k<N;k++) {
        int u = -1, best = INF;
        for (int i=0;i<N;i++)
            if (!used[i] && key[i] < best)
                best = key[i], u = i;

        if (u == -1) break;
        used[u] = true;

        for (int v=0;v<N;v++)
            if (!used[v] && graphMat[u][v] < key[v]) {
                key[v] = graphMat[u][v];
                parent[v] = u;
            }
    }

    cout << "\n--- COMMUNICATION BACKBONE (PRIM MST) ---\n";

    long long total = 0;
    int printed = 0;

    for (int i=1;i<N;i++) {
        if (parent[i] != -1) {
            total += graphMat[parent[i]][i];
            if (printed < 5) {
                cout << " " << parent[i] << " → " << i
                     << " (cost " << graphMat[parent[i]][i] << ")\n";
                printed++;
            }
        }
    }

    cout << "Total Backbone Cost = " << total << "\n";
}

// =======================================================
// FLOYD WARSHALL
// =======================================================
void runFW() {
    for (int i=0;i<N;i++)
        for (int j=0;j<N;j++) {
            distFW[i][j] = graphMat[i][j];
            nextFW[i][j] = (graphMat[i][j] < INF && i != j ? j : -1);
        }

    for (int k=0;k<N;k++)
        for (int i=0;i<N;i++)
            if (distFW[i][k] < INF)
                for (int j=0;j<N;j++)
                    if (distFW[k][j] < INF) {
                        int nd = distFW[i][k] + distFW[k][j];
                        if (nd < distFW[i][j]) {
                            distFW[i][j] = nd;
                            nextFW[i][j] = nextFW[i][k];
                        }
                    }
}

void printPath(int u, int v) {
    while (u != v) {
        cout << u << " → ";
        u = nextFW[u][v];
    }
    cout << v;
}

// =======================================================
// FAILURE HANDLING
// =======================================================
void failureDemo() {
    cout << "\n--- FAILURE RECOVERY (HASH TABLE) ---\n";

    int fail[3] = {10, 25, 50};

    for (int i=0;i<3;i++) {
        cout << " Tower " << fail[i] << " FAILED\n";
        BackupRoute* br = getBackup(fail[i]);

        if (!br || br->count == 0) {
            cout << "   No backup route.\n";
        } else {
            cout << "   Backup: ";
            for (int j=0;j<br->count;j++) {
                cout << br->backups[j];
                if (j+1 < br->count) cout << " → ";
            }
            cout << "\n";
        }
        cout << "\n";
    }
}

// =======================================================
// MAIN
// =======================================================
int main() {

    for (int i=0;i<MAX_NODES;i++)
        for (int j=0;j<MAX_NODES;j++)
            graphMat[i][j] = (i==j ? 0 : INF);

    initHash();

    cout << "SAMARTHAKA SMART GRID SYSTEM\n";
    cout << "Loading CSV...\n";

    loadCSV("samarthaka_network.csv");

    cout << "Loaded Nodes: " << N << "\n";

    runPrim();

    runFW();
    cout << "\n--- ROUTING ENGINE (FLOYD WARSHALL) ---\n";
    int tests[2][2] = {{0,50},{20,999}};

    for (int i=0;i<2;i++) {
        int u = tests[i][0], v = tests[i][1];
        cout << " Fastest path " << u << " → " << v
             << " (cost " << distFW[u][v] << ")\n   ";
        printPath(u,v);
        cout << "\n";
    }

    failureDemo();

    cout << "\n--- LOAD BALANCING (BST) ---\n";

    topCount=0;
    collectTop(Root);

    for (int i=0;i<10;i++)
        cout << " User " << topUsers[i][0]
             << " → " << topUsers[i][1] << " Mbps\n";

    int maxUser=-1,maxBW=-1;
    findMaxUser(Root,maxBW,maxUser);

    cout << "\n Highest Consumer: User " << maxUser
         << " (" << maxBW << " Mbps)\n";

    cout << "\nDONE.\n";
    return 0;
}
