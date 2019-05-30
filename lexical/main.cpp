#include <iostream>
#include<string>
#include <fstream>
#include <sstream>

using namespace std;
int num=0;
int line=1;

struct BIN{
    string num;
    int val;
};
string getFilePath(){
    string path="./eee.pas";
    return path;
}

string getFileContent(const string &file_path){
    ifstream input_file;
    input_file.open(file_path);
    if(!input_file){
        cout<<"打开文件错误！"<<endl;
    } else{
        stringstream content;
        content<<input_file.rdbuf();
        return content.str();
    }
}

char getNextChar(string content){
    while (content[num]==' '){
        num++;
    }
    return content[num];
}

bool isLetter(char letter){
    return (97 <= letter && letter <= 122) || (65 <= letter && letter <= 90);
}

bool isDigit(char letter){
    return (48 <= letter && letter <= 57);
}

int reserve(string token){
    if(token=="begin"){
        return 1;
    } else if(token=="if"){
        return 4;
    } else if(token=="function"){
        return 7;
    } else if(token=="end"){
        return 2;
    } else if(token=="then"){
        return 5;
    } else if(token=="read"){
        return 8;
    } else if(token=="integer"){
        return 3;
    } else if(token=="else"){
        return 6;
    } else if(token=="write"){
        return 9;
    } else
        //如果是标识符
        return 10;
}

BIN LexAnalyze(string content){
    string token;
    char next_char=getNextChar(content);
    if((97<=next_char&&next_char<=122)||(65<=next_char&&next_char<=90)){
        //当第一个字母是a-z或者A-Z的时候
        token.push_back(next_char);
        while (isLetter(content[++num])||isDigit(content[num])){
            token.push_back(content[num]);
        }
        num--;//回退
        int result =reserve(token);
        return {token,result};
    } else if(48 <= next_char && next_char <= 57){
        //如果第一个字符是1-9
        token.push_back(next_char);
        while (isDigit(content[++num])){
            token.push_back(content[num]);
        }
        num--;//回退
        return {token,11};
    } else if(next_char=='<'){
        num++;
        if(content[num]=='=')
            return {"<=",14};
        else if(content[num]=='>')
            return {"<>",13};
        else{
            num--;
            return {"<",15};
        }
    } else if(next_char=='>'){
        num++;
        if(content[num]=='=')
            return {">=",16};
        else if(content[num]=='<'){
            return {"error",-3};        //如果是><则出错
        }
        else{
            num--;
            return {">",17};
        }
    } else if(next_char==':'){
        num++;
        if(content[num]=='=')
            return {":=",20};
        else{
            num--;
            return {"error",-2};        //如果是:则出错
        }
    } else if(next_char==';'){
        return {";",23};
    } else if(next_char=='='){
        return {"=",12};
    } else if(next_char=='-'){
        return {"-",18};
    } else if(next_char=='*'){
        return {"*",19};
    } else if(next_char=='('){
        return {"(",21};
    } else if(next_char==')'){
        return {")",22};
    } else if(next_char=='\n'){
        line++;
        return {"EOLN",24};
    } else
        return {"error",-1};        //如果输入错误字符则出错
}

int main() {
    string file_path=getFilePath();
    string file_content=getFileContent(file_path);
    bool isError=false;
    string output_dyd;
    string output_err;
    string example="                "; //16个空格
    while (num<file_content.length()){
        BIN a=LexAnalyze(file_content);
        num++;
//        cout<<a.num<<" "<<a.val<<endl;

        //检测错误
        if(a.val<0){
            isError=true;
            output_err.append(example,0,12).append("LINE:").append(to_string(line))
                    .append("  ").append(to_string(abs(a.val))).append("\n");
        }

        if(!isError){
            //处理得到的二元组，to_string使用了c++11的新特性
            output_dyd.append(example,0,16-a.num.length()).append(a.num).append(" ");
            if(a.val<10){
                output_dyd.append("0").append(to_string(a.val)).append("\n");
            } else{
                output_dyd.append(to_string(a.val)).append("\n");
            }
        } else{
            output_dyd="";
        }
    }
    if(!isError)
        output_dyd.append(example,0,16-3).append("EOF").append(" ").append(to_string(25));
//    cout<<output_dyd;
//    cout<<output_err;

    //输出字符串内容到文件里面
    string output_path=file_path.substr(0,file_path.find_last_of("/\\")).append("\\");
    string file_name_fake = file_path.substr(file_path.find_last_of("/\\")+1);
    string file_name_true=file_name_fake.substr(0,file_name_fake.find_last_of('.'));
    ofstream dyd;
    dyd.open(output_path+file_name_true+".dyd",ios::out|ios::trunc);
    dyd<<output_dyd;
    dyd.close();

    ofstream err;
    err.open(output_path+file_name_true+".err",ios::out|ios::trunc);
    err<<output_err;
    err.close();
    return 0;
}