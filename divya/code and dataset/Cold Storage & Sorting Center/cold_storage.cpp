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
   ITEM STRUCT
   ========================== */
struct Item {
    string itemID;
    string supplier;
    string type;
    int expiry;       // days to expiry (smaller = urgent)
    int quantity;
};

/* ==========================
   MIN HEAP COMPARATOR
   ========================== */
struct ExpiryCompare {
    bool operator()(const Item& a, const Item& b) {
        return a.expiry > b.expiry; // min-heap by expiry
    }
};

/* ==========================
   AVL TREE NODE (BY EXPIRY)
   ========================== */
struct Node {
    Item item;
    Node *left, *right;
    int height;
    Node(Item i): item(i), left(nullptr), right(nullptr), height(1) {}
};

int height(Node* n){ return n ? n->height : 0; }

Node* rightRotate(Node* y) {
    Node* x = y->left;
    Node* T2 = x->right;
    x->right = y;
    y->left = T2;
    y->height = max(height(y->left), height(y->right)) + 1;
    x->height = max(height(x->left), height(x->right)) + 1;
    return x;
}

Node* leftRotate(Node* x) {
    Node* y = x->right;
    Node* T2 = y->left;
    y->left = x;
    x->right = T2;
    x->height = max(height(x->left), height(x->right)) + 1;
    y->height = max(height(y->left), height(y->right)) + 1;
    return y;
}

int getBalance(Node* n){
    return n ? height(n->left) - height(n->right) : 0;
}

Node* insertAVL(Node* node, Item item) {
    if (!node) return new Node(item);

    if (item.expiry < node->item.expiry)
        node->left = insertAVL(node->left, item);
    else
        node->right = insertAVL(node->right, item);

    node->height = 1 + max(height(node->left), height(node->right));

    int balance = getBalance(node);

    // Rotations
    if (balance > 1 && item.expiry < node->left->item.expiry)
        return rightRotate(node);
    if (balance < -1 && item.expiry > node->right->item.expiry)
        return leftRotate(node);
    if (balance > 1 && item.expiry > node->left->item.expiry) {
        node->left = leftRotate(node->left);
        return rightRotate(node);
    }
    if (balance < -1 && item.expiry < node->right->item.expiry) {
        node->right = rightRotate(node->right);
        return leftRotate(node);
    }
    return node;
}

/* ==========================
   INORDER TRAVERSAL
   ========================== */
void inorder(Node* root) {
    if (!root) return;
    inorder(root->left);
    cout << root->item.itemID
         << " Expiry: " << root->item.expiry << endl;
    inorder(root->right);
}

int main() {

    auto data = readCSV("cold_storage_items.csv");

    priority_queue<Item, vector<Item>, ExpiryCompare> minHeap;
    unordered_map<string, vector<Item>> supplierMap;
    vector<Item> items;
    Node* avlRoot = nullptr;

    // Skip header
    for (int i = 1; i < data.size(); i++) {
        Item it;
        it.itemID = data[i][0];
        it.supplier = data[i][1];
        it.type = data[i][2];
        it.expiry = stoi(data[i][3]);
        it.quantity = stoi(data[i][4]);

        minHeap.push(it);
        supplierMap[it.supplier].push_back(it);
        items.push_back(it);
        avlRoot = insertAVL(avlRoot, it);
    }

    cout << "Dispatch Priority (Near Expiry First):\n";
    while (!minHeap.empty()) {
        Item it = minHeap.top(); minHeap.pop();
        cout << it.itemID << " | Expiry in "
             << it.expiry << " days\n";
    }

    cout << "\nItems Sorted by Expiry (AVL Tree):\n";
    inorder(avlRoot);

    cout << "\nSupplier Inventory Lookup:\n";
    for (auto &p : supplierMap) {
        cout << "Supplier " << p.first
             << " has " << p.second.size() << " items\n";
    }

    return 0;
}
