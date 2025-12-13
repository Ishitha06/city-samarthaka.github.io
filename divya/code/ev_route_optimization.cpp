#include <bits/stdc++.h>
using namespace std;

/* ==========================
   CSV READER FUNCTION
   ========================== */
vector<vector<string>> readCSV(const string& filename) {
    vector<vector<string>> data;
    ifstream file(filename);
    string line, cell;

    if (!file.is_open()) {
        cout << "Error opening file: " << filename << endl;
        exit(1);
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

/* ==========================
   MAIN FUNCTION
   ========================== */
int main() {

    /* --------------------------
       READ EV STATION DATA
       -------------------------- */
    unordered_map<int, string> stationStatus;
    auto stations = readCSV("ev_stations.csv");

    // Skip header row
    for (int i = 1; i < stations.size(); i++) {
        int node = stoi(stations[i][1]);
        string status = stations[i][3];
        stationStatus[node] = status;
    }

    /* --------------------------
       READ ROAD NETWORK DATA
       -------------------------- */
    auto roads = readCSV("roads.csv");

    int maxNode = 0;
    for (int i = 1; i < roads.size(); i++) {
        int u = stoi(roads[i][0]);
        int v = stoi(roads[i][1]);
        maxNode = max(maxNode, max(u, v));
    }

    vector<vector<pair<int,int>>> graph(maxNode + 1);

    for (int i = 1; i < roads.size(); i++) {
        int u = stoi(roads[i][0]);
        int v = stoi(roads[i][1]);
        int w = stoi(roads[i][2]);

        graph[u].push_back({v, w});
        graph[v].push_back({u, w}); // Undirected graph
    }

    /* --------------------------
       DIJKSTRA'S ALGORITHM
       -------------------------- */
    int source = 0;
    int destination = 3;

    vector<int> dist(graph.size(), INT_MAX);
    priority_queue<pair<int,int>, vector<pair<int,int>>, greater<pair<int,int>>> pq;

    dist[source] = 0;
    pq.push({0, source});

    while (!pq.empty()) {
        auto [currentDist, u] = pq.top();
        pq.pop();

        if (currentDist > dist[u])
            continue;

        for (auto [v, weight] : graph[u]) {
            if (stationStatus[v] == "FAULTY" || stationStatus[v] == "CROWDED")
                continue;

            if (dist[v] > dist[u] + weight) {
                dist[v] = dist[u] + weight;
                pq.push({dist[v], v});
            }
        }
    }

    /* --------------------------
       OUTPUT RESULT
       -------------------------- */
    if (dist[destination] == INT_MAX) {
        cout << "No safe route available to destination." << endl;
    } else {
        cout << "Shortest safe distance from node "
             << source << " to node "
             << destination << " = "
             << dist[destination] << endl;
    }

    return 0;
}
