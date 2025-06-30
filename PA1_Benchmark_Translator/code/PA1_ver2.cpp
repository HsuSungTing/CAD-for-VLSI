#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm> // 用於 std::remove
//這個版本可以順利建構net_vector，只差放到輸出格式再寫檔

using namespace std;

struct net_gate_relation{  //儲存gat5/IN1
    int gate_id;
    string role;//只有三種可能 IN1,IN2,OUT1
};

struct net_node{
    int net_id;  
    string role;
    vector<struct net_gate_relation> relation_vector;
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

struct net_gate_relation splitBySlash(const string& str) {
    vector<string> result;
    stringstream ss(str);
    string token;
    struct net_gate_relation relation;

    while (getline(ss, token, '/')) {
        result.push_back(token);
    }
    result[0]=result[0].substr(3);
    relation.gate_id=stoi(result[0]);
    relation.role=removeCommas(result[1]);

    return relation;
}

void construct_net_vector(vector<struct net_node>&net_vector, vector<string>& net_line, vector<string>& pin_line){
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
                struct net_gate_relation relation=splitBySlash(pin_line_vec[i]);
                my_net_node.relation_vector.push_back(relation);
            }
        }
        else{
            for(int i=2;i<pin_line_vec.size();i++){
                struct net_gate_relation relation=splitBySlash(pin_line_vec[i]);
                my_net_node.relation_vector.push_back(relation);
            }
        }
        net_vector.push_back(my_net_node);
    }
}

void read_file(string inputFilename, string& first_line, vector<string>& gate_line, vector<string>& net_line, vector<string>& pin_line) {
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
            //cout<<"line"<<line<<endl;
            if (line.empty()) { // **這次正確判斷空行**
                if (!found_first_empty) {
                    //cout<<"here 1"<<endl;
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

void write_file(string outputFilename,vector<string> lines){
    ofstream outputFile(outputFilename);
    if (!outputFile) {
        cerr << "can't open output file: " << outputFilename << endl;   
    }
    // 寫入新的檔案
    for (const auto& str : lines) {
        outputFile << str << endl;
    }

    outputFile.close(); // 關閉檔案
    cout << "writeback successfully " << outputFilename << endl;
}

int main() {
    string first_line;          //儲存input.txt line
    vector<string> gate_line;   //儲存input.txt line
    vector<string> net_line;    //儲存input.txt line
    vector<string> pin_line;    //儲存input.txt line
    
    vector<struct net_node>net_vector;

    read_file("input.txt",first_line,gate_line,net_line,pin_line);
    
    construct_net_vector(net_vector,net_line,pin_line);

    for(int i=0;i<net_vector.size();i++){
        cout<<"net_id: "<<net_vector[i].net_id<<" role: "<<net_vector[i].role<<endl;
        for(int j=0;j<net_vector[i].relation_vector.size();j++){
            cout<<"{"<<net_vector[i].relation_vector[j].gate_id<<": "<<net_vector[i].relation_vector[j].role<<"}"<<endl;
        }
    }

    return 0;
}
