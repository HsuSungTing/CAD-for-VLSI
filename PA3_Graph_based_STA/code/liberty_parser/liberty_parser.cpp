#include "liberty_parser.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>

using namespace std;

//===專門處理字串的function===
vector<double> extractAndSplit(string input){
    vector<double> result;
    size_t firstQuote = input.find('"');//找到第一個與第二個引號的位置
    size_t secondQuote = input.find('"', firstQuote + 1);
    if (firstQuote == string::npos || secondQuote == string::npos || secondQuote <= firstQuote){
        return result;
    }
    string extracted = input.substr(firstQuote + 1, secondQuote - firstQuote - 1);  //擷取引號之間的內容
    size_t start = 0;//以逗號分割
    size_t end;
    while ((end = extracted.find(',', start)) != string::npos) {
        result.push_back(stod(extracted.substr(start, end - start)));
        start = end + 1;
    }
    if (start < extracted.size()) {     //加入最後一個項目
        result.push_back(stod(extracted.substr(start)));
    }
    return result;
}

bool startsWithTrimmed(const string& line, const string& keyword) {
    size_t firstChar = line.find_first_not_of(" \t");
    if (firstChar == string::npos) return false;
    return line.substr(firstChar, keyword.length()) == keyword;
}

bool isClosingBrace(const string& line) {
    size_t firstChar=line.find_first_not_of(" \t");
    return firstChar!=string::npos && line[firstChar]=='}'&&line.find_first_not_of(" \t",firstChar+1)==string::npos;
}

vector<vector<string>> extractBlocks(const vector<string>& lines) {//依照cell_rise、cell_fall、rise_transition、fall_transition分成4個part
    vector<vector<string>> blocks;
    vector<string> currentBlock;
    bool inBlock = false;
    for (const auto& line : lines) {
        if(!inBlock&&(startsWithTrimmed(line,"cell_rise")||startsWithTrimmed(line,"cell_fall")||
            startsWithTrimmed(line,"rise_transition")||startsWithTrimmed(line,"fall_transition"))){
            inBlock = true;
            currentBlock.clear();
            currentBlock.push_back(line);
        } 
        else if(inBlock){
            currentBlock.push_back(line);
            if(isClosingBrace(line)){
                blocks.push_back(currentBlock);
                inBlock=false;
            }
        }
    }
    return blocks;
}

vector<vector<string>> readBlocksFromFile(const string& filename) {
    ifstream infile(filename);
    vector<vector<string>> blocks;
    vector<string> currentBlock;
    string line;
    if (!infile) {
        cerr << "Error opening file: " << filename << endl;
        return blocks;
    }
    while(getline(infile, line)){
        if (line.empty()) {
            if (!currentBlock.empty()) {
                blocks.push_back(currentBlock);
                currentBlock.clear();
            }
        } 
        else {
            currentBlock.push_back(line);
        }
    }
    if (!currentBlock.empty()) {// 加入最後一個 block（如果有）
        blocks.push_back(currentBlock);
    }
    return blocks;
}
//===========================

void build_index_vec(struct liberty_file &file, vector<vector<string>> result){
    vector<double> index_1_results=extractAndSplit(result[1][3]);
    vector<double> index_2_results=extractAndSplit(result[1][4]);
    for(int i=0;i<index_1_results.size();i++){
        file.index_1.push_back(index_1_results[i]);
    }
    for(int i=0;i<index_2_results.size();i++){
        file.index_2.push_back(index_2_results[i]);
    }
}

void build_NAND_TB(struct liberty_file &file, vector<string> result){
    file.NAND.A1_capcitance=0.00683597;
    file.NAND.A2_capcitance=0.00798456;
    vector<vector<string>> gate_results=extractBlocks(result);
    for(int i=0;i<gate_results.size();i++){
        for(int j=1;j<gate_results[i].size()-1;j++){
            vector<double> results;
            results=extractAndSplit(gate_results[i][j]);
            if(i==0)file.NAND.cell_rise_TB.push_back(results);
            else if(i==1)file.NAND.cell_fall_TB.push_back(results);
            else if(i==2)file.NAND.rise_transision_TB.push_back(results);
            else if(i==3)file.NAND.fall_transision_TB.push_back(results);
        }
    }
}

void build_NOR_TB(struct liberty_file &file, vector<string> result){
    file.NOR.A1_capcitance=0.0105008;
    file.NOR.A2_capcitance=0.0108106;
    vector<vector<string>> gate_results=extractBlocks(result);
    for(int i=0;i<gate_results.size();i++){
        for(int j=1;j<gate_results[i].size()-1;j++){
            vector<double> results;
            results=extractAndSplit(gate_results[i][j]);
            if(i==0)file.NOR.cell_rise_TB.push_back(results);
            else if(i==1)file.NOR.cell_fall_TB.push_back(results);
            else if(i==2)file.NOR.rise_transision_TB.push_back(results);
            else if(i==3)file.NOR.fall_transision_TB.push_back(results);
        }
    }
}

void build_INV_TB(struct liberty_file &file, vector<string> result){
    vector<vector<string>> gate_results=extractBlocks(result);
    file.INV.A1_capcitance=0.0109115;
    for(int i=0;i<gate_results.size();i++){
        for(int j=1;j<gate_results[i].size()-1;j++){
            vector<double> results;
            results=extractAndSplit(gate_results[i][j]);
            if(i==0)file.INV.cell_rise_TB.push_back(results);
            else if(i==1)file.INV.cell_fall_TB.push_back(results);
            else if(i==2)file.INV.rise_transision_TB.push_back(results);
            else if(i==3)file.INV.fall_transision_TB.push_back(results);
        }
    }
}

void show_liberty_file(struct liberty_file file){ //for debug
    cout<<"index 1: "<<endl;
    for(int i=0;i<file.index_1.size();i++){
        cout<<file.index_1[i]<<" ";
    }
    cout<<endl;
    cout<<"index 2: "<<endl;
    for(int i=0;i<file.index_2.size();i++){
        cout<<file.index_2[i]<<" ";
    }
    cout<<endl;

    cout<<"file.NOR.A1_capcitance: "<<file.NOR.A1_capcitance<<" file.NOR.A2_capcitance:"<<file.NOR.A2_capcitance<<endl;
    cout<<"file.NOR.cell_rise_TB: "<<endl;
    for(int i=0;i<file.NOR.cell_rise_TB.size();i++){
        for(int j=0;j<file.NOR.cell_rise_TB[i].size();j++){
            cout<<file.NOR.cell_rise_TB[i][j]<<" ";
        }
        cout<<endl;
    }
    cout<<"file.NOR.cell_fall_TB: "<<endl;
    for(int i=0;i<file.NOR.cell_fall_TB.size();i++){
        for(int j=0;j<file.NOR.cell_fall_TB[i].size();j++){
            cout<<file.NOR.cell_fall_TB[i][j]<<" ";
        }
        cout<<endl;
    }
    cout<<"file.NOR.rise_transision_TB: "<<endl;
    for(int i=0;i<file.NOR.rise_transision_TB.size();i++){
        for(int j=0;j<file.NOR.rise_transision_TB[i].size();j++){
            cout<<file.NOR.rise_transision_TB[i][j]<<" ";
        }
        cout<<endl;
    }
    cout<<"file.NOR.fall_transision_TB: "<<endl;
    for(int i=0;i<file.NOR.fall_transision_TB.size();i++){
        for(int j=0;j<file.NOR.fall_transision_TB[i].size();j++){
            cout<<file.NOR.fall_transision_TB[i][j]<<" ";
        }
        cout<<endl;
    }

    cout<<"file.INV.A1_capcitance: "<<file.INV.A1_capcitance<<" file.INV.A2_capcitance:"<<file.INV.A2_capcitance<<endl;
    cout<<"file.INV.cell_rise_TB: "<<endl;
    for(int i=0;i<file.INV.cell_rise_TB.size();i++){
        for(int j=0;j<file.INV.cell_rise_TB[i].size();j++){
            cout<<file.INV.cell_rise_TB[i][j]<<" ";
        }
        cout<<endl;
    }
    cout<<"file.INV.cell_fall_TB: "<<endl;
    for(int i=0;i<file.INV.cell_fall_TB.size();i++){
        for(int j=0;j<file.INV.cell_fall_TB[i].size();j++){
            cout<<file.INV.cell_fall_TB[i][j]<<" ";
        }
        cout<<endl;
    }
    cout<<"file.INV.rise_transision_TB: "<<endl;
    for(int i=0;i<file.INV.rise_transision_TB.size();i++){
        for(int j=0;j<file.INV.rise_transision_TB[i].size();j++){
            cout<<file.INV.rise_transision_TB[i][j]<<" ";
        }
        cout<<endl;
    }
    cout<<"file.INV.fall_transision_TB: "<<endl;
    for(int i=0;i<file.INV.fall_transision_TB.size();i++){
        for(int j=0;j<file.INV.fall_transision_TB[i].size();j++){
            cout<<file.INV.fall_transision_TB[i][j]<<" ";
        }
        cout<<endl;
    }

    cout<<"file.NAND.A1_capcitance: "<<file.NAND.A1_capcitance<<" file.NAND.A2_capcitance:"<<file.NAND.A2_capcitance<<endl;
    cout<<"file.NAND.cell_rise_TB: "<<endl;
    for(int i=0;i<file.NAND.cell_rise_TB.size();i++){
        for(int j=0;j<file.NAND.cell_rise_TB[i].size();j++){
            cout<<file.NAND.cell_rise_TB[i][j]<<" ";
        }
        cout<<endl;
    }
    cout<<"file.NAND.cell_fall_TB: "<<endl;
    for(int i=0;i<file.NAND.cell_fall_TB.size();i++){
        for(int j=0;j<file.NAND.cell_fall_TB[i].size();j++){
            cout<<file.NAND.cell_fall_TB[i][j]<<" ";
        }
        cout<<endl;
    }
    cout<<"file.NAND.rise_transision_TB: "<<endl;
    for(int i=0;i<file.NAND.rise_transision_TB.size();i++){
        for(int j=0;j<file.NAND.rise_transision_TB[i].size();j++){
            cout<<file.NAND.rise_transision_TB[i][j]<<" ";
        }
        cout<<endl;
    }
    cout<<"file.NAND.fall_transision_TB: "<<endl;
    for(int i=0;i<file.NAND.fall_transision_TB.size();i++){
        for(int j=0;j<file.NAND.fall_transision_TB[i].size();j++){
            cout<<file.NAND.fall_transision_TB[i][j]<<" ";
        }
        cout<<endl;
    }
}

void build_liberty_file(struct liberty_file &file){
    vector<vector<string>> result=readBlocksFromFile("test_lib.lib");
    build_index_vec(file, result);
    build_NOR_TB (file, result[2]);
    build_INV_TB (file, result[3]);
    build_NAND_TB(file, result[4]);
}

