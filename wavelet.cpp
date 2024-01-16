#include<bits/stdc++.h>
#include <fstream>
#include <chrono>
using namespace std;

//Global variables
map<char, string> charMap;
int logsigma;

//Utility functions

//function to implement string split on a delimiter
vector<string> split(const string &text, char sep) {
  vector<string> tokens;
  int start = 0, end = 0;
  while ((end = text.find(sep, start)) != string::npos) {
    tokens.push_back(text.substr(start, end - start));
    start = end + 1;
  }
  tokens.push_back(text.substr(start));
  return tokens;
}

//used for debugging
void display(vector<int>p) {
  for(int i=0;i<p.size();i++)
    cout<<p[i]<<" ";
  cout<<endl;
}

//Structure of a Wavelet Node
struct WtNode {
  vector<int>ranks;
  WtNode* left;
  WtNode* right;
  bool isLeaf;
  char leaf;
};

//Create a new Wavelet Node
WtNode* createNewNode(vector<int>ranks, WtNode* left, WtNode* right, bool isLeaf, char leaf) {
  WtNode* node = new WtNode;
  node->ranks = ranks;
  node->left = left;
  node->right = right;
  node->isLeaf = isLeaf;
  node->leaf = leaf;
  return node;
}

//a recursive function to build the tree
void buildWt(WtNode* node, string text, int level, map<char, string>& charMap) {
  vector<int> ranks;
  int rc = 0;
  string leftstr = "", rightstr = "";
  for(int i=0;i<text.length();i++){
    //getting the character code from the map
    string code = charMap[text[i]];
    if(code[level] == '0') {
      leftstr+=text[i];
      ranks.push_back(rc);
    } else {
      rightstr+=text[i];
      ranks.push_back(rc);
      rc+=1;
    }
  }
  //for calculating bit vector
  string cod = charMap[text[text.length()-1]];
  if(cod[level] == '0') ranks.push_back(ranks[ranks.size()-1]);
  else ranks.push_back(ranks[ranks.size()-1]+1);

  node->ranks = ranks;
  //cout<<"Displaying rank for level :"<<level<<endl;
  //display(ranks);
  //cout<<"Leftstr : "<<leftstr<<" "<<" Rightstr : "<<rightstr<<endl;

  //Base condition: if either left or right string is empty and the its the last level then mark the node as leaf
  if((leftstr == "" || rightstr == "") && level == charMap.begin()->second.length()) {
    node->isLeaf = true;
    node->leaf = text[0];
    return;
  }
  WtNode* leftchild = createNewNode({}, NULL, NULL, false, '#');
  WtNode* rightchild = createNewNode({}, NULL, NULL, false, '#');
  node->left = leftchild;
  node->right = rightchild;
  if(leftstr != "")
    buildWt(node->left, leftstr, level+1, charMap);
  if(rightstr != "")
    buildWt(node->right, rightstr, level+1, charMap);
}

//function to get the rank of 0 at a given index i
int rankzero(int& i, vector<int>& rank) {
  if(i==0)
    return 0;
  return (i) - rank[i];
}

//function for access query
char access(WtNode* root, int index, int& logsigma) {
  int l = index;
  for(int i=0;i<logsigma;i++){
    //checking if bitvector at l is 0
    vector<int>ran = root->ranks;
    if(ran[l] == ran[l+1]) {
      l = rankzero(l, ran);
      root = root->left;
    } else {
      l = ran[l];
      root = root->right;
    }
    if(root->isLeaf)
      return root->leaf;
  }
  return '#';
}

//function for rank query
int rankq(WtNode* root, int index, int& logsigma, string& code) {
  int l = index;
  for(int i=0;i<logsigma;i++){
    //checking if bitvector at l is 0
    vector<int>ran = root->ranks;
    if(code[i] == '0') {
      l = rankzero(l, ran);
      root = root->left;
    } else {
      l = ran[l];
      root = root->right;
    }
    if(root->isLeaf)
      break;
  }
  return l;
}

//generates the code for all distinct characters in the string
void setup(string& text){
  int distinctChar = 0;
  for(auto p = text.begin(); p != text.end(); ++p)
      if(charMap.find(*p) == charMap.end())
          charMap[*p] = "0";

  cout<<charMap.size()<<endl;
  logsigma = ceil(log2(charMap.size()));
  string sig = bitset<8> (charMap.size()-1).to_string();
  cout<<logsigma<<" "<<sig<<endl;
  //to get the length of the longest code possible
  const auto loc = sig.find('1');
  for(auto p = charMap.begin(); p != charMap.end(); ++p) {
    string s = bitset<8> (distinctChar).to_string();
    string f = s.substr(loc);
    charMap[p->first] = f;
    distinctChar++;
  }
}

int main(){

  string text;
  ifstream file("in.txt");
  getline(file, text);
  cout<<"Length = "<<text.length()<<endl;
  setup(text);

  //display charMap
  for(auto p = charMap.begin(); p != charMap.end(); ++p) {
    cout<<p->first<<" "<<p->second<<endl;
  }

  WtNode* root = createNewNode({}, NULL, NULL, false, '#');

   // Get the start time
  auto build_sta = std::chrono::high_resolution_clock::now();
  buildWt(root, text, 0, charMap);
  auto build_sto = std::chrono::high_resolution_clock::now();
  auto duration_build = std::chrono::duration_cast<std::chrono::milliseconds>(build_sto - build_sta);
  cout << "Build Runtime: " << duration_build.count() << "ms" << std::endl;


  cout<<"Enter an index for access query :"<<endl;
  int index;
  cin>>index;
  if(index >= text.length()){
    cout<<"Index out of bounds!"<<endl;
    return 0;
  }
  auto access_sta = std::chrono::high_resolution_clock::now();
  cout<<access(root, index, logsigma)<<endl;
  auto access_sto = std::chrono::high_resolution_clock::now();
  auto duration_access = std::chrono::duration_cast<std::chrono::milliseconds>(access_sto - access_sta);
  cout << "Access Runtime: " << duration_access.count() << "ms" << std::endl;


  cout<<"Enter an index and a character for rank query :"<<endl;
  int rin;
  char rc;
  cin>>rin>>rc;
  if(rin >= text.length()){
    cout<<"Index out of bounds!"<<endl;
    return 0;
  }
  if(charMap[rc] == ""){
    cout<<"Character does not exist in the string!"<<endl;
    return 0;
  }
  auto rank_sta = std::chrono::high_resolution_clock::now();
  cout<<rankq(root, rin, logsigma, charMap[rc])<<endl;
  auto rank_sto = std::chrono::high_resolution_clock::now();
  auto duration_rank = std::chrono::duration_cast<std::chrono::milliseconds>(rank_sto - rank_sta);
  cout << "Rank Runtime: " << duration_rank.count() << "ms" << std::endl;
}
