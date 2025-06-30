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

//FM  
//update max gain有小問題，不能直接往後移，要把max_gain記錄下來後重找(要重找)，可以等全部更新完(所有cell都移動完再重找):目前已解決
//我們用投影片的方法，base cell id的gain還是錯的
//這個版本是直接重新建構bucket list，但仍有修改成最標準方法的餘地(line 763)
//已確認assign_bucket_list_object沒有問題，問題出在base cell 的gain用投影片的方法算的是錯的


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

void make_set_for_write_file(unordered_map<int,string> partition_result, unordered_set<string> &set_A, unordered_set<string> &set_B){
    for(auto it:partition_result){
        if(it.second=="A"){
            set_A.insert("c"+to_string(it.first));
        }
        else{
            set_B.insert("c"+to_string(it.first));
        }
    }
}

void write_file(string output_file_name, int cutsize, unordered_map<int,string> partition_result){ //讀取檔案並將每一行存入 vector<string>
    
    ofstream outFile(output_file_name);
    if(!outFile){
        cerr<<"can't open file: "<<output_file_name<<endl;
        return ;
    }

    unordered_set<string> set_A;
    unordered_set<string> set_B;
    make_set_for_write_file(partition_result, set_A, set_B);

    outFile<<"cut_size "<<cutsize<<endl;
    outFile<<"A"<<endl;
    for(auto it:set_A)outFile<<it<<endl;

    outFile<<"B"<<endl;
    for(auto it:set_B)outFile<<it<<endl;
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

bool determine_net_undercut_or_not(int net_id,unordered_map<int,struct net_node>net_set,unordered_map<int,string> patition_result){
    int nums_of_part_A_cell=0;
    int nums_of_part_B_cell=0;

    for(auto related_cell_id:net_set[net_id].cell_id_set){
        if(patition_result[related_cell_id]=="A")nums_of_part_A_cell++;
        else nums_of_part_B_cell++; 
        if(nums_of_part_A_cell!=0&&nums_of_part_B_cell!=0)return true;
    }

    if(nums_of_part_B_cell==0||nums_of_part_A_cell==0)return false;   //not under cut
    else return true;                                                 //under cut
}

int calculate_cutsize(unordered_map<int,struct net_node>net_set, unordered_map<int,string> partition_result){
    int cutsize=0;
    for (const auto it : net_set) { //分別統計在Part A有出現過的net以及在Part B有出現過的net
        int net_id=it.first;
        if(determine_net_undercut_or_not(net_id,net_set,partition_result)==true)cutsize++;
    }
    return cutsize;
}


void show_partition_result(unordered_map<int,struct net_node>net_set,unordered_map<int,string> patition_result){
    for(const auto it : patition_result){
        cout<<"cell id: "<<it.first<<" ->"<<it.second<<endl;
    }
    //cout<<"cutsize: "<<calculate_cutsize(net_set,patition_result)<<endl;
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
//OK
void calculate_From_and_To(int net_id,string From_part,unordered_map<int,struct net_node>net_set,unordered_map<int,string> partition_result,int &From,int &To){
    From=0;
    To=0;
    for(auto related_cell_id:net_set[net_id].cell_id_set){
        if(partition_result[related_cell_id]==From_part){
            From++;
        }
        else{
            To++;
        }
        if(From>1&&To>0){
            From=-1;To=-1;
            break;
        }
    }
}

int calculate_gain_of_cell(int cell_id,unordered_map<int,struct cell_node>cell_set,unordered_map<int,struct net_node>net_set,unordered_map<int,string> &patition_result){
    int gain=0;  //可以把所有net travel過一次就好，不會重複計算一個net的from和to
    string From_part=patition_result[cell_id];
    for(auto net_id: cell_set[cell_id].net_id_set){
        int From=0;int To=0;
        calculate_From_and_To(net_id,From_part,net_set,patition_result,From,To);
        if(From==1)gain++;
        if(To==0)gain--;
    }
    //cout<<"gain: "<<gain<<" correct gain: "<<calculate_gain_of_cell_correct(cell_id,cell_set,net_set,patition_result);
    return gain;
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
    my_bucket_list_class.bucket_list_part_A.max_gain_node_ptr = find_max_gain_node_ptr(my_bucket_list_class.bucket_list_part_A.bucket_list);
    my_bucket_list_class.bucket_list_part_B.max_gain_node_ptr = find_max_gain_node_ptr(my_bucket_list_class.bucket_list_part_B.bucket_list);
    return my_bucket_list_class;
}

struct bucket_list_class assign_bucket_list_object(struct bucket_list_class my_bucket_list_class, unordered_map<int,struct cell_node>cell_set,unordered_map<int,struct net_node>net_set,unordered_map<int,string> partition_result){
    struct bucket_list_class bucket_list_class_to_be_assigned;

    int max_gain_A,max_gain_B;    
    //find_max_num_of_net(cell_set, max_gain,partition_result);
    max_gain_A=(my_bucket_list_class.bucket_list_part_A.bucket_list.size()-1)/2;
    max_gain_B=(my_bucket_list_class.bucket_list_part_B.bucket_list.size()-1)/2;
    
    //用max_gain初始化bucket list的list本身
    resize_bucket_list(max_gain_A,bucket_list_class_to_be_assigned.bucket_list_part_A.bucket_list);   //可以被替換，但等等
    resize_bucket_list(max_gain_B,bucket_list_class_to_be_assigned.bucket_list_part_B.bucket_list);   //可以被替換，但等等

    //初始化每個cell的cell node
    for(auto it:cell_set){
        int cell_id=it.first;
        struct cell_node_of_bucket_list* my_cell_node_of_bucket_list_ptr=new cell_node_of_bucket_list; //malloc
        my_cell_node_of_bucket_list_ptr->cell_id=cell_id;
        my_cell_node_of_bucket_list_ptr->locked_bool=false;
        my_cell_node_of_bucket_list_ptr->next_cell_node_ptr=NULL;
        my_cell_node_of_bucket_list_ptr->prev_cell_node_ptr=NULL;

        if(partition_result[cell_id]=="A"){
            //把node插入bucket list要額外寫個function
            int gain=my_bucket_list_class.cell_id_to_node_map_part_A[cell_id]->gain;
            my_cell_node_of_bucket_list_ptr->gain=gain;

            insert_cell_node_to_bucket_list(gain+max_gain_A,my_cell_node_of_bucket_list_ptr,bucket_list_class_to_be_assigned.bucket_list_part_A.bucket_list);
            bucket_list_class_to_be_assigned.cell_id_to_node_map_part_A[cell_id]=my_cell_node_of_bucket_list_ptr;
        }
        else{
            int gain=my_bucket_list_class.cell_id_to_node_map_part_B[cell_id]->gain;
            my_cell_node_of_bucket_list_ptr->gain=gain;

            insert_cell_node_to_bucket_list(gain+max_gain_B,my_cell_node_of_bucket_list_ptr,bucket_list_class_to_be_assigned.bucket_list_part_B.bucket_list);
            bucket_list_class_to_be_assigned.cell_id_to_node_map_part_B[cell_id]=my_cell_node_of_bucket_list_ptr;
        }
    }

    bucket_list_class_to_be_assigned.bucket_list_part_A.max_gain_node_ptr = find_max_gain_node_ptr(bucket_list_class_to_be_assigned.bucket_list_part_A.bucket_list);
    bucket_list_class_to_be_assigned.bucket_list_part_B.max_gain_node_ptr = find_max_gain_node_ptr(bucket_list_class_to_be_assigned.bucket_list_part_B.bucket_list);
    return bucket_list_class_to_be_assigned;
}


void unlock_bucket_list_object(struct bucket_list_class &my_bucket_list_class){
    //解鎖所有cell node
    for(auto it : my_bucket_list_class.cell_id_to_node_map_part_A){
        it.second->locked_bool=false;
    }

    for(auto it : my_bucket_list_class.cell_id_to_node_map_part_B){
        it.second->locked_bool=false;
    }
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
    else{                                   //直接插在head
        node_ptr_to_insert->next_cell_node_ptr=bucket_list[index].list_ptr;
        bucket_list[index].list_ptr->prev_cell_node_ptr=node_ptr_to_insert;

        bucket_list[index].list_ptr=node_ptr_to_insert;
        node_ptr_to_insert->prev_cell_node_ptr=NULL;
    }
}

void move_cell_and_update_gain(int cell_id_to_move, int new_gain, unordered_map<int,struct cell_node>cell_set, unordered_map<int,struct net_node>net_set, bool locked_bool,
    unordered_map<int,struct cell_node_of_bucket_list*> &cell_map_src, struct bucket_list &bucket_list_src, struct bucket_list &bucket_list_des, unordered_map<int,string> partition_result){
    struct cell_node_of_bucket_list* cur_node_ptr = cell_map_src[cell_id_to_move];//要被移動的cell
    cout<<"cell_id_to_move in function: "<<cell_id_to_move<<endl;
    if(cur_node_ptr==NULL)cout<<"cur_node_ptr is NULL"<<endl;
    
    int max_gain=(bucket_list_src.bucket_list.size()-1)/2;
    int index=max_gain+cur_node_ptr->gain;

    
    //要找到當前node的前一個node
    if(bucket_list_src.bucket_list[index].list_ptr==cur_node_ptr){    //代表當前的node是這個Linked list的head node
        bucket_list_src.bucket_list[index].list_ptr=cur_node_ptr->next_cell_node_ptr;  //此時cur_node_ptr的下一個node會變成head node
        //cout<<"make sense -1"<<endl;
    }
    else{
        //cout<<"make sense 2"<<endl;
        struct cell_node_of_bucket_list* prev_node_ptr_of_cur_node=cur_node_ptr->prev_cell_node_ptr;
        if(prev_node_ptr_of_cur_node)prev_node_ptr_of_cur_node->next_cell_node_ptr=cur_node_ptr->next_cell_node_ptr; 
        if(cur_node_ptr->next_cell_node_ptr)cur_node_ptr->next_cell_node_ptr->prev_cell_node_ptr=prev_node_ptr_of_cur_node;
        //cout<<"make sense 5"<<endl;
    }

    //重新計算cell_id_to_move的gain
    cur_node_ptr->gain=new_gain;
    cur_node_ptr->locked_bool=locked_bool; //上鎖
    cur_node_ptr->next_cell_node_ptr=NULL;
    cur_node_ptr->prev_cell_node_ptr=NULL;

    //移動cur_node到另一個bucket list(已上鎖)對應gain的head
    insert_node_to_tail_of_certain_gain(new_gain,bucket_list_des.bucket_list,cur_node_ptr);
    // cout<<"this is function end"<<endl;
}

void find_F_N_and_T_N_map_before_move(int cell_id_to_move, int critical_net_id, unordered_map<int,string> partition_result, unordered_map<int,struct net_node> net_set, 
    struct bucket_list_class my_bucket_list_class, unordered_map<int,int> &F_N_cell_id_to_gain_map_before_move, unordered_map<int,int> &T_N_cell_id_to_gain_map_before_move){
    
        //這邊的partition_result是移動以前的
    for(auto related_cell:net_set[critical_net_id].cell_id_set){
        struct cell_node_of_bucket_list* related_cell_node;
        if(partition_result[related_cell]=="A") related_cell_node = my_bucket_list_class.cell_id_to_node_map_part_A[related_cell];
        else related_cell_node = my_bucket_list_class.cell_id_to_node_map_part_B[related_cell];

        if(partition_result[related_cell]==partition_result[cell_id_to_move]){
            F_N_cell_id_to_gain_map_before_move[related_cell]=related_cell_node->gain;
        }
        else{
            T_N_cell_id_to_gain_map_before_move[related_cell]=related_cell_node->gain;
        }
    }
}

void update_F_N_and_T_N_map_after_move(int cell_id_to_move, unordered_map<int,int> F_N_cell_id_to_gain_map_before_move, unordered_map<int,int> T_N_cell_id_to_gain_map_before_move, unordered_map<int,int> &F_N_cell_id_to_gain_map_after_move, unordered_map<int,int> &T_N_cell_id_to_gain_map_after_move){
    T_N_cell_id_to_gain_map_after_move=T_N_cell_id_to_gain_map_before_move;
    T_N_cell_id_to_gain_map_after_move[cell_id_to_move]=F_N_cell_id_to_gain_map_before_move[cell_id_to_move];
    
    F_N_cell_id_to_gain_map_after_move=F_N_cell_id_to_gain_map_before_move;
    F_N_cell_id_to_gain_map_after_move.erase(cell_id_to_move);
}

void update_all_updated_cell_id_to_gain_map(int cell_id_to_move,int critical_net_id, unordered_map<int,struct cell_node>cell_set,unordered_map<int,struct net_node>net_set,struct bucket_list_class my_bucket_list_class, unordered_map<int,string> partition_result, unordered_map<int,int> &all_updated_cell_id_to_gain_map){
    cout<<"critical_net_id: "<<critical_net_id<<endl;

    unordered_map<int,int> F_N_cell_id_to_gain_map_before_move;
    unordered_map<int,int> T_N_cell_id_to_gain_map_before_move;
    find_F_N_and_T_N_map_before_move (cell_id_to_move, critical_net_id, partition_result, net_set, my_bucket_list_class, F_N_cell_id_to_gain_map_before_move, T_N_cell_id_to_gain_map_before_move);
    
    //=====for debug======
    // cout<<"cell gain before base cell is moved in From: "<<endl;
    // for(auto it:F_N_cell_id_to_gain_map_before_move){
    //     cout<<"cell id: "<<it.first<<" gain: "<<it.second<<endl;
    // }

    // cout<<"cell gain before base cell is moved in To: "<<endl;
    // for(auto it:T_N_cell_id_to_gain_map_before_move){
    //     cout<<"cell id: "<<it.first<<" gain: "<<it.second<<endl;
    // }
    //====================

    if(T_N_cell_id_to_gain_map_before_move.size()==0){
        for(auto it:F_N_cell_id_to_gain_map_before_move){
            if(all_updated_cell_id_to_gain_map.find(it.first)!=all_updated_cell_id_to_gain_map.end()){
                all_updated_cell_id_to_gain_map[it.first]=all_updated_cell_id_to_gain_map[it.first]+1;
            }
            else all_updated_cell_id_to_gain_map[it.first]=it.second+1;
        }
    }
    else if(T_N_cell_id_to_gain_map_before_move.size()==1){
        for(auto it:T_N_cell_id_to_gain_map_before_move){
            if(all_updated_cell_id_to_gain_map.find(it.first)!=all_updated_cell_id_to_gain_map.end()){
                all_updated_cell_id_to_gain_map[it.first]=all_updated_cell_id_to_gain_map[it.first]-1;
            }
            else all_updated_cell_id_to_gain_map[it.first]=it.second-1;
        }
    }

    //=====for debug=====
    // cout<<"all_updated_cell_id_to_gain_map before base cell is moved : "<<endl;
    // for(auto it:all_updated_cell_id_to_gain_map){
    //     cout<<"cell id: "<<it.first<<" gain: "<<it.second<<endl;
    // }
    //===================

    unordered_map<int,int> F_N_cell_id_to_gain_map_after_move;
    unordered_map<int,int> T_N_cell_id_to_gain_map_after_move;

    update_F_N_and_T_N_map_after_move(cell_id_to_move,F_N_cell_id_to_gain_map_before_move, T_N_cell_id_to_gain_map_before_move, F_N_cell_id_to_gain_map_after_move, T_N_cell_id_to_gain_map_after_move);
    
    //=====for debug======
    // cout<<"cell gain after base cell is moved in From: "<<endl;
    // for(auto it:F_N_cell_id_to_gain_map_after_move){
    //     cout<<"cell id: "<<it.first<<" gain: "<<it.second<<endl;
    // }

    // cout<<"cell gain after base cell is moved in To: "<<endl;
    // for(auto it:T_N_cell_id_to_gain_map_after_move){
    //     cout<<"cell id: "<<it.first<<" gain: "<<it.second<<endl;
    // }
    //====================

    if(F_N_cell_id_to_gain_map_after_move.size()==0){
        for(auto it:T_N_cell_id_to_gain_map_after_move){
            //如果在上半部被更新過
            if(all_updated_cell_id_to_gain_map.find(it.first)!=all_updated_cell_id_to_gain_map.end()){
                all_updated_cell_id_to_gain_map[it.first]=all_updated_cell_id_to_gain_map[it.first]-1;
            }
            else all_updated_cell_id_to_gain_map[it.first]=it.second-1;
        }
    }
    else if(F_N_cell_id_to_gain_map_after_move.size()==1){
        for(auto it:F_N_cell_id_to_gain_map_after_move){
            if(all_updated_cell_id_to_gain_map.find(it.first)!=all_updated_cell_id_to_gain_map.end()){
                all_updated_cell_id_to_gain_map[it.first]=all_updated_cell_id_to_gain_map[it.first]+1;
            }
            else all_updated_cell_id_to_gain_map[it.first]=it.second+1;
        }
    }

    //=====for debug=====
    // cout<<"all_updated_cell_id_to_gain_map after base cell is moved : "<<endl;
    // for(auto it:all_updated_cell_id_to_gain_map){
    //     cout<<"cell id: "<<it.first<<" gain: "<<it.second<<endl;
    // }
    //===================
}


void update_all_the_related_cell_gain(int cell_id_to_move, unordered_map<int,struct cell_node>cell_set,unordered_map<int,struct net_node>net_set,struct bucket_list_class &my_bucket_list_class, unordered_map<int,string> partition_result, int &new_gain_of_base_cell){
    //在這邊會真的移動related cell在bucket list中的node(在同個bucket list中移動)
    unordered_map<int,int> all_updated_cell_id_to_gain_map;
    cout<<"base cell: "<<cell_id_to_move<<" ,the following process we update related cell gain"<<endl;
    for(auto critical_net_id: cell_set[cell_id_to_move].net_id_set){
        update_all_updated_cell_id_to_gain_map(cell_id_to_move, critical_net_id, cell_set,net_set,my_bucket_list_class, partition_result,all_updated_cell_id_to_gain_map);
    }

    //更新all_updated_cell_id_to_gain_map的所有cell成對應的gain(要移動)
    for(auto it:all_updated_cell_id_to_gain_map){
        int cell_id=it.first;

        if(partition_result[cell_id_to_move]=="A")partition_result[cell_id_to_move]="B";
        else partition_result[cell_id_to_move]="A";

        //int correct_new_gain=calculate_gain_of_cell(cell_id,cell_set,net_set,partition_result);
        int new_gain=it.second;

        //cout<<"cell_id: "<<cell_id<<" correct new_gain: "<<correct_new_gain<<" new_gain: "<<it.second<<endl;
        
        if(partition_result[cell_id_to_move]=="A")partition_result[cell_id_to_move]="B";
        else partition_result[cell_id_to_move]="A";

        if(cell_id==cell_id_to_move)new_gain_of_base_cell=new_gain;
        else if(cell_id!=cell_id_to_move){
            if(partition_result[cell_id]=="A"){
                bool locked_bool=my_bucket_list_class.cell_id_to_node_map_part_A[cell_id]->locked_bool;
                move_cell_and_update_gain(cell_id, new_gain, cell_set,net_set,locked_bool, my_bucket_list_class.cell_id_to_node_map_part_A, my_bucket_list_class.bucket_list_part_A, my_bucket_list_class.bucket_list_part_A, partition_result);
            }
            else{
                bool locked_bool=my_bucket_list_class.cell_id_to_node_map_part_B[cell_id]->locked_bool;
                move_cell_and_update_gain(cell_id, new_gain,cell_set,net_set,locked_bool, my_bucket_list_class.cell_id_to_node_map_part_B, my_bucket_list_class.bucket_list_part_B, my_bucket_list_class.bucket_list_part_B, partition_result);
            }
        }
    }
    cout<<"base cell: "<<cell_id_to_move<<" ,related cell gain are all updated"<<endl;

    //===for debug===
    cout<<"After Updating: "<<endl;
    cout<<"bucket list part A:"<<endl;
    show_bucket_list(my_bucket_list_class.bucket_list_part_A.bucket_list);
    cout<<"bucket list part B:"<<endl;
    show_bucket_list(my_bucket_list_class.bucket_list_part_B.bucket_list);
    cout<<endl;
    //===============
    cout<<endl;
}


void move_cell_and_update_bucket_list_and_partition(int cell_id_to_move,unordered_map<int,struct cell_node>cell_set,unordered_map<int,struct net_node>net_set,struct bucket_list_class &my_bucket_list_class, unordered_map<int,string> &partition_result){
    if(partition_result[cell_id_to_move]=="A"){
        int new_gain_of_base_cell=0;
        
        //step 1: 更新相關的cell的gain以及在位置(這邊的map不用動，因為part A的cell還是那些，part B的cell也還是那些)
        struct cell_node_of_bucket_list* cell_node_to_move=my_bucket_list_class.cell_id_to_node_map_part_A[cell_id_to_move];
        update_all_the_related_cell_gain(cell_id_to_move,cell_set,net_set,my_bucket_list_class, partition_result, new_gain_of_base_cell);
        
        //step 2: 把cell_id從A移動到B
        partition_result[cell_id_to_move]="B"; //partition已更新

        //step 3: 重新計算cell_id_to_move的gain並移動該node，並上鎖(上鎖部分確定OK)，這邊的map要動，因為part A map的cell ptr會被刪除，part Bmap的cell ptr會增加
        int new_gain=-(cell_node_to_move->gain);
        //cout<<"base cell id: "<<cell_id_to_move<<" new_gain: "<<-(cell_node_to_move->gain)<<" correct new_gain: "<<new_gain<<endl;
        move_cell_and_update_gain(cell_id_to_move,new_gain, cell_set, net_set,true, my_bucket_list_class.cell_id_to_node_map_part_A, my_bucket_list_class.bucket_list_part_A, my_bucket_list_class.bucket_list_part_B, partition_result);

        my_bucket_list_class.cell_id_to_node_map_part_B[cell_id_to_move]=my_bucket_list_class.cell_id_to_node_map_part_A[cell_id_to_move];
        my_bucket_list_class.cell_id_to_node_map_part_A.erase(cell_id_to_move);

        //step 4: 更新max_gain_ptr
        update_max_gain_ptr_A_and_B(my_bucket_list_class);

    }
    else{
        int new_gain_of_base_cell=0;

        //step 1: 更新相關的cell的gain以及在位置
        struct cell_node_of_bucket_list* cell_node_to_move=my_bucket_list_class.cell_id_to_node_map_part_B[cell_id_to_move];
        update_all_the_related_cell_gain(cell_id_to_move,cell_set,net_set,my_bucket_list_class, partition_result, new_gain_of_base_cell);
        
        //step 2: 把cell_id從B移動到A
        partition_result[cell_id_to_move]="A"; //partition已更新
        
        //step 3: 重新計算cell_id_to_move的gain並移動該node，並上鎖(上鎖部分確定OK)
        int new_gain=-(cell_node_to_move->gain);
        //cout<<"base cell id: "<<cell_id_to_move<<" new_gain: "<<-(cell_node_to_move->gain)<<" correct new_gain: "<<new_gain<<endl;
        move_cell_and_update_gain(cell_id_to_move,new_gain, cell_set, net_set,true, my_bucket_list_class.cell_id_to_node_map_part_B, my_bucket_list_class.bucket_list_part_B, my_bucket_list_class.bucket_list_part_A, partition_result);

        my_bucket_list_class.cell_id_to_node_map_part_A[cell_id_to_move]=my_bucket_list_class.cell_id_to_node_map_part_B[cell_id_to_move];
        my_bucket_list_class.cell_id_to_node_map_part_B.erase(cell_id_to_move);

        //step 4: 更新max_gain_ptr
        update_max_gain_ptr_A_and_B(my_bucket_list_class);
    } 
}

void Fiduccia_Mattheyses(unordered_map<int,struct cell_node>cell_set,unordered_map<int,struct net_node>net_set){
    unordered_map<int,string> cur_best_partition=initialize_partition(cell_set);  //只存best
    unordered_map<int,string> history_best_partition=cur_best_partition;
    unordered_map<int,string> partition_result=cur_best_partition;

    int cur_cutsize = calculate_cutsize(net_set, partition_result);
    int cur_best_cutsize = cur_cutsize;
    int history_best_cutsize = cur_best_cutsize;

    struct bucket_list_class my_bucket_list_class = initialize_bucket_list_object(cell_set,net_set,partition_result);   //這個步驟最久
    struct bucket_list_class cur_best_bucket_list_class;
    struct bucket_list_class history_best_bucket_list_class;

    int ct=0;
    while(1){
        //根據當前的best_partition初始化gain_map (所有的cell都還沒被鎖上)
        cur_best_partition=partition_result;
        cur_best_cutsize=cur_cutsize;

        cur_best_bucket_list_class = assign_bucket_list_object(my_bucket_list_class,cell_set,net_set,partition_result);
        history_best_bucket_list_class = assign_bucket_list_object(my_bucket_list_class,cell_set,net_set,partition_result);
        
        while(1){
            int gain_of_cell_id_to_move=0;
            int cell_id_to_move=find_max_gain_cell_under_balance_constraint(my_bucket_list_class,partition_result,gain_of_cell_id_to_move);
            cout<<"FM: selected cell_id_to_move: "<<cell_id_to_move<<" ct: "<<ct<<endl;
            
            if(cell_id_to_move==-1||gain_of_cell_id_to_move<0) break;
            else{
                //step 1: 確實移動該cell_id(更新partition和bucket_list(移動cell的pointer，max_gain_ptr也要))
                cur_cutsize-=gain_of_cell_id_to_move;
                move_cell_and_update_bucket_list_and_partition(cell_id_to_move,cell_set,net_set,my_bucket_list_class, partition_result);
                //step 2: 如果當前的cutsize比cur_best_cutsize好(更新cur_best_cutsize以及partition)
                if(cur_cutsize<=cur_best_cutsize){
                    //cout<<"cur_cutsize is samller than cur_best_cutsize. "<<endl;
                    cur_best_cutsize = cur_cutsize;
                    cur_best_partition = partition_result;
                    cur_best_bucket_list_class = assign_bucket_list_object(my_bucket_list_class,cell_set,net_set,partition_result);

                    //===for debug===
                    // cout<<"inner inner loop: "<<endl;
                    // cout<<"cur_best_partition: "<<endl;
                    // show_partition_result(net_set,cur_best_partition);
                    // cout<<"bucket list part A:"<<endl;
                    // show_bucket_list(cur_best_bucket_list_class.bucket_list_part_A.bucket_list);
                    // cout<<"bucket list part B:"<<endl;
                    // show_bucket_list(cur_best_bucket_list_class.bucket_list_part_B.bucket_list);
                    // cout<<endl;
                    //===============
                }
            }

            cout<<"Inner Loop cur_best_cutsize: "<<cur_best_cutsize<<" cur_cutsize: "<<cur_cutsize<<endl;
        }
        if(cur_best_cutsize<history_best_cutsize){ //不確定這樣的做法是不是最好
            //cout<<"cur_best_cutsize is smaller than history_best_cutsize. "<<endl;
            history_best_cutsize = cur_best_cutsize;
            history_best_partition = cur_best_partition;
            history_best_bucket_list_class = assign_bucket_list_object(cur_best_bucket_list_class,cell_set,net_set,cur_best_partition);
            
            //===for debug===
            // cout<<"Outer Loop: "<<endl;
            // cout<<"bucket list part A:"<<endl;
            // show_bucket_list(history_best_bucket_list_class.bucket_list_part_A.bucket_list);
            // cout<<"bucket list part B:"<<endl;
            // show_bucket_list(history_best_bucket_list_class.bucket_list_part_B.bucket_list);
            // cout<<endl;
            //===============
        }
        else break;
        
        cur_cutsize = history_best_cutsize;
        partition_result = history_best_partition;
        my_bucket_list_class = assign_bucket_list_object(history_best_bucket_list_class,cell_set,net_set, history_best_partition);  //assign的同時會解鎖
        
        //===for debug===
        // cout<<"my_bucket_list_class using assign: "<<endl;
        // cout<<"A:"<<endl;
        // show_bucket_list(my_bucket_list_class.bucket_list_part_A.bucket_list);
        // cout<<"B:"<<endl;
        // show_bucket_list(my_bucket_list_class.bucket_list_part_B.bucket_list);
        // cout<<endl;
        
        // cout<<"my_bucket_list_class using initialization: "<<endl;
        // my_bucket_list_class = initialize_bucket_list_object(cell_set,net_set,history_best_partition);
        // cout<<"A:"<<endl;
        // show_bucket_list(my_bucket_list_class.bucket_list_part_A.bucket_list);
        // cout<<"B:"<<endl;
        // show_bucket_list(my_bucket_list_class.bucket_list_part_B.bucket_list);
        // cout<<endl;
        //==============

        cout<<"history_best_cutsize: "<<history_best_cutsize<<" history_best_cutsize: "<<history_best_cutsize<<endl;
        ct++;
    }
    
    write_file("output.txt", history_best_cutsize,  history_best_partition);
    cout<<"final result: "<<history_best_cutsize<<endl;
}

int main(){
    unordered_map<int, struct cell_node> cell_set;  //以cell為單位，記錄所有cell的id以及所有相關的net
    unordered_map<int, struct net_node> net_set;    //以net為單位，記錄所有net的id以及所有相關的cell
    
    read_file("input.txt",cell_set,net_set);
    //show_cell_set(cell_set);
    //show_net_set(net_set);
    Fiduccia_Mattheyses(cell_set,net_set);
    
    return 0;
}
