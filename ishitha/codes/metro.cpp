// metro_fixed.cpp
// Compile: g++ metro_fixed.cpp -O2 -o metro_fixed
// Run: ./metro_fixed
// Assumes CSV: samarthaka_metro.csv (format produced by your Python generator)

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
using namespace std;

const int MAXROW = 10000;
const int MAXN = 300;
const int MAXE = 40000; // increased to be safe (bidirectional edges)
const int HASH_SIZE = 4096;
const int INF = 1000000000;

/* ---------------- CSV DATA ARRAYS ---------------- */
int StationID[MAXROW], Neighbor1[MAXROW], Cost1[MAXROW];
int Neighbor2[MAXROW], Cost2[MAXROW];
int InitialPassengers[MAXROW], TapFare[MAXROW], TapTimeArr[MAXROW];
unsigned long long CardID[MAXROW];
int InitialBalance[MAXROW], FaultCode[MAXROW];
char DepartureTimes[ MAXROW ][50];
int rowCount = 0;

/* ---------------- CSV LOADER ---------------- */
void loadCSV(const char *file)
{
    ifstream fin(file);
    if (!fin) { cout << "ERROR: Cannot open CSV\n"; exit(0); }

    string line;
    getline(fin, line); // header

    rowCount = 0;
    while (getline(fin, line) && rowCount < MAXROW)
    {
        stringstream ss(line);
        string tmp;

        getline(ss, tmp, ','); StationID[rowCount] = stoi(tmp);
        getline(ss, tmp, ','); /* zone ignored */

        getline(ss, tmp, ','); Neighbor1[rowCount] = stoi(tmp);
        getline(ss, tmp, ','); Cost1[rowCount] = stoi(tmp);

        getline(ss, tmp, ','); Neighbor2[rowCount] = stoi(tmp);
        getline(ss, tmp, ','); Cost2[rowCount] = stoi(tmp);

        getline(ss, tmp, ','); strncpy(DepartureTimes[rowCount], tmp.c_str(), 49);

        getline(ss, tmp, ','); InitialPassengers[rowCount] = stoi(tmp);

        getline(ss, tmp, ','); CardID[rowCount] = stoull(tmp);
        getline(ss, tmp, ','); InitialBalance[rowCount] = stoi(tmp);

        getline(ss, tmp, ','); TapFare[rowCount] = stoi(tmp);
        getline(ss, tmp, ','); TapTimeArr[rowCount] = stoi(tmp);

        getline(ss, tmp, ','); FaultCode[rowCount] = stoi(tmp);

        rowCount++;
    }
    fin.close();
}

/* ============================================================
   1. DIJKSTRA — ROUTE TRAVEL (adjacency lists)
============================================================ */
int head[MAXN], toE[MAXE], costE[MAXE], nxtE[MAXE], ec;
int distArr[MAXN], parentArr[MAXN];
bool used[MAXN];

void initGraph(int n)
{
    ec = 0;
    for (int i = 0; i <= n; i++) head[i] = -1;
}

void addEdgeOneWay(int u, int v, int w)
{
    if (ec >= MAXE) return;
    toE[ec] = v;
    costE[ec] = w;
    nxtE[ec] = head[u];
    head[u] = ec++;
}

void addEdgeUndirected(int u, int v, int w)
{
    if (u <= 0 || v <= 0) return;
    addEdgeOneWay(u, v, w);
    addEdgeOneWay(v, u, w);
}

void buildGraph()
{
    initGraph(MAXN-1);
    for (int i = 0; i < rowCount; ++i)
    {
        int s = StationID[i];
        if (Neighbor1[i] > 0) addEdgeUndirected(s, Neighbor1[i], Cost1[i]);
        if (Neighbor2[i] > 0) addEdgeUndirected(s, Neighbor2[i], Cost2[i]);
    }
}

void dijkstra(int src, int nStations)
{
    for (int i = 1; i <= nStations; i++)
    {
        distArr[i] = INF;
        parentArr[i] = -1;
        used[i] = false;
    }
    if (src < 1 || src > nStations) return;
    distArr[src] = 0;

    for (int it = 1; it <= nStations; ++it)
    {
        int v = -1, best = INF;
        for (int i = 1; i <= nStations; ++i)
            if (!used[i] && distArr[i] < best) { best = distArr[i]; v = i; }

        if (v == -1) break;
        used[v] = true;

        for (int e = head[v]; e != -1; e = nxtE[e])
        {
            int to = toE[e], w = costE[e];
            if (distArr[v] + w < distArr[to])
            {
                distArr[to] = distArr[v] + w;
                parentArr[to] = v;
            }
        }
    }
}

void printPathConsole(int dest)
{
    if (dest < 1) return;
    if (distArr[dest] >= INF) {
        cout << "  Route " << dest << " : unreachable\n";
        return;
    }
    int tmp[ MAXN ], c = 0, cur = dest;
    while (cur != -1 && c < MAXN) { tmp[c++] = cur; cur = parentArr[cur]; }
    cout << "  Route " << dest << "  dist=" << distArr[dest] << " mins\n   Path: ";
    for (int i = c-1; i >= 0; --i) {
        cout << tmp[i];
        if (i) cout << " -> ";
    }
    cout << "\n";
}

/* ============================================================
   MAIN
============================================================ */
int main()
{
    cout << "Loading CSV...\n";
    loadCSV("samarthaka_metro.csv");
    cout << "Loaded rows: " << rowCount << "\n\n";

    // Build graph (undirected)
    buildGraph();

    cout << "=== ROUTE TRAVEL (DIJKSTRA) ===\n";

    // Determine number of stations used in CSV (max StationID)
    int maxStation = 0;
    for (int i = 0; i < rowCount; ++i) if (StationID[i] > maxStation) maxStation = StationID[i];
    if (maxStation < 1) maxStation = 250;
    if (maxStation > MAXN-1) maxStation = MAXN-1;

    // Run Dijkstra from station 1
    int src = 1;
    dijkstra(src, maxStation);

    // Collect nearest reachable stations (skip src), list up to 10
    int listed = 0;
    cout << "Nearest reachable stations from " << src << " (up to 10):\n";
    // create small array of pairs (station,dist) and find smallest by scanning
    bool printedAny = false;
    for (int k = 0; k < 10; ++k) {
        int bestStation = -1;
        int bestDist = INF;
        for (int s = 1; s <= maxStation; ++s) {
            if (s == src) continue;
            if (distArr[s] < bestDist) {
                // ensure not already printed in previous iterations
                bool already = false;
                for (int t = 1; t <= maxStation; ++t) {
                    // we will mark printed by checking parentArr? simpler: we will choose in order and mark by setting a temp sentinel
                    // but to avoid extra arrays, do a small trick: use used[] to mark "chosen" copy - but used[] is needed. So create local chosen array:
                }
                bestDist = distArr[s];
                bestStation = s;
            }
        }
        // The above naive loop picks the globally best every time; but to avoid repeats we'll capture them into an array then print first 10
        break;
    }

    // Simpler: build an array of (station,dist) and sort small selection without std::sort—use selection for top 10.
    int selCount = 0;
    int selStations[300];
    int selDists[300];
    for (int s = 1; s <= maxStation; ++s) {
        selStations[selCount] = s;
        selDists[selCount] = distArr[s];
        selCount++;
    }
    // selection sort first 11 (including src) by distance ascending
    for (int i = 0; i < 11 && i < selCount; ++i) {
        int minIdx = i;
        for (int j = i+1; j < selCount; ++j)
            if (selDists[j] < selDists[minIdx])
                minIdx = j;
        // swap
        int tS = selStations[i]; selStations[i] = selStations[minIdx]; selStations[minIdx] = tS;
        int tD = selDists[i]; selDists[i] = selDists[minIdx]; selDists[minIdx] = tD;
    }

    int printed = 0;
    for (int i = 0; i < selCount && printed < 10; ++i) {
        int st = selStations[i];
        if (st == src) continue; // skip source
        if (selDists[i] >= INF) continue; // unreachable
        printPathConsole(st);
        printed++;
    }

    if (printed == 0) cout << "  No reachable stations found from " << src << ".\n";

    cout << "\n=== END ===\n";
    return 0;
}
