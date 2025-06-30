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

//FM  已確定bucket list的建立沒有問題
// update max gain有小問題，不能直接往後移，要把max_gain記錄下來後重找(要重找)，可以等全部更新完(所有cell都移動完再重找):目前已解決
//測試利用gain推算new cutsize的方式是否計算正確(正確)
//可以交作業了(不要管結果了，能做完真的已經很不容易了)
//test case 2有問題，直接在inner loop塞一個counter，確認是不是有反覆被修改的問題
//doubly linked list完成


using namespace std;

struct cell_node{    //for  cell_set
    string cell_name;
    unordered_set<int> net_id_set; //紀錄一個cell擁有的net_id，不用有順序(這邊可以改成map)
};

struct net_node{    //for  net_set
    string net_name;
    unordered_set<int> cell_id_set; //紀錄一個cell擁有的net_id，不用有順序(這邊可以改成map)
};

//-------------------------------------------

struct cell_node_of_bucket_list{
	int gain;   //雖然從bucket list可以直接拿，但還是紀錄一下
	int cell_id;
    bool locked_bool;
	struct cell_node_of_bucket_list *next_cell_node_ptr;
    struct cell_node_of_bucket_list *prev_cell_node_ptr;
};

struct bucket_list_node{
	int gain;	//中間等於0
	struct cell_node_of_bucket_list *list_ptr;
};

struct bucket_list{
	struct cell_node_of_bucket_list* max_gain_node_ptr;
	vector <struct bucket_list_node> bucket_list;
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
    while (ss >> word) {// 使用空格分割字串
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

void construct_net_node(string cell_name,string net_name,unordered_map<int, struct net_node> &net_set){
    //先尋找這個net_id是否有出現過
    int cell_id=string_to_Int(cell_name);
    int net_id=string_to_Int(net_name);
    auto it = net_set.find(net_id);

    if (it == net_set.end()){     //沒有出現過，就先初始化該net的node插入
        struct net_node my_net_node;
        my_net_node.net_name=net_name;
        my_net_node.cell_id_set.insert(cell_id);
        net_set[net_id]=my_net_node;
    }
    else{ //有出現過
        it->second.cell_id_set.insert(cell_id);
    }
}

void construct_net_lines_to_cell_set_and_net_set(vector<string> net_lines, unordered_map<int, struct cell_node>&cell_set,unordered_map<int, struct net_node>&net_set){
    for(int i=0;i<net_lines.size();i++){
        net_lines[i]=remove_braces(net_lines[i]);
        vector<string> result=split_string(net_lines[i]);
        
        for(int i=2; i<result.size(); i++){
            if((i==2 || i==result.size()-1)){     //先把 { 和 } 去除
                result[i]=remove_braces(result[i]);
            }
            if((result[i]!="{"||result[i]!="}")){
                //cout<<"result[i]: "<<result[i]<<" result[1]:"<<result[1]<<endl;
                construct_cell_node(result[i],result[1],cell_set);
                construct_net_node(result[i],result[1],net_set);
            }
        }
        
    }
}

void read_file(string input_file_name, unordered_map<int, struct cell_node>&cell_set, unordered_map<int, struct net_node>&net_set){ //讀取檔案並將每一行存入 vector<string>
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
    construct_net_lines_to_cell_set_and_net_set (net_lines,cell_set,net_set);
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

void show_net_set(unordered_map<int, struct net_node> net_set){
    for (const auto& it : net_set) {
        cout<<"net id:"<<it.first <<" ";
        cout<<"net name: "<< it.second.net_name << endl;
        cout<<"all the related cells: ";
        for(auto related_cells: it.second.cell_id_set){
            cout<<related_cells<<" ";
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
    if(abs(size_A-size_B)>(size_A+size_B)/5)return false;
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


//==========bucket list的function========
void find_max_num_of_net(unordered_map<int,struct cell_node>cell_set, int &max_gain, unordered_map<int,string> patition_result){
    max_gain=0;

    for(const auto it : patition_result){
        int cell_id=it.first;
        int net_num=cell_set[cell_id].net_id_set.size();//查看該cell共有多少net
        if(max_gain<net_num)max_gain=net_num;
    }
}

bool determine_net_undercut_or_not(int net_id,unordered_map<int,struct net_node>net_set,unordered_map<int,string> patition_result){
    int nums_of_part_A_cell=0;
    int nums_of_part_B_cell=0;

    for(auto related_cell_id:net_set[net_id].cell_id_set){
        if(patition_result[related_cell_id]=="A")nums_of_part_A_cell++;
        else nums_of_part_B_cell++; 
    }

    if(nums_of_part_B_cell==0||nums_of_part_A_cell==0)return false;   //not under cut
    else return true;                                                 //under cut
}

void find_nums_of_net_undercut_and_not_undercut(int cell_id,unordered_map<int,struct cell_node>cell_set,unordered_map<int,struct net_node>net_set,unordered_map<int,string> patition_result,int &nums_of_net_not_under_cut,int &nums_of_net_under_cut){
    nums_of_net_under_cut=0;
    nums_of_net_not_under_cut=0;

    for(auto related_net_id:cell_set[cell_id].net_id_set){
        if(determine_net_undercut_or_not(related_net_id,net_set,patition_result)==true){
            nums_of_net_under_cut++;
        }
        else{
            nums_of_net_not_under_cut++;
        }
    }
}

int calculate_gain_of_cell(int cell_id,unordered_map<int,struct cell_node>cell_set,unordered_map<int,struct net_node>net_set,unordered_map<int,string> &patition_result){
    int nums_of_net_not_under_cut_before_move=0;  //檢查該cell的所有net not_under_cut的數量
    int nums_of_net_under_cut_before_move=0;      //檢查該cell的所有net under_cut的數量
    find_nums_of_net_undercut_and_not_undercut(cell_id,cell_set,net_set,patition_result,nums_of_net_not_under_cut_before_move,nums_of_net_under_cut_before_move);

    int nums_of_net_not_under_cut_after_move=0;  //檢查該cell的所有net not_under_cut的數量
    int nums_of_net_under_cut_after_move=0;      //檢查該cell的所有net under_cut的數量
    //cout<<"cutsize before move: "<<calculate_cutsize(cell_set, patition_result)<<" ";

    if(patition_result[cell_id]=="A")patition_result[cell_id]="B"; //移動該cell
    else patition_result[cell_id]="A";
    find_nums_of_net_undercut_and_not_undercut(cell_id,cell_set,net_set,patition_result,nums_of_net_not_under_cut_after_move,nums_of_net_under_cut_after_move);
    //cout<<"cutsize after move: "<<calculate_cutsize(cell_set, patition_result)<<" ";

    if(patition_result[cell_id]=="A")patition_result[cell_id]="B"; //復原該cell
    else patition_result[cell_id]="A";
    
    return nums_of_net_under_cut_before_move-nums_of_net_under_cut_after_move;
}

void insert_cell_node_to_bucket_list(int index,struct cell_node_of_bucket_list* my_cell_node_of_bucket_list, vector<struct bucket_list_node> &bucket_list){
    //要先查詢當前node的next node是否為空，不為空的話要往下走，走到底再插入
    //直接插在頭就好
    struct cell_node_of_bucket_list * node_ptr=bucket_list[index].list_ptr;
    if(bucket_list[index].list_ptr==NULL){      //當前要插入的node為head，且原本沒有任何node，故prev_node_ptr=NULL
        my_cell_node_of_bucket_list->next_cell_node_ptr=NULL;
        bucket_list[index].list_ptr=my_cell_node_of_bucket_list;
        my_cell_node_of_bucket_list->prev_cell_node_ptr=NULL;
    }
    else{                                      //當前要插入的node為head，且原本有node，故prev_node_ptr=NULL
        my_cell_node_of_bucket_list->next_cell_node_ptr=bucket_list[index].list_ptr;
        bucket_list[index].list_ptr->prev_cell_node_ptr=my_cell_node_of_bucket_list;

        bucket_list[index].list_ptr= my_cell_node_of_bucket_list;
        my_cell_node_of_bucket_list->prev_cell_node_ptr=NULL;
    }
}

void resize_bucket_list(int max_gain,vector<struct bucket_list_node> &bucket_list){
    bucket_list.resize(2*max_gain+1);
    for(int i=0;i<2*max_gain+1;i++){
        bucket_list[i].gain=i-max_gain;
    }
}

void show_bucket_list(vector<struct bucket_list_node> bucket_list){
    for(int i=0;i<bucket_list.size();i++){
        cout<<"gain: ";
        cout<<bucket_list[i].gain<<endl;
        
        struct cell_node_of_bucket_list* node_ptr=bucket_list[i].list_ptr;
        //if(bucket_list[i].list_ptr!=NULL)cout<<"bucket_list[i].list_ptr: "<<bucket_list[i].list_ptr->cell_id<<endl;
        while(node_ptr!=NULL){
            cout<<"cell_id: "<<node_ptr->cell_id<<" gain:"<<bucket_list[i].gain<<endl;
            cout<<"locked_bool: "<<node_ptr->locked_bool<<endl;
            if(bucket_list[i].list_ptr==node_ptr){
                cout<<"this is head node"<<endl;
                if(node_ptr->prev_cell_node_ptr==NULL)cout<<"the prev node of head is null"<<endl;
            }
            node_ptr=node_ptr->next_cell_node_ptr;
        }
    }
}

struct cell_node_of_bucket_list* find_max_gain_node_ptr(vector<struct bucket_list_node> bucket_list){
    for(int i=bucket_list.size()-1;i>=0;i--){
        struct cell_node_of_bucket_list* cur_node=bucket_list[i].list_ptr;
        while(1){
            if(cur_node!=NULL&&cur_node->locked_bool==false){
                return cur_node;
            }
            else if(cur_node==NULL)break;
            else cur_node=cur_node->next_cell_node_ptr;//這邊還是得travel
        }
    }
    return NULL;
}

struct bucket_list_class initialize_bucket_list_object(unordered_map<int,struct cell_node>cell_set,unordered_map<int,struct net_node>net_set,unordered_map<int,string> partition_result){
    struct bucket_list_class my_bucket_list_class;

    //找max_gain才能知道bucket list要開多大
    int max_gain=0;
    find_max_num_of_net(cell_set, max_gain,partition_result);
    
    //用max_gain初始化bucket list的list本身
    resize_bucket_list(max_gain,my_bucket_list_class.bucket_list_part_A.bucket_list);
    resize_bucket_list(max_gain,my_bucket_list_class.bucket_list_part_B.bucket_list);

    //初始化每個cell的cell node
    for(auto it:cell_set){
        int cell_id=it.first;
        int gain=calculate_gain_of_cell(cell_id,cell_set,net_set,partition_result);
        //struct cell_node_of_bucket_list* my_cell_node_of_bucket_list_ptr=(struct cell_node_of_bucket_list *)malloc(sizeof(struct cell_node_of_bucket_list));
        struct cell_node_of_bucket_list* my_cell_node_of_bucket_list_ptr=new cell_node_of_bucket_list;
        
        my_cell_node_of_bucket_list_ptr->cell_id=cell_id;
        my_cell_node_of_bucket_list_ptr->gain=gain;
        my_cell_node_of_bucket_list_ptr->locked_bool=false;
        my_cell_node_of_bucket_list_ptr->next_cell_node_ptr=NULL;
        my_cell_node_of_bucket_list_ptr->prev_cell_node_ptr=NULL;

        if(partition_result[cell_id]=="A"){
            //把node插入bucket list要額外寫個function
            insert_cell_node_to_bucket_list(gain+max_gain,my_cell_node_of_bucket_list_ptr,my_bucket_list_class.bucket_list_part_A.bucket_list);
            my_bucket_list_class.cell_id_to_node_map_part_A[cell_id]=my_cell_node_of_bucket_list_ptr;
        }
        else{
            insert_cell_node_to_bucket_list(gain+max_gain,my_cell_node_of_bucket_list_ptr,my_bucket_list_class.bucket_list_part_B.bucket_list);
            my_bucket_list_class.cell_id_to_node_map_part_B[cell_id]=my_cell_node_of_bucket_list_ptr;
        }
    }
    //show_bucket_list(my_bucket_list_class.bucket_list_part_A.bucket_list);
    my_bucket_list_class.bucket_list_part_A.max_gain_node_ptr = find_max_gain_node_ptr(my_bucket_list_class.bucket_list_part_A.bucket_list);
    //cout<<"max_gain_node_ptr for A: "<<my_bucket_list_class.bucket_list_part_A.max_gain_node_ptr->cell_id<<endl;

    //show_bucket_list(my_bucket_list_class.bucket_list_part_B.bucket_list);
    my_bucket_list_class.bucket_list_part_B.max_gain_node_ptr = find_max_gain_node_ptr(my_bucket_list_class.bucket_list_part_B.bucket_list);
    // cout<<"max_gain_node_ptr for B: "<<my_bucket_list_class.bucket_list_part_B.max_gain_node_ptr->cell_id<<endl;

    return my_bucket_list_class;
}

void check_move_from_A_or_move_from_B(bool &can_move_from_A_bool,bool &can_move_from_B_bool,unordered_map<int,string> partition_result){
    int nums_of_cell_in_part_A=0;
    int nums_of_cell_in_part_B=0;
    for(auto it:partition_result){
        if(it.second=="A") nums_of_cell_in_part_A++;
        else nums_of_cell_in_part_B++;
    }
    // cout<<"in function nums_of_cell_in_part_A: "<<nums_of_cell_in_part_A<<endl;
    // cout<<"in function nums_of_cell_in_part_B: "<<nums_of_cell_in_part_B<<endl;

    if(abs(nums_of_cell_in_part_A+1-(nums_of_cell_in_part_B-1))<=(nums_of_cell_in_part_A+nums_of_cell_in_part_B)/5)can_move_from_B_bool=true;
    else can_move_from_B_bool=false;
    if(abs(nums_of_cell_in_part_B+1-(nums_of_cell_in_part_A-1))<=(nums_of_cell_in_part_A+nums_of_cell_in_part_B)/5)can_move_from_A_bool=true;
    else can_move_from_A_bool=false;

}

int find_max_gain_cell_under_balance_constraint(struct bucket_list_class my_bucket_list_class,unordered_map<int,string> partition_result,int &cell_gain){
    //每次都要從part_A的max gain或是part_B的max gain指到的node當作要移動的cell
    bool can_move_from_A_bool,can_move_from_B_bool;
    check_move_from_A_or_move_from_B(can_move_from_A_bool,can_move_from_B_bool,partition_result);
    
    //===for debug===
    // if(my_bucket_list_class.bucket_list_part_A.max_gain_node_ptr!=NULL)cout<<"max_gain_node_ptr cell id: "<<my_bucket_list_class.bucket_list_part_A.max_gain_node_ptr->cell_id<<endl;
    // if(my_bucket_list_class.bucket_list_part_B.max_gain_node_ptr!=NULL)cout<<"max_gain_node_ptr cell id: "<<my_bucket_list_class.bucket_list_part_B.max_gain_node_ptr->cell_id<<endl;
    // cout<<"can_move_from_A_bool: "<<can_move_from_A_bool<<" can_move_from_B_bool: "<<can_move_from_A_bool<<endl;
    //===============
    string choice="NONE";//NONE or A or B

    if(can_move_from_A_bool==false&&can_move_from_B_bool==false){
        choice="NONE"; //這邊理論上不會發生，沒有合法解
    }
    else if(can_move_from_A_bool==true&&can_move_from_B_bool==true){ //從兩邊移動都是合法的
        if(my_bucket_list_class.bucket_list_part_A.max_gain_node_ptr!=NULL&&my_bucket_list_class.bucket_list_part_B.max_gain_node_ptr!=NULL){
            //兩者皆可選(選gain大的)
            if(my_bucket_list_class.bucket_list_part_A.max_gain_node_ptr->gain>my_bucket_list_class.bucket_list_part_B.max_gain_node_ptr->gain)choice="A";
            else choice="B";
        }
        else if(my_bucket_list_class.bucket_list_part_A.max_gain_node_ptr)choice="A";
        else if(my_bucket_list_class.bucket_list_part_B.max_gain_node_ptr)choice="B";
        else choice="NONE";//沒有合法解
    }
    else if(can_move_from_A_bool==true&&can_move_from_B_bool==false&&my_bucket_list_class.bucket_list_part_A.max_gain_node_ptr!=NULL){  
        choice="A";//代表要從A移動到B才會符合限制
    }
    else if(can_move_from_B_bool==true&&can_move_from_A_bool==false&&my_bucket_list_class.bucket_list_part_B.max_gain_node_ptr!=NULL){
        choice="B";//代表要從B移動到A才會符合限制
    }
    else{
        choice="NONE";//沒有合法解
    }
    
    if(choice=="A"){
        cell_gain=my_bucket_list_class.bucket_list_part_A.max_gain_node_ptr->gain;
        return my_bucket_list_class.bucket_list_part_A.max_gain_node_ptr->cell_id;
    }
    else if(choice=="B"){
        cell_gain=my_bucket_list_class.bucket_list_part_B.max_gain_node_ptr->gain;
        return my_bucket_list_class.bucket_list_part_B.max_gain_node_ptr->cell_id;
    }
    else{
        cell_gain=0;//不選擇
        return -1;
    }
}

void update_max_gain_ptr_A_and_B(struct bucket_list_class &my_bucket_list_class){
    my_bucket_list_class.bucket_list_part_A.max_gain_node_ptr = find_max_gain_node_ptr(my_bucket_list_class.bucket_list_part_A.bucket_list);
    my_bucket_list_class.bucket_list_part_B.max_gain_node_ptr = find_max_gain_node_ptr(my_bucket_list_class.bucket_list_part_B.bucket_list);
}

void insert_node_to_tail_of_certain_gain(int new_gain,vector <struct bucket_list_node> &bucket_list,struct cell_node_of_bucket_list* node_ptr_to_insert){
    int max_gain=(bucket_list.size()-1)/2;
    int index=new_gain+max_gain; //已確認index計算正確
    cout<<"new_gain: "<<new_gain<<endl;

    if(bucket_list[index].list_ptr==NULL){  //直接插在head
        node_ptr_to_insert->next_cell_node_ptr=NULL;
        bucket_list[index].list_ptr=node_ptr_to_insert;
        node_ptr_to_insert->prev_cell_node_ptr=NULL;
    }
    else{                                  //直接插在head
        node_ptr_to_insert->next_cell_node_ptr=bucket_list[index].list_ptr;
        bucket_list[index].list_ptr->prev_cell_node_ptr=node_ptr_to_insert;

        bucket_list[index].list_ptr=node_ptr_to_insert;
        node_ptr_to_insert->prev_cell_node_ptr=NULL;
    }
}

void move_cell_and_update_gain(int cell_id_to_move, unordered_map<int,struct cell_node>cell_set, unordered_map<int,struct net_node>net_set, bool locked_bool,
    unordered_map<int,struct cell_node_of_bucket_list*> &cell_map_src, struct bucket_list &bucket_list_src, struct bucket_list &bucket_list_des, unordered_map<int,string> partition_result){

    struct cell_node_of_bucket_list* cur_node_ptr = cell_map_src[cell_id_to_move];//要被移動的cell

    int max_gain=(bucket_list_src.bucket_list.size()-1)/2;
    int index=max_gain+cur_node_ptr->gain;
    
    cout<<"cell_id_to_move: "<<cell_id_to_move<<endl; 
    cout<<"cell_id_to_move check: "<<cur_node_ptr->cell_id<<endl;
    //要找到當前node的前一個node
    
    if(bucket_list_src.bucket_list[index].list_ptr==cur_node_ptr){    //代表當前的node是這個Linked list的head node
        //cout<<"make sense 1"<<endl;
        bucket_list_src.bucket_list[index].list_ptr=cur_node_ptr->next_cell_node_ptr;  //此時cur_node_ptr的下一個node會變成head node
        //cout<<"make sense -1"<<endl;
    }
    else{
        //cout<<"make sense 2"<<endl;
        struct cell_node_of_bucket_list* prev_node_ptr_of_cur_node=cur_node_ptr->prev_cell_node_ptr;
        //cout<<"make sense 3"<<endl;
        if(prev_node_ptr_of_cur_node)prev_node_ptr_of_cur_node->next_cell_node_ptr=cur_node_ptr->next_cell_node_ptr; 
        //cout<<"make sense 4"<<endl;
        if(cur_node_ptr->next_cell_node_ptr)cur_node_ptr->next_cell_node_ptr->prev_cell_node_ptr=prev_node_ptr_of_cur_node;
        //cout<<"make sense 5"<<endl;
    }

    //重新計算cell_id_to_move的gain
    cur_node_ptr->gain=calculate_gain_of_cell(cell_id_to_move,cell_set,net_set,partition_result);
    cur_node_ptr->locked_bool=locked_bool; //上鎖
    cur_node_ptr->next_cell_node_ptr=NULL;
    cur_node_ptr->prev_cell_node_ptr=NULL;

    //移動cur_node到另一個bucket list(已上鎖)對應gain的head
    insert_node_to_tail_of_certain_gain(cur_node_ptr->gain,bucket_list_des.bucket_list,cur_node_ptr);
}

unordered_map<int,string> find_all_the_related_cell(int cell_id_to_move,unordered_map<int,struct cell_node>cell_set,unordered_map<int,struct net_node>net_set,unordered_map<int,string> partition_result){
    unordered_map<int,string> related_cell_map;
    for(auto net_element : cell_set[cell_id_to_move].net_id_set){
        for(auto related_cell:net_set[net_element].cell_id_set){
            if(related_cell!=cell_id_to_move){
                related_cell_map[related_cell]=partition_result[related_cell];
            }
        }
    }
    return  related_cell_map;
}

void update_all_the_related_cell_gain(int cell_id_to_move,struct bucket_list_class &my_bucket_list_class , unordered_map<int,struct cell_node>cell_set,unordered_map<int,struct net_node>net_set,unordered_map<int,string> partition_result){
    //重新計算和cell_id_to_move持有相同net的cell的gain(比對過後先找出來，再重新計算)
    unordered_map<int,string> related_cell_map=find_all_the_related_cell(cell_id_to_move,cell_set,net_set,partition_result);
    for(auto it:related_cell_map){
        cout<<"related cell: "<<it.first<<endl;
    }
    for(auto it:related_cell_map){
        int cell_id=it.first;
        cout<<"cell_id: "<<cell_id<<" "<<it.second<<endl;
        if(it.second=="A"){//某些cell會卡住做不完
            //都是part A的cell，在part A內移動就好
            bool locked_bool=my_bucket_list_class.cell_id_to_node_map_part_A[cell_id]->locked_bool;
            move_cell_and_update_gain(cell_id, cell_set,net_set,locked_bool, my_bucket_list_class.cell_id_to_node_map_part_A, my_bucket_list_class.bucket_list_part_A, my_bucket_list_class.bucket_list_part_A, partition_result);
        }
        else{
            //都是part B的cell，在part B內移動就好
            bool locked_bool=my_bucket_list_class.cell_id_to_node_map_part_B[cell_id]->locked_bool;
            move_cell_and_update_gain(cell_id, cell_set,net_set,locked_bool, my_bucket_list_class.cell_id_to_node_map_part_B, my_bucket_list_class.bucket_list_part_B, my_bucket_list_class.bucket_list_part_B, partition_result);
        }
        //===for debugging===
        // cout<<"bucket list part A:"<<endl;
        // show_bucket_list(my_bucket_list_class.bucket_list_part_A.bucket_list);
        // cout<<"bucket list part B:"<<endl;
        // show_bucket_list(my_bucket_list_class.bucket_list_part_B.bucket_list);
        // cout<<"cell_id: "<<cell_id<<" movement is done"<<endl<<endl;
        //===================
    }
    
}

void move_cell_and_update_bucket_list_and_partition(int cell_id_to_move,unordered_map<int,struct cell_node>cell_set,unordered_map<int,struct net_node>net_set,struct bucket_list_class &my_bucket_list_class, unordered_map<int,string> &partition_result){
    if(partition_result[cell_id_to_move]=="A"){
        //把cell_id從A移動到B
        struct cell_node_of_bucket_list* cell_node_to_move=my_bucket_list_class.cell_id_to_node_map_part_A[cell_id_to_move];
        partition_result[cell_id_to_move]="B"; //partition已更新
        
        //更新相關的cell的gain以及在位置(這邊的map不用動，因為part A的cell還是那些，part B的cell也還是那些)
        update_all_the_related_cell_gain(cell_id_to_move,my_bucket_list_class , cell_set,net_set,partition_result);

        //重新計算cell_id_to_move的gain並移動該node，並上鎖(上鎖部分確定OK)，這邊的map要動，因為part A map的cell ptr會被刪除，part Bmap的cell ptr會增加
        move_cell_and_update_gain(cell_id_to_move, cell_set, net_set,true, my_bucket_list_class.cell_id_to_node_map_part_A, my_bucket_list_class.bucket_list_part_A, my_bucket_list_class.bucket_list_part_B, partition_result);
        my_bucket_list_class.cell_id_to_node_map_part_B[cell_id_to_move]=my_bucket_list_class.cell_id_to_node_map_part_A[cell_id_to_move];
        my_bucket_list_class.cell_id_to_node_map_part_A.erase(cell_id_to_move);

        //更新max_gain_ptr
        update_max_gain_ptr_A_and_B(my_bucket_list_class);

        //===for debugging===
        // if(my_bucket_list_class.bucket_list_part_A.max_gain_node_ptr)cout<<"max_gain_node_ptr part A: "<<my_bucket_list_class.bucket_list_part_A.max_gain_node_ptr->cell_id<<endl;
        // cout<<"bucket list part A:"<<endl;
        // show_bucket_list(my_bucket_list_class.bucket_list_part_A.bucket_list);

        // if(my_bucket_list_class.bucket_list_part_B.max_gain_node_ptr)cout<<"max_gain_node_ptr part B: "<<my_bucket_list_class.bucket_list_part_B.max_gain_node_ptr->cell_id<<endl;
        // cout<<"bucket list part B:"<<endl;
        // show_bucket_list(my_bucket_list_class.bucket_list_part_B.bucket_list);
        //===================
    }
    else{
        //把cell_id從B移動到A
        struct cell_node_of_bucket_list* cell_node_to_move=my_bucket_list_class.cell_id_to_node_map_part_B[cell_id_to_move];
        partition_result[cell_id_to_move]="A"; //partition已更新
        
        //更新相關的cell的gain以及在位置
        update_all_the_related_cell_gain(cell_id_to_move,my_bucket_list_class , cell_set,net_set,partition_result);

        //重新計算cell_id_to_move的gain並移動該node，並上鎖(上鎖部分確定OK)
        move_cell_and_update_gain(cell_id_to_move, cell_set, net_set,true, my_bucket_list_class.cell_id_to_node_map_part_B, my_bucket_list_class.bucket_list_part_B, my_bucket_list_class.bucket_list_part_A, partition_result);
        my_bucket_list_class.cell_id_to_node_map_part_A[cell_id_to_move]=my_bucket_list_class.cell_id_to_node_map_part_B[cell_id_to_move];
        my_bucket_list_class.cell_id_to_node_map_part_B.erase(cell_id_to_move);

        //更新max_gain_ptr
        update_max_gain_ptr_A_and_B(my_bucket_list_class);

        //===for debugging===
        // if(my_bucket_list_class.bucket_list_part_A.max_gain_node_ptr)cout<<"max_gain_node_ptr part A: "<<my_bucket_list_class.bucket_list_part_A.max_gain_node_ptr->cell_id<<endl;
        // cout<<"bucket list part A:"<<endl;
        // show_bucket_list(my_bucket_list_class.bucket_list_part_A.bucket_list);
        
        // if(my_bucket_list_class.bucket_list_part_B.max_gain_node_ptr)cout<<"max_gain_node_ptr part B: "<<my_bucket_list_class.bucket_list_part_B.max_gain_node_ptr->cell_id<<endl;
        // cout<<"bucket list part B:"<<endl;
        // show_bucket_list(my_bucket_list_class.bucket_list_part_B.bucket_list);
        //===================
    } 
}

void Fiduccia_Mattheyses(unordered_map<int,struct cell_node>cell_set,unordered_map<int,struct net_node>net_set){
    unordered_map<int,string> cur_best_partition=initialize_partition(cell_set);  //只存best
    unordered_map<int,string> history_best_partition=cur_best_partition;
    unordered_map<int,string> partition_result=cur_best_partition;

    int cur_best_cutsize=calculate_cutsize(cell_set, cur_best_partition);
    int history_best_cutsize=cur_best_cutsize;

    while(1){
        //根據當前的best_partition初始化gain_map (所有的cell都還沒被鎖上)
        struct bucket_list_class my_bucket_list_class=initialize_bucket_list_object(cell_set,net_set,partition_result);
        //===for debug===
        cout<<"bucket list part A:"<<endl;
        show_bucket_list(my_bucket_list_class.bucket_list_part_A.bucket_list);
        cout<<"bucket list part B:"<<endl;
        show_bucket_list(my_bucket_list_class.bucket_list_part_B.bucket_list);
        cout<<endl;
        //===============

        int cur_cutsize=calculate_cutsize(cell_set, partition_result);
        
        cur_best_partition=partition_result;
        cur_best_cutsize=cur_cutsize;
        while(1){
            int gain_of_cell_id_to_move=0;
            int cell_id_to_move=find_max_gain_cell_under_balance_constraint(my_bucket_list_class,partition_result,gain_of_cell_id_to_move);
            cout<<"FM: selected cell_id_to_move: "<<cell_id_to_move<<endl;
            if(cell_id_to_move==-1)break;
            else{
                //確實移動該cell_id(更新partition和bucket_list(移動cell的pointer，max_gain_ptr也要))
                cur_cutsize-=gain_of_cell_id_to_move;
                move_cell_and_update_bucket_list_and_partition(cell_id_to_move,cell_set,net_set,my_bucket_list_class, partition_result);
                //如果當前的cutsize比cur_best_cutsize好(更新cur_best_cutsize以及partition)
                if(cur_cutsize<cur_best_cutsize){
                    cur_best_cutsize=cur_cutsize;
                    cur_best_partition=partition_result;
                }
            }
            // cout<<"bucket list part A:"<<endl;
            // show_bucket_list(my_bucket_list_class.bucket_list_part_A.bucket_list);
            // cout<<"bucket list part B:"<<endl;
            // show_bucket_list(my_bucket_list_class.bucket_list_part_B.bucket_list);
            // cout<<"inner loop done"<<endl;
            // cout<<endl;
        }
        if(cur_best_cutsize<history_best_cutsize){ //不確定這樣的做法是不是最好
            history_best_cutsize=cur_best_cutsize;
            history_best_partition=cur_best_partition;
        }
        else break;
        partition_result=history_best_partition;
        cout<<"cur_best_cutsize: "<<cur_best_cutsize<<" history_best_cutsize: "<<history_best_cutsize<<endl;
    }
    cout<<"final result: "<<calculate_cutsize(cell_set, history_best_partition)<<endl;
}

int main(){
    unordered_map<int, struct cell_node> cell_set;  //以cell為單位，記錄所有cell的id以及所有相關的net
    unordered_map<int, struct net_node> net_set;    //以net為單位，記錄所有net的id以及所有相關的cell
    //會用unordered_map的原因是會需要反覆以cell name查找，且不保證cell name有序與連續

    read_file("input.txt",cell_set,net_set);
    //show_cell_set(cell_set);
    //show_net_set(net_set);
    //unordered_map<int,string> partition_result=initialize_partition(cell_set);
    Fiduccia_Mattheyses(cell_set,net_set);
    
    return 0;
}
