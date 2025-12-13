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
   TRIE
   ========================== */
struct TrieNode {
    bool end=false;
    unordered_map<char, TrieNode*> next;
};

class Trie {
    TrieNode* root;
public:
    Trie() { root = new TrieNode(); }
    void insert(const string& s) {
        TrieNode* cur=root;
        for(char c:s) {
            if(!cur->next[c]) cur->next[c]=new TrieNode();
            cur=cur->next[c];
        }
        cur->end=true;
    }
    bool search(const string& s) {
        TrieNode* cur=root;
        for(char c:s) {
            if(!cur->next[c]) return false;
            cur=cur->next[c];
        }
        return cur->end;
    }
};

/* ==========================
   GRAPH + DFS
   ========================== */
class Graph {
    unordered_map<string, vector<string>> adj;
public:
    void addEdge(const string& u,const string& v){
        adj[u].push_back(v);
    }
    void dfs(const string& u, unordered_set<string>& vis){
        vis.insert(u);
        cout<<u<<endl;
        for(auto &v:adj[u])
            if(!vis.count(v))
                dfs(v,vis);
    }
    void startDFS(const string& src){
        unordered_set<string> vis;
        cout << "\nDFS from mentor " << src << ":\n";
        dfs(src,vis);
    }
};

int main(){
    Trie trie;
    unordered_map<string,string> profiles;
    Graph g;

    auto startups = readCSV("startups.csv");
    for(int i=1;i<startups.size();i++){
        trie.insert(startups[i][0]);
        profiles[startups[i][0]] = startups[i][1];
    }

    auto relations = readCSV("mentor_startup.csv");
    for(int i=1;i<relations.size();i++)
        g.addEdge(relations[i][0], relations[i][1]);

    string query = startups[1][0];
    cout << "Search Startup " << query << ": "
         << (trie.search(query) ? "FOUND" : "NOT FOUND") << endl;

    g.startDFS(relations[1][0]);

    cout << "\nProfile Lookup:\n" << profiles[query] << endl;
    return 0;
}
