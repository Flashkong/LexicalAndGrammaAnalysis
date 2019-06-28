#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include "GradientDescentAnalyzer.cpp"
#define MAX 100

using namespace std;

string getFilePath(){
    string path="eee.dyd";
    return path;
}

string getFileContent(const string &file_path,vector<string>& list){
    ifstream input_file;
    input_file.open(file_path);
    string total;
    if(!input_file){
        cout<<"无法打开输入文件！"<<endl;
    } else{
        string line;
        while(getline(input_file, line))
        {
            list.push_back(line);
            total+=line+"\n";
        }
    }
    return total;
}

void outPutFile(string content , string file_name){
    ofstream out_file;
    out_file.open(file_name,ios::out|ios::trunc);
    if(out_file.is_open()){
        out_file<<content;
        out_file.close();
    } else{
        cout<<"写入文件时错误！"<<endl;
    }
}

int main() {
    GradientDescentAnalyzer analyzer;
    string dyd_file_path=getFilePath();
    string dyd_content = getFileContent(dyd_file_path,analyzer.list);

    analyzer.GrammaAnalysis();

    outPutFile(analyzer.varMessage,"eee.var");
    outPutFile(analyzer.funMessage,"eee.pro");
    outPutFile(analyzer.errorMessage,"eee.err");
    outPutFile(dyd_content,"eee.dys");
    return 0;
}
