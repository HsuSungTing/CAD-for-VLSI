#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <unordered_set>
#include <algorithm>
#include <unordered_map>
#include <climits>
#include <cmath> //包含abs()函數

//這個版本是尚未使用bucket list優化的版本，但其實不用太認真，反正最後一定會改(真的...)，這邊只是試寫而已
using namespace std;

struct cell_node{
    string cell_name;
    vector<int> net_id_vector; //紀錄一個cell擁有的net_id，不用有順序(這邊可以改成map)
};

struct gain_node{   //計算移動每個cell後可以得到的cut_size、目前是否可以被移動、目前的移動方式是否合法
    bool locked_bool;
    int gain;       //該cell前的cutsize-該cell後的cutsize，越大表示下降越多
    int cutsize;    //移動該cell後的cutsize
    bool met_constraint_bool;
};

string remove_braces(const string& str) {//去除'{' 和 '}'
    string result;
    for (char ch : str) {
        if (ch != '{' && ch != '}') {
            result += ch;  
        }
    }
    return result;
}

vector<string> split_string(const string str){     //以空格切割字串後回傳
    vector<string> result;
    istringstream iss(str);
    string word;
    while (iss >> word) { // 以空格分割
        result.push_back(word);
    }
    return result;
}

int string_to_Int(const string str) {
    string numberPart = str.substr(1);
    return stoi(numberPart);// 字串轉整數
}

void construct_cell_node(string cell_name,string net_name,unordered_map<int, struct cell_node> &cell_set){
    //先尋找這個cell_id是否有出現過
    int cell_id=string_to_Int(cell_name);
    int net_id=string_to_Int(net_name);
    auto it = cell_set.find(cell_id);

    if (it == cell_set.end()){     //沒有出現過，就先初始化該cell的node插入
        struct cell_node my_cell_node;
        my_cell_node.cell_name=cell_name;
        my_cell_node.net_id_vector.push_back(net_id);
        cell_set[cell_id]=my_cell_node;
    }
    else{ //有出現過
        it->second.net_id_vector.push_back(net_id);
    }
}


void construct_net_lines_to_cell_set (vector<string> net_lines, unordered_map<int, struct cell_node>&cell_set){
    for(int i=0;i<net_lines.size();i++){
        vector<string> result=split_string(net_lines[i]);

        for(int i=2; i<result.size(); i++){
            if(i==2 || i==result.size()-1){     //先把 { 和 } 去除
                result[i]=remove_braces(result[i]);
            }
            construct_cell_node(result[i],result[1],cell_set);
        }
        
    }
}


void read_file(string input_file_name, unordered_map<int, struct cell_node>&cell_set){ //讀取檔案並將每一行存入 vector<string>
    vector<string> net_lines;     
    ifstream file(input_file_name);

    if(!file){
        cerr<<"can't open file: "<<input_file_name<<endl;
        return ;
    }

    string line;
    while(getline(file, line)){ //使用 getline()以行為單位讀取
        net_lines.push_back(line);
    }
    file.close();
    construct_net_lines_to_cell_set (net_lines,cell_set);
}

void show_cell_set(unordered_map<int, struct cell_node> cell_set){
    for (const auto& it : cell_set) {
        cout<<"cell id:"<<it.first <<" ";
        cout<<"cell name: "<< it.second.cell_name << endl;
        cout<<"all the related nets: ";
        for(int i=0;i<it.second.net_id_vector.size();i++){
            cout<<it.second.net_id_vector[i]<<" ";
        }
        cout<<endl;
    }
}

unordered_map<int,string> initialize_partition(unordered_map<int, struct cell_node> cell_set){
    int count=0;
    unordered_map<int,string> initial_partition;
    for (const auto& it : cell_set) {
        if(count%2==0)initial_partition[it.first]="A";
        else initial_partition[it.first]="B";
        count++;
    }
    return initial_partition;
}

int count_common_elements(unordered_set<int> all_net_in_part_A, unordered_set<int> all_net_in_part_B){
    int count = 0;
    for (const auto& elem : all_net_in_part_A) {// 遍歷 setA，檢查每個元素是否在 setB 中存在
        if (all_net_in_part_B.find(elem) != all_net_in_part_B.end()) {
            count++;  // 如果元素存在於 setB 中，計數增加
        }
    }
    return count;
}

int calculate_cutsize(unordered_map<int,struct cell_node>cell_set, unordered_map<int,string> patition_result){
    unordered_set<int> all_net_in_part_A; //以net_id的形式儲存
    unordered_set<int> all_net_in_part_B; //以net_id的形式儲存
    for (const auto it : patition_result) { //分別統計在Part A有出現過的net以及在Part B有出現過的net
        int cell_id=it.first;
        string category=it.second;
        auto iterator = cell_set.find(cell_id);
        if(category=="A"){
            for(int i=0;i<iterator->second.net_id_vector.size();i++){
                all_net_in_part_A.insert(iterator->second.net_id_vector[i]);
            }
        }
        else{
            for(int i=0;i<iterator->second.net_id_vector.size();i++){
                all_net_in_part_B.insert(iterator->second.net_id_vector[i]);
            }
        }
    }
    return count_common_elements(all_net_in_part_A, all_net_in_part_B);
}

void test_function(unordered_map<int,struct cell_node>cell_set){
    unordered_map<int,string> patition_result=initialize_partition(cell_set);
    for(const auto it : patition_result){
        cout<<"cell id: "<<it.first<<" ->"<<it.second<<endl;
    }
    
    cout<<calculate_cutsize(cell_set,patition_result);
}

void show_patition_result_and_cutsize(unordered_map<int,struct cell_node>cell_set,unordered_map<int,string> patition_result){
    for(const auto it : patition_result){
        cout<<"cell id: "<<it.first<<" ->"<<it.second<<endl;
    }
    cout<<"cutsize: "<<calculate_cutsize(cell_set,patition_result)<<endl;
}

bool check_constraint_of_partition(unordered_map<int,string> patition_result){
    int size_A=0,size_B=0;
    for(const auto it : patition_result){
        if(it.second=="A")size_A++;
        else size_B++;
    }
    if(abs(size_A-size_B)>(size_A+size_B)/5)return false;
    else return true;
}

void move_specific_net_in_partition(unordered_map<int,string>&patition_result,int cell_id_to_be_moved){
    if(patition_result[cell_id_to_be_moved]=="A")patition_result[cell_id_to_be_moved]="B";
    else patition_result[cell_id_to_be_moved]="A";
}

struct gain_node construct_gate_node(unordered_map<int,struct cell_node>cell_set,unordered_map<int,string> &patition_result,int cell_id_to_be_moved){
    struct gain_node my_gain_node;
    int cutsize_before_move_the_cell=calculate_cutsize(cell_set, patition_result);
    //先把該net_id移動過去
    move_specific_net_in_partition(patition_result,cell_id_to_be_moved);

    my_gain_node.met_constraint_bool=check_constraint_of_partition(patition_result);  
    my_gain_node.cutsize=calculate_cutsize(cell_set, patition_result);
    my_gain_node.gain=cutsize_before_move_the_cell-my_gain_node.cutsize;

    //再把該net_id移動回來
    move_specific_net_in_partition(patition_result,cell_id_to_be_moved);
    return my_gain_node;
}

void initialize_gain_map(unordered_map<int,struct gain_node>&gain_map, unordered_map<int,struct cell_node>cell_set, unordered_map<int,string> patition_result){
    //因為初始化所以要全算。
    for(auto it:patition_result){
        struct gain_node my_gain_node;
        int cell_id_to_be_moved=it.first;
        my_gain_node=construct_gate_node(cell_set,patition_result,cell_id_to_be_moved);
        my_gain_node.locked_bool=false;  //外層迴圈初始化時都沒有上鎖
        gain_map[cell_id_to_be_moved]=my_gain_node;     //完成該gate_node
    }
}

void revise_gain_map(int cell_id_moved,unordered_map<int,struct gain_node>&gain_map, unordered_map<int,struct cell_node>cell_set, unordered_map<int,string> patition_result){
    
    //只要cell被鎖定之後就不用算
    for(auto it:gain_map){
        struct gain_node my_gain_node;
        int cell_id_to_be_moved=it.first;
        if(it.second.locked_bool==false){       //要尚未被鎖上才能更新
            my_gain_node=construct_gate_node(cell_set,patition_result,cell_id_to_be_moved);
            if(cell_id_moved==cell_id_to_be_moved){
                my_gain_node.locked_bool=true; //如果是已經確定在這一輪內部迴圈被移動過的就上鎖
            }
            gain_map[cell_id_to_be_moved]=my_gain_node;     //修改該gate_node
        }
    }
}

bool check_if_positive_and_valid_gain_exist(unordered_map<int,struct gain_node> gain_map){
    for(auto it:gain_map){
        if(it.second.gain>0&&it.second.met_constraint_bool==true)return true;
    }
    return false;
}

void show_gain_map(unordered_map<int,struct gain_node> gain_map){
    for(auto it:gain_map){
        cout<<"cell id: "<<it.first<<endl;
        cout<<"gain:"<<it.second.gain<<" cutsize: "<<it.second.cutsize<<" locked_bool: "<<it.second.locked_bool<<" met_constraint_bool: "<<it.second.met_constraint_bool<<endl;
    }
}

int find_the_unlocked_max_gain_cell_under_constraint(unordered_map<int,struct gain_node> gain_map){
    int cell_id=-1;
    int cur_best_gain=INT_MIN;
    for(auto it:gain_map){
        if(it.second.locked_bool==false&&it.second.met_constraint_bool==true){  //沒有鎖上且合法cell的才算
            if(it.second.gain>cur_best_gain){
                cur_best_gain=it.second.gain;
                cell_id=it.first;
            }
        }
    }
    return cell_id;
}

void update_partition(int cell_id_to_be_moved, unordered_map<int,string> &patition_result){   //把gain_map中該cell_id鎖上並更新
    if(patition_result[cell_id_to_be_moved]=="A")patition_result[cell_id_to_be_moved]="B";
    else patition_result[cell_id_to_be_moved]="A";
}

void Fiduccia_Mattheyses_algorithm(unordered_map<int,struct cell_node>cell_set){
    unordered_map<int,string> best_patition=initialize_partition(cell_set);  //只存best
    unordered_map<int,string> patition_result=initialize_partition(cell_set);

    int cur_best_cutsize=calculate_cutsize(cell_set, patition_result);
    unordered_map<int,struct gain_node> gain_map;
    //initialize_gain_map(gain_map,cell_set, patition_result);  
    //show_gain_map(gain_map);
    while(1){
        //根據當前的best_partition初始化gain_map (所有的cell都還沒被鎖上)
        initialize_gain_map(gain_map,cell_set, patition_result);  
        show_gain_map(gain_map);
        if(check_if_positive_and_valid_gain_exist(gain_map)==false) break;//先檢查一次，是否移動任何cell可得gain>0，且該移動為合法，如果沒有就break
        while(1){
            //根據當前的gain_map找出gain最大且合法且尚未移動過的cell(回傳id即可)

            int cell_id_to_be_moved=find_the_unlocked_max_gain_cell_under_constraint(gain_map);
            if(cell_id_to_be_moved==-1)break;  //當前全部cell都被鎖上 or 不合法，就break
            else{                              //如果該cell id存在      
                update_partition(cell_id_to_be_moved, patition_result);                   //1.更新partition
                revise_gain_map(cell_id_to_be_moved,gain_map, cell_set, patition_result); //2.gain_map中該cell_id鎖上並更新gain_map(revise那個)
                int cur_cutsize=calculate_cutsize(cell_set,patition_result);
                if(cur_cutsize<cur_best_cutsize){                   //3.如果有best_cutsize，紀錄成best_partition(patition_result)
                    cur_best_cutsize=cur_cutsize;
                    best_patition=patition_result;
                }
            }
        }
        //把patition_result回到best_partition
        patition_result=best_patition;
    }
    show_patition_result_and_cutsize(cell_set,best_patition);
}

int main(){
    unordered_map<int, struct cell_node> cell_set;  //以cell為單位，記錄所有cell的id以及所有相關的net
    //會用unordered_map的原因是會需要反覆以cell name查找，且不保證cell name有序
    read_file("input.txt",cell_set);
    //show_cell_set(cell_set);
    //test_function(cell_set);
    Fiduccia_Mattheyses_algorithm(cell_set);
    return 0;
}
