#include <bits/stdc++.h>
using namespace std;

/* ==========================
   CSV READER
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
   ZONE STRUCTURE
   ========================== */
struct Zone {
    int id;
    int wasteAmount;
    bool hazardous;
};

/* ==========================
   PRIORITY QUEUE COMPARATOR
   ========================== */
struct Compare {
    bool operator()(const Zone& a, const Zone& b) {
        return a.hazardous < b.hazardous;
    }
};

int main() {

    /* --------------------------
       READ ZONE DATA
       -------------------------- */
    auto zoneData = readCSV("ewaste_zones.csv");

    unordered_map<int, Zone> zones;
    for (int i = 1; i < zoneData.size(); i++) {
        Zone z;
        z.id = stoi(zoneData[i][0]);
        z.wasteAmount = stoi(zoneData[i][1]);
        z.hazardous = (zoneData[i][2] == "HAZARDOUS");
        zones[z.id] = z;
    }

    /* --------------------------
       READ ROAD NETWORK
       -------------------------- */
    auto roadData = readCSV("ewaste_roads.csv");

    int maxNode = 0;
    for (int i = 1; i < roadData.size(); i++) {
        maxNode = max(maxNode,
            max(stoi(roadData[i][0]), stoi(roadData[i][1])));
    }

    vector<vector<int>> graph(maxNode + 1);

    for (int i = 1; i < roadData.size(); i++) {
        int u = stoi(roadData[i][0]);
        int v = stoi(roadData[i][1]);
        graph[u].push_back(v);
        graph[v].push_back(u);
    }

    /* --------------------------
       BFS TRAVERSAL
       -------------------------- */
    vector<bool> visited(graph.size(), false);
    queue<int> q;
    priority_queue<Zone, vector<Zone>, Compare> pq;

    int depot = 0;
    q.push(depot);
    visited[depot] = true;

    cout << "BFS Discovery Order:\n";

    while (!q.empty()) {
        int u = q.front();
        q.pop();

        cout << "Zone " << u << " discovered\n";
        pq.push(zones[u]);

        for (int v : graph[u]) {
            if (!visited[v]) {
                visited[v] = true;
                q.push(v);
            }
        }
    }

    /* --------------------------
       WASTE COLLECTION
       -------------------------- */
    cout << "\nWaste Collection Order (Hazard Priority):\n";
    while (!pq.empty()) {
        Zone z = pq.top();
        pq.pop();
        cout << "Zone " << z.id
             << " | Waste: " << z.wasteAmount
             << " | Type: "
             << (z.hazardous ? "HAZARDOUS" : "NORMAL") << endl;
    }

    return 0;
}
