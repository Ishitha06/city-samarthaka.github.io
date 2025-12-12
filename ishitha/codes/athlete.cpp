#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <cstdlib>
using namespace std;

const int MAXR = 10050;
const int HSIZE = 20011;

/* CSV arrays */
int AthleteID[MAXR];
int Speed[MAXR];
int Endurance[MAXR];
int Strength[MAXR];
int Score[MAXR];
char Pattern[MAXR][30];

int ROWS = 0;

/* ================= CSV LOADING ================= */
void loadCSV(const char* file) {
    ifstream fin(file);
    if (!fin) { cout<<"CSV not found\n"; exit(1); }

    string line;
    getline(fin, line);

    while (getline(fin, line) && ROWS < MAXR-1) {
        string tok;
        stringstream ss(line);

        getline(ss, tok, ','); AthleteID[ROWS] = atoi(tok.c_str());
        getline(ss, tok, ','); Speed[ROWS] = atoi(tok.c_str());
        getline(ss, tok, ','); Endurance[ROWS] = atoi(tok.c_str());
        getline(ss, tok, ','); Strength[ROWS] = atoi(tok.c_str());
        getline(ss, tok, ','); Score[ROWS] = atoi(tok.c_str());
        getline(ss, tok, ','); strcpy(Pattern[ROWS], tok.c_str());

        ROWS++;
    }
    fin.close();
}

/* ================= HASH TABLE ================= */
struct Slot {
    int id, idx;
    bool used;
    Slot(){ id=0; idx=0; used=false; }
} HT[HSIZE];

int hashF(int x) { return (x * 2654435761u) % HSIZE; }

void addHash(int id, int idx) {
    int h = hashF(id);
    for(int i=0;i<HSIZE;i++){
        int p = (h + i) % HSIZE;
        if(!HT[p].used){
            HT[p].used = true;
            HT[p].id = id;
            HT[p].idx = idx;
            return;
        }
    }
}

int findHash(int id){
    int h=hashF(id);
    for(int i=0;i<HSIZE;i++){
        int p=(h+i)%HSIZE;
        if(!HT[p].used) return -1;
        if(HT[p].used && HT[p].id==id) return HT[p].idx;
    }
    return -1;
}

/* ================= FENWICK TREE ================= */
struct Fenwick {
    int n;
    int bit[MAXR];

    void init(int N){
        n=N;
        for(int i=0;i<=n;i++) bit[i]=0;
    }

    void add(int i, int v){
        for(; i<=n; i+=i&-i) bit[i]+=v;
    }

    int sum(int i){
        int s=0;
        for(; i>0; i-=i&-i) s+=bit[i];
        return s;
    }
} fenw;

/* ================= KMP PATTERN MATCHING ================= */
void buildLPS(const char* pat, int m, int* lps){
    int len=0;
    lps[0]=0;
    int i=1;

    while(i<m){
        if(pat[i]==pat[len]){
            len++; lps[i]=len; i++;
        } else {
            if(len!=0) len=lps[len-1];
            else { lps[i]=0; i++; }
        }
    }
}

bool KMP(const char* text, const char* pat){
    int n=strlen(text);
    int m=strlen(pat);
    int lps[40];

    buildLPS(pat, m, lps);

    int i=0,j=0;
    while(i<n){
        if(text[i]==pat[j]){
            i++; j++;
            if(j==m) return true;
        }
        else{
            if(j!=0) j=lps[j-1];
            else i++;
        }
    }
    return false;
}

/* ================= MERGE SORT (Ranking) ================= */
void merge(int* arr,int* idx,int l,int mid,int r){
    int n1=mid-l+1;
    int n2=r-mid;

    int L[10050], R[10050];
    int Li[10050], Ri[10050];

    for(int i=0;i<n1;i++){ L[i]=arr[l+i]; Li[i]=idx[l+i]; }
    for(int i=0;i<n2;i++){ R[i]=arr[mid+1+i]; Ri[i]=idx[mid+1+i]; }

    int i=0,j=0,k=l;

    while(i<n1 && j<n2){
        if(L[i] > R[j]){
            arr[k]=L[i]; idx[k]=Li[i]; i++;
        } else {
            arr[k]=R[j]; idx[k]=Ri[j]; j++;
        }
        k++;
    }

    while(i<n1){ arr[k]=L[i]; idx[k]=Li[i]; i++; k++; }
    while(j<n2){ arr[k]=R[j]; idx[k]=Ri[j]; j++; k++; }
}

void mergeSort(int* arr,int* idx,int l,int r){
    if(l>=r) return;
    int mid=(l+r)/2;
    mergeSort(arr,idx,l,mid);
    mergeSort(arr,idx,mid+1,r);
    merge(arr,idx,l,mid,r);
}

/* ================= MAIN ================= */
int main(){

    loadCSV("athlete_performance.csv");
    cout<<"Loaded rows: "<<ROWS<<"\n";

    /* HASH TABLE */
    cout<<"\n=== REAL-TIME DATA LOOKUP (Hash) ===\n";
    for(int i=0;i<2000;i++) addHash(AthleteID[i], i);
    int idx = findHash(500);
    cout<<"Athlete 500 found at index: "<<idx<<"\n";


    /* FENWICK TREE */
    cout<<"\n=== PERFORMANCE TRACKING (Fenwick Tree) ===\n";
    fenw.init(ROWS);
    for(int i=1;i<=ROWS;i++) fenw.add(i, Score[i-1]);
    cout<<"Total performance score of first 500 athletes = "<<fenw.sum(500)<<"\n";


    /* KMP PATTERN MATCHING */
    cout<<"\n=== PATTERN RECOGNITION (KMP) ===\n";
    const char* findPat = "ABC";
    cout<<"Searching for pattern \"ABC\" in first 20 athletes...\n";
    for(int i=0;i<20;i++){
        bool ok = KMP(Pattern[i], findPat);
        cout<<"Athlete "<<AthleteID[i]<<" → "
            <<(ok?"MATCH":"NO MATCH")<<"\n";
    }


    /* MERGE SORT FOR RANKING */
    cout<<"\n=== ATHLETE RANKING (Merge Sort) ===\n";
    int scoreArr[MAXR], idxArr[MAXR];

    for(int i=0;i<ROWS;i++){
        scoreArr[i] = Score[i];
        idxArr[i] = i;
    }

    mergeSort(scoreArr, idxArr, 0, ROWS-1);

    cout<<"Top 10 ranked athletes:\n";
    for(int i=0;i<10;i++){
        int k = idxArr[i];
        cout<<i+1<<". Athlete "<<AthleteID[k]
            <<" Score="<<Score[k]<<"\n";
    }

    cout<<"\n=== END OF REPORT ===\n";
    return 0;
}
