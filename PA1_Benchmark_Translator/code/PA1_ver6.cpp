#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <string>
#include <sstream>
#include <algorithm> // 用於 std::remove
//下次開始寫以前直接跟吳育承討論比較快，不然算法可能會出錯

//這個版本能處理N input gate，完成寫檔，不從gate name抓編號
using namespace std;

struct net_gate_relation{  //儲存gat5/IN1
    int gate_id; //幫gate編號方便net去對應
    string role; //只有三種可能 IN1,IN2,OUT1
};

struct net_node{
    int net_id;  
    string role;
    vector<struct net_gate_relation> relation_vector;
};

struct gate_node{
    string gate_type;  
    int gate_id;        //我們依然按照順序幫gate編號(從0開始)，為了方便net去對應
    int gate_class_id;  //這個gate在當前的類別中是編號幾，用hash table來數，所以hash table只用來數，不會參與最後輸出
    vector<int> gate_IO_vector{0,0};//分別是OUT1的net_id, IN1的net_id，如果有更多會再繼續擴張 
};

string removeCommas(const string& str) {
    string result = str;
    result.erase(remove(result.begin(), result.end(), ','), result.end());
    return result;
}

vector<string> splitString(const string& str){     //以空格切割字串後回傳
    vector<string> result;
    istringstream iss(str);
    string word;
    while (iss >> word) { // 以空格分割
        result.push_back(word);
    }
    return result;
}

string determine_role_type(string role){
    string sub_str=role.substr(0, 3);
    if(sub_str=="inp")return "input"; 
    else if(sub_str=="out")return "output"; 
    else return "wire"; 
}

struct net_gate_relation splitBySlash(const string& str,unordered_map<string,int> gate_name_to_id_hash_table) {
    vector<string> result;
    stringstream ss(str);
    string token;
    struct net_gate_relation relation;

    while (getline(ss, token, '/')) {
        result.push_back(token);
    }
    auto it =gate_name_to_id_hash_table.find(result[0]);    //直接用gate的名字去找
    relation.gate_id=it->second;
    relation.role=removeCommas(result[1]);
    
    return relation;
}

void construct_gate_name_to_id_hash_table(vector<string> gate_line,unordered_map<string,int> &gate_name_to_id_hash_table){
    for(int i=0;i<gate_line.size();i++){
        vector<string>result=splitString(gate_line[i]);
        gate_name_to_id_hash_table[result[1]]=i;   //名字去對應gate_id
    }
}

void construct_net_vector(vector<struct net_node>&net_vector, vector<string>& net_line, vector<string>& pin_line,unordered_map<string,int> gate_name_to_id_hash_table){
    for(int i=0;i<net_line.size();i++){
        struct net_node my_net_node;

        //這邊的net_line[i]和pin_line[i]都還沒詳細切割
        vector<string> net_line_vec=splitString(net_line[i]);
        vector<string> pin_line_vec=splitString(pin_line[i]);
        string net_id_str = net_line_vec[1].substr(3);//從index 3開始取
        my_net_node.net_id=stoi(net_id_str);
        my_net_node.role=determine_role_type(pin_line_vec[1]);

        if(my_net_node.role=="wire"){
            for(int i=1;i<pin_line_vec.size();i++){
                struct net_gate_relation relation=splitBySlash(pin_line_vec[i],gate_name_to_id_hash_table);
                
                my_net_node.relation_vector.push_back(relation);
            }
        }
        else{
            for(int i=2;i<pin_line_vec.size();i++){
                struct net_gate_relation relation=splitBySlash(pin_line_vec[i],gate_name_to_id_hash_table);
                my_net_node.relation_vector.push_back(relation);
            }
        }
        net_vector.push_back(my_net_node);
    }
}

int role_check(string role){        //確認當前的net是OUT1(0)還是IN1(1)、IN2(2)、IN3(3)或之類的
    string role_str=role.substr(0,2);
    if(role_str=="IN"&&role.length()>2){
        return stoi(role.substr(2));
    }
    else return 0;
}
int count_gate_class_id(string gate_type,unordered_map<string, int> &hash_table){
    if (hash_table.find(gate_type) != hash_table.end()) {   //代表當前的gate種類已經存在
        hash_table[gate_type]= hash_table[gate_type]+1;
        return hash_table[gate_type];
    } 
    else {
        hash_table[gate_type]=1;
        return 1;
    }
}

void construct_gate_vector(vector<struct gate_node>&gate_vector,vector<struct net_node>net_vector,vector<string> gate_line){
    unordered_map<string, int> gate_class_hash_table;  //紀錄每種gate出現過幾次
    int gate_id_counter=0;
    for(int i=0;i<gate_line.size();i++){
        struct gate_node my_gate_node;
        vector<string> gate_line_vec=splitString(gate_line[i]);

        my_gate_node.gate_id=gate_id_counter;  //gate id不需要用gate的名字來取，直接用計數的編號就好
        my_gate_node.gate_type=gate_line_vec[2];
        my_gate_node.gate_class_id=count_gate_class_id(my_gate_node.gate_type, gate_class_hash_table);

        gate_vector.push_back(my_gate_node);
        gate_id_counter++;
    }
    
    //把net_vector放入gate_vector中
    for(int i=0;i<net_vector.size();i++){       //未必會是2 inputs，可以是1-N，如果不夠大，gate_vector[net_vector[i].relation_vector[j].gate_id-1].gate_IO_vector要先resize
        //cout<<"net_id: "<<net_vector[i].net_id<<" role: "<<net_vector[i].role<<endl;
        
        for(int j=0;j<net_vector[i].relation_vector.size();j++){    //因為gate_id是連續的才能直接把gate_id當gate_vector的index
            int role_index=role_check(net_vector[i].relation_vector[j].role);
            //OUT1的role_index=0, IN1的role_index=1, IN2的role_index=2, IN3的role_index=3,
            if(role_index+1>gate_vector[net_vector[i].relation_vector[j].gate_id].gate_IO_vector.size()){
                gate_vector[net_vector[i].relation_vector[j].gate_id].gate_IO_vector.resize(role_index+1, 0);
            }
            gate_vector[net_vector[i].relation_vector[j].gate_id].gate_IO_vector[role_index]=net_vector[i].net_id;
        }
    }
}

class ouput_object {
    public: //在main中也可以被access
        vector<int> input_net_id;
        vector<int> output_net_id;
        vector<int> wire_net_id;
        vector<struct gate_node> gate_vector; //紀錄.v 的gate level描述，包含gate的種類、編號以及

        ouput_object(vector<struct net_node>net_vector,vector<string> gate_line){
            //建構input_net_id、output_net_id、wire_net_id
            for(int i=0;i<net_vector.size();i++){
                if(net_vector[i].role=="input")input_net_id.push_back(net_vector[i].net_id);
                else if(net_vector[i].role=="output")output_net_id.push_back(net_vector[i].net_id);
                else wire_net_id.push_back(net_vector[i].net_id);
            }

            //建構vector<struct gate_node> gate_vector
            construct_gate_vector(gate_vector,net_vector,gate_line);
        }
};

void read_file(string inputFilename, string& first_line, vector<string>& gate_line, vector<string>& net_line, vector<string>& pin_line){
    ifstream inputFile(inputFilename);
    if (!inputFile) {
        cerr<<"can't open input file: "<<inputFilename<<endl;
        return;
    }

    string line;
    bool found_first_line = false;
    bool found_first_empty = false;
    bool found_second_empty = false;
    bool Net_or_Pin=true;   //true代表當前要存Net

    while (getline(inputFile, line)) {
        if (!found_first_line) {
            first_line = line;
            found_first_line = true;
        } 
        else {
            //這行也改一下
            if (line.empty()){             // **這次正確判斷空行**
                if (!found_first_empty) {
                    found_first_empty = true;
                } 
                else {
                    found_second_empty = true;
                }
            } 
            else {
                if (found_first_empty==true&&found_second_empty==false) {
                    gate_line.push_back(line);
                } 
                else if (found_first_empty==true&&found_second_empty==true) {
                    if(Net_or_Pin==true)net_line.push_back(line);
                    else pin_line.push_back(line);
                        
                    Net_or_Pin=!Net_or_Pin;
                }
            }
        }
    }
    inputFile.close();
}

string module_declaration(string module_name,vector<int> input_net_id,vector<int> output_net_id){
    string ans="module "+module_name+" (";
    for(int i=0;i<input_net_id.size();i++){
        ans=ans+"N"+to_string(input_net_id[i])+",";
    }
    for(int i=0;i<output_net_id.size();i++){
        ans=ans+"N"+to_string(output_net_id[i]);
        if(i!=output_net_id.size()-1)ans+=",";//如果不是最後的net要記得加逗號
    }
    ans+=");";
    return ans;
}

string IO_or_Wire_declaration(string net_type,vector<int> net_id){
    string ans=net_type+" ";
    for(int i=0;i<net_id.size();i++){
        ans=ans+"N"+to_string(net_id[i]);
        if(i!=net_id.size()-1)ans+=",";//如果不是最後的net要記得加逗號
    }
    ans+=";";
    return ans;
}
string to_uppercase(string input_str){
    transform(input_str.begin(), input_str.end(), input_str.begin(), ::toupper);
    return input_str;
}

string gate_declaration(struct gate_node my_gate_node){ //未必會是2 inputs，可以是1-N
    string ans=my_gate_node.gate_type+" ";
    string nums_of_gate_input=to_string(my_gate_node.gate_IO_vector.size()-1);

    ans=ans+to_uppercase(my_gate_node.gate_type)+nums_of_gate_input+"_"+to_string(my_gate_node.gate_class_id)+" (";
    for(int i=0;i<my_gate_node.gate_IO_vector.size();i++){
        ans=ans+"N"+to_string(my_gate_node.gate_IO_vector[i]);
        if(i!=my_gate_node.gate_IO_vector.size()-1)ans+=", ";
        else ans+=");";
    }
    return ans;
}

void write_file(string outputFilename,ouput_object ouput_object_PA1,string first_line){
    ofstream outputFile(outputFilename);
    if (!outputFile) {
        cerr << "can't open output file: " << outputFilename << endl;   
    }
    // 寫入第一行`timescale 1ns/1ps
    outputFile <<"`timescale 1ns/1ps"<<endl;

    // 寫入第二行 module c17 (N1,N2,N3,N6,N7,N22,N23); 
    vector<string>first_line_vec=splitString(first_line);
    string row_2=module_declaration(first_line_vec[5],ouput_object_PA1.input_net_id,ouput_object_PA1.output_net_id);
    outputFile <<row_2<<endl;
    
    //寫入第三到第五行
    string row_3=IO_or_Wire_declaration("input",ouput_object_PA1.input_net_id);
    outputFile <<row_3<<endl;

    string row_4=IO_or_Wire_declaration("output",ouput_object_PA1.output_net_id);
    outputFile <<row_4<<endl;

    string row_5=IO_or_Wire_declaration("wire",ouput_object_PA1.wire_net_id);
    outputFile <<row_5<<endl;

    //寫入gate-level
    for(int i=0;i<ouput_object_PA1.gate_vector.size();i++){
        string row=gate_declaration(ouput_object_PA1.gate_vector[i]);
        outputFile <<row<<endl;
    }
    outputFile <<"endmodule"<<endl;

    outputFile.close(); // 關閉檔案
    cout << "writeback successfully " << endl;
}

int main() {
    string first_line;          //儲存input.txt line
    vector<string> gate_line;   //儲存input.txt line
    vector<string> net_line;    //儲存input.txt line
    vector<string> pin_line;    //儲存input.txt line
    
    unordered_map<string,int> gate_name_to_id_hash_table; //開一個hash table去統計所有gate對應的gate_id，construct_net_vector()會用到
    vector<struct net_node>net_vector;

    read_file("input.txt",first_line,gate_line,net_line,pin_line);
    construct_gate_name_to_id_hash_table(gate_line,gate_name_to_id_hash_table);
    
    // for (const auto& pair : gate_name_to_id_hash_table) {
    //     cout << pair.first << " -> " << pair.second << endl;
    // }
    
    construct_net_vector(net_vector,net_line,pin_line,gate_name_to_id_hash_table); //建構net與
    
    // for(int i=0;i<net_vector.size();i++){
    //     cout<<"net_id: "<<net_vector[i].net_id<<" role: "<<net_vector[i].role<<endl;
    //     for(int j=0;j<net_vector[i].relation_vector.size();j++){
    //         cout<<"{"<<net_vector[i].relation_vector[j].gate_id<<": "<<net_vector[i].relation_vector[j].role<<"}"<<endl;
    //     }
    // }

    ouput_object ouput_object_PA1(net_vector, gate_line);//可以再把hash table丟進去
    write_file("output.txt",ouput_object_PA1,first_line);
    
    return 0;
}
