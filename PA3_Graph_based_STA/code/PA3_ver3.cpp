#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <utility>
#include <unordered_map>
#include <vector>
#include "liberty_parser.h"
//完成dependency建構，有加入liberty file

using namespace std;
struct gate_node; 

struct wire_node{
    string name;
    vector<gate_node*> input_gate;//在此gate之前所有的gate
    vector<gate_node*> output_gate;
};

struct gate_node{
    string type;
    string name;
    int in_degree;
    vector<gate_node*> input_gate;//在此gate之前所有的gate
    vector<gate_node*> output_gate;
    double load_capacitance;
    double input_transition_time;
    double output_transition_time;
    double longest_delay;
    double shortest_delay;
    double arrival_time;    //已加上自身delay
    vector<wire_node*>input_wire;
    vector<wire_node*>output_wire;
};

struct RTL_obj{
    unordered_map<string,gate_node*>gate_map;//name->node 希望少用
    unordered_map<string,wire_node*>net_map;
    vector <string> input_net;
    vector <string> wire_net;
    vector <string> output_net;
};

//===以下是read RTL code的用來處理字串的function===
bool check_two_slash(const string line){
    size_t i = 0;
    while (i < line.size() && isspace(line[i])) {   //跳過開頭空白
        ++i;
    }
    return (i+1<line.size()&&line[i]=='/' && line[i+1]=='/');//檢查是否為
}

bool check_slash_star(const string line){
    size_t i = 0;
    while (i < line.size() && isspace(line[i])) {   //跳過開頭空白
        ++i;
    }
    return (i+1<line.size()&&line[i]=='/' && line[i+1]=='*');//檢查是否為 /*
}

bool endsWithBlockCommentEnd(string line){
    if (line.length()<2) return false;
    return line[line.length()- 2]=='*'&& line[line.length() - 1] == '/';
}

bool is_blank_line(const string line){
    for(char ch : line) {
        if(!isspace(static_cast<unsigned char>(ch))) {
            return false;   //發現非空白字元，代表不是空行
        }
    }
    return true;    //全部都是空白
}

void split_by_outer_parentheses(string line,string &part_1,string &part_2){
    size_t start = line.find('(');
    if (start == string::npos) return;
    int level = 0;
    size_t end = start;
    for (; end < line.size(); ++end) {
        if (line[end] == '(') ++level;
        else if (line[end] == ')') --level;
        if (level == 0 && line[end] == ')') {
            ++end; // include the closing ')'
            break;
        }
    }
    part_1=line.substr(0,start);
    part_2=line.substr(start,end - start); // 取得括號內含（含括號）
}

string remove_spaces_and_outer_parentheses(const string& input) {
    string no_space;
    for (char c : input) {              //1. 移除所有空白
        if (!isspace(static_cast<unsigned char>(c))) {
            no_space += c;
        }
    }
    size_t start = no_space.find('(');  //找到第一個 '(' 和與之配對的最外層 ')'
    if (start == string::npos) return no_space;  //沒有括號就原樣回傳
    int balance = 0;
    size_t end = string::npos;
    for (size_t i = start; i < no_space.size(); ++i) {
        if (no_space[i] == '(') ++balance;
        else if (no_space[i] == ')') {
            --balance;
            if (balance == 0) {
                end = i;
                break;
            }
        }
    }
    if (end != string::npos && end > start) {
        return no_space.substr(start + 1, end - start - 1);  //拿括號中間的內容
    } 
    else {
        return "";  //括號不完整時回傳空字串
    }
}

vector<string> split_by_delimiter(const string input, char delimiter) {
    vector<string> tokens;
    string token;
    istringstream iss(input);
    if (delimiter==' '){ //對空格進行特殊處理：跳過多餘空格
        while (iss >> token){
            tokens.push_back(token);
        }
    } 
    else{                // 一般分隔符，例如 ','
        while(getline(iss, token, delimiter)){
            if (!token.empty()){
                tokens.push_back(token);
            }
        }
    }
    return tokens;
}

void split_signal_and_wire(const string input,string &signal,string &wire){//把.A1(n11)拆成A1和n11
    size_t start = input.find('(');
    size_t end = input.find(')');
    if (start == string::npos || end == string::npos||start < 1 || end <= start + 1) {
        return; // 若格式錯誤，回傳空 pair
    }
    signal = input.substr(1, start - 1); // 忽略開頭的 '.'，取 signal 名稱
    wire = input.substr(start + 1, end - start - 1); //取括號內部的 wire 名稱
}

string get_first_token(string line){
    size_t i = 0;
    while (i < line.size() && isspace(line[i])){ //1.跳過開頭空白
        ++i;
    }
    size_t start = i;
    while (i < line.size() && !isspace(line[i])){//2.擷取第一個詞（直到遇到空白或行尾）
        ++i;
    }
    return line.substr(start, i - start);
}
//=======================================

vector<string> read_RTL(const string filename){    //切comment的部分我真的寫得蠻好的
    ifstream file(filename);
    vector<string> lines;
    string line;
    if (!file) {
        cout << "Failed to open file: " << filename << endl;
        return lines;
    }
    bool comment_mode=false;

    while (getline(file, line)){
        bool head_slash_star_bool=check_slash_star(line);
        bool end_star_slash_bool=endsWithBlockCommentEnd(line);
        
        if(head_slash_star_bool==true){
            comment_mode=true;
        }
    
        if(comment_mode==false&&check_two_slash(line)==false&&is_blank_line(line)==false){
            lines.push_back(line);
        }
        
        if(head_slash_star_bool==false&&end_star_slash_bool==true){
            comment_mode=false;
        }
        else if(head_slash_star_bool==true&&end_star_slash_bool==true){
            comment_mode=false;
        }
    }
    // for(int i=0;i<lines.size();i++){
    //     cout<<lines[i]<<endl;
    // }
    file.close();
    return lines;
}

vector<string> wire_parser(const string& line){//切割 wire n8, n9, n10, n11, n12;
    vector<string> result;
    istringstream iss(line);
    string token;
    while (iss >> token) {  //移除token中的所有逗號
        token.erase(remove(token.begin(), token.end(), ','), token.end());
        result.push_back(token);
    }
    //移除字串尾部的分號
    result[result.size()-1].erase(remove(result[result.size()-1].begin(),result[result.size()-1].end(),';'),result[result.size()-1].end());
    return result;
}

void parse_wire_and_bulid_map(string input, unordered_map<string,wire_node*>&wire_map, vector<string> &vec){
    vector<string> results=wire_parser(input);
    for(int i=1;i<results.size();i++){
        vec.push_back(results[i]);
    }
    for(int i=1;i<results.size();i++){
        wire_node* node= new wire_node;//要用動態記憶體配置
        node->name=results[i];
        wire_map[node->name]=node;
    }
}

void gate_parser(gate_node* node,vector<string>part_2_result, unordered_map<string,wire_node*> wire_map){
    for(int i=0;i<part_2_result.size();i++){
        string signal,wire;
        split_signal_and_wire(part_2_result[i],signal,wire);
        if(signal=="A1"||signal=="I"){
            node->input_wire[0]=wire_map[wire];//要查到再放進來
        }
        else if(signal=="A2"){
            node->input_wire[1]=wire_map[wire];//要查到再放進來
        }
        else if(signal=="ZN"){
            node->output_wire[0]=wire_map[wire];//要查到再放進來
        }
    }
}

void parse_gate_and_bulid_map(string input, unordered_map<string,gate_node*>&gate_map, unordered_map<string,wire_node*> wire_map){
    string part_1,part_2;
    split_by_outer_parentheses(input,part_1,part_2);
    part_2=remove_spaces_and_outer_parentheses(part_2);
    vector<string>part_1_result=split_by_delimiter(part_1,' ');
    string gate_name=part_1_result[1];

    gate_node* node=new gate_node;  //動態記憶體配置
    node->name=part_1_result[1];
    node->type=part_1_result[0];
    if(node->type=="INVX1"){
        node->input_wire.resize(1);
        node->in_degree=1;
    }
    else {
        node->input_wire.resize(2);
        node->in_degree=2;
    }

    node->output_wire.resize(1);
    vector<string>part_2_result=split_by_delimiter(part_2,',');
    gate_parser(node,part_2_result, wire_map);
    gate_map[node->name]=node;
}

void bulid_net_dependency(struct RTL_obj &RTL_object){
    for(auto it:RTL_object.gate_map){
        for(int i=0;i<it.second->input_wire.size();i++){
            it.second->input_wire[i]->output_gate.push_back(it.second);
        }
        it.second->output_wire[0]->input_gate.push_back(it.second);
    }
    //=====for debug=====
    // cout<<"Net Dependency: "<<endl;
    // for(auto it:RTL_object.net_map){
    //     cout<<"net_name: "<<it.second->name<<endl;
    //     cout<<"input_gate: "<<endl;
    //     for(int i=0;i<it.second->input_gate.size();i++){
    //         cout<<it.second->input_gate[i]->name<<endl;
    //     }
    //     cout<<"output_gate: "<<endl;
    //     for(int i=0;i<it.second->output_gate.size();i++){
    //         cout<<it.second->output_gate[i]->name<<endl;
    //     }
    // }
    //===================
}

void bulid_gate_dependency(struct RTL_obj &RTL_object){
    for(auto it:RTL_object.gate_map){
        for(int j=0;j<it.second->output_wire.size();j++){
            for(int k=0;k<it.second->output_wire[j]->output_gate.size();k++){
                it.second->output_gate.push_back(it.second->output_wire[j]->output_gate[k]);
            }
        }
        for(int i=0;i<it.second->input_wire.size();i++){
            for(int k=0;k<it.second->input_wire[i]->input_gate.size();k++){
                it.second->input_gate.push_back(it.second->input_wire[i]->input_gate[k]);
            }
        }
    }
    //=====for debug=====
    // cout<<"Gate Dependency: "<<endl;
    // for(auto it:RTL_object.gate_map){
    //     cout<<"gate_name: "<<it.second->name<<endl;
    //     cout<<"input_gate: "<<endl;
    //     for(int i=0;i<it.second->input_gate.size();i++){
    //         cout<<it.second->input_gate[i]->name<<endl;
    //     }
    //     cout<<"output_gate: "<<endl;
    //     for(int i=0;i<it.second->output_gate.size();i++){
    //         cout<<it.second->output_gate[i]->name<<endl;
    //     }
    // }
    //===================
}

void build_RTL_object(struct RTL_obj &RTL_object, vector<string> RTL_lines){
    for(int i=1;i<RTL_lines.size();i++){
        string first_token=get_first_token(RTL_lines[i]);
        if(first_token=="input"){
            parse_wire_and_bulid_map(RTL_lines[i], RTL_object.net_map, RTL_object.input_net);//建立unordered_map<string,wire_node*>wire_map，把node放進去
        }
        else if(first_token=="output"){
            parse_wire_and_bulid_map(RTL_lines[i], RTL_object.net_map, RTL_object.output_net);//建立unordered_map<string,wire_node*>wire_map，把node放進去
        }
        else if(first_token=="wire"){
            parse_wire_and_bulid_map(RTL_lines[i], RTL_object.net_map, RTL_object.wire_net);//建立unordered_map<string,wire_node*>wire_map，把node放進去
        }
        else if(first_token=="INVX1"||first_token=="NOR2X1"||first_token=="NANDX1"){
            parse_gate_and_bulid_map(RTL_lines[i], RTL_object.gate_map, RTL_object.net_map);
        }
    }
    bulid_net_dependency(RTL_object);
    bulid_gate_dependency(RTL_object);//建構完dependency之後可以變成vector加速遍歷
    //=====for debug=====
    // for(auto it:RTL_object.net_map){
    //     cout<<"net_name: "<<it.second->name<<endl;
    // }
    // for(auto it:RTL_object.gate_map){
    //     cout<<"gate_name: "<<it.second->name<<endl;
    //     cout<<"gate_type: "<<it.second->type<<endl;
    //     cout<<"input_wire: "<<endl;
    //     for(int i=0;i<it.second->input_wire.size();i++){
    //         cout<<it.second->input_wire[i]->name<<endl;
    //     }
    //     cout<<"output_wire: ";
    //     for(int i=0;i<it.second->output_wire.size();i++){
    //         cout<<it.second->output_wire[i]->name<<endl;
    //     }
    // }
    //=================
}

double find_input_capacitance(wire_node* net, struct liberty_file file){
    string net_name=net->name;
    double input_capacitance=0;
    if(net->output_gate.size()==0) return 0.03; //primary output net理論上不會貢獻電容，題目假設0.03
    for(int i=0;i<net->output_gate.size();i++){
        if(net->output_gate[i]->type=="INVX1"){
            input_capacitance+=file.INV.A1_capcitance;
        }
        else{
            if(net->output_gate[i]->input_wire[0]->name==net_name){
                if(net->output_gate[i]->type=="NANDX1")input_capacitance+=file.NAND.A1_capcitance;
                else input_capacitance+=file.NOR.A1_capcitance;
            }
            else if(net->output_gate[i]->input_wire[1]->name==net_name){
                if(net->output_gate[i]->type=="NANDX1")input_capacitance+=file.NAND.A2_capcitance;
                else input_capacitance+=file.NOR.A2_capcitance;
            }
        }
    }
    return input_capacitance;
}

void find_load_capacitance(struct RTL_obj &RTL_object, struct liberty_file file){
    for(auto it:RTL_object.gate_map){   //之後可以轉成vector，因為不需要用gate名稱去查找了
        gate_node* gate=it.second;
        //找到該gate所有的output wire連接到的gate的種類，以及是A1 or A2
        gate->load_capacitance=find_input_capacitance(gate->output_wire[0],file);
    }
    //=====for debug=====
    cout<<"Gate Dependency: "<<endl;
    for(auto it:RTL_object.gate_map){
        cout<<"gate_name: "<<it.second->name<<endl;
        cout<<"input_gate: "<<endl;
        for(int i=0;i<it.second->input_gate.size();i++){
            cout<<it.second->input_gate[i]->name<<endl;
        }
        cout<<"output_gate: "<<endl;
        for(int i=0;i<it.second->output_gate.size();i++){
            cout<<it.second->output_gate[i]->name<<endl;
        }
        cout<<"load_capcotance: "<<it.second->load_capacitance<<endl;
    }
    //===================
}

int main(){
    vector<string> RTL_lines=read_RTL("c17.v");
    struct RTL_obj RTL_object;
    build_RTL_object(RTL_object, RTL_lines);
    struct liberty_file file;
    build_liberty_file(file);
    //show_liberty_file(file);
    find_load_capacitance(RTL_object, file);
    return 0;
}