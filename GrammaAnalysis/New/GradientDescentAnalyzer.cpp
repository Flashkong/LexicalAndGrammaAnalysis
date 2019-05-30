#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <stack>
#include <algorithm>

using namespace std;

/*
 * 问题统计:todo 错误说明那里要加一个行号说明第几行出了问题
 * todo 把结果写到那两个文件中
 */

/*
 * 单词符号与种别对照表
 */
#define BEGIN_CODE 1
#define END_CODE 2
#define  INTEGER_CODE 03
#define  IF_CODE 4
#define  THEN_CODE 5
#define  ELSE_CODE 6
#define  FUNCTION_CODE 7
#define  READ_CODE 8
#define  WRITE_CODE 9
#define  IDENTIFIER_CODE 10
#define  CONSTANT_CODE 11
#define  EQUAL 12
#define  NOT_EQUAL 13
#define  LESS_EQUAL 14
#define  LESS 15
#define  MORE_EQUAL 16
#define  MORE 17
#define  MINUS_CODE 18
#define  MUL_CODE 19
#define  ASSIGN_CODE 20
#define  LEFT_BRACKET_CODE 21
#define  RIGHT_BRACKET_CODE 22
#define  SEMI_CODE 23  // ;号
#define  EOLN 24
#define  EOFF 25

/*
 * 设置一个variable的声明和使用的标识
 */
#define DECLARE 1
#define USE 0

/*
 * 标记标识符到底是变量的标识符还是函数的标识符
 */
#define ISVARIABLE 0
#define ISFUNCTION 1
#define ISPARAMETER 2

/*
 * 标记搜索变量表的结果
 */
#define HAVEANDPRA 2
#define HAVEANDVAR 1
#define DONTHAVE 0

struct Var{
    string name;
    string functionName;
    int kind=0;
    string type;
    int level=0;
    int position=0;
};

struct Fun{
    string name;
    string type;  //过程的类型根据文法的定义，它只能是integer
    int level=0;
    vector<int> positions;
    int first;
    int last;
};

class GradientDescentAnalyzer {

private:
    /*
     * sym代表的是当前的字符（包含前面的空格）
     * sum_code代表的是当前字符的种别
     * num是指向单词符号的指针
     */
    string sym="";
    int sym_code=0;
    int num=0;
    /*
     * 设置一个变量FunLevel来记录过程层次
     */
    int FunLevel = 0;
    /*
     * 设置一个functionDefine堆栈来记录过程
     * main 表示是最外层的过程名字
     */
    stack<string> functionDefine;
    /*
     * 设置一个变量，记录当前的行数是多少
     */
    int line=1;
    /*
     * 变量表和过程表
     */
    vector<Var> varList;
    vector<Fun> funList;

public:
    vector<string>list;

    /*
     * 设置一个errorMessage，记录错误信息
     */
    string errorMessage;
    /*
     * 设置varmessage用来输出Var变量表的信息
     */
    string varMessage;
    /*
     * 设置funmessage用来输出fun过程表的信息
     */
    string funMessage;

    void getSym(){
        this->sym_code=decodeSymCode(this->num);
        this->sym=list[num].substr(0,16);
        this->sym=trim(sym);
        if(this->sym_code==EOLN){
            this->line++;
            this->num++;
            getSym();
        }
    }

    int decodeSymCode(int n){
        return atoi(list[n].substr(list[n].length()-2,2).c_str());
    }

    void movePointer(){
        this->num++;
    }

    void advance(){
        /*
         * 规定只有在遇到终结符之后才会调用advance函数来移动指针
         */
        movePointer();
        getSym();
    }

    template <typename T>
    int isInItVar2(vector<T> list,string s){
        for(auto &iter:list){
            if(iter.level==this->FunLevel){
                if(iter.name==s){
                    if(iter.kind==0){
                        return HAVEANDVAR;
                    }
                    else
                        return HAVEANDPRA;
                }
            }
        }
        return DONTHAVE;
    }

    template <typename T>
    bool isInItUse(vector<T> list,string s){
        for(auto &iter:list){
            if(iter.level==this->FunLevel){
                if(iter.name==s){
                    return true;
                }
            }
        }
        return false;
    }

    template <typename T>
    bool isInItPra(vector<T> list,string s){
        for(auto &iter:list){
            if(iter.level==this->FunLevel){
                if(iter.name==s){
                    if(iter.kind==1){
                        return HAVEANDPRA;
                    }
                    else
                        return HAVEANDVAR;
                }
            }
        }
        return DONTHAVE;
    }

    template <typename T>
    bool isInItFun(vector<T> list,string s){
        for(auto &iter:list){
            if(iter.name==s){
                return true;
            }
        }
        return false;
    }



    string& trim(string &s)
    {
        if (s.empty())
        {
            return s;
        }
        s.erase(0,s.find_first_not_of(" "));
        s.erase(s.find_last_not_of(" ") + 1);
        return s;
    }

    void GrammaAnalysis(){

        this->functionDefine.push("main");

        //加入函数表里面
        Fun F;
        F.name="main";
        //过程的类型
        F.type="integer";
        //过程的层次
        F.level=this->FunLevel;
        funList.push_back(F);

        /* 从这里开始进行语法分析
         * 首先是对于文法的第一步，<程序>→<分程序>
         * <分程序>使用函数，childProcedure来表示
         * 根据递归下降分析法，则要调用childProcedure
        */
        chileProcedure();

        //遍历获取函数表中第一个变量和最后一个变量在变量表中的位置
        getVarInFun();
        //将变量表的结果和过程表的结果转化为字符串
        toMessage();
    }

    void chileProcedure(){
        /*
         * 这里跳转到了<分程序>这里
         * <分程序>→begin <说明语句表>;<执行语句表> end
         *
         * <说明语句表>使用函数statementTable来表示
         * <执行语句表>使用函数executionTable来表示
         */
        getSym();
        if(sym_code==BEGIN_CODE){
            //移动指针
            advance();
            statementTable();
            if(sym_code==SEMI_CODE){
                //移动指针
                advance();
                executionTable();
                if(sym_code==END_CODE){
                    //语法分析就结束了
                } else{
                    error("缺少end");
                }
            } else{
                error("缺少';'号");
            }
        } else{
            error("缺少begin");
        }
    }

    void statementTable(){
        /*
         * 这里跳转到了<说明语句表>这里
         * <说明语句表>→<说明语句>│<说明语句表>;<说明语句>
         * 在这里出现了左递归，要消除左递归，消除左递归之后的产生式如下:
         *
         * <说明语句表>→<说明语句><说明语句表___>
         * <说明语句表___>→;<说明语句><说明语句表___>│ε
         *
         * <说明语句表___>使用函数statementTable___函数来表示
         * <说明语句>使用函数statement函数来表示
         */
        statement();
        statementTable___();
    }

    void statementTable___(){
        /*
         * 这里跳转到了<说明语句表___>这里
         * <说明语句表___>→;<说明语句><说明语句表___>│ε
         *
         * <说明语句>使用函数statement函数来表示
         *
         * 由于statementTable函数执行结束有一个;  那么这里的sym一定就是;  这时候预读取下一个sym，判断还是不是说明语句
         */
        int a=num+1;
        while (decodeSymCode(a)==EOLN){
            a++;
        }
        if(decodeSymCode(a)==INTEGER_CODE){
            advance();
            statement();
            statementTable___();
        }//else的话就匹配了ε
    }

    void statement(){
        /*
         * 这里跳转到了<说明语句>这里:
         * <说明语句>→<变量说明>│<函数说明>
         * <变量说明>→integer <变量>
         * <函数说明>→integer function <标识符>(<参数>);<函数体>
         *
         * 可以将这些合并，合并后结果如下:
         * <说明语句>→integer <说明语句___>
         * <说明语句___>→function <标识符>(<参数>);<函数体>│<变量>
         *
         * <说明语句___>使用函数statement___来表示
         */
        if(sym_code==INTEGER_CODE){
            advance();
            statement___();
        } else{
            error("说明语句缺少integer");
        }
    }

    void statement___(){
        /*
         * 这里跳转到了<说明语句___>这里:
         * <说明语句___>→function <标识符>(<参数>);<函数体>│<变量>
         *
         * <标识符>使用函数identifier来表示
         * <参数>使用函数parameter来表示
         * <函数体>使用函数functionBody来表示
         * <变量>使用函数variable来表示
         */
        if(sym_code==FUNCTION_CODE){

            //进入函数的标识符的时候Funlevel就让他+1
            this->FunLevel+=1;

            advance();
            identifier(DECLARE,ISFUNCTION);
            if(sym_code==LEFT_BRACKET_CODE){
                advance();
                parameter();
                if(sym_code==RIGHT_BRACKET_CODE){
                    advance();
                    if(sym_code==SEMI_CODE){
                        advance();
                        functionBody();
                    } else{
                        error("函数声明中缺少分号");
                    }
                } else{
                    error("函数声明中缺少右括号");
                }
            } else{
                error("函数声明中缺少左括号");
            }
        } else{
            //error("函数声明中缺少function保留字");
            //todo 这里有问题，如果错误的把function少些了一个字母，那么就匹配不到function转而去匹配变量，如果变量也没有匹配成功，
            // 那么我给的错误提示就出错了
            variable(DECLARE,ISVARIABLE);
        }
    }

    void identifier(int mark,int fromPos){
        /*
         * 这里跳转到了<标识符>这里:
         * <说明语句___>→function <标识符>(<参数>);<函数体>│<变量>
         *
         * <标识符>使用函数identifier来表示
         * <参数>使用函数parameter来表示
         * <函数体>使用函数functionBody来表示
         * <变量>使用函数variable来表示
         */
        if(mark==DECLARE){
            /*
             * 这时候表明此时指针所指的那个标识符是声明的时候的标识符
             * 此时应该区分标识符是变量标识符还是函数标识符
             * 并且将他们分别加入到对应的表当中
             */
            if(fromPos==ISVARIABLE){
                /*
                 * 首先要检测是不是表里面已经存在了这个变量
                 */
                if(isInItVar2(varList,sym)==HAVEANDVAR||isInItVar2(varList,sym)==HAVEANDPRA){
                    //如果在表里面存在，并且存在的那个是变量或者是参数，那么就是变量重复声明
                    error(sym+"变量重复声明");
                    advance();
                } else {
                    //加入变量表里面
                    Var v;
                    v.name = sym;     //我在存的时候就也把前面的那些补位的空格都存下来了
                    //变量的类型
                    v.type="integer";
                    //所属过程名字
                    v.functionName=this->functionDefine.top();
                    //变量还是形参
                    v.kind=0;
                    //变量的层次
                    v.level=this->functionDefine.size()-1;
                    v.position = varList.size();
                    varList.push_back(v);
                    advance();
                }
            } else if(fromPos==ISFUNCTION){
                if(isInItFun(funList,sym)){
                    error(sym+"函数重复声明");
                    advance();
                } else{
                    //加入函数表里面
                    Fun F;
                    F.name=sym;     //我在存的时候就也把前面的那些补位的空格都存下来了
                    //过程的类型
                    F.type="integer";
                    //过程的层次
                    F.level=this->FunLevel;
                    //第一个变量在变量表中的位置
                    //最后一个变量在变量表中的位置
                    funList.push_back(F);

                    advance();
                }
            } else if(fromPos==ISPARAMETER){
                /*
                 * 首先要检测是不是表里面已经存在了这个变量
                 */
                if(isInItPra(varList,sym)==HAVEANDPRA||isInItPra(varList,sym)==HAVEANDVAR){
                    error(sym+"参数重复声明");
                    advance();
                } else {
                    //加入变量表里面
                    Var v;
                    v.name = sym;     //我在存的时候就也把前面的那些补位的空格都存下来了
                    //变量的类型
                    v.type="integer";
                    //所属过程名字
                    v.functionName=this->functionDefine.top();
                    //变量还是形参
                    v.kind=1;
                    //变量的层次
                    v.level = this->functionDefine.size()-1;
                    v.position = varList.size();
                    varList.push_back(v);
                    advance();
                }
            }
        } else if(mark==USE){
            /*
             * 这时候是使用这个变量
             * 要检查变量表当中是否有这个变量，即这个变量是否声明了
             */
            if(fromPos==ISVARIABLE){
                if(!isInItUse(varList,sym)){
                    error(sym+"变量未声明");
                    advance();
                } else
                    advance();
            } else if(fromPos==ISFUNCTION){
                if(!isInItFun(funList,sym)){
                    error(sym+"函数未定义");
                    advance();
                } else
                    advance();
            }
        }
    }

    void parameter(){
        /*
         * 这里跳转到了<参数>这里:
         * <参数>→<变量>
         *
         * <变量>使用函数variable来表示
         */

        this->functionDefine.push(this->funList[funList.size()-1].name);

        variable(DECLARE,ISPARAMETER);
    }

    void functionBody(){
        /*
         * 这里跳转到了<函数体>这里:
         * <函数体>→begin <说明语句表>;<执行语句表> end
         *
         * <说明语句表>使用函数statementTable来表示
         * <执行语句表>使用函数executionTable来表示
         */
        if(sym_code==BEGIN_CODE){
            advance();
            statementTable();
            if(sym_code==SEMI_CODE){
                advance();
                executionTable();
                if(sym_code==END_CODE){
                    /*
                     * 出函数体的时候就-1
                     */
                    this->FunLevel-=1;
                    this->functionDefine.pop();
                    advance();
                } else{
                    error("函数体缺少end");
                }
            } else{
                error("函数体中缺少分号");
            }
        } else{
            error("函数体缺少begin");
        }
    }

    void executionTable(){
        /*
         * 这里跳转到了<执行语句表>这里
         * <执行语句表>→<执行语句>│<执行语句表>;<执行语句>
         *
         * 这里出现了左递归，需要消除左递归:
         * <执行语句表>→<执行语句><执行语句表___>
         * <执行语句表___>→;<执行语句><执行语句表___>│ε
         *
         * <说明语句表___>使用函数executionTable___来表示
         * <执行语句>使用函数execution来表示
         */
        execution();
        executionTable___();
    }

    void executionTable___(){
        /*
         * 这里跳转到了<执行语句表___>这里
         * <执行语句表___>→;<执行语句><执行语句表___>│ε
         *
         * <说明语句表___>使用函数executionTable___来表示
         * <执行语句>使用函数execution来表示
         */
        if(sym_code==SEMI_CODE){
            advance();
            execution();
            executionTable___();
        }//ε就不用处理了
    }

    void execution(){
        /*
         * 这里跳转到了<执行语句>这里
         * <执行语句>→<读语句>│<写语句>│<赋值语句>│<条件语句>
         *
         * <读语句>→read(<变量>)
         * <写语句>→write(<变量>)
         * <条件语句>→if<条件表达式>then<执行语句>else <执行语句>
         * 因此根据read,write,if来判断
         *
         * <读语句>使用函数readStatement来表示
         * <写语句>使用函数writeStatement来表示
         * <条件语句>使用函数ifStatement来表示
         * <赋值语句>使用函数assignmentStatement来表示
         *
         * 在这里当判断是read的时候不进行advance操作
         * 当进入readStatement函数之后再判断一次是否是read
         */
        if(sym_code==READ_CODE){
            //不执行advance()
            readStatement();
        } else if(sym_code==WRITE_CODE){
            //不执行advance()
            writeStatement();
        } else if(sym_code==IF_CODE){
            //不执行advance()
            ifStatement();
        } else{
            //进行赋值语句的匹配
            assignmentStatement();
        }
    }

    void readStatement(){
        /*
         * 这里跳转到了<读语句>这里
         * <读语句>→read(<变量>)
         *
         * <变量>使用函数variable来表示
         */
        if(sym_code==READ_CODE){
            advance();
            if(sym_code==LEFT_BRACKET_CODE){
                advance();
                variable(USE,ISVARIABLE);
                if(sym_code==RIGHT_BRACKET_CODE){
                    advance();
                } else{
                    error("read缺少右括号");
                }
            } else{
                error("read缺少左括号");
            }
        } else{
            error("缺少read");
        }
    }

    void variable(int mark,int temp){
        /*
         * 这里跳转到了<变量>这里:
         * <变量>→<标识符>
         *
         * <标识符>使用函数identifier来表示
         */
        identifier(mark,temp);
    }

    void writeStatement(){
        /*
         * 这里跳转到了<读语句>这里
         * <写语句>→write(<变量>)
         *
         * <变量>使用函数variable来表示
         */
        if(sym_code==WRITE_CODE){
            advance();
            if(sym_code==LEFT_BRACKET_CODE){
                advance();
                variable(USE,ISVARIABLE);
                if(sym_code==RIGHT_BRACKET_CODE){
                    advance();
                } else{
                    error("write缺少右括号");
                }
            } else{
                error("write缺少左括号");
            }
        } else{
            error("缺少write");
        }
    }

    void ifStatement(){
        /*
         * 这里跳转到了<条件语句>这里
         * <条件语句>→if<条件表达式>then<执行语句>else <执行语句>
         *
         * <条件表达式>使用函数conditionalExpression来表示
         * <执行语句>使用函数execution来表示
         */
        if(sym_code==IF_CODE){
            advance();
            conditionalExpression();
            if(sym_code==THEN_CODE){
                advance();
                execution();
                if(sym_code==ELSE_CODE){
                    advance();
                    execution();
                } else{
                    error("条件语句缺少else");
                }
            } else{
                error("条件语句缺少then");
            }
        } else{
            error("条件语句缺少if");
        }
    }

    void conditionalExpression(){
        /*
         * 这里跳转到了<条件表达式>这里
         * <条件表达式>→<算术表达式><关系运算符><算术表达式>
         *
         * <算术表达式>使用函数arithmeticExpression来表示
         * <关系运算符>使用函数relationalOperator来表示
         */
        arithmeticExpression();
        relationalOperator();
        arithmeticExpression();
    }

    void arithmeticExpression(){
        /*
         * 这里跳转到了<算术表达式>这里
         * <算术表达式>→<算术表达式>-<项>│<项>
         *
         * 这里是存在左递归的，所以要消除左递归:
         * <算术表达式>→<项><算术表达式___>
         * <算术表达式___>→-<项><算术表达式___>|ε
         *
         * <算术表达式___>使用函数arithmeticExpression___来表示
         * <项>使用函数item来表示
         */
        item();
        arithmeticExpression___();
    }

    void arithmeticExpression___(){
        /*
         * 这里跳转到了<算术表达式___>这里
         * <算术表达式___>→-<项><算术表达式___>|ε
         *
         * <项>使用函数item来表示
         */
        if(sym_code==MINUS_CODE){
            advance();
            item();
            arithmeticExpression___();
        }
    }

    void item(){
        /*
         * 这里跳转到了<项>这里
         * <项>→<项>*<因子>│<因子>
         *
         * 这里存在了左递归，消除左递归之后:
         * <项>→<因子><项___>
         * <项___>→*<因子><项___>│ε
         *
         * <项___>使用函数item___来表示
         * <因子>使用函数factor来表示
         */
        factor();
        item___();
    }

    void item___(){
        /*
         * 这里跳转到了<项___>这里
         * <项___>→*<因子><项___>│ε
         *
         * <因子>使用函数item来表示
         */
        if(sym_code==MUL_CODE){
            advance();
            factor();
            item___();
        }
    }

    void factor(){
        /*
         * 这里跳转到了<因子>这里
         * <因子>→<变量>│<常数>│<函数调用>
         *
         * <变量>→<标识符>
         * <函数调用>→<标识符>(<算数表达式>)  PPT上面没有给出这个产生式，这个产生式是通过查阅资料得到的
         *
         * <变量>使用函数variable来表示
         * <常数>使用函数constant来表示
         * <函数调用>使用函数functionCall来表示
         */
        if(sym_code==CONSTANT_CODE){
            //如果发现了下一个词是常数，就转常数的匹配
            constant();
        } else if(sym_code==IDENTIFIER_CODE){
            //如果发现下一个词是标识符，那么这个标识符有可能是变量的也有可能是函数调用的
            if(decodeSymCode(num+1)==LEFT_BRACKET_CODE){
                //如果下一个单词符号是左括号(注意此时指针没有往后移，指向的还是<标识符>),那么就表明是函数调用而不是变量
                functionCall();
            } else{
                variable(USE,ISVARIABLE);
            }
        } else{
            error("因子出错");
        }
    }

    void constant(){
        /*
         * 这里跳转到了<常数>这里
         *
         * 根据文法里面的描述，文法应该是不支持负数的
         */
        advance();
    }

    void functionCall(){
        /*
         * 这里跳转到了<函数调用>这里
         * <函数调用>→<标识符>(<算数表达式>)
         *
         * <标识符>使用函数identifier来表示
         * <算术表达式>使用函数arithmeticExpression来表示
         */
        identifier(USE,ISFUNCTION);  //这一步还是需要的，因为就算在上面已经判断了当前指向的单词符号是标识符，但是不能保证这个标识符是声明过的，所以这里要匹配这个标识符
        if(sym_code==LEFT_BRACKET_CODE){
            advance();
            arithmeticExpression();
            if(sym_code==RIGHT_BRACKET_CODE){
                advance();
            } else{
                error("函数调用缺少右括号");
            }
        } else{
            error("函数调用缺少左括号");
        }
    }

    void relationalOperator(){
        /*
         * 这里跳转到了<关系运算符>这里
         * <关系运算符> →<│<=│>│>=│=│<>
         */
        if(sym_code==LESS){
            advance();
        } else if(sym_code==LESS_EQUAL){
            advance();
        }else if(sym_code==MORE){
            advance();
        }else if(sym_code==MORE_EQUAL){
            advance();
        }else if(sym_code==EQUAL){
            advance();
        }else if(sym_code==NOT_EQUAL){
            advance();
        }else {
            error("关系运算符出错");
//            advance();
            //他的代码这里加了advance
        }
    }

    void assignmentStatement(){
        /*
         * 这里跳转到了<赋值语句>这里
         * <赋值语句>→<变量>:=<算术表达式>
         *
         * <变量>使用函数variable来表示
         * <算术表达式>使用函数arithmeticExpression来表示
         */
        variable(USE,ISVARIABLE);
        if(sym_code==ASSIGN_CODE){
            advance();
            arithmeticExpression();
        } else{
            error("赋值语句出错");
        }
    }

    void error(string message){
        this->errorMessage+=("***LINE:"+to_string(this->line)+"  "+message+"\n");
        cout<<"***LINE:"<<line<<"  "<<message<<endl;
    }

    void getVarInFun(){
        for(int i=0;i<this->funList.size();i++){
            for(int j=0;j<this->varList.size();j++){
                if(this->funList[i].name==this->varList[j].functionName){
                    this->funList[i].positions.push_back(j);
                }
            }
            this->funList[i].first=*min_element(begin(funList[i].positions),end(funList[i].positions));
            this->funList[i].last=*max_element(begin(funList[i].positions),end(funList[i].positions));
        }
    }

    void toMessage(){
        /*
         * 将变量表和过程表里面的值转为字符串存在变量中
         */
        for(int i=0;i<this->funList.size();i++){
            this->funMessage+=funList[i].name+" "+funList[i].type+" "+to_string(funList[i].level)+" "+
                    to_string(funList[i].first)+" "+to_string(funList[i].last)+"\n";
        }

        for(int j=0;j<this->varList.size();j++){
            this->varMessage+=varList[j].name+" "+varList[j].functionName+" "+
                    to_string(varList[j].kind)+" "+varList[j].type+" "+to_string(varList[j].level)+" "+
                    to_string(varList[j].position)+"\n";
        }
    }
};