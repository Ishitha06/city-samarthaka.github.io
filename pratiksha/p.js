/* p.js - final fixed version
   - Uses your exact C++ blocks (unchanged)
   - Puts them in window.codeMap
   - Shows them inside the modal when .code-btn is clicked
   - Modal open/close handling
*/

/* Ensure a global map exists */
window.codeMap = window.codeMap || {};

document.addEventListener('DOMContentLoaded', function(){

  /* -----------------------
     Insert your C++ blocks into window.codeMap (exactly as provided)
     ----------------------- */

  window.codeMap['commercial'] = `/* CommercialServices.cpp
   Syllabus-friendly C++ program for Commercial Services
   - Hash map for business lookup by ID
   - multiset (balanced) for revenue ranking
   - priority_queue (heap) for urgent approvals
   - CSV reader (header-aware)
*/
#include <bits/stdc++.h>
using namespace std;

struct Biz {
  string id, name, category, location;
  double revenue;
  double rating;
  int priority;
};

vector<string> splitCSV(const string &s){
  vector<string> out; string cur; bool inq=false;
  for(char c: s){
    if (c=='\"') inq=!inq;
    else if (c==',' && !inq){ out.push_back(cur); cur.clear(); }
    else cur.push_back(c);
  }
  out.push_back(cur); return out;
}

string lowerS(const string &s){ string r=s; for(auto &c:r) c=tolower(c); return r; }

int main(int argc,char**argv){
  ios::sync_with_stdio(false);
  cin.tie(NULL);
  string path = "pratiksha/datasets/commercial_dataset.csv";
  if(argc>1) path = argv[1];

  ifstream fin(path);
  if(!fin.is_open()){ cerr<<"Cannot open "<<path<<"\\n"; return 1; }

  string header; getline(fin, header);
  auto cols = splitCSV(header);
  int idx_id=-1, idx_name=-1, idx_cat=-1, idx_rev=-1, idx_loc=-1, idx_rat=-1, idx_pr=-1;
  for(int i=0;i<(int)cols.size();++i){
    string c = lowerS(cols[i]);
    if(c.find("id")!=string::npos && idx_id==-1) idx_id=i;
    if(c.find("name")!=string::npos && idx_name==-1) idx_name=i;
    if(c.find("cat")!=string::npos && idx_cat==-1) idx_cat=i;
    if(c.find("rev")!=string::npos && idx_rev==-1) idx_rev=i;
    if(c.find("loc")!=string::npos && idx_loc==-1) idx_loc=i;
    if(c.find("rat")!=string::npos && idx_rat==-1) idx_rat=i;
    if(c.find("priority")!=string::npos && idx_pr==-1) idx_pr=i;
  }
  // fallback positions
  if(idx_id==-1) idx_id=0; if(idx_name==-1) idx_name=1; if(idx_cat==-1) idx_cat=2;
  if(idx_rev==-1) idx_rev=3; if(idx_loc==-1) idx_loc=4; if(idx_rat==-1) idx_rat=5; if(idx_pr==-1) idx_pr=6;

  unordered_map<string,Biz> byId;
  // revenue set: pair(-revenue,id) to sort descending
  multiset<pair<double,string>> revenueSet;
  priority_queue<pair<int,string>> urgent; // priority, id

  string line; int rows=0;
  while(getline(fin,line)){
    if(line.size()==0) continue;
    auto p = splitCSV(line);
    if((int)p.size() <= max({idx_id,idx_name,idx_cat,idx_rev,idx_loc,idx_rat,idx_pr})) continue;
    Biz b;
    b.id = p[idx_id];
    b.name = p[idx_name];
    b.category = p[idx_cat];
    try{ b.revenue = stod(p[idx_rev]); } catch(...) { b.revenue=0; }
    b.location = p[idx_loc];
    try{ b.rating = stod(p[idx_rat]); } catch(...) { b.rating=0; }
    try{ b.priority = stoi(p[idx_pr]); } catch(...) { b.priority=0; }

    byId[b.id] = b;
    revenueSet.insert({-b.revenue, b.id});
    if(b.priority>0) urgent.push({b.priority, b.id});
    rows++;
  }
  fin.close();
  cout<<"Loaded "<<rows<<" commercial records.\\n";

  while(true){
    cout<<"Menu - Commercial (1: lookup,2: top-k revenue,3: urgent approvals,4: stats,0:exit): ";
    int opt; if(!(cin>>opt)) break;
    if(opt==0) break;
    if(opt==1){
      cout<<"Enter business id: "; string id; cin>>id;
      if(byId.count(id)){
        auto &b=byId[id];
        cout<<b.id<<" | "<<b.name<<" | "<<b.category<<" | rev="<<b.revenue<<" | loc="<<b.location<<" | rating="<<b.rating<<" | pr="<<b.priority<<"\\n";
      } else cout<<"Not found\\n";
    } else if(opt==2){
      cout<<"K? "; int k; cin>>k; int c=0;
      for(auto it=revenueSet.begin(); it!=revenueSet.end() && c<k; ++it,++c){
        cout<<c+1<<") "<<it->second<<" rev="<<-it->first<<"\\n";
      }
    } else if(opt==3){
      cout<<"Top K urgent? "; int k; cin>>k; int c=0;
      auto copy = urgent;
      while(!copy.empty() && c<k){
        auto pr = copy.top(); copy.pop();
        cout<<"#"<<(++c)<<" id="<<pr.second<<" priority="<<pr.first<<"\\n";
      }
    } else if(opt==4){
      cout<<"Total businesses="<<byId.size()<<" top categories sample:\\n";
      unordered_map<string,int> catc; for(auto &kv:byId) catc[kv.second.category]++;
      for(auto &kv:catc) cout<<kv.first<<":"<<kv.second<<"\\n";
    } else cout<<"Unknown\\n";
  }
  cout<<"Bye\\n";
  return 0;
}
`;

  window.codeMap['healthcare'] = `/* HealthcareSystem.cpp
   - unordered_map for patient lookup
   - max-heap for triage by severity & wait time
   - simple graph + dijkstra for ambulance routing between zones
*/
#include <bits/stdc++.h>
using namespace std;
struct Patient {
  string id, name, zone;
  int severity; // bigger => more severe
  int wait; // minutes
};
vector<string> splitCSV(const string &s){
  vector<string> out; string cur; bool inq=false;
  for(char c: s){ if(c=='\"') inq=!inq; else if(c==',' && !inq){ out.push_back(cur); cur.clear(); } else cur.push_back(c); }
  out.push_back(cur); return out;
}
string lowerS(const string &s){ string r=s; for(char &c:r) c=tolower(c); return r; }

// small hash for zones
long hname(const string &s){ long h=0; for(char c:s) h = h*131 + (unsigned char)c; return llabs(h); }
double weight(const string &a, const string &b){ return 1.0 + fabs((double)((hname(a)%1000)-(hname(b)%1000)))/20.0; }

class Healthcare {
public:
  unordered_map<string,Patient> patients;
  struct Node{ int sev; int wait; string id; };
  struct Cmp { bool operator()(const Node &a,const Node&b)const{ if(a.sev!=b.sev) return a.sev < b.sev; return a.wait < b.wait; } };
  priority_queue<Node, vector<Node>, Cmp> triage;
  unordered_set<string> zones;
  unordered_map<string, vector<pair<string,double>>> graph;

  bool loadCSV(const string &path){
    ifstream fin(path); if(!fin.is_open()){ cerr<<"Cannot open "<<path<<"\\n"; return false; }
    string header; getline(fin, header); auto cols = splitCSV(header);
    int idx_id=-1, idx_name=-1, idx_zone=-1, idx_sev=-1, idx_wait=-1;
    for(int i=0;i<(int)cols.size();++i){
      string c=lowerS(cols[i]);
      if(c.find("id")!=string::npos && idx_id==-1) idx_id=i;
      if(c.find("name")!=string::npos && idx_name==-1) idx_name=i;
      if(c.find("zone")!=string::npos && idx_zone==-1) idx_zone=i;
      if(c.find("sev")!=string::npos || c.find("severity")!=string::npos) idx_sev=i;
      if(c.find("wait")!=string::npos) idx_wait=i;
    }
    if(idx_id==-1) idx_id=0; if(idx_name==-1) idx_name=1; if(idx_zone==-1) idx_zone=2;
    if(idx_sev==-1) idx_sev=3; if(idx_wait==-1) idx_wait=4;
    string line; while(getline(fin,line)){
      if(line.empty()) continue;
      auto p = splitCSV(line);
      if((int)p.size()<=max({idx_id,idx_name,idx_zone,idx_sev,idx_wait})) continue;
      Patient pat; pat.id=p[idx_id]; pat.name=p[idx_name]; pat.zone=p[idx_zone];
      try{ pat.severity = stoi(p[idx_sev]); } catch(...) { pat.severity=0; }
      try{ pat.wait = stoi(p[idx_wait]); } catch(...) { pat.wait=0; }
      patients[pat.id]=pat; triage.push({pat.severity, pat.wait, pat.id}); zones.insert(pat.zone);
    }
    fin.close();
    buildGraph();
    cout<<"Loaded "<<patients.size()<<" patients\\n";
    return true;
  }

  void buildGraph(){
    vector<string> vs(zones.begin(), zones.end());
    for(auto &a:vs) for(auto &b:vs) if(a!=b) graph[a].push_back({b, weight(a,b)});
  }

  vector<pair<string,double>> route(const string &start, const vector<string>&targets){
    // dijkstra
    unordered_map<string,double> dist;
    for(auto &kv:graph) dist[kv.first]=1e18;
    if(!graph.count(start)) return {};
    using P = pair<double,string>;
    priority_queue<P, vector<P>, greater<P>> pq;
    dist[start]=0; pq.push({0,start});
    while(!pq.empty()){
      auto [d,u]=pq.top(); pq.pop();
      if(d!=dist[u]) continue;
      for(auto &e:graph[u]){
        if(dist[e.first] > dist[u] + e.second){ dist[e.first]=dist[u]+e.second; pq.push({dist[e.first], e.first}); }
      }
    }
    vector<pair<string,double>> res;
    for(auto &t:targets) res.push_back({t, dist.count(t)?dist[t]:1e18});
    return res;
  }
};

int main(int argc,char**argv){
  string path = "pratiksha/datasets/healthcare_dataset.csv";
  if(argc>1) path=argv[1];
  Healthcare hc;
  if(!hc.loadCSV(path)) return 1;
  while(true){
    cout<<"(1) Triage top, (2) Lookup patient, (3) Route from zone, (0) exit: ";
    int opt; if(!(cin>>opt)) break;
    if(opt==0) break;
    if(opt==1){
      cout<<"K? "; int k; cin>>k; auto copy = hc.triage; int c=0;
      while(!copy.empty() && c<k){ auto n=copy.top(); copy.pop(); auto &p=hc.patients[n.id]; cout<<++c<<") "<<p.id<<" "<<p.name<<" sev="<<p.severity<<" wait="<<p.wait<<" zone="<<p.zone<<"\\n"; }
    } else if(opt==2){
      cout<<"Patient id: "; string id; cin>>id;
      if(hc.patients.count(id)){ auto &p=hc.patients[id]; cout<<p.id<<" "<<p.name<<" zone="<<p.zone<<" sev="<<p.severity<<" wait="<<p.wait<<"\\n"; }
      else cout<<"Not found\\n";
    } else if(opt==3){
      cout<<"Start zone: "; string s; cin>>ws; getline(cin,s);
      cout<<"Targets (comma separated): "; string tline; getline(cin,tline);
      auto parts = splitCSV(tline);
      auto res = hc.route(s, parts);
      for(auto &r:res) cout<<"-> "<<r.first<<" dist="<<(isfinite(r.second)?to_string(r.second):string("N/A"))<<"\\n";
    } else cout<<"Unknown\\n";
  }
  return 0;
}
`;

  window.codeMap['education'] = `/* EducationSystem.cpp
   - unordered_map for student records
   - multiset/set used as balanced structure for top-k (AVL behaviour)
   - simple graph coloring demonstration for timetable (greedy)
   - CSV: pratiksha/datasets/education_dataset.csv
*/
#include <bits/stdc++.h>
using namespace std;
vector<string> splitCSV(const string &s){
  vector<string> out; string cur; bool inq=false;
  for(char c:s){ if(c=='\"') inq=!inq; else if(c==',' && !inq){ out.push_back(cur); cur.clear(); } else cur.push_back(c); }
  out.push_back(cur); return out;
}
string lowerS(const string &s){ string r=s; for(auto &c:r) c=tolower(c); return r; }

struct Student { string id,name,course,zone; double score; };

int main(int argc,char**argv){
  string path = "pratiksha/datasets/education_dataset.csv";
  if(argc>1) path=argv[1];
  ifstream fin(path); if(!fin.is_open()){ cerr<<"Cannot open "<<path<<"\\n"; return 1; }
  string header; getline(fin, header); auto cols = splitCSV(header);
  int ii=-1,in=-1,ic=-1,iscr=-1,iz=-1;
  for(int i=0;i<cols.size();++i){ string c=lowerS(cols[i]);
    if(c.find("id")!=string::npos && ii==-1) ii=i;
    if(c.find("name")!=string::npos && in==-1) in=i;
    if(c.find("course")!=string::npos && ic==-1) ic=i;
    if(c.find("score")!=string::npos && iscr==-1) iscr=i;
    if(c.find("zone")!=string::npos && iz==-1) iz=i;
  }
  if(ii==-1) ii=0; if(in==-1) in=1; if(ic==-1) ic=2; if(iscr==-1) iscr=3; if(iz==-1) iz=4;
  unordered_map<string, Student> byId;
  // balanced order: multiset by (-score, id)
  multiset<pair<double,string>> rank;
  string line; while(getline(fin,line)){
    if(line.empty()) continue;
    auto p = splitCSV(line);
    if((int)p.size() <= max({ii,in,ic,iscr,iz})) continue;
    Student s; s.id=p[ii]; s.name=p[in]; s.course=p[ic];
    try{ s.score = stod(p[iscr]); } catch(...) { s.score=0; }
    s.zone = p[iz];
    byId[s.id]=s; rank.insert({-s.score, s.id});
  }
  fin.close();
  cout<<"Loaded "<<byId.size()<<" students\\n";

  while(true){
    cout<<"(1) lookup (2) top-K (3) timetable coloring demo (0 exit): ";
    int opt; if(!(cin>>opt)) break;
    if(opt==0) break;
    if(opt==1){
      cout<<"student id: "; string id; cin>>id;
      if(byId.count(id)){ auto &s=byId[id]; cout<<s.id<<" "<<s.name<<" course="<<s.course<<" score="<<s.score<<" zone="<<s.zone<<"\\n"; } else cout<<"Not found\\n";
    } else if(opt==2){
      cout<<"K: "; int k; cin>>k; int c=0;
      for(auto it=rank.begin(); it!=rank.end() && c<k; ++it,++c){
        cout<<c+1<<") "<<it->second<<" score="<<-it->first<<"\\n";
      }
    } else if(opt==3){
      cout<<"Timetable coloring (sample). Enter number of classes: "; int n; cin>>n;
      vector<vector<int>> g(n);
      cout<<"Enter adjacency pairs (u v) 0-based, -1 -1 to stop:\\n";
      while(true){
        int u,v; cin>>u>>v; if(u==-1&&v==-1) break;
        if(u>=0 && v>=0 && u<n && v<n){ g[u].push_back(v); g[v].push_back(u); }
      }
      vector<int> color(n, -1);
      for(int i=0;i<n;i++){
        set<int> used;
        for(int nb:g[i]) if(color[nb]!=-1) used.insert(color[nb]);
        int c=0; while(used.count(c)) c++;
        color[i]=c;
      }
      cout<<"Assigned colors (room slots):\\n"; for(int i=0;i<n;i++) cout<<i<<":"<<color[i]<<"\\n";
    } else cout<<"Unknown\\n";
  }
  return 0;
}
`;

  window.codeMap['solar'] = `/* SolarManagement.cpp
   Full solar management demo that:
   - Reads CSV (header-aware)
   - Uses unordered_map for panel lookup
   - Uses priority_queue (max-heap) for maintenance allocation
   - Builds a deterministic graph of locations and runs Dijkstra for routing
   CSV path: pratiksha/datasets/solar_dataset.csv
*/
#include <bits/stdc++.h>
using namespace std;

struct Panel { string id, location; double output_kw, health; int priority; };

vector<string> splitCSV(const string &s){
  vector<string> out; string cur; bool inq=false;
  for(char c: s){ if(c=='\"') inq=!inq; else if(c==',' && !inq){ out.push_back(cur); cur.clear(); } else cur.push_back(c); }
  out.push_back(cur); return out;
}
string lowerS(const string &s){ string r=s; for(auto &c:r) c=tolower(c); return r; }
long hname(const string &s){ long h=1469598103934665603ULL; for(char c:s) h = (h ^ (unsigned char)c) * 1099511628211ULL; return llabs(h); }
double edgeW(const string &a,const string &b){ return 1.0 + fabs((double)((hname(a)%1000)-(hname(b)%1000)))/10.0; }

using Adj = unordered_map<string, vector<pair<string,double>>>;

class Solar {
public:
  unordered_map<string,Panel> panels;
  unordered_set<string> locs;
  struct Node { int pr; double health; string id; };
  struct Cmp { bool operator()(Node const& a, Node const& b) const {
    if(a.pr!=b.pr) return a.pr < b.pr; return a.health > b.health;
  }};
  priority_queue<Node, vector<Node>, Cmp> heap;
  Adj graph;

  bool loadCSV(const string &path){
    ifstream fin(path); if(!fin.is_open()){ cerr<<"Cannot open "<<path<<"\\n"; return false; }
    string header; getline(fin, header); auto cols = splitCSV(header);
    int i_id=-1,i_loc=-1,i_out=-1,i_health=-1,i_pr=-1;
    for(int i=0;i<cols.size();++i){
      string c = lowerS(cols[i]);
      if(c.find("panel")!=string::npos && c.find("id")!=string::npos) i_id=i;
      if(c.find("id")!=string::npos && i_id==-1) i_id=i;
      if(c.find("location")!=string::npos) i_loc=i;
      if(c.find("output")!=string::npos || c.find("capacity")!=string::npos) i_out=i;
      if(c.find("health")!=string::npos) i_health=i;
      if(c.find("priority")!=string::npos) i_pr=i;
    }
    if(i_id==-1) i_id=0; if(i_loc==-1) i_loc=1; if(i_out==-1) i_out=2;
    if(i_health==-1) i_health=3; if(i_pr==-1) i_pr=4;
    string line; int r=0;
    while(getline(fin,line)){
      if(line.empty()) continue;
      auto p = splitCSV(line);
      if((int)p.size() <= max({i_id,i_loc,i_out,i_health,i_pr})) continue;
      Panel pan; pan.id=p[i_id]; pan.location=p[i_loc];
      try{ pan.output_kw = stod(p[i_out]); } catch(...) { pan.output_kw = 0; }
      try{ pan.health = stod(p[i_health]); } catch(...) { pan.health = 100; }
      try{ pan.priority = stoi(p[i_pr]); } catch(...) { pan.priority = 0; }
      panels[pan.id]=pan; locs.insert(pan.location); heap.push({pan.priority, pan.health, pan.id}); r++;
    }
    fin.close();
    buildGraph();
    cout<<"Loaded "<<r<<" panels\\n";
    return true;
  }

  void buildGraph(){
    vector<string> v(locs.begin(), locs.end());
    int n=v.size();
    for(int i=0;i<n;i++){
      vector<pair<long,int>> list;
      long hi = hname(v[i])%1000;
      for(int j=0;j<n;j++){ if(i==j) continue; long hj = hname(v[j])%1000; list.push_back({(long)llabs(hi-hj), j}); }
      sort(list.begin(), list.end());
      int k = min((int)list.size(), 4);
      for(int t=0;t<k;t++){
        int j=list[t].second;
        double w = edgeW(v[i], v[j]);
        graph[v[i]].push_back({v[j], w});
        graph[v[j]].push_back({v[i], w});
      }
    }
  }

  bool lookup(const string &id, Panel &out) const {
    auto it = panels.find(id); if(it==panels.end()) return false; out = it->second; return true;
  }

  vector<Panel> topK(int k){
    vector<Panel> out; auto copy = heap;
    while(!copy.empty() && (int)out.size()<k){ auto n=copy.top(); copy.pop(); out.push_back(panels.at(n.id)); }
    return out;
  }

  unordered_map<string,double> dijkstra(const string &src){
    unordered_map<string,double> dist;
    for(auto &kv:graph) dist[kv.first]=1e18;
    if(!graph.count(src)) return dist;
    using P=pair<double,string>;
    priority_queue<P, vector<P>, greater<P>>pq;
    dist[src]=0; pq.push({0,src});
    while(!pq.empty()){
      auto [d,u]=pq.top(); pq.pop();
      if(d!=dist[u]) continue;
      for(auto &e: graph.at(u)){
        if(dist[e.first] > dist[u] + e.second){ dist[e.first] = dist[u] + e.second; pq.push({dist[e.first], e.first}); }
      }
    }
    return dist;
  }
};

int main(int argc,char**argv){
  string path = "pratiksha/datasets/solar_dataset.csv";
  if(argc>1) path=argv[1];
  Solar sm; if(!sm.loadCSV(path)) return 1;
  cout<<"Solar management loaded. Panels="<<sm.panels.size()<<"\\n";
  while(true){
    cout<<"(1) lookup id, (2) top-k maintenance, (3) routes from loc, (4) stats, (0)exit: ";
    int opt; if(!(cin>>opt)) break;
    if(opt==0) break;
    if(opt==1){ cout<<"id: "; string id; cin>>id; Panel p; if(sm.lookup(id,p)) cout<<p.id<<" "<<p.location<<" out="<<p.output_kw<<" health="<<p.health<<" pr="<<p.priority<<"\\n"; else cout<<"Not found\\n"; }
    else if(opt==2){ cout<<"K? "; int k; cin>>k; auto v=sm.topK(k); for(int i=0;i<v.size();++i) cout<<i+1<<") "<<v[i].id<<" loc="<<v[i].location<<" pr="<<v[i].priority<<" health="<<v[i].health<<"\\n"; }
    else if(opt==3){
      cout<<"start location: "; string s; cin>>ws; getline(cin,s);
      cout<<"targets (comma sep): "; string tline; getline(cin,tline);
      vector<string> parts = splitCSV(tline);
      auto dist = sm.dijkstra(s);
      for(auto &t: parts){
        double d = (dist.count(t)?dist[t]:1e18);
        cout<<"to "<<t<<" -> "<<(isfinite(d)?to_string(d):string("N/A"))<<"\\n";
      }
    } else if(opt==4){
      cout<<"Panels="<<sm.panels.size()<<" Locations="<<sm.locs.size()<<"\\n";
      int c=0; for(auto &kv:sm.panels){ cout<<kv.first<<" loc="<<kv.second.location<<" pr="<<kv.second.priority<<"\\n"; if(++c>=5) break; }
    } else cout<<"unknown\\n";
  }
  return 0;
}
`;

  /* -----------------------
     Modal handling (use window.codeMap)
     ----------------------- */
  const modalBack = document.getElementById('modalBackdrop');
  const modalTitle = document.getElementById('modalTitle');
  const modalCode = document.getElementById('modalCode');
  const closeBtn = document.getElementById('closeModal');

  // Guard: if elements missing, bail gracefully (avoid runtime errors)
  function elementsExist() {
    return modalBack && modalTitle && modalCode && closeBtn;
  }

  // Hook up code buttons
  document.querySelectorAll('.code-btn').forEach(btn=>{
    btn.addEventListener('click', function(){
      const module = this.dataset.module;
      const text = window.codeMap[module] || '// Code not available for ' + module;
      if (!elementsExist()) {
        // fallback: open a new tab with a plain text page containing code (if modal missing)
        const w = window.open();
        w.document.body.style.whiteSpace = 'pre';
        w.document.title = module + ' C++ Code';
        w.document.body.textContent = text;
        return;
      }
      modalTitle.textContent = 'C++ — ' + module;
      modalCode.textContent = text;
      modalBack.classList.remove('hidden');
      modalCode.focus();
    });
  });

  // close handlers
  closeBtn.addEventListener('click', ()=> {
    if (elementsExist()) modalBack.classList.add('hidden');
  });
  modalBack.addEventListener('click', e => {
    if (e.target === modalBack) modalBack.classList.add('hidden');
  });
  window.addEventListener('keydown', e => {
    if (e.key === 'Escape' && elementsExist()) modalBack.classList.add('hidden');
  });

  // done DOMContentLoaded
}); // end DOMContentLoaded
