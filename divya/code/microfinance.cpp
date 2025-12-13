#include <bits/stdc++.h>
using namespace std;

/* =====================================================
   CSV READER
===================================================== */
vector<vector<string>> readCSV(const string& filename) {
    vector<vector<string>> data;
    ifstream file(filename);
    string line, cell;

    if (!file.is_open()) {
        cerr << "Error opening file: " << filename << endl;
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

/* =====================================================
   FENWICK TREE (FAST AGGREGATION)
===================================================== */
class FenwickTree {
    vector<long long> bit;
    int n;

public:
    FenwickTree(int n) : n(n), bit(n + 1, 0) {}

    void update(int idx, long long val) {
        for (; idx <= n; idx += idx & -idx)
            bit[idx] += val;
    }

    long long query(int idx) const {
        long long sum = 0;
        for (; idx > 0; idx -= idx & -idx)
            sum += bit[idx];
        return sum;
    }

    long long rangeQuery(int l, int r) const {
        return query(r) - query(l - 1);
    }
};

/* =====================================================
   MICROFINANCE DOMAIN MODELS
===================================================== */
struct Account {
    string id;
    long long totalLoan = 0;
    long long totalRepaid = 0;

    long long outstanding() const {
        return totalLoan - totalRepaid;
    }

    string riskCategory() const {
        if (outstanding() > 7000) return "HIGH RISK";
        if (outstanding() > 3000) return "MEDIUM RISK";
        return "LOW RISK";
    }
};

/* =====================================================
   MICROFINANCE ENGINE
===================================================== */
class MicroFinanceSystem {
    unordered_map<string, Account> accounts;
    FenwickTree transactionBIT;
    int txnIndex = 0;

public:
    MicroFinanceSystem(int maxTxns) : transactionBIT(maxTxns) {}

    void processTransaction(
        const string& accId,
        const string& type,
        long long amount
    ) {
        txnIndex++;
        transactionBIT.update(txnIndex, amount);

        Account &acc = accounts[accId];
        acc.id = accId;

        if (type == "loan") {
            acc.totalLoan += amount;
        }
        else if (type == "repayment") {
            acc.totalRepaid += amount;
        }
    }

    void printAccountSummary() const {
        cout << "\n===== ACCOUNT SUMMARY =====\n";
        for (const auto& [id, acc] : accounts) {
            cout << "Account: " << id
                 << " | Loan: " << acc.totalLoan
                 << " | Repaid: " << acc.totalRepaid
                 << " | Outstanding: " << acc.outstanding()
                 << " | Risk: " << acc.riskCategory()
                 << "\n";
        }
    }

    void analytics() const {
        cout << "\n===== TRANSACTION ANALYTICS =====\n";
        cout << "Total of first 10 transactions: "
             << transactionBIT.query(10) << "\n";

        cout << "Total of first 50 transactions: "
             << transactionBIT.query(50) << "\n";

        cout << "Transactions [10–30]: "
             << transactionBIT.rangeQuery(10, 30) << "\n";
    }
};

/* =====================================================
   MAIN
===================================================== */
int main() {

    vector<vector<string>> data = readCSV("transactions.csv");

    int maxTransactions = data.size();
    MicroFinanceSystem system(maxTransactions);

    cout << "Processing Microfinance Transactions...\n";

    for (int i = 1; i < data.size(); i++) {
        string accId = data[i][0];
        string type  = data[i][1];
        long long amount = stoll(data[i][2]);

        system.processTransaction(accId, type, amount);

        cout << "Account: " << accId
             << " | Type: " << type
             << " | Amount: " << amount << "\n";
    }

    system.printAccountSummary();
    system.analytics();

    return 0;
}

