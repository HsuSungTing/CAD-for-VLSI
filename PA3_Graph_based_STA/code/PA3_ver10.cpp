#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <utility>
#include <unordered_map>
#include <vector>
#include <queue>
#include "liberty_parser.h"
#include "RTL_parser.h"
//完成dependency建構，有加入liberty file
//有拆檔的版本
//開始處理step 3，目前插值正確
//step 2完成

using namespace std;
struct gate_node; 

struct wire_node{
    string name;
    double start_time; //訊號到達該wire的時間，未加上自身delay
    double input_gate_transition_time; //會記錄input gate的transition_time(方便step 2查找)
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
    double propagation_delay;
    wire_node* latest_input_wire; //紀錄訊號較晚到達的input wire(為了step 3)
    double start_time;    //訊號到達gate的時間，未加上自身delay
    int output_situation; //1代表rise 0代表fall，只是為了step 2的輸出檔，本身沒意義
    vector<wire_node*>input_wire;
    vector<wire_node*>output_wire;
};

struct RTL_obj{
    unordered_map<string,gate_node*>gate_map;//name->node 希望少用
    unordered_map<string,wire_node*>net_map;
    vector<wire_node*> input_net;
    vector<wire_node*> wire_net;
    vector<wire_node*> output_net;
    vector<wire_node*> longest_path;    //最後要倒著輸出
    vector<wire_node*> shortest_path;    //最後要倒著輸出
};

vector<string> read_RTL(const string filename){    //切comment的部分我真的寫得蠻好的
    ifstream file(filename);
    vector<string> lines;
    string line;
    if (!file) {
        cout<<"Failed to open file: " << filename << endl;
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

void parse_wire_and_bulid_map(string input, unordered_map<string,wire_node*>&wire_map, vector<wire_node*> &vec){
    vector<string> results=wire_parser(input);

    for(int i=1;i<results.size();i++){
        wire_node* node= new wire_node;//要用動態記憶體配置
        node->name=results[i];
        node->start_time=0;
        node->input_gate_transition_time=0;
        wire_map[node->name]=node;
        vec.push_back(node);
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
    node->start_time=0;
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

void bulid_gate_dependency(struct RTL_obj &RTL_object, vector<gate_node*> &ready_gate){
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
        if(it.second->input_gate.size()==0){
            ready_gate.push_back(it.second);
        }
        it.second->in_degree=it.second->input_gate.size();
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

void build_RTL_object(struct RTL_obj &RTL_object, vector<string> RTL_lines, vector<gate_node*> &ready_gate){
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
    bulid_gate_dependency(RTL_object,ready_gate);//建構完dependency之後可以變成vector加速遍歷
    //===for debug===
    // cout<<"input net: "<<endl;
    // for(int i=0;i<RTL_object.input_net.size();i++){
    //     cout<<RTL_object.input_net[i]->name<<" ";
    // }
    // cout<<endl;
    // cout<<"output net: "<<endl;
    // for(int i=0;i<RTL_object.output_net.size();i++){
    //     cout<<RTL_object.output_net[i]->name<<" ";
    // }
    // cout<<endl;
    // cout<<"wire net: "<<endl;
    // for(int i=0;i<RTL_object.wire_net.size();i++){
    //     cout<<RTL_object.wire_net[i]->name<<" ";
    // }
    // cout<<endl;
    //===============
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
    //     cout<<"load_capcotance: "<<it.second->load_capacitance<<endl;
    //     cout<<"in_degree: "<<it.second->in_degree<<endl;
    // }
    //===================
}

void find_index_and_value(double input_transition,double load_capacitance,struct liberty_file file,int &var1_upper_idx,int &var1_lower_idx,int &var2_upper_idx,int &var2_lower_idx,double &var1_upper,double &var1_lower,double &var2_upper,double &var2_lower){
    // load_capacitance是 var1
    // input_transition是 var2
    if(load_capacitance<file.index_1[0]){
        var1_upper_idx=1; var1_lower_idx=0;
        var1_upper=file.index_1[1]; var1_lower=file.index_1[0];
    }
    else if(load_capacitance>file.index_1[6]){
        var1_upper_idx=6; var1_lower_idx=5;
        var1_upper=file.index_1[6]; var1_lower=file.index_1[5];
    }
    else{
        for(int i=1;i<file.index_1.size();i++){
            if(load_capacitance<=file.index_1[i]&&load_capacitance>=file.index_1[i-1]){
                var1_upper_idx=i; var1_lower_idx=i-1;
                var1_upper=file.index_1[i]; var1_lower=file.index_1[i-1];
            }    
        }
    }

    if(input_transition<file.index_2[0]){
        var2_upper_idx=1; var2_lower_idx=0;
        var2_upper=file.index_2[1]; var2_lower=file.index_2[0];
    }
    else if(input_transition>file.index_2[6]){
        var2_upper_idx=6; var2_lower_idx=5;
        var2_upper=file.index_2[6]; var2_lower=file.index_2[5];
    }
    else{
        for(int i=1;i<file.index_2.size();i++){
            if(input_transition<=file.index_2[i]&&input_transition>=file.index_2[i-1]){
                var2_upper_idx=i; var2_lower_idx=i-1;
                var2_upper=file.index_2[i]; var2_lower=file.index_2[i-1];
            }    
        }
    }
}

double bilinear_interpolation(double load_capacitance,double input_transition, double x0,double x1,double y0,double y1,double f_0_0,double f_1_0,double f_0_1,double f_1_1){
    //load_capacitance就是x，input_transition就是y
    double f_x_y0=f_0_0*(x1-load_capacitance)/(x1-x0)+f_1_0*(load_capacitance-x0)/(x1-x0);
    double f_x_y1=f_0_1*(x1-load_capacitance)/(x1-x0)+f_1_1*(load_capacitance-x0)/(x1-x0);
    return f_x_y0*(y1-input_transition)/(y1-y0)+f_x_y1*(input_transition-y0)/(y1-y0);
}

void find_cell_rise_and_fall(struct gate_info info, double input_transition,double load_capacitance,
    int var1_upper_idx,int  var1_lower_idx,int var2_upper_idx,int var2_lower_idx,double var1_upper,double var1_lower,double var2_upper,double var2_lower, double &cell_rise, double &cell_fall){
    double f_0_0, f_1_0, f_0_1, f_1_1;
    f_0_0=info.cell_rise_TB[var2_lower_idx][var1_lower_idx];
    f_1_0=info.cell_rise_TB[var2_lower_idx][var1_upper_idx];
    f_0_1=info.cell_rise_TB[var2_upper_idx][var1_lower_idx];
    f_1_1=info.cell_rise_TB[var2_upper_idx][var1_upper_idx];
    cell_rise=bilinear_interpolation(load_capacitance,input_transition, var1_lower,var1_upper, var2_lower,var2_upper,f_0_0,f_1_0,f_0_1,f_1_1);
        
    f_0_0=info.cell_fall_TB[var2_lower_idx][var1_lower_idx];
    f_1_0=info.cell_fall_TB[var2_lower_idx][var1_upper_idx];
    f_0_1=info.cell_fall_TB[var2_upper_idx][var1_lower_idx];
    f_1_1=info.cell_fall_TB[var2_upper_idx][var1_upper_idx];
    cell_fall=bilinear_interpolation(load_capacitance,input_transition, var1_lower,var1_upper, var2_lower,var2_upper,f_0_0,f_1_0,f_0_1,f_1_1);
}

void find_rise_and_fall_transition(struct gate_info info, double input_transition,double load_capacitance,
    int var1_upper_idx,int  var1_lower_idx,int var2_upper_idx,int var2_lower_idx,double var1_upper,double var1_lower,double var2_upper,double var2_lower, double &rise_transition, double &fall_transition){
    double f_0_0, f_1_0, f_0_1, f_1_1;
    f_0_0=info.rise_transision_TB[var2_lower_idx][var1_lower_idx];
    f_1_0=info.rise_transision_TB[var2_lower_idx][var1_upper_idx];
    f_0_1=info.rise_transision_TB[var2_upper_idx][var1_lower_idx];
    f_1_1=info.rise_transision_TB[var2_upper_idx][var1_upper_idx];
    rise_transition=bilinear_interpolation(load_capacitance,input_transition, var1_lower,var1_upper, var2_lower,var2_upper,f_0_0,f_1_0,f_0_1,f_1_1);
    
    f_0_0=info.fall_transision_TB[var2_lower_idx][var1_lower_idx];
    f_1_0=info.fall_transision_TB[var2_lower_idx][var1_upper_idx];
    f_0_1=info.fall_transision_TB[var2_upper_idx][var1_lower_idx];
    f_1_1=info.fall_transision_TB[var2_upper_idx][var1_upper_idx];
    fall_transition=bilinear_interpolation(load_capacitance,input_transition, var1_lower,var1_upper, var2_lower,var2_upper,f_0_0,f_1_0,f_0_1,f_1_1);
}

void find_result_from_TB(string gate_type,double input_transition,double load_capacitance,struct liberty_file file,double &cell_rise,double &cell_fall,double &rise_transition,double &fall_transition){
    int var1_upper_idx, var1_lower_idx,var2_upper_idx,var2_lower_idx;
    double var1_upper,var1_lower,var2_upper,var2_lower;
    find_index_and_value(input_transition,load_capacitance,file, var1_upper_idx,var1_lower_idx,var2_upper_idx,var2_lower_idx,var1_upper,var1_lower,var2_upper,var2_lower);
    //cout<<"var1_upper: "<<var1_upper<<" var1_lower: "<<var1_lower<<" var2_upper: "<<var2_upper<<" var2_lower: "<<var2_lower<<endl;

    if(gate_type=="INVX1"){
        find_cell_rise_and_fall(file.INV,input_transition,load_capacitance,var1_upper_idx,var1_lower_idx,var2_upper_idx,var2_lower_idx,var1_upper,var1_lower,var2_upper,var2_lower,cell_rise, cell_fall);
        find_rise_and_fall_transition(file.INV,input_transition,load_capacitance,var1_upper_idx,var1_lower_idx,var2_upper_idx,var2_lower_idx,var1_upper,var1_lower,var2_upper,var2_lower,rise_transition,fall_transition);
    }
    else if(gate_type=="NANDX1"){
        find_cell_rise_and_fall(file.NAND,input_transition,load_capacitance,var1_upper_idx,var1_lower_idx,var2_upper_idx,var2_lower_idx,var1_upper,var1_lower,var2_upper,var2_lower,cell_rise, cell_fall);
        find_rise_and_fall_transition(file.NAND,input_transition,load_capacitance,var1_upper_idx,var1_lower_idx,var2_upper_idx,var2_lower_idx,var1_upper,var1_lower,var2_upper,var2_lower,rise_transition,fall_transition);
    }
    else if(gate_type=="NOR2X1"){
        find_cell_rise_and_fall(file.NOR,input_transition,load_capacitance,var1_upper_idx,var1_lower_idx,var2_upper_idx,var2_lower_idx,var1_upper,var1_lower,var2_upper,var2_lower,cell_rise, cell_fall);
        find_rise_and_fall_transition(file.NOR,input_transition,load_capacitance,var1_upper_idx,var1_lower_idx,var2_upper_idx,var2_lower_idx,var1_upper,var1_lower,var2_upper,var2_lower,rise_transition,fall_transition);
    }
}

void update_start_time_of_output_wire(gate_node* node){//更新gate的output wire
    for(int i=0;i<node->output_wire.size();i++){
        node->output_wire[i]->input_gate_transition_time=node->output_transition_time;
        node->output_wire[i]->start_time=node->start_time+node->propagation_delay;
    }
}

double find_input_transition_time_and_update_gate_start_time(gate_node* node){
    if(node->input_gate.size()==0){
        node->latest_input_wire=node->input_wire[0];//對於primary input gate，取A1當latest_input_wire
        node->start_time=0;
        return 0;
    }
    else if(node->type=="INVX1"){
        node->latest_input_wire=node->input_wire[0];
        node->start_time=node->input_wire[0]->start_time+0.005;
        return node->input_wire[0]->input_gate_transition_time;
    }
    else{
        if(node->input_wire[0]->start_time+0.005 > node->input_wire[1]->start_time+0.005){
            node->latest_input_wire=node->input_wire[0];
            node->start_time=node->input_wire[0]->start_time+0.005; //取較晚到達的wire訊號作為start time
            return node->input_wire[0]->input_gate_transition_time;
        }
        else{
            node->latest_input_wire=node->input_wire[1];
            node->start_time=node->input_wire[1]->start_time+0.005; //取較晚到達的wire訊號作為start time
            return node->input_wire[1]->input_gate_transition_time;
        }
    }
}

void find_output_transition_and_delay(gate_node* node,struct liberty_file file){
    double input_transition=find_input_transition_time_and_update_gate_start_time(node);
    double load_capacitance=node->load_capacitance;
    string gate_type=node->type;
    double cell_rise,cell_fall,rise_transition,fall_transition;
    find_result_from_TB(gate_type,input_transition,load_capacitance,file,cell_rise,cell_fall,rise_transition,fall_transition);
    //===for debug===
    //cout<<"gate name: "<<node->name<<" input_transition: "<<input_transition<<" load_capacitance: "<<load_capacitance<<endl;
    //cout<<"gate name: "<<node->name<<" cell_rise: "<<cell_rise<<" cell_fall: "<<cell_fall<<" rise_transition: "<<rise_transition<<" fall_transition: "<<fall_transition<<endl;
    //===============
    if(cell_rise>cell_fall){
        node->propagation_delay=cell_rise;
        node->output_situation=1;
        node->output_transition_time=rise_transition;
    }
    else{
        node->propagation_delay=cell_fall;
        node->output_situation=0;
        node->output_transition_time=fall_transition;
    }
    update_start_time_of_output_wire(node);
    //===for debug===
    cout<<"gate name: "<<node->name<<" propagation_delay: "<<node->propagation_delay<<" coutput_transition_time: "<<node->output_transition_time<<" output_situation: "<<node->output_situation<<" start_time: "<<node->start_time<<endl;
    cout<<"output wire: "<<endl;
    for(int j=0;j<node->output_wire.size();j++){
        cout<<"wire name: "<<node->output_wire[j]->name<<" input_gate_transition_time: "<<node->output_wire[j]->input_gate_transition_time<<" start time: "<<node->output_wire[j]->start_time<<endl;
    }
    cout<<endl;
    //===============
}

void topoligical_ordering(struct RTL_obj &RTL_object, vector<gate_node*> &ready_gate, struct liberty_file file){
    queue<gate_node*>gate_queue;
    for(int i=0;i<ready_gate.size();i++){
        gate_queue.push(ready_gate[i]);
    }

    //接下來就是正常步驟，檢查該node的所有output_gate，其indegree--，如果是0就放入queue
    while(gate_queue.empty()==false){
        gate_node* gate=gate_queue.front();
        gate_queue.pop();
        find_output_transition_and_delay(gate,file);
        for(int i=0;i<gate->output_gate.size();i++){
            gate->output_gate[i]->in_degree--;
            if(gate->output_gate[i]->in_degree==0){
                gate_queue.push(gate->output_gate[i]);
            }
        }
    }
}

void find_path(wire_node* out_wire,vector<wire_node*> &path){
    wire_node* cur_wire=out_wire;
    while(cur_wire->input_gate.size()!=0){
        //cout<<"cur_wire: "<<cur_wire->name<<" ";
        path.push_back(cur_wire);
        cur_wire=cur_wire->input_gate[0]->latest_input_wire;
    }
    //cout<<"cur_wire: "<<cur_wire->name<<" ";
    path.push_back(cur_wire);
    cout<<endl;
}

void find_longest_and_shortest_path(struct RTL_obj &RTL_object){
    vector<wire_node*>out_wire=RTL_object.output_net;
    wire_node* latest_arrived_net=out_wire[0];
    wire_node* earliest_arrived_net=out_wire[0];
    for(int i=1;i<out_wire.size();i++){
        if(latest_arrived_net->start_time < out_wire[i]->start_time){
            latest_arrived_net= out_wire[i];
        }
        if(earliest_arrived_net->start_time > out_wire[i]->start_time){
            earliest_arrived_net= out_wire[i];
        }
    }
    //===for debug===
    cout<<"latest_arrived_net: "<<latest_arrived_net->name<<" delay: "<<latest_arrived_net->start_time<<endl<<" path: ";
    find_path(latest_arrived_net,RTL_object.longest_path);
    for(int i=RTL_object.longest_path.size()-1;i>=0;i--){
        cout<<RTL_object.longest_path[i]->name<<" ";
    }
    cout<<endl;

    cout<<"earliest_arrived_net: "<<earliest_arrived_net->name<<" delay: "<<earliest_arrived_net->start_time<<endl<<" path: ";
    find_path(earliest_arrived_net,RTL_object.shortest_path);
    for(int i=RTL_object.shortest_path.size()-1;i>=0;i--){
        cout<<RTL_object.shortest_path[i]->name<<" ";
    }
    cout<<endl;
    //================
}

int main(){
    vector<string> RTL_lines=read_RTL("c17.v");
    struct RTL_obj RTL_object;
    vector<gate_node*> ready_gate;  //紀錄indegree=0的gate
    vector<gate_node*> out_gate;    //紀錄outdegree=0的gate
    build_RTL_object(RTL_object, RTL_lines, ready_gate);
    struct liberty_file file;
    build_liberty_file(file);
    //show_liberty_file(file);
    find_load_capacitance(RTL_object, file);
    topoligical_ordering(RTL_object, ready_gate, file);
    find_longest_and_shortest_path(RTL_object);

    return 0;
}