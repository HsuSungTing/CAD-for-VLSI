#include <iostream>
#include <fstream>
#include <string>
#include <cctype>
#include <vector>

using namespace std;

//===以下是read RTL code的function===
bool check_two_slash(const string line){
    size_t i = 0;
    while (i < line.size() && isspace(line[i])) {// 跳過開頭空白
        ++i;
    }
    return (i+1<line.size()&&line[i]=='/' && line[i+1]=='/');// 檢查是否為 //
}

bool check_slash_star(const string line){
    size_t i = 0;
    while (i < line.size() && isspace(line[i])) {// 跳過開頭空白
        ++i;
    }
    return (i+1<line.size()&&line[i]=='/' && line[i+1]=='*');// 檢查是否為 /*
}

bool endsWithBlockCommentEnd(string line){
    if (line.length()<2) return false;
    return line[line.length()- 2]=='*'&& line[line.length() - 1] == '/';
}


bool is_blank_line(const string line){
    for(char ch : line) {
        if(!isspace(static_cast<unsigned char>(ch))) {
            return false;   //發現非空白字元，代表不是空行
        }
    }
    return true; // 全部都是空白
}

vector<string> read_RTL(const string filename){    //切comment的部分我真的寫得蠻好的
    ifstream file(filename);
    vector<string> lines;
    string line;
    if (!file) {
        cout << "Failed to open file: " << filename << endl;
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
    for(int i=0;i<lines.size();i++){
        cout<<lines[i]<<endl;
    }
    file.close();
    return lines;
}

int main(){
    read_RTL("c17.v");
    return 0;
}