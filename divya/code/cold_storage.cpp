
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
    int expiry;        // days to expiry
    int quantity;
    string status;     // STORED, SORTED, DISPATCHED
};

/* ==========================
   MIN HEAP (NEAR EXPIRY)
   ========================== */
struct ExpiryCompare {
    bool operator()(const Item& a, const Item& b) {
        return a.expiry > b.expiry;
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

int height(Node* n) { return n ? n->height : 0; }

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

int getBalance(Node* n) {
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
   INORDER AVL TRAVERSAL
   ========================== */
void inorder(Node* root) {
    if (!root) return;
    inorder(root->left);
    cout << root->item.itemID
         << " | Expiry: " << root->item.expiry
         << " | Qty: " << root->item.quantity << endl;
    inorder(root->right);
}

/* ==========================
   BINARY SEARCH (BY ITEM ID)
   ========================== */
int binarySearchItem(const vector<Item>& items, const string& id) {
    int l = 0, r = items.size() - 1;
    while (l <= r) {
        int mid = (l + r) / 2;
        if (items[mid].itemID == id) return mid;
        if (items[mid].itemID < id) l = mid + 1;
        else r = mid - 1;
    }
    return -1;
}

/* ==========================
   MAIN
   ========================== */
int main() {

    cout << "=== Cold Storage & Sorting Center ===\n";

    auto data = readCSV("cold_storage_items.csv");

    priority_queue<Item, vector<Item>, ExpiryCompare> minHeap;
    unordered_map<string, vector<Item>> supplierMap;
    unordered_map<string, int> typeCount;
    vector<Item> items;
    Node* avlRoot = nullptr;

    /* --------------------------
       LOAD ITEMS
       -------------------------- */
    for (int i = 1; i < data.size(); i++) {
        Item it;
        it.itemID = data[i][0];
        it.supplier = data[i][1];
        it.type = data[i][2];
        it.expiry = stoi(data[i][3]);
        it.quantity = stoi(data[i][4]);
        it.status = "STORED";

        minHeap.push(it);
        supplierMap[it.supplier].push_back(it);
        typeCount[it.type]++;
        items.push_back(it);
        avlRoot = insertAVL(avlRoot, it);
    }

    /* --------------------------
       SORT ITEMS BY ID (SEARCH)
       -------------------------- */
    sort(items.begin(), items.end(),
         [](const Item& a, const Item& b) {
             return a.itemID < b.itemID;
         });

    /* --------------------------
       DISPATCH PRIORITY
       -------------------------- */
    cout << "\nDispatch Priority (Greedy by Expiry):\n";
    while (!minHeap.empty()) {
        Item it = minHeap.top(); minHeap.pop();
        it.status = "DISPATCHED";
        cout << it.itemID
             << " | Expiry in " << it.expiry
             << " days | Qty: " << it.quantity << endl;
    }

    /* --------------------------
       AVL SORTED VIEW
       -------------------------- */
    cout << "\nItems Sorted by Expiry (AVL Tree):\n";
    inorder(avlRoot);

    /* --------------------------
       SUPPLIER INVENTORY
       -------------------------- */
    cout << "\nSupplier Inventory Summary:\n";
    for (auto &p : supplierMap) {
        cout << p.first << " supplies "
             << p.second.size() << " items\n";
    }

    /* --------------------------
       CATEGORY STATISTICS
       -------------------------- */
    cout << "\nItem Type Distribution:\n";
    for (auto &p : typeCount) {
        cout << p.first << " : " << p.second << " items\n";
    }

    /* --------------------------
       ITEM SEARCH (DEMO)
       -------------------------- */
    string searchID;
    cout << "\nEnter item ID to search: ";
    cin >> searchID;

    int idx = binarySearchItem(items, searchID);
    if (idx != -1) {
        Item &it = items[idx];
        cout << "Item Found: "
             << it.itemID
             << " | Supplier: " << it.supplier
             << " | Type: " << it.type
             << " | Expiry: " << it.expiry
             << " | Qty: " << it.quantity << endl;
    } else {
        cout << "Item not found.\n";
    }

    cout << "\nSystem execution completed.\n";
    return 0;
}
