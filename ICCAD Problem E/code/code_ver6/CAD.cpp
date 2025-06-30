#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <string>
#include <list>
#include <utility>
#include <algorithm>
#include <numeric>
#include <cctype>
#include "AST.h"

using namespace std;

struct comb;

struct Signal{
    string name;              //不會被動到，會被動到的是AST* node
    string IO;
    int width;
    vector<AST_node*> AST_ptr;//in_sig才有
    comb* comb_ptr;           //out_sig才有
};

struct comb{
    Signal* out_sig;
    AST logic_tree;
};

struct sig_class{
    string IO;
    int width;
    vector<Signal*> old_sig;
    vector<Signal*> new_sig;
};

struct Module{
    list<Signal> sig_list;
    list<comb> logic;
    unordered_map<string, Signal*> sig_map;
};

using sig_match= vector<pair<Signal*, Signal*>>; //單一sig_class的配對結果
using all_class_match= vector<vector<sig_match>>;//對單個module而言，所有sig_class的配對結果

struct match_info{
    vector<Module> old_module;
    vector<Module> new_module;

    vector<unordered_map<string,sig_class>> class_map;//配對依據

    vector<sig_match> match_result;           //儲存所有module的正確配對結果
};

//for parse_submodule()
vector<string> split_by_semicolon(const string& input){ 
    vector<string> result;
    string current;
    bool inWhitespace = false;
    for (char ch : input) {
        if (isspace(ch)) {
            if (!inWhitespace && !current.empty()) {
                current += ' ';  //只保留一個空格
                inWhitespace = true;
            }
        } else {
            inWhitespace = false;
            current += ch;
            if (ch == ';'){//去除最後可能多餘的空格
                if(!current.empty()&&current.back()==' '){
                    current.pop_back();
                }
                result.push_back(current);
                current.clear();
            }
        }
    }
    if (!current.empty()) {//處理最後一個沒有分號的部分（如果有）
        if (!current.empty() && current.back() == ' ') {
            current.pop_back();
        }
        result.push_back(current);
    }
    return result;
}

//for parse_submodule()
vector<string> split_modules(const string& filename){ 
    ifstream file(filename);
    if (!file.is_open()) {
        cout<<"can't open file: "<<filename<<endl;
        return {};
    }
    vector<string> modules;
    string currentModule, line;
    bool insideModule = false;

    while (getline(file, line)) {
        if (line.find("module") != string::npos) {//檢查是否遇到 "module" 關鍵字
            if (insideModule) { //如果已經在一個 module 內部，則先儲存當前 module
                modules.push_back(currentModule);
                currentModule.clear();
            }
            insideModule = true;
        }
        if (insideModule) currentModule += line;  // 保留換行符號
        if (line.find("endmodule") != string::npos) {// 檢查是否遇到 "endmodule"
            currentModule.clear();
            insideModule = false;
        }
    }
    file.close();
    //===for debug===
    // for(int i=0;i<modules.size();i++){
    //     cout<<modules[i]<<"EOL"<<endl;
    // }
    //===============
    return modules;
}

//for initilaize_module_vec()
vector<vector<string>> parse_submodule(const string& filename){
    vector<vector<string>> results;
    vector<string> module_result=split_modules(filename);
    for(int i=0;i<module_result.size();i++){
        vector<string> module_detail=split_by_semicolon(module_result[i]);
        results.push_back(module_detail);
    }
    //===for debug===
    // for(int i=0;i<results.size();i++){
    //     for(int j=0;j<results[i].size();j++){
    //         cout<<results[i][j]<<endl;
    //     }
    // }
    //===============
    return results;
}

//for make_signal_list()
vector<string> extract_split_in_paren(const string& input){
    vector<string> result;
    size_t start = input.find('(');
    size_t end = input.find(')');
    if (start == string::npos || end == string::npos || start >= end) {// 檢查是否找到 '(' 和 ')'
        return result;  //返回空的vector
    }
    string inside = input.substr(start + 1, end - start - 1);//提取括號內的內容
    inside.erase(0, inside.find_first_not_of(" \t"));        //去除前後空格
    inside.erase(inside.find_last_not_of(" \t") + 1);

    size_t pos = 0;     //按','拆分
    while (true) {
        size_t comma_pos = inside.find(',', pos);
        string token = inside.substr(pos, comma_pos - pos);
        token.erase(0, token.find_first_not_of(" \t"));     //去除 token 前後空格
        token.erase(token.find_last_not_of(" \t") + 1);
        if (!token.empty()) {
            result.push_back(token);
        }
        if (comma_pos == string::npos) {
            break;
        }
        pos = comma_pos + 1;
    }
    return result;
}

//for make_signal()
vector<string> split_by_space(const string& input) {
    istringstream iss(input);
    vector<string> tokens;
    string token;
    while (iss >> token) {//使用 >> 自動跳過空格
        tokens.push_back(token);
    }
    return tokens;
}

//for make_signal_list()
Signal make_signal(string input){
    vector<string> results=split_by_space(input);
    Signal my_signal;
    my_signal.IO=results[0];
    if(my_signal.IO=="input")my_signal.comb_ptr=nullptr; 
    if(results.size()==3)my_signal.width=1;
    else my_signal.width=results[2][1]-'0';
    my_signal.name=results[results.size()-1];
    return my_signal;
}

//for initilaize_module_vec()
void make_signal_list(const string& input, list<Signal> &sig_list, unordered_map<string, Signal*> &sig_map){ 
    vector<string> results=extract_split_in_paren(input);
    for(int i=0;i<results.size();i++){
        Signal my_signal=make_signal(results[i]);
        sig_list.push_back(my_signal);
        sig_map[my_signal.name]=&(sig_list.back());
    }
}

//for make_module_logic()
void process_assign_line(const string& line,string &out_str,string &logic_str){
    string str = line;
    const string prefix = "assign";//去除開頭 "assign"（假設是固定開頭）
    size_t pos = str.find(prefix);
    if (pos != string::npos) {
        str = str.substr(pos + prefix.size());
    }
    size_t equal_pos = str.find('=');      //找等號
    if (equal_pos == string::npos)return;  //無等號，返回空 pair
    
    string lhs = str.substr(0, equal_pos); //切割左右
    string rhs = str.substr(equal_pos + 1);
    lhs.erase(remove(lhs.begin(), lhs.end(), ' '), lhs.end());//移除所有空格
    rhs.erase(remove(rhs.begin(), rhs.end(), ' '), rhs.end());
    if (!rhs.empty() && rhs.back() == ';'){//去除尾部的分號
        rhs.pop_back();
    }
    out_str=lhs;
    logic_str=rhs;
}

//for make_module_logic()
void make_AST(const string& expr,AST &ast){
    ast.root = Build_AST(expr, ast.lst);
}

//for initilaize_module_vec()
void make_module_logic(const string& input, Module &my_module){
    string out_str,logic_str;
    process_assign_line(input,out_str,logic_str);
    comb my_logic;                  //local變數，離開函數就死了，所以不能取他的address
    my_module.logic.push_back(my_logic);

    auto it=my_module.sig_map.find(out_str);
    my_module.logic.back().out_sig=it->second;//這樣才會call by reference
    my_module.logic.back().out_sig->comb_ptr=&my_module.logic.back();
    make_AST(logic_str,my_module.logic.back().logic_tree);
    //===for debug===
    //print_AST(my_module.logic.back().logic_tree.root,0);
    //===============
}

//for initilaize_module_vec()
void link_sig_and_AST(Module &my_module){  
    for(auto &it : my_module.logic){
        for (auto& jt :it.logic_tree.lst){ //加上&
            string sig_name = jt.Name_or_Op;
            auto map_it = my_module.sig_map.find(sig_name);
            if (map_it != my_module.sig_map.end()) {    //代表當前的sig_name是訊號(而非OP)
                map_it->second->AST_ptr.push_back(&jt); //建立dependency
            }
        }
    }
}

//for initialize_module()
void initilaize_module_vec(const string& filename, vector<Module> &module_vec){
    vector<vector<string>> results=parse_submodule(filename);
    module_vec.resize(results.size());  //這行很重要
    for(int i=0;i<module_vec.size();i++){
        make_signal_list(results[i][0],module_vec[i].sig_list, module_vec[i].sig_map);
        for(int j=1;j<results[i].size();j++){
            make_module_logic(results[i][j], module_vec[i]);
        }
        link_sig_and_AST(module_vec[i]);//建立dependency
    }
    //===for debug===
    // for(int i=0;i<module_vec.size();i++){
    //     cout<<"sig_map: "<<endl;
    //     for (const auto& pair :module_vec[i].sig_map){
    //         cout<<pair.first<<": ["<<pair.second->IO<<", "<<pair.second->width<<"]"<<endl;
    //     }
    //     cout<<"sig_list: "<<endl;
    //     for (const auto& value :module_vec[i].sig_list) {
    //         if(value.IO=="input"){
    //             cout<<value.name<<" AST_ptr.size():"<<value.AST_ptr.size()<<endl;
    //             for(int j=0;j<value.AST_ptr.size();j++){
    //                 cout<<value.AST_ptr[j]->Name_or_Op<<" ";
    //             }
    //             cout<<endl;
    //         }
    //         else{
    //             cout<<"comb_ptr:"<<endl;
    //             for(auto it:module_vec[i].logic){
    //                 cout<<"out_sig: "<<it.out_sig->name<<endl;
    //                 print_AST(it.logic_tree.root,0);
    //             }
    //         }
    //     }
    //     cout<<endl<<"logic: "<<endl;
    //     for(auto it:module_vec[i].logic){
    //         cout<<"out_sig: "<<it.out_sig->name<<" AST: "<<endl;
    //         print_AST(it.logic_tree.root,0);
    //     }
    // }
    //===============
}

//for initialize_module()
void make_class_map(unordered_map<string,sig_class>&map,Module &old_module,Module &new_module){
    for (auto& sig:old_module.sig_list){
        string key=sig.IO+to_string(sig.width);
        auto it=map.find(key);
        if(it==map.end()){
            sig_class my_class;
            my_class.IO=sig.IO; my_class.width=sig.width;
            my_class.old_sig.push_back(&sig);
            map.insert({key,my_class});
        }
        else{
            map[key].old_sig.push_back(&sig);
        }
    }
    for (auto& sig:new_module.sig_list){
        string key=sig.IO+to_string(sig.width);
        map[key].new_sig.push_back(&sig);
    }
}

void initialize_module(match_info &info,string sub_mod_name,string new_sub_mod_name){
    initilaize_module_vec(sub_mod_name, info.old_module);
    initilaize_module_vec(new_sub_mod_name, info.new_module);
    for(int i=0;i<info.old_module.size();i++){
        unordered_map<string,sig_class>map;
        make_class_map(map,info.old_module[i],info.new_module[i]);
        info.class_map.push_back(map);
    }
    //===for debug===
    // for(int i=0;i<info.class_map.size();i++){
    //     cout<<"class_map: "<<i<<endl;
    //     for (const auto& pair:info.class_map[i]) {
    //         cout<<"Key: "<<pair.first<<endl;
    //         cout<<"old_sig: ";
    //         for(int j=0;j<pair.second.old_sig.size();j++){
    //             cout<<pair.second.old_sig[j]->name<<" ";
    //         }
    //         cout<<endl<<"new_sig: ";
    //         for(int j=0;j<pair.second.old_sig.size();j++){
    //             cout<<pair.second.new_sig[j]->name<<" ";
    //         }
    //         cout<<endl;
    //     }
    // }
    //===============
}

//for make_all_sig_match()
vector<sig_match> match_one_class(const vector<Signal*>& old_sig,const vector<Signal*>& new_sig){
    vector<sig_match> results;
    if (new_sig.size() > old_sig.size()) return results;
    vector<int> indices(old_sig.size());
    iota(indices.begin(), indices.end(), 0);
    bool first = true;  //先做一次permutation結果處理
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

//for make_all_sig_match()
void dfs_permutate(const all_class_match& match_lists,
    size_t idx, sig_match& current, vector<sig_match>& all_solutions){
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

vector<sig_match> make_all_sig_match(const unordered_map<string,sig_class>& class_map){
    all_class_match match_lists;
    for (const auto& kv : class_map) {
        const auto& sc = kv.second;
        auto matches = match_one_class(sc.old_sig, sc.new_sig);
        match_lists.push_back(matches);
    }
    vector<vector<pair<Signal*, Signal*>>> all_solutions;
    vector<pair<Signal*, Signal*>> current;
    dfs_permutate(match_lists, 0, current, all_solutions);
    return all_solutions;
}

//for find_sig_match()
bool check_sig_match(sig_match match){
    vector<comb*> new_out_comb,old_out_comb;
    bool ans=false;
    for(int i=0;i<match.size();i++){ //拆分input和output
        Signal* new_sig=match[i].first; Signal* old_sig=match[i].second;
        if(new_sig->IO=="input"){
            for(int j=0;j<new_sig->AST_ptr.size();j++){
                new_sig->AST_ptr[j]->Name_or_Op=old_sig->name; //替換AST node中的訊號名稱
            }
        }
        else{
            new_out_comb.push_back(new_sig->comb_ptr); old_out_comb.push_back(old_sig->comb_ptr);
        }
    }
    for(int j=0;j<new_out_comb.size();j++){
        normalize_AST(old_out_comb[j]->logic_tree.root);
        normalize_AST(new_out_comb[j]->logic_tree.root);
        if(isSameAST(old_out_comb[j]->logic_tree.root, new_out_comb[j]->logic_tree.root)==true){
            ans=true;               //只要有一組output對不到，就算沒有對到
            break;
        }
    }
    for(int i=0;i<match.size();i++){ //把AST node中的訊號名稱改回來
        Signal* new_sig=match[i].first; Signal* old_sig=match[i].second;
        if(new_sig->IO=="input"){
            for(int j=0;j<new_sig->AST_ptr.size();j++){
                new_sig->AST_ptr[j]->Name_or_Op=new_sig->name; 
            }
        }
    }
    return ans;
}

void find_sig_match(match_info &info){
    //把sig_match的結果放入result
    for(int i=0;i<info.class_map.size();i++){
        vector<sig_match> possible_match=make_all_sig_match(info.class_map[i]);//目前正確
        //===for debug===
        // for(int i=0;i<possible_match.size();i++){
        //     cout<<"match "<<i<<endl;
        //     for(int j=0;j<possible_match[i].size();j++){
        //         cout<<"new: "<<possible_match[i][j].first->name<<" ";
        //         cout<<"old: "<<possible_match[i][j].second->name<<endl;
        //     }
        // }
        //===============
        for(int j=0;j<possible_match.size();j++){
            if(check_sig_match(possible_match[j])==true){ //一個module的結果是sig_match
                info.match_result.push_back(possible_match[j]);
                //===for debug===
                for(int i=0;i<info.match_result.size();i++){
                    for(int j=0;j<info.match_result[i].size();j++){
                        cout<<"new: "<<info.match_result[i][j].first->name<<" ";
                        cout<<"old: "<<info.match_result[i][j].second->name<<endl;
                    }
                }
                //===============
                break;
            }
        }
    }
    //===for debug===
    // for(int i=0;i<info.match_result.size();i++){
    //     for(int j=0;j<info.match_result[i].size();j++){
    //         cout<<"new: "<<info.match_result[i][j].first->name<<" ";
    //         cout<<"old: "<<info.match_result[i][j].second->name<<endl;
    //     }
    // }
    //===============
}

int main(int argc, char *argv[]){
    string top_mod_name= argv[1];
    string sub_mod_name= argv[2];
    string new_sub_mod_name=argv[3];
    string updt_info_name = argv[4];
    string new_top_mod_name=argv[5];

    match_info info;
    initialize_module(info,sub_mod_name,new_sub_mod_name);
    find_sig_match(info);
    return 0;
}
