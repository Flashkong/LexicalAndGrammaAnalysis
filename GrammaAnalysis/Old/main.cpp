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

void getFileContent(const string &file_path,vector<string>& list){
    ifstream input_file;
    input_file.open(file_path);
    if(!input_file){
        cout<<"打开文件错误！"<<endl;
    } else{
        string line;
        while(getline(input_file, line))
        {
            list.push_back(line);
        }
    }
}

void outPutFile(string content , string file_name){
    ofstream out_file;
    out_file.open(file_name,ios::out|ios::trunc);
    if(out_file.is_open()){
        out_file<<content;
        out_file.close();
    } else{
        cout<<"写文件失败！"<<endl;
    }
}

int main() {
    GradientDescentAnalyzer analyzer;
    string dyd_file_path=getFilePath();
    getFileContent(dyd_file_path,analyzer.list);

    analyzer.GrammaAnalysis();

    outPutFile(analyzer.varMessage,"eee.var");
    outPutFile(analyzer.funMessage,"eee.pro");
    outPutFile(analyzer.errorMessage,"eee.err");
    return 0;
}
