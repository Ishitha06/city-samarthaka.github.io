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
    return data;
}

/* ==========================
   DATA MODELS
   ========================== */
struct Order {
    string id;
    int destination;
    int urgency;
};

struct DeliveryAgent {
    string id;
    int location;
    bool available;
};

struct DeliveryLog {
    string orderId;
    string agentId;
    int distance;
};

/* ==========================
   COMPARATOR
   ========================== */
struct CompareUrgency {
    bool operator()(const Order& a, const Order& b) {
        return a.urgency < b.urgency;
    }
};

/* ==========================
   GRAPH ALGORITHMS
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

bool bfsReachable(int src, int dest, vector<vector<pair<int,int>>>& graph) {
    vector<bool> visited(graph.size(), false);
    queue<int> q;

    q.push(src);
    visited[src] = true;

    while (!q.empty()) {
        int u = q.front(); q.pop();
        if (u == dest) return true;

        for (auto [v,_] : graph[u]) {
            if (!visited[v]) {
                visited[v] = true;
                q.push(v);
            }
        }
    }
    return false;
}

/* ==========================
   MAIN
   ========================== */
int main() {

    cout << "=== CSV Driven Food & Grocery Delivery System ===\n";

    /* --------------------------
       READ ROAD NETWORK
       -------------------------- */
    auto roads = readCSV("roads.csv");
    int maxNode = 0;

    for (int i = 1; i < roads.size(); i++) {
        maxNode = max(maxNode,
            max(stoi(roads[i][0]), stoi(roads[i][1])));
    }

    vector<vector<pair<int,int>>> graph(maxNode + 1);
    for (int i = 1; i < roads.size(); i++) {
        int u = stoi(roads[i][0]);
        int v = stoi(roads[i][1]);
        int w = stoi(roads[i][2]);
        graph[u].push_back({v, w});
        graph[v].push_back({u, w});
    }

    /* --------------------------
       READ ORDERS (CSV)
       -------------------------- */
    auto ordersCSV = readCSV("orders.csv");
    priority_queue<Order, vector<Order>, CompareUrgency> urgentOrders;
    unordered_map<string,string> customerMap;

    for (int i = 1; i < ordersCSV.size(); i++) {
        Order o;
        o.id = ordersCSV[i][0];
        o.destination = stoi(ordersCSV[i][1]);
        o.urgency = stoi(ordersCSV[i][2]);
        customerMap[o.id] = ordersCSV[i][3];
        urgentOrders.push(o);
    }

    /* --------------------------
       READ DELIVERY AGENTS (CSV)
       -------------------------- */
    auto agentsCSV = readCSV("agents.csv");
    vector<DeliveryAgent> agents;

    for (int i = 1; i < agentsCSV.size(); i++) {
        DeliveryAgent a;
        a.id = agentsCSV[i][0];
        a.location = stoi(agentsCSV[i][1]);
        a.available = (agentsCSV[i][2] == "YES");
        agents.push_back(a);
    }

    /* --------------------------
       SHORTEST PATHS
       -------------------------- */
    vector<int> distFromDepot = dijkstra(0, graph);

    cout << "\nShortest distances from depot:\n";
    for (int i = 0; i < distFromDepot.size(); i++) {
        cout << "Node " << i << " -> " << distFromDepot[i] << endl;
    }

    /* --------------------------
       PROCESS DELIVERIES
       -------------------------- */
    vector<DeliveryLog> history;

    cout << "\nProcessing Deliveries (Urgency-Based):\n";

    while (!urgentOrders.empty()) {
        Order o = urgentOrders.top();
        urgentOrders.pop();

        if (!bfsReachable(0, o.destination, graph)) {
            cout << "Order " << o.id << " unreachable. Skipped.\n";
            continue;
        }

        DeliveryAgent* chosen = nullptr;
        for (auto& a : agents) {
            if (a.available) {
                chosen = &a;
                break;
            }
        }

        if (!chosen) {
            cout << "No available agent for order " << o.id << endl;
            continue;
        }

        int distance = distFromDepot[o.destination];

        cout << "Order " << o.id
             << " | Customer: " << customerMap[o.id]
             << " | Agent: " << chosen->id
             << " | Distance: " << distance
             << " | Urgency: " << o.urgency << endl;

        chosen->available = false;
        history.push_back({o.id, chosen->id, distance});
    }

    /* --------------------------
       DELIVERY SUMMARY
       -------------------------- */
    cout << "\nDelivery Summary:\n";
    for (auto& log : history) {
        cout << "Order " << log.orderId
             << " delivered by " << log.agentId
             << " | Distance: " << log.distance << endl;
    }

    cout << "\nSystem execution completed successfully.\n";
    return 0;
}
