#include <bits/stdc++.h>
using namespace std;

vector<vector<string>> readCSV(const string& filename) {
    vector<vector<string>> data;
    ifstream file(filename);
    string line, cell;
    if (!file.is_open()) {
        cout << "Error opening file\n";
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

struct Component {
    string id, type, errorLog;
    int performance;
};

struct TreeNode {
    Component c;
    TreeNode *left, *right;
    TreeNode(Component x): c(x), left(nullptr), right(nullptr) {}
};

TreeNode* insertBST(TreeNode* root, Component c) {
    if (!root) return new TreeNode(c);
    if (c.performance < root->c.performance)
        root->left = insertBST(root->left, c);
    else
        root->right = insertBST(root->right, c);
    return root;
}

void bfs(TreeNode* root) {
    if (!root) return;
    queue<TreeNode*> q;
    q.push(root);
    cout << "\nBFS Testing Order:\n";
    while (!q.empty()) {
        auto t = q.front(); q.pop();
        cout << t->c.id << " " << t->c.type
             << " Performance=" << t->c.performance << endl;
        if (t->left) q.push(t->left);
        if (t->right) q.push(t->right);
    }
}

void merge(vector<int>& a, int l, int m, int r) {
    vector<int> L(a.begin()+l, a.begin()+m+1);
    vector<int> R(a.begin()+m+1, a.begin()+r+1);
    int i=0,j=0,k=l;
    while(i<L.size() && j<R.size())
        a[k++] = (L[i]<=R[j]) ? L[i++] : R[j++];
    while(i<L.size()) a[k++] = L[i++];
    while(j<R.size()) a[k++] = R[j++];
}

void mergeSort(vector<int>& a, int l, int r) {
    if (l>=r) return;
    int m=(l+r)/2;
    mergeSort(a,l,m);
    mergeSort(a,m+1,r);
    merge(a,l,m,r);
}

int main() {
    auto data = readCSV("hardware_components.csv");
    queue<Component> assembly;
    TreeNode* root=nullptr;
    vector<int> scores;

    for (int i=1;i<data.size();i++) {
        Component c;
        c.id=data[i][0];
        c.type=data[i][1];
        c.performance=stoi(data[i][2]);
        c.errorLog=data[i][3];
        assembly.push(c);
    }

    while(!assembly.empty()) {
        Component c=assembly.front(); assembly.pop();
        cout << "Assembling " << c.id << endl;
        root = insertBST(root,c);
        scores.push_back(c.performance);
        if (c.errorLog.find("error")!=string::npos)
            cout << "Error detected in " << c.id << endl;
    }

    bfs(root);
    mergeSort(scores,0,scores.size()-1);

    cout << "\nSorted Performance Scores:\n";
    for(int x:scores) cout<<x<<" ";
    cout<<endl;
    return 0;
}
