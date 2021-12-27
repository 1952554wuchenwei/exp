#include <iostream>
#include <string>
#include <stdio.h>
#include "dfa.h"
using namespace std;


//保留字
const string KeyWord[9] = { "PROGRAM","BEGIN","END","CONST","VAR","WHILE","DO","IF","THEN" };   


int syn;  //单词种别码
string  token;       //单词自身字符串

int sum;  //整型里的码内值

int i = 0;  

int tag = 1;     //后面判断STRING字符的 


void showAll();     //输入部分单词符号所对应的种别码（可自行扩展）

bool IsLetter(char ch);   	//判断是否为字母

bool IsDigit(char ch);   	//判断是否为数字

void scan(string s, DFA dfa);     	//扫描

bool IsDFAInpit(char ch);

DFA generateDFA(){
// 生成DFA
	string str = "l(l|d)*";
	str = infixToSuffix(str);		/*将中缀表达式转换为后缀表达式*/

	/***初始化所有的数组***/
	int i, j;
	for(i = 0; i < MAX; i++)
	{
		
		NfaStates[i].index = i;
		NfaStates[i].input = '#';
		NfaStates[i].chTrans = -1;
	}
	
	for(i = 0; i < MAX; i++)
	{
		
		DfaStates[i].index = i;
		DfaStates[i].isEnd = false;
		
		for(j = 0; j < 10; j++)
		{
			
			DfaStates[i].Edges[j].input = '#';
			DfaStates[i].Edges[j].Trans = -1;
		}
	}
	
	for(i = 0; i < MAX; i++)
	{
		
		minDfaStates[i].index = i;
		minDfaStates[i].isEnd = false;
		
		for(int j = 0; j < 10; j++)
		{
			
			minDfaStates[i].Edges[j].input = '#';
			minDfaStates[i].Edges[j].Trans = -1;
		}
	}
	
	NFA n = strToNfa(str);
	printNFA(n);
	
	DFA d = nfaToDfa(n, str);
	printDFA(d);
	
	DFA minDfa = minDFA(d);
	printMinDFA(minDfa);
	return minDfa;
}

int main()
{
	

// 词法分析器
	
	DFA dfa = generateDFA();
	showAll();
	string a="";
	cout << "\nwelcome!plz ending with #" << endl;
	getline(cin,a);
	cout<<"\ntriple:"<<endl;
	do
	{
		scan(a,dfa);
		switch (syn)   //最后判断一波syn
		{
		case -1:
			cout << "error" << endl;
			syn = 0;
			break;
		case -2:      //遇到空格跳过
			break;
		default:
			if(syn!=0 && syn<=24){
				cout << "< " << token << " >" << endl;
			}
			else if(syn!=0 && syn>24)
				cout << "< " << syn << "," << token << " >" << endl;
		}

	} while (syn!=0);
		
	system("pause");
	return 0;
	
}

void showAll()    //展示部分单词符号所对应的种别码（可自行扩展）
{

	cout << "---------- table---------------------- " << endl;
	cout << "symbol\tcode" << "\t" << "symbol\tcode" << endl;
	cout << "PROGRAM" << '\t' << '1' << '\t' << "/" << '\t' << "13" << endl;
	cout << "BEGIN" << '\t' << '2' << '\t' << ":=" << '\t' << "14" << endl;
	cout << "END" << '\t' << '3' << '\t' << "=" << '\t' << "15" << endl;
	cout << "CONST" << '\t' << '4' << '\t' << "<>" << '\t' << "16" << endl;
	cout << "VAR" << '\t' << '5' << '\t' << ">" << '\t' << "17" << endl;
	cout << "WHILE" << '\t' << '6' << '\t' << ">=" << '\t' << "18" << endl;
	cout << "DO" << '\t' << '7' << '\t' << "<" << '\t' << "19" << endl;
	cout << "IF" << '\t' << '8' << '\t' << "<=" << '\t' << "20" << endl;
	cout << "THEN" << '\t' << '9' << '\t' << "(" << '\t' << "21" << endl;
	cout << "+" << '\t' << "10" << '\t' << ")" << '\t' << "22" << endl;
	cout << "-" << '\t' << "11" << '\t' << ";" << '\t' << "23" << endl;
	cout << "*" << '\t' << "12" << '\t' << "," << '\t' << "24" << endl;
	cout << "id" << '\t' << "30" << '\t' << "digit" << '\t' << "40" << endl;
	cout << "---------------------------------------" << endl;
}


bool IsLetter(char ch)  //判断是否为字母
{
	if ((ch >= 'a'&&ch <= 'z') || (ch >= 'A'&&ch <= 'Z'))
		return true;
	else
		return false;

}

bool IsDigit(char ch)  //判断是否为数字
{
	if (ch >= '0'&&ch <= '9')
		return true;
	else
		return false;

}
bool IsDFAInput(char ch) // 判断是否为DFA的输入
{
	if(IsLetter(ch)||IsDigit(ch)){ //DFA接收字母和数字
		return 1;
	}else{
		return 0;
	}

}

void scan(string s, DFA dfa)    //扫描 在这里面使用DFA状态转移矩阵即可
{
	if (s[i] == ' ')
	{
		syn = -2;
		i++;
	}
	else
	{
		token = "";   //清空当前字符串

		// 判断首字符是否为DFA的输入
		if(IsDFAInput(s[i]))
		{
		   //  1.判断字符是否为数字
			if (IsDigit(s[i]))
			{
				token = ""; //清空当前字符串
				sum = 0;
				while (IsDigit(s[i])) {
					sum = sum * 10 + (s[i] - '0');
					i++;  //字符位置++
					syn = 40;   //数字种别码为40
				}
				token += to_string(sum);     //骚操作，直接转化字符串
				// 下一个字符不是数字也不是空格,#
				if(!IsDigit(s[i])&&(s[i]!=' '&&s[i]!='#')){
						syn = -1;
						token = ""; 
					}
				
			}
			// 2.字符为字符串，表现为字母开头衔接任意个数字或字母
			else if (IsLetter(s[i]))
			{

				token = ""; //清空当前字符串
				// 一直扫描，直到这个字符结束
				int state = 1; //初态为1
				while(IsDigit(s[i]) || IsLetter(s[i])){
					if (IsLetter(s[i]))
					{
						state = dfa.trans[state][11]; // 状态转换，11代表letter
					}
					else if(IsDigit(s[i])){
						state = dfa.trans[state][3]; // 状态转换，3代表d
					}else{
						state = -1; //其他字符报错
					}
					token += s[i]; // 用token记住当前的字符串，用来判断是否是关键字
					i++;
				}
				
				
				if(state == 0){
					syn = 30;  // 如果是标识符，种别码为30
					//如果是关键字，则用for循环将token与keyword比较找对应的种别码
					for (int j = 0; j < 9; j++)
					{
						if (token == KeyWord[j])    //如果都是string类型，可以直接=相比较，若相等则返回1，否则为0
						{
							syn = j + 1;   //种别码从1开始所以要加1
							break;
						}
					}
				}else{
					syn = -1; //出错
				}
			}
		}
		//判断为符号
		else {
			token = ""; //清空当前字符串
			switch (s[i]) {
			case'=':
				syn = 15;
				i++;
				token = "=";
				break;

			case'+':
				syn = 10;
				i++;
				token = "+";
				break;

			case'-':
				syn = 11;
				i++;
				token = "-";
				break;

			case'*':
				syn = 12;
				i++;
				token = "*";
				break;

			case'/':
				syn = 13;
				i++;
				token = "/";
				break;

			case'(':
				syn = 21;
				i++;
				token = "(";
				break;

			case')':
				syn = 22;
				i++;
				token = ")";
				break;

			case',':
				syn = 24;
				i++;
				token = ",";
				break;

			case':':
				i++;
				if(s[i] == '=')
				{
					syn = 14;
					token = ":=";
				}
				break;

			case';':
				syn = 23;
				i++;
				token = ";";
				break;

			case'>':
				syn = 17;
				i++;
				token = ">";
				if (s[i] == '=')
				{
					syn = 18;
					i++;
					token = ">=";
				}
				break;

			case'<':
				syn = 19;
				i++;
				token = "<";
				if (s[i] == '=')
				{
					syn = 20;
					i++;
					token = "<=";
				}
				else if(s[i] == '>'){
					syn = 16;
					i++;
					token = "<>";
				}
				break;
			case '#': //结束
				syn = 0;
				cout << "\n#end" << endl;
				break;

			default:
				syn = -1;
				break;
			}
		}
	}
	        
		
}