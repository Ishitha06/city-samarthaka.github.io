#include <bits/stdc++.h>
using namespace std;

/* =====================================================
   Utility: CSV Reader
   ===================================================== */
vector<vector<string>> readCSV(const string& filename) {
    vector<vector<string>> data;
    ifstream file(filename);
    string line, cell;

    if (!file.is_open()) {
        cerr << "Error opening file: " << filename << endl;
        exit(EXIT_FAILURE);
    }

    while (getline(file, line)) {
        stringstream ss(line);
        vector<string> row;
        while (getline(ss, cell, ',')) {
            row.push_back(cell);
        }
        data.push_back(row);
    }
    file.close();
    return data;
}

/* =====================================================
   Data Models
   ===================================================== */
struct Station {
    int stationId;
    int node;
    int capacity;
    string status; // ACTIVE / FAULTY / CROWDED
};

struct Edge {
    int to;
    int weight;
};

/* =====================================================
   Graph Builder
   ===================================================== */
vector<vector<Edge>> buildGraph(const vector<vector<string>>& roads, int& maxNode) {
    maxNode = 0;
    for (int i = 1; i < roads.size(); i++) {
        maxNode = max(maxNode, max(stoi(roads[i][0]), stoi(roads[i][1])));
    }

    vector<vector<Edge>> graph(maxNode + 1);

    for (int i = 1; i < roads.size(); i++) {
        int u = stoi(roads[i][0]);
        int v = stoi(roads[i][1]);
        int w = stoi(roads[i][2]);

        graph[u].push_back({v, w});
        graph[v].push_back({u, w});
    }
    return graph;
}

/* =====================================================
   Load EV Station Status
   ===================================================== */
unordered_map<int, Station> loadStations(const string& filename) {
    unordered_map<int, Station> stations;
    auto data = readCSV(filename);

    for (int i = 1; i < data.size(); i++) {
        Station s;
        s.stationId = stoi(data[i][0]);
        s.node      = stoi(data[i][1]);
        s.capacity  = stoi(data[i][2]);
        s.status    = data[i][3];

        stations[s.node] = s;
    }
    return stations;
}

/* =====================================================
   Dijkstra with Safety Constraints
   ===================================================== */
vector<int> safeDijkstra(
    int source,
    const vector<vector<Edge>>& graph,
    const unordered_map<int, Station>& stations
) {
    vector<int> dist(graph.size(), INT_MAX);
    priority_queue<pair<int,int>, vector<pair<int,int>>, greater<>> pq;

    dist[source] = 0;
    pq.push({0, source});

    while (!pq.empty()) {
        auto [currDist, u] = pq.top();
        pq.pop();

        if (currDist > dist[u]) continue;

        for (const auto& e : graph[u]) {
            int v = e.to;

            // Skip unsafe EV nodes
            if (stations.count(v)) {
                string status = stations.at(v).status;
                if (status == "FAULTY" || status == "CROWDED")
                    continue;
            }

            if (dist[v] > dist[u] + e.weight) {
                dist[v] = dist[u] + e.weight;
                pq.push({dist[v], v});
            }
        }
    }
    return dist;
}

/* =====================================================
   Path Reachability Check (BFS)
   ===================================================== */
bool isReachable(int source, int destination, const vector<vector<Edge>>& graph) {
    vector<bool> visited(graph.size(), false);
    queue<int> q;

    q.push(source);
    visited[source] = true;

    while (!q.empty()) {
        int u = q.front(); q.pop();
        if (u == destination) return true;

        for (auto& e : graph[u]) {
            if (!visited[e.to]) {
                visited[e.to] = true;
                q.push(e.to);
            }
        }
    }
    return false;
}

/* =====================================================
   MAIN
   ===================================================== */
int main() {

    cout << "=== Smart EV Safe Routing System ===\n";

    /* ---------------------------
       Load Data
       --------------------------- */
    unordered_map<int, Station> stations = loadStations("ev_stations.csv");
    auto roads = readCSV("roads.csv");

    int maxNode;
    vector<vector<Edge>> graph = buildGraph(roads, maxNode);

    int source = 0;
    int destination = 3;

    /* ---------------------------
       Connectivity Check
       --------------------------- */
    if (!isReachable(source, destination, graph)) {
        cout << "Destination not reachable in road network.\n";
        return 0;
    }

    /* ---------------------------
       Shortest Safe Path
       --------------------------- */
    vector<int> dist = safeDijkstra(source, graph, stations);

    /* ---------------------------
       Output
       --------------------------- */
    if (dist[destination] == INT_MAX) {
        cout << "No safe EV route available (all paths blocked by faulty/crowded stations).\n";
    } else {
        cout << "Shortest SAFE distance from node "
             << source << " to node "
             << destination << " = "
             << dist[destination] << " units\n";
    }

    cout << "System execution complete.\n";
    return 0;
}
