#include <iostream>
#include <fstream>
#include <vector>
#include <string>

using namespace std;


void read_file(string inputFilename, string& first_line, vector<string>& gate_block, vector<string>& net_block) {
    ifstream inputFile(inputFilename);
    if (!inputFile) {
        cerr << "can't open input file: " << inputFilename << endl;
        return;
    }

    string line;
    bool found_first_line = false;
    bool found_first_empty = false;
    bool found_second_empty = false;

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
                    //cout<<"here 2"<<endl;
                    found_second_empty = true;
                }
            } 
            else {
                if (found_first_empty==true&&found_second_empty==false) {
                    gate_block.push_back(line);
                } 
                else if (found_first_empty==true&&found_second_empty==true) {
                    net_block.push_back(line);
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
    string first_line;
    vector<string> gate_block;
    vector<string> net_block;
    vector<string> pin_block; 
    
    read_file("input.txt",first_line,gate_block,net_block);
    cout<<"first_line: "<<first_line<<endl;

    for(int i=0;i<gate_block.size();i++){
        cout<<gate_block[i]<<endl;
    }

    for(int i=0;i<net_block.size();i++){
        cout<<net_block[i]<<endl;
    }
    

    return 0;
}
