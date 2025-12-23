#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <cmath>
#include <iomanip>
#include <cstdlib>

using namespace std;

/* ============================================================
   CONSTANTS
============================================================ */
const int MAX_BINS   = 11000;
const int MAX_ROUTES = 1200;
const int MAX_EDGES  = 200000;
const int INF        = 1000000000;
const double EARTH_R = 6371000.0;

/* ============================================================
   SAFE CONVERSIONS
============================================================ */
int safe_atoi(const string &s) {
    if (s.empty()) return 0;
    try { return atoi(s.c_str()); } catch(...) { return 0; }
}

double safe_atof(const string &s) {
    if (s.empty()) return 0.0;
    try { return atof(s.c_str()); } catch(...) { return 0.0; }
}

/* ============================================================
   CSV STORAGE
============================================================ */
int    BinID[MAX_BINS];
char   Zone[MAX_BINS][16];
double Lat[MAX_BINS], Lon[MAX_BINS];
char   Waste[MAX_BINS][24];
int    RouteID[MAX_BINS];
int    NearbyPop[MAX_BINS];
char   Sensor[MAX_BINS][8];

int BIN_COUNT = 0;

/* ============================================================
   ROUTE GRAPH
============================================================ */
double Rlat[MAX_ROUTES], Rlon[MAX_ROUTES];
int    Rcount[MAX_ROUTES];

struct Edge { int u, v, w; };
Edge edges[MAX_EDGES];
int edgeCount = 0, Nnodes = 0;

int distBF[MAX_ROUTES], parentBF[MAX_ROUTES];

/* ============================================================
   DEG → RAD
============================================================ */
double deg2rad(double d) { return d * 0.017453292519943295; }

double haversine(double a1, double b1, double a2, double b2) {
    double dlat = deg2rad(a2 - a1);
    double dlon = deg2rad(b2 - b1);
    double s = sin(dlat/2), t = sin(dlon/2);
    double aa = s*s + cos(deg2rad(a1))*cos(deg2rad(a2))*t*t;
    return EARTH_R * 2 * atan2(sqrt(aa), sqrt(1-aa));
}

/* ============================================================
   CSV LOADER (PREVIEW ONLY DATASET)
============================================================ */
bool loadCSV(const char* file) {
    ifstream fin(file);
    if (!fin) {
        cout << "ERROR: Cannot open CSV\n";
        return false;
    }

    string line, tok;
    getline(fin, line); // header

    BIN_COUNT = 0;
    while (getline(fin, line) && BIN_COUNT < MAX_BINS) {
        stringstream ss(line);

        getline(ss, tok, ','); BinID[BIN_COUNT] = safe_atoi(tok);
        getline(ss, tok, ','); strncpy(Zone[BIN_COUNT], tok.c_str(), 15);
        getline(ss, tok, ','); Lat[BIN_COUNT] = safe_atof(tok);
        getline(ss, tok, ','); Lon[BIN_COUNT] = safe_atof(tok);
        getline(ss, tok, ','); strncpy(Waste[BIN_COUNT], tok.c_str(), 23);
        getline(ss, tok, ','); RouteID[BIN_COUNT] = safe_atoi(tok);
        getline(ss, tok, ','); NearbyPop[BIN_COUNT] = safe_atoi(tok);
        getline(ss, tok, ','); strncpy(Sensor[BIN_COUNT], tok.c_str(), 7);

        BIN_COUNT++;
    }
    fin.close();
    return true;
}

/* ============================================================
   ROUTE GRAPH + BELLMAN FORD
============================================================ */
void buildRouteGraph(int K = 8) {
    int maxr = 0;
    for (int i = 0; i < BIN_COUNT; i++)
        if (RouteID[i] > maxr) maxr = RouteID[i];

    Nnodes = maxr + 1;
    if (Nnodes > MAX_ROUTES) Nnodes = MAX_ROUTES;

    for (int i = 0; i < Nnodes; i++) {
        Rlat[i] = Rlon[i] = 0;
        Rcount[i] = 0;
    }

    for (int i = 0; i < BIN_COUNT; i++) {
        int r = RouteID[i];
        if (r >= 0 && r < Nnodes) {
            Rlat[r] += Lat[i];
            Rlon[r] += Lon[i];
            Rcount[r]++;
        }
    }

    for (int i = 0; i < Nnodes; i++) {
        if (Rcount[i]) {
            Rlat[i] /= Rcount[i];
            Rlon[i] /= Rcount[i];
        }
    }

    edgeCount = 0;
    for (int u = 0; u < Nnodes; u++) {
        if (!Rcount[u]) continue;
        for (int v = 0; v < Nnodes; v++) {
            if (u == v || !Rcount[v]) continue;
            double d = haversine(Rlat[u], Rlon[u], Rlat[v], Rlon[v]);
            if (edgeCount < MAX_EDGES) {
                edges[edgeCount++] = {u, v, (int)d};
            }
        }
    }
}

void bellmanFord(int src) {
    for (int i = 0; i < Nnodes; i++) {
        distBF[i] = INF;
        parentBF[i] = -1;
    }
    distBF[src] = 0;

    for (int i = 0; i < Nnodes-1; i++) {
        for (int e = 0; e < edgeCount; e++) {
            if (distBF[edges[e].u] + edges[e].w < distBF[edges[e].v]) {
                distBF[edges[e].v] = distBF[edges[e].u] + edges[e].w;
                parentBF[edges[e].v] = edges[e].u;
            }
        }
    }
}

/* ============================================================
   WASTE NORMALIZATION
============================================================ */
const char* normalize(const char* s) {
    static char b[32];
    int j=0;
    for(int i=0;s[i];i++){
        char c=tolower(s[i]);
        if(c>='a'&&c<='z') b[j++]=c;
    }
    b[j]=0;
    if(strstr(b,"organic")) return "Organic";
    if(strstr(b,"recycl"))  return "Recyclable";
    if(strstr(b,"ew"))      return "E-Waste";
    if(strstr(b,"hazard"))  return "Hazardous";
    return "Other";
}

/* ============================================================
   MAIN
============================================================ */
int main() {

    cout << "=============================================\n";
    cout << " SAMARTHAKA – WASTE MANAGEMENT SYSTEM\n";
    cout << "=============================================\n\n";

    if (!loadCSV("samarthaka_waste.csv")) return 0;

    cout << "Bins loaded: " << BIN_COUNT << "\n\n";

    /* ROUTE OPTIMIZATION */
    cout << "--- ROUTE OPTIMIZATION (Bellman–Ford) ---\n";
    cout << "NOTE: Route graph uses aggregated routes, not individual bins.\n\n";

    buildRouteGraph();
    bellmanFord(0);

    for (int i = 0, shown = 0; i < Nnodes && shown < 5; i++) {
        if (distBF[i] < INF) {
            cout << "Route " << i << " | Distance = "
                 << distBF[i] << " m\n";
            shown++;
        }
    }

    /* WASTE SEGREGATION */
    cout << "\n--- WASTE SEGREGATION ---\n";
    int org=0, rec=0, ew=0, haz=0, oth=0;
    for(int i=0;i<BIN_COUNT;i++){
        const char* c = normalize(Waste[i]);
        if(!strcmp(c,"Organic")) org++;
        else if(!strcmp(c,"Recyclable")) rec++;
        else if(!strcmp(c,"E-Waste")) ew++;
        else if(!strcmp(c,"Hazardous")) haz++;
        else oth++;
    }

    cout << "Organic     : " << org << "\n";
    cout << "Recyclable  : " << rec << "\n";
    cout << "E-Waste     : " << ew  << "\n";
    cout << "Hazardous   : " << haz << "\n";
    cout << "Other       : " << oth << "\n";

    /* LOAD BALANCING */
    cout << "\n--- LOAD BALANCING (Nearby Population) ---\n";
    int maxP=0, maxBin=-1;
    for(int i=0;i<BIN_COUNT;i++){
        if(NearbyPop[i]>maxP){
            maxP=NearbyPop[i];
            maxBin=BinID[i];
        }
    }
    cout << "Highest load bin: " << maxBin
         << " (" << maxP << " people)\n";

    /* FAULT DETECTION */
    cout << "\n--- FAULT DETECTION ---\n";
    for(int i=0,shown=0;i<BIN_COUNT && shown<5;i++){
        if(strcmp(Sensor[i],"OK")!=0){
            cout<<"Bin "<<BinID[i]<<" status: "<<Sensor[i]<<"\n";
            shown++;
        }
    }

    cout << "\n=== END OF WASTE REPORT ===\n";
    return 0;
}
