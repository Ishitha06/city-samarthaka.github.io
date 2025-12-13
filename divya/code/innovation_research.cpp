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
   EXPERIMENT STRUCT
   ========================== */
struct Experiment {
    string expID;
    int priority;
    int progress; // percentage
};

/* ==========================
   PRIORITY QUEUE (CRITICAL FIRST)
   ========================== */
struct PriorityCompare {
    bool operator()(const Experiment& a, const Experiment& b) {
        return a.priority < b.priority;
    }
};

/* ==========================
   TREE NODE (PROJECT HIERARCHY)
   ========================== */
struct TreeNode {
    string projectID;
    vector<TreeNode*> children;
    TreeNode(string id) : projectID(id) {}
};

/* ==========================
   DFS TRAVERSAL
   ========================== */
void dfs(TreeNode* node) {
    if (!node) return;
    cout << node->projectID << endl;
    for (auto child : node->children)
        dfs(child);
}

int main() {

    /* --------------------------
       READ PROJECT HIERARCHY
       -------------------------- */
    auto projectsCSV = readCSV("projects.csv");
    unordered_map<string, TreeNode*> projectMap;

    for (int i = 1; i < projectsCSV.size(); i++) {
        string parent = projectsCSV[i][0];
        string child = projectsCSV[i][1];

        if (!projectMap[parent])
            projectMap[parent] = new TreeNode(parent);
        if (!projectMap[child])
            projectMap[child] = new TreeNode(child);

        projectMap[parent]->children.push_back(projectMap[child]);
    }

    /* --------------------------
       READ EXPERIMENTS
       -------------------------- */
    auto experimentsCSV = readCSV("experiments.csv");
    vector<Experiment> experiments;
    priority_queue<Experiment, vector<Experiment>, PriorityCompare> pq;

    for (int i = 1; i < experimentsCSV.size(); i++) {
        Experiment e;
        e.expID = experimentsCSV[i][0];
        e.priority = stoi(experimentsCSV[i][1]);
        e.progress = stoi(experimentsCSV[i][2]);
        experiments.push_back(e);
        pq.push(e);
    }

    /* --------------------------
       SORT EXPERIMENTS BY PRIORITY
       -------------------------- */
    sort(experiments.begin(), experiments.end(),
         [](const Experiment& a, const Experiment& b) {
             return a.priority > b.priority;
         });

    cout << "Experiments Sorted by Priority:\n";
    for (auto &e : experiments)
        cout << e.expID << " Priority: " << e.priority << endl;

    /* --------------------------
       DFS PROJECT TRAVERSAL
       -------------------------- */
    cout << "\nProject Hierarchy (DFS):\n";
    dfs(projectMap.begin()->second);

    /* --------------------------
       PROCESS CRITICAL EXPERIMENTS
       -------------------------- */
    cout << "\nCritical Experiment Execution Order:\n";
    while (!pq.empty()) {
        Experiment e = pq.top(); pq.pop();
        cout << e.expID << " (Priority " << e.priority << ")\n";
    }

    return 0;
}
