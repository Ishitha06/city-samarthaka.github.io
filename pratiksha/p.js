/* ========= Modal + Code Loader ========= */

const RAW = "https://raw.githubusercontent.com/Ishitha06/city-samarthaka.github.io/main/pratiksha/datasets/";

const codeSamples = {
  commercial: `// Commercial CSV Reader (Syallabus Friendly)
// Reads top 5 businesses by revenue
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
using namespace std;

struct Biz { string id,name,category,location,status; double revenue; };

int split(const string &s, string p[], int m){
  string c=""; int x=0;
  for(char ch : s){
    if(ch==','){ p[x++]=c; c=""; }
    else c+=ch;
  }
  p[x++] = c;
  return x;
}

int main(){
  ifstream fin("${RAW}commercial_dataset.csv");
  if(!fin){ cout<<"File not found"; return 0; }

  Biz A[2000]; int n=0;
  string line; getline(fin,line);
  while(getline(fin,line)){
    string p[10];
    split(line,p,10);
    A[n].id=p[0];
    A[n].name=p[1];
    A[n].category=p[2];
    A[n].revenue = atof(p[3].c_str());
    A[n].location=p[4];
    A[n].status=p[5];
    n++;
  }

  for(int i=0;i<5;i++){
    int b=i;
    for(int j=i+1;j<n;j++)
      if(A[j].revenue > A[b].revenue) b=j;
    swap(A[i], A[b]);
  }

  for(int i=0;i<5;i++)
    cout<<A[i].id<<" "<<A[i].name<<" "<<A[i].revenue<<"\\n";
}
`,

  healthcare: `// Healthcare Severity Sort
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
using namespace std;

struct P { string id,name,zone,status; int sev; };

int split(const string &s,string p[],int m){
  string c=""; int x=0;
  for(char ch:s){
    if(ch==','){ p[x++]=c; c=""; }
    else c+=ch;
  }
  p[x++]=c;
  return x;
}

int main(){
  ifstream fin("${RAW}healthcare_dataset.csv");
  if(!fin){ cout<<"File Error"; return 0; }

  P A[2000]; int n=0;
  string line; getline(fin,line);
  while(getline(fin,line)){
    string p[10]; split(line,p,10);
    A[n].id=p[0];
    A[n].name=p[1];
    A[n].sev=atoi(p[3].c_str());
    A[n].zone=p[6];
    A[n].status=p[7];
    n++;
  }

  for(int s=5;s>=1;s--)
    for(int i=0;i<n;i++)
      if(A[i].sev==s)
        cout<<A[i].id<<" "<<A[i].name<<" sev="<<A[i].sev<<"\\n";
}
`,

  education: `// Education Top 5 Scorers
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

struct S { string id,name,course; int score; };

int split(const string&s,string p[],int m){
  string c=""; int x=0;
  for(char ch:s){
    if(ch==','){ p[x++]=c; c=""; }
    else c+=ch;
  }
  p[x++] = c;
  return x;
}

int main(){
  ifstream fin("${RAW}education_dataset.csv");
  if(!fin){ cout<<"Error"; return 0; }

  S A[2000]; int n=0;
  string line; getline(fin,line);
  while(getline(fin,line)){
    string p[10]; split(line,p,10);
    A[n].id=p[0];
    A[n].name=p[1];
    A[n].score=atoi(p[2].c_str());
    A[n].course=p[3];
    n++;
  }

  for(int i=0;i<5;i++){
    int b=i;
    for(int j=i+1;j<n;j++)
      if(A[j].score > A[b].score) b=j;
    swap(A[i],A[b]);
  }

  for(int i=0;i<5;i++)
    cout<<A[i].id<<" "<<A[i].name<<" "<<A[i].score<<"\\n";
}
`,

  solar: `// Solar Priority Panel Reader
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
using namespace std;

struct Panel{ string id,loc,status; double out; int pr; };

int split(const string&s,string p[],int m){
  string c=""; int x=0;
  for(char ch:s){
    if(ch==','){ p[x++]=c; c=""; }
    else c+=ch;
  }
  p[x++]=c;
  return x;
}

int main(){
  ifstream fin("${RAW}solar_dataset.csv");
  if(!fin){ cout<<"Not Found"; return 0; }

  Panel P[2000]; int n=0;
  string line; getline(fin,line);
  while(getline(fin,line)){
    string p[10]; split(line,p,10);
    P[n].id=p[0];
    P[n].loc=p[1];
    P[n].status=p[2];
    P[n].out=atof(p[3].c_str());
    P[n].pr=atoi(p[4].c_str());
    n++;
  }

  int printed=0;
  for(int pr=100;pr>=0 && printed<5;pr--)
    for(int i=0;i<n && printed<5;i++)
      if(P[i].pr==pr && P[i].status=="Active"){
        cout<<P[i].id<<" "<<P[i].loc<<" pr="<<P[i].pr<<"\\n";
        printed++;
      }
}
`
};

/* ========= Modal Control ========= */
const modalBack = document.getElementById("modalBackdrop");
const modalCode = document.getElementById("modalCode");
const modalTitle = document.getElementById("modalTitle");
const closeBtn = document.getElementById("closeModal");

document.addEventListener("click", e => {
  const btn = e.target.closest("[data-code]");
  if (btn) {
    const kind = btn.getAttribute("data-code");
    modalTitle.textContent = "C++ Code — " + kind.toUpperCase();
    modalCode.textContent = codeSamples[kind];
    modalBack.style.display = "flex";
  }

  if (e.target === closeBtn || e.target === modalBack) {
    modalBack.style.display = "none";
  }
});

document.addEventListener("keydown", e => {
  if (e.key === "Escape") modalBack.style.display = "none";
});
