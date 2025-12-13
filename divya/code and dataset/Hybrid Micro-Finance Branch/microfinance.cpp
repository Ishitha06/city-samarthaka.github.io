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
   FENWICK TREE (BIT)
   ========================== */
class Fenwick {
    vector<long long> bit;
public:
    Fenwick(int n) : bit(n+1, 0) {}
    void update(int i, long long val) {
        for (; i < bit.size(); i += i & -i)
            bit[i] += val;
    }
    long long query(int i) {
        long long s = 0;
        for (; i > 0; i -= i & -i)
            s += bit[i];
        return s;
    }
};

int main() {

    auto data = readCSV("transactions.csv");

    int n = data.size() - 1; // excluding header
    Fenwick ft(n);

    cout << "Processing Transactions:\n";
    for (int i = 1; i <= n; i++) {
        long long amount = stoll(data[i][2]);
        ft.update(i, amount);
        cout << "Account " << data[i][0]
             << " Transaction: " << amount << endl;
    }

    cout << "\nPrefix Sum Queries:\n";
    cout << "Total of first 10 transactions: "
         << ft.query(10) << endl;
    cout << "Total of first 50 transactions: "
         << ft.query(50) << endl;

    return 0;
}
