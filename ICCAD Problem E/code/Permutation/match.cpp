#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>
#include <algorithm>
#include <numeric>

using namespace std;

struct Signal {
    string name;
    Signal(string n) : name(n) {}
};

struct sig_class {
    string IO;
    int width;
    vector<Signal*> old_sig;
    vector<Signal*> new_sig;
};

using sig_match = vector<pair<Signal*, Signal*>>;

//產生new_sig對應到old_sig的所有one to one配對（new_sig[i] -> old_sig[perm[i]]）
vector<sig_match>match_one_class(const vector<Signal*>& old_sig,const vector<Signal*>& new_sig){
    vector<sig_match> results;
    if (new_sig.size() > old_sig.size()) return results;
    vector<int> indices(old_sig.size());
    iota(indices.begin(), indices.end(), 0);

    bool first = true;  //先做一次 permutation 結果處理
    while (first || next_permutation(indices.begin(), indices.end())) {
        first = false;
        sig_match m;
        for (size_t i = 0; i < new_sig.size(); ++i) {
            m.emplace_back(old_sig[indices[i]], new_sig[i]);
        }
        results.push_back(m);
    }
    return results;
}


using all_class_matches= vector<vector<sig_match>>;//所有sig_class 的配對結果存在這裡

//遞迴列舉所有 class 配對組合
void dfs_permutate(const all_class_matches& match_lists,size_t idx, 
    sig_match& current,vector<sig_match>& all_solutions){
    if(idx== match_lists.size()){
        all_solutions.push_back(current);
        return;
    }

    for(const auto& match : match_lists[idx]){
        current.insert(current.end(),match.begin(), match.end());
        dfs_permutate(match_lists, idx+1, current, all_solutions);
        current.erase(current.end()-match.size(), current.end()); //backtrack
    }
}

vector<sig_match> make_all_sig_matches(const unordered_map<string, sig_class>& class_map) {
    all_class_matches match_lists;
    for (const auto& [key, sc] : class_map){
        auto matches=match_one_class(sc.old_sig, sc.new_sig);
        match_lists.push_back(matches);
    }
    vector<sig_match> all_solutions;
    sig_match current;
    dfs_permutate(match_lists, 0, current, all_solutions);
    return all_solutions;
}

int main() {
    Signal* o1= new Signal("o1");
    Signal* o2= new Signal("o2");
    Signal* n1= new Signal("n1");
    Signal* n2= new Signal("n2");

    Signal* o3= new Signal("o3");
    Signal* n3= new Signal("n3");
    Signal* o4= new Signal("o4");
    Signal* n4= new Signal("n4");
    Signal* o5= new Signal("o5");
    Signal* n5= new Signal("n5");

    //sig_class A
    sig_class scA;
    scA.old_sig= {o1, o2};
    scA.new_sig= {n1, n2};

    //sig_class B
    sig_class scB;
    scB.old_sig= {o3,o4,o5};
    scB.new_sig= {n3,n4,n5};

    unordered_map<string, sig_class> class_map;
    class_map["A"]= scA;
    class_map["B"]= scB;

    vector<sig_match> all_matches=make_all_sig_matches(class_map);   //產生所有解

    int count=1;
    for(const auto& match_set :all_matches){             //輸出所有配對組合
        cout<<"Match "<<count++ << ":\n";
        for(const auto& p : match_set) {
            cout<<"  "<<p.first->name<<" -> "<<p.second->name<<"\n";
        }
        cout<<"\n";
    }

    delete o1; delete o2; delete o3;
    delete n1; delete n2; delete n3;

    return 0;
}
