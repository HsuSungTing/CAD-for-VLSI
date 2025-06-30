#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <unordered_set>
#include <algorithm>
#include <unordered_map>
#include <climits>

//這個版本有必要的資料結構和function，但還沒開始FM
using namespace std;

struct cell_node{
    string cell_name;
    vector<int> net_id_vector; //紀錄一個cell擁有的net_id，不用有順序
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
//增加一個check size(A)-size(B)的部分

// void Fiduccia_Mattheyses_algorithm(){
//     //會用到initialize_partition和calculate_cutsize
// }

int main(){
    unordered_map<int, struct cell_node> cell_set;  //以cell為單位，記錄所有cell的id以及所有相關的net
    //會用unordered_map的原因是會需要反覆以cell name查找，且不保證cell name有序
    read_file("input.txt",cell_set);
    show_cell_set(cell_set);
    //test_function(cell_set);
    return 0;
}
