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
   BATCH STRUCT
   ========================== */
struct Batch {
    string batchID;
    string supplier;
    int quantity;
    int deadline; // lower = more urgent
};

/* ==========================
   PRIORITY QUEUE (URGENT ORDERS)
   ========================== */
struct DeadlineCompare {
    bool operator()(const Batch& a, const Batch& b) {
        return a.deadline > b.deadline; // min deadline first
    }
};

int main() {

    /* --------------------------
       READ BATCH DATA
       -------------------------- */
    auto data = readCSV("cocoa_batches.csv");

    vector<Batch> batches;                 // Vector
    queue<Batch> productionQueue;          // FIFO production
    priority_queue<Batch, vector<Batch>, DeadlineCompare> urgentPQ; // Heap
    unordered_map<string,int> supplierStock; // Hash map

    // Skip header
    for (int i = 1; i < data.size(); i++) {
        Batch b;
        b.batchID = data[i][0];
        b.supplier = data[i][1];
        b.quantity = stoi(data[i][2]);
        b.deadline = stoi(data[i][3]);

        batches.push_back(b);
        productionQueue.push(b);
        urgentPQ.push(b);
        supplierStock[b.supplier] += b.quantity;
    }

    /* --------------------------
       SORT BATCHES BY DEADLINE
       -------------------------- */
    sort(batches.begin(), batches.end(),
         [](const Batch& a, const Batch& b) {
             return a.deadline < b.deadline;
         });

    cout << "Batches Sorted by Delivery Deadline:\n";
    for (auto &b : batches)
        cout << b.batchID << " Deadline: " << b.deadline << endl;

    /* --------------------------
       PROCESS PRODUCTION LINE
       -------------------------- */
    cout << "\nProduction Line Processing (FIFO):\n";
    while (!productionQueue.empty()) {
        Batch b = productionQueue.front();
        productionQueue.pop();
        cout << "Processing batch " << b.batchID << endl;
    }

    /* --------------------------
       URGENT DISPATCH
       -------------------------- */
    cout << "\nUrgent Dispatch Order:\n";
    while (!urgentPQ.empty()) {
        Batch b = urgentPQ.top();
        urgentPQ.pop();
        cout << "Dispatch batch " << b.batchID
             << " (Deadline " << b.deadline << ")\n";
    }

    /* --------------------------
       SUPPLIER STOCK LOOKUP
       -------------------------- */
    cout << "\nSupplier Stock Levels:\n";
    for (auto &p : supplierStock)
        cout << p.first << " -> " << p.second << " units\n";

    return 0;
}
