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
   TRIE FOR ATTRACTION TAGS
   ========================== */
struct TrieNode {
    bool end = false;
    unordered_map<char, TrieNode*> next;
};

class Trie {
    TrieNode* root;
public:
    Trie() { root = new TrieNode(); }

    void insert(const string& s) {
        TrieNode* cur = root;
        for (char c : s) {
            if (!cur->next[c])
                cur->next[c] = new TrieNode();
            cur = cur->next[c];
        }
        cur->end = true;
    }

    bool search(const string& s) {
        TrieNode* cur = root;
        for (char c : s) {
            if (!cur->next[c]) return false;
            cur = cur->next[c];
        }
        return cur->end;
    }
};

/* ==========================
   ATTRACTION MODEL
   ========================== */
struct Attraction {
    int id;
    string name;
    string tag;
    int rating;
    int popularity;
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
        auto [d, u] = pq.top(); pq.pop();
        if (d > dist[u]) continue;

        for (auto [v, w] : graph[u]) {
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

    cout << "=== Smart Tourism Recommendation System ===\n";

    Trie tagIndex;
    unordered_map<int, Attraction> attractions;

    /* --------------------------
       READ ATTRACTIONS CSV
       -------------------------- */
    auto attractionsCSV = readCSV("attractions.csv");

    for (int i = 1; i < attractionsCSV.size(); i++) {
        Attraction a;
        a.id = stoi(attractionsCSV[i][0]);
        a.name = attractionsCSV[i][1];
        a.tag = attractionsCSV[i][2];
        a.rating = stoi(attractionsCSV[i][3]);
        a.popularity = stoi(attractionsCSV[i][4]);

        attractions[a.id] = a;
        tagIndex.insert(a.tag);
    }

    /* --------------------------
       READ TOURISM ROAD NETWORK
       -------------------------- */
    auto roadsCSV = readCSV("tourism_roads.csv");
    int maxNode = 0;

    for (int i = 1; i < roadsCSV.size(); i++) {
        maxNode = max(maxNode,
            max(stoi(roadsCSV[i][0]), stoi(roadsCSV[i][1])));
    }

    vector<vector<pair<int,int>>> graph(maxNode + 1);

    for (int i = 1; i < roadsCSV.size(); i++) {
        int u = stoi(roadsCSV[i][0]);
        int v = stoi(roadsCSV[i][1]);
        int w = stoi(roadsCSV[i][2]);
        graph[u].push_back({v, w});
        graph[v].push_back({u, w});
    }

    /* --------------------------
       COMPUTE SHORTEST PATHS
       -------------------------- */
    vector<int> dist = dijkstra(0, graph);

    /* --------------------------
       USER INTERACTION (NO CSV)
       -------------------------- */
    string preferredTag;
    int maxDistance;

    cout << "\nEnter preferred attraction type (tag): ";
    cin >> preferredTag;

    cout << "Enter maximum travel distance: ";
    cin >> maxDistance;

    if (!tagIndex.search(preferredTag)) {
        cout << "No attractions found for this preference.\n";
        return 0;
    }

    /* --------------------------
       RANK ATTRACTIONS
       -------------------------- */
    priority_queue<pair<int,int>> recPQ; // score, attraction id

    for (auto& p : attractions) {
        auto& a = p.second;

        if (a.tag != preferredTag) continue;
        if (!bfsReachable(0, a.id, graph)) continue;
        if (dist[a.id] > maxDistance) continue;

        int score = a.rating * 10 + a.popularity - dist[a.id];
        recPQ.push({score, a.id});
    }

    /* --------------------------
       OUTPUT RECOMMENDATIONS
       -------------------------- */
    cout << "\nTop Recommended Attractions:\n";

    int k = 5;
    while (k-- && !recPQ.empty()) {
        auto [score, id] = recPQ.top(); recPQ.pop();
        cout << attractions[id].name
             << " | Tag: " << attractions[id].tag
             << " | Rating: " << attractions[id].rating
             << " | Distance: " << dist[id]
             << " | Score: " << score << endl;
    }

    cout << "\nSystem execution completed.\n";
    return 0;
}
