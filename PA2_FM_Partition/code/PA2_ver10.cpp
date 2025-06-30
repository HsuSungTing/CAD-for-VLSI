#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <random>
#include <ctime>
#include <sstream>
#include <unordered_set>
#include <algorithm>
#include <unordered_set>
#include <unordered_map>
#include <climits>
#include <cmath> //包含abs()函數

//cell_node改用unordered_set，並更新parser邏輯
using namespace std;

struct cell_node{
    string cell_name;
    unordered_set<int> net_id_set; //紀錄一個cell擁有的net_id，不用有順序(這邊可以改成map)
};

//-------------------------------------------

struct cell_node_of_bucket_list{
	int gain;   //雖然從bucket list可以直接拿，但還是紀錄一下
	int cell_id;
	struct cell_node_of_bucket_list *ptr;
};

struct bucket_list_node{
	int gain;	//中間等於0
	bool locked_bool;
	struct cell_node_of_bucket_list *ptr;
};

struct bucket_list{
	struct bucket_list_node* max_gain_node_ptr;
	vector <struct bucket_list_node*> bucket_list;
};

struct bucket_list_class{
    struct bucket_list  bucket_list_part_A;
    unordered_map<int,struct cell_node_of_bucket_list*> cell_id_to_node_map_part_A;

    struct bucket_list  bucket_list_part_B;
    unordered_map<int,struct cell_node_of_bucket_list*> cell_id_to_node_map_part_B;
};
//--------------------------------------------

string remove_braces(const string &str) {//去除'{' 和 '}'
    string result;
    for (char ch : str) {
        if (ch != '{' && ch != '}') {
            result += ch;  
        }
    }
    return result;
}

vector<string> split_string(const string str){
    stringstream ss(str);
    string word;
    vector<string> words;

    // 使用空格分割字串
    while (ss >> word) {
        words.push_back(word);  // 將每個單詞存入 vector
    }

    return words;
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
        my_cell_node.net_id_set.insert(net_id);
        cell_set[cell_id]=my_cell_node;
    }
    else{ //有出現過
        it->second.net_id_set.insert(net_id);
    }
}


void construct_net_lines_to_cell_set(vector<string> net_lines, unordered_map<int, struct cell_node>&cell_set){
    for(int i=0;i<net_lines.size();i++){
        net_lines[i]=remove_braces(net_lines[i]);
        vector<string> result=split_string(net_lines[i]);
        
        for(int i=2; i<result.size(); i++){
            if((i==2 || i==result.size()-1)){     //先把 { 和 } 去除
                result[i]=remove_braces(result[i]);
            }
            if((result[i]!="{"||result[i]!="}"))
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
        for(auto related_nets: it.second.net_id_set){
            cout<<related_nets<<" ";
        }
        cout<<endl;
    }
}

bool check_constraint_of_partition(unordered_map<int,string> patition_result){
    int size_A=0,size_B=0;
    for(const auto it : patition_result){
        if(it.second=="A")size_A++;
        else size_B++;
    }
    if(abs(size_A-size_B)>(size_A+size_B)/4)return false;
    else return true;
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

void move_random_cell_to_meet_constraint(vector<int> cell_in_part_A,vector<int>cell_in_part_B, unordered_map<int,string> &patition_result) {
    int size_A=cell_in_part_A.size();
    int size_B=cell_in_part_B.size();
    int constraint=(size_A+size_B)/5;

    // 初始化隨機數生成器
    random_device rd;
    mt19937 rng(rd());

    int move_index=0;
    if(size_A-size_B>constraint){
        //把size_A-size_B-constraint個cell從A移動到B
        shuffle(cell_in_part_A.begin(), cell_in_part_A.end(), rng);
        while(size_A-size_B>constraint){
            patition_result[cell_in_part_A[move_index]]="B";
            move_index++;
            size_A--;
            size_B++;
        }
    }
    else if(size_B-size_A>constraint){
        //把size_B-size_A-constraint個cell從B移動到A
        shuffle(cell_in_part_B.begin(), cell_in_part_B.end(), rng);
        while(size_B-size_A>constraint){
            patition_result[cell_in_part_B[move_index]]="A";
            move_index++;
            size_B--;
            size_A++;
        }
    }
}

void force_partition_to_meet_constraint(unordered_map<int,string> &patition_result){
    vector<int> cell_in_part_A;
    vector<int> cell_in_part_B;

    for(const auto it : patition_result){
        if(it.second=="A")cell_in_part_A.push_back(it.first);
        else cell_in_part_B.push_back(it.first);
    }
    move_random_cell_to_meet_constraint(cell_in_part_A,cell_in_part_B, patition_result);
}

int calculate_cutsize(unordered_map<int,struct cell_node>cell_set, unordered_map<int,string> patition_result){
    unordered_set<int> all_net_in_part_A;   //以net_id的形式儲存
    unordered_set<int> all_net_in_part_B;   //以net_id的形式儲存
    for (const auto it : patition_result) { //分別統計在Part A有出現過的net以及在Part B有出現過的net
        int cell_id=it.first;
        string category=it.second;
        auto iterator = cell_set.find(cell_id);
        if(category=="A"){
            for(auto related_nets_id: iterator->second.net_id_set){
                all_net_in_part_A.insert(related_nets_id);
            }
        }
        else{
            for(auto related_nets_id: iterator->second.net_id_set){
                all_net_in_part_B.insert(related_nets_id);
            }
        }
    }
    return count_common_elements(all_net_in_part_A, all_net_in_part_B);
}


void show_patition_result_and_cutsize(unordered_map<int,struct cell_node>cell_set,unordered_map<int,string> patition_result){
    for(const auto it : patition_result){
        cout<<"cell id: "<<it.first<<" ->"<<it.second<<endl;
    }
    cout<<"cutsize: "<<calculate_cutsize(cell_set,patition_result)<<endl;
}

int get_random_cell_id(unordered_map<int,struct cell_node>cell_set){
    // Create a vector of map elements (key-value pairs)
    vector<pair<int,struct cell_node>> cell_set_vec(cell_set.begin(), cell_set.end());
    
    // Seed random number generator with current time
    srand(time(0));
    
    // Generate a random index
    int randomIndex = rand() % cell_set_vec.size();
    
    // Return the random element
    return cell_set_vec[randomIndex].first;
}

// 對Partition做Simulated Annealing
void initialize_with_SA(unordered_map<int,struct cell_node>cell_set, unordered_map<int,string> &patition_result, double initialTemp, double coolingRate, int iterations){
    int cur_bestCutSize = calculate_cutsize(cell_set,patition_result);

    unordered_map<int,string> cur_bestPartition = patition_result;
    double temp = initialTemp;

    srand(time(0));
    for(int i=0;i<100;i++){
        for (int iter = 0; iter < iterations; ++iter) {
            // 選擇一個節點進行翻轉，以當前最好的cur_bestPartition為基礎去調整
            patition_result=cur_bestPartition;
            int cell_index=get_random_cell_id(cell_set);
            
            if(patition_result[cell_index]=="A")patition_result[cell_index]="B";
            else patition_result[cell_index]="A";

            //移動完後
            bool cur_solution_valid_bool=check_constraint_of_partition(patition_result);

            // 計算新的 cut-size
            int newCutSize = calculate_cutsize(cell_set,patition_result);

            // 計算接受機率
            int delta = newCutSize - cur_bestCutSize;
            if((delta < 0 || exp(-delta / temp) > ((double)rand() / RAND_MAX))) {
                // 接受新解(更新)
                cur_bestCutSize = newCutSize;
                cur_bestPartition = patition_result;
            }
            else{
                // 恢復舊解，且cur_bestPartition和cur_bestCutSize不變
                if(patition_result[cell_index]=="B")patition_result[cell_index]="A";
                else patition_result[cell_index]="B";
            }
            // 降溫
            temp *= coolingRate;
        }
        patition_result=cur_bestPartition;
        force_partition_to_meet_constraint(patition_result);
    }
}

//======================================================
void find_max_num_of_net(unordered_map<int,struct cell_node>cell_set, int &part_A_max_gain,int &part_B_max_gain, unordered_map<int,string> patition_result){
    part_A_max_gain=0;
    part_B_max_gain=0;

    for(const auto it : patition_result){
        int cell_id=it.first;
        int net_num=cell_set[cell_id].net_id_set.size();//查看該cell共有多少net

        if(it.second=="A"&&part_A_max_gain<net_num)part_A_max_gain=net_num;
        else if(it.second=="B"&&part_B_max_gain<net_num)part_B_max_gain=net_num;
    }
}

struct bucket_list_class initialize_bucket_list(unordered_map<int,struct cell_node>cell_set,unordered_map<int,string> patition_result){
    struct bucket_list_class my_bucket_list_class;
    
}
//======================================================

void test_function(unordered_map<int,struct cell_node>cell_set){
    unordered_map<int,string> patition_result=initialize_partition(cell_set);
    double initialTemp = 1000;
    double coolingRate = 0.95;
    int iterations = 1000;
    
    cout<<"initial cutsize:"<<calculate_cutsize(cell_set,patition_result);
    initialize_with_SA(cell_set, patition_result,initialTemp,  coolingRate,iterations);
    
    for(const auto it : patition_result){
        cout<<"cell id: "<<it.first<<" ->"<<it.second<<endl;
    }
    
    cout<<"cutsize after SA:"<<calculate_cutsize(cell_set,patition_result);
}

int main(){
    unordered_map<int, struct cell_node> cell_set;  //以cell為單位，記錄所有cell的id以及所有相關的net
    //會用unordered_map的原因是會需要反覆以cell name查找，且不保證cell name有序
    read_file("input.txt",cell_set);
    //show_cell_set(cell_set);
    //debug(cell_set);
    test_function(cell_set);
    
    return 0;
}
