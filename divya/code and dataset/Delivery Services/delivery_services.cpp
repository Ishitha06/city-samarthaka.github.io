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
        while (getline(ss, cell, ','))
            row.push_back(cell);
        data.push_back(row);
    }
    return data;
}

/* ==========================
   ORDER STRUCT
   ========================== */
struct Order {
    string id;
    int destination;
    int urgency;
};

struct CompareUrgency {
    bool operator()(const Order& a, const Order& b) {
        return a.urgency < b.urgency;
    }
};

/* ==========================
   DIJKSTRA
   ========================== */
vector<int> dijkstra(int src, vector<vector<pair<int,int>>>& graph) {
    vector<int> dist(graph.size(), INT_MAX);
    priority_queue<pair<int,int>, vector<pair<int,int>>, greater<>> pq;
    dist[src] = 0;
    pq.push({0, src});

    while (!pq.empty()) {
        auto [d,u] = pq.top(); pq.pop();
        if (d > dist[u]) continue;
        for (auto [v,w] : graph[u]) {
            if (dist[v] > dist[u] + w) {
                dist[v] = dist[u] + w;
                pq.push({dist[v], v});
            }
        }
    }
    return dist;
}

int main() {

    /* --------------------------
       READ ROAD NETWORK
       -------------------------- */
    auto roads = readCSV("roads.csv");
    int maxNode = 0;
    for (int i=1;i<roads.size();i++)
        maxNode = max(maxNode, max(stoi(roads[i][0]), stoi(roads[i][1])));

    vector<vector<pair<int,int>>> graph(maxNode+1);
    for (int i=1;i<roads.size();i++) {
        int u=stoi(roads[i][0]);
        int v=stoi(roads[i][1]);
        int w=stoi(roads[i][2]);
        graph[u].push_back({v,w});
        graph[v].push_back({u,w});
    }

    /* --------------------------
       READ ORDERS
       -------------------------- */
    auto ordersCSV = readCSV("orders.csv");
    queue<Order> pending;
    priority_queue<Order, vector<Order>, CompareUrgency> urgentPQ;
    unordered_map<string,string> customer;

    for (int i=1;i<ordersCSV.size();i++) {
        Order o;
        o.id = ordersCSV[i][0];
        o.destination = stoi(ordersCSV[i][1]);
        o.urgency = stoi(ordersCSV[i][2]);
        string cust = ordersCSV[i][3];

        pending.push(o);
        urgentPQ.push(o);
        customer[o.id] = cust;
    }

    /* --------------------------
       SHORTEST ROUTES
       -------------------------- */
    auto dist = dijkstra(0, graph);
    cout << "Shortest distance from depot:\n";
    for (int i=0;i<dist.size();i++)
        cout << "Location " << i << " : " << dist[i] << endl;

    /* --------------------------
       PROCESS DELIVERIES
       -------------------------- */
    cout << "\nProcessing Deliveries (Urgency Order):\n";
    while (!urgentPQ.empty()) {
        Order o = urgentPQ.top(); urgentPQ.pop();
        cout << "Delivering Order " << o.id
             << " to node " << o.destination
             << " | Customer: " << customer[o.id]
             << " | Urgency: " << o.urgency << endl;
    }

    return 0;
}
