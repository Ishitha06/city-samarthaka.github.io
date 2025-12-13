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
   BATCH STRUCT
   ========================== */
struct Batch {
    string batchID;
    string supplier;
    int quantity;
    int deadline;        // lower = more urgent
    string status;       // RECEIVED, PROCESSED, PACKED, DISPATCHED
    int qualityScore;    // derived score
};

/* ==========================
   PRIORITY QUEUE (URGENT)
   ========================== */
struct DeadlineCompare {
    bool operator()(const Batch& a, const Batch& b) {
        return a.deadline > b.deadline;
    }
};

/* ==========================
   QUALITY ASSESSMENT
   ========================== */
int assessQuality(int quantity, int deadline) {
    int score = quantity / 10 - deadline;
    return max(1, min(score, 100));
}

/* ==========================
   BINARY SEARCH BY BATCH ID
   ========================== */
int findBatch(const vector<Batch>& batches, const string& id) {
    int l = 0, r = batches.size() - 1;
    while (l <= r) {
        int mid = (l + r) / 2;
        if (batches[mid].batchID == id) return mid;
        if (batches[mid].batchID < id) l = mid + 1;
        else r = mid - 1;
    }
    return -1;
}

/* ==========================
   MAIN
   ========================== */
int main() {

    cout << "=== Cocoa–Chocolate Factory Management System ===\n";

    /* --------------------------
       READ CSV DATA
       -------------------------- */
    auto data = readCSV("cocoa_batches.csv");

    vector<Batch> batches;                                 // Storage
    queue<Batch*> productionQueue;                         // FIFO
    priority_queue<Batch*, vector<Batch*>, DeadlineCompare> urgentPQ;
    unordered_map<string, int> supplierStock;              // Supplier → quantity

    /* --------------------------
       LOAD BATCHES
       -------------------------- */
    for (int i = 1; i < data.size(); i++) {
        Batch b;
        b.batchID = data[i][0];
        b.supplier = data[i][1];
        b.quantity = stoi(data[i][2]);
        b.deadline = stoi(data[i][3]);
        b.status = "RECEIVED";
        b.qualityScore = assessQuality(b.quantity, b.deadline);

        batches.push_back(b);
        supplierStock[b.supplier] += b.quantity;
    }

    /* --------------------------
       SORT BY BATCH ID (SEARCH)
       -------------------------- */
    sort(batches.begin(), batches.end(),
         [](const Batch& a, const Batch& b) {
             return a.batchID < b.batchID;
         });

    /* --------------------------
       BUILD QUEUES
       -------------------------- */
    for (auto& b : batches) {
        productionQueue.push(&b);
        urgentPQ.push(&b);
    }

    /* --------------------------
       SORT BY DEADLINE (REPORT)
       -------------------------- */
    vector<Batch> deadlineSorted = batches;
    sort(deadlineSorted.begin(), deadlineSorted.end(),
         [](const Batch& a, const Batch& b) {
             return a.deadline < b.deadline;
         });

    cout << "\nBatches Sorted by Deadline:\n";
    for (auto& b : deadlineSorted) {
        cout << b.batchID
             << " | Deadline: " << b.deadline
             << " | Quality: " << b.qualityScore << endl;
    }

    /* --------------------------
       PRODUCTION LINE (FIFO)
       -------------------------- */
    cout << "\nProduction Line Processing:\n";
    while (!productionQueue.empty()) {
        Batch* b = productionQueue.front();
        productionQueue.pop();
        b->status = "PROCESSED";
        cout << "Processed batch " << b->batchID << endl;
    }

    /* --------------------------
       PACKAGING STAGE
       -------------------------- */
    cout << "\nPackaging High-Quality Batches:\n";
    for (auto& b : batches) {
        if (b.qualityScore >= 50) {
            b.status = "PACKED";
            cout << "Packed batch " << b.batchID << endl;
        }
    }

    /* --------------------------
       URGENT DISPATCH
       -------------------------- */
    cout << "\nUrgent Dispatch Order:\n";
    while (!urgentPQ.empty()) {
        Batch* b = urgentPQ.top();
        urgentPQ.pop();
        if (b->status == "PACKED") {
            b->status = "DISPATCHED";
            cout << "Dispatched batch "
                 << b->batchID
                 << " | Deadline: " << b->deadline << endl;
        }
    }

    /* --------------------------
       SUPPLIER STOCK REPORT
       -------------------------- */
    cout << "\nSupplier Stock Levels:\n";
    for (auto& p : supplierStock) {
        cout << p.first << " -> " << p.second << " units\n";
    }

    /* --------------------------
       SEARCH BATCH (DEMO)
       -------------------------- */
    string searchID;
    cout << "\nEnter batch ID to search: ";
    cin >> searchID;

    int idx = findBatch(batches, searchID);
    if (idx != -1) {
        auto& b = batches[idx];
        cout << "Batch Found: "
             << b.batchID
             << " | Supplier: " << b.supplier
             << " | Status: " << b.status
             << " | Quality: " << b.qualityScore << endl;
    } else {
        cout << "Batch not found.\n";
    }

    cout << "\nSystem execution completed.\n";
    return 0;
}
