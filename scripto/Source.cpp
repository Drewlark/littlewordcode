#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <stack>
#include <sstream>
#include "lwc_Scope.h"

//Convert all variable names with memory locations in situ.

using namespace std;

lwc_Scope &global = lwc_Scope().global_scope;

int* parseVar(string &s, lwc_Scope &addScope) {
	if (s[0] == '@') {
		return (int*)(atoi(s.substr(1).c_str()));
	}
	if (addScope.count(s) > 0) {
		return &addScope[s];
	}
	else if (global.count(s) > 0) {
		return &global[s];
	}
}

bool parseConditional(string pre, string post, lwc_Scope &addScope) {
	string sp = "";
	char q = pre.at(pre.length() - 1);
	pre = pre.substr(0, pre.length()-1);
	//cout << pre << endl;
	int val1 = addScope.parseName(pre);
	int val2 = addScope.parseName(post);
	//cout << (val1 == val2) << endl;
	//cout << val1 << " and val2: " << val2 << endl;
	switch (q) {
	case '>':
		return val1 > val2;
	case '<':
		return val1 < val2;
	case '~':
		return val1 == val2;
	}
}

void _printLn(string s, lwc_Scope &addScope) {
	if (s[0] == '*') {
		s = s.substr(1);
		char value = addScope.parseName(s);
		cout << value << endl;
	}
	else {
		cout << addScope.parseName(s) << endl;
	}
	
		
	
}

void _print(string s, lwc_Scope &addScope) {
	if (s[0] == '*') {
		s = s.substr(1);
		char value = addScope.parseName(s);
		cout << value;
	}
	else {
		cout << addScope.parseName(s);
	}
}

vector<string> splitString(string s, char delim)
{
	vector<string> ret;
	string curr = "";
	for (char c : s) {
		if (c == delim) {
			ret.push_back(curr);
			curr = "";
		}
		else {
			curr += c;
		}
	}
	if(curr!="")
		ret.push_back(curr);
	return ret;
}

typedef void(*print_func)(string, lwc_Scope&);
typedef int(*builtin)(vector<string>, lwc_Scope&);
typedef unordered_map<string, builtin> builtin_set;

int builtin_dereference(vector<string> args, lwc_Scope& addScope)
{
	return 0;
}

int builtin_get_addr(vector<string> args,  lwc_Scope& addScope)
{
	return 0;
}

builtin_set BUILT_IN({ 
	{"deref", builtin_dereference},
	{"ptr", builtin_get_addr}

	});

unordered_map<char, print_func> pfuncs(
	{
	{':', _printLn},
	{'\'', _print} 
	});

stack<int> return_stack;

bool compute(vector<string> &words, lwc_Scope &addScope) {
	/*cout << "+++START+++" << endl;
	for (string w : words) {
		cout << w << endl;
	}
	cout << "+++END+++" << endl;*/
	bool cSeeking = false;
	bool funcSeeking = false;
	int cSeeki = 0;
	bool condTrue = false;
	bool inLoop = false;
	string origps, origval;
	string funcName;
	string func_content;
	int s_paren;
	int e_paren;
	int end_paren_c;
	vector<string> newvec;
	for (int line_num = 0; line_num < words.size(); ++line_num) // use the range-based for to print the list one item at a time
	{
		
		bool prnt_line = false;
		int i = 0;
		string w = words[line_num];
		if (w.substr(0, 2) == "//") {
			continue;
		}
		if (!cSeeking && !funcSeeking) {
			s_paren = w.find('(');
			e_paren = w.rfind(')');
			string wsub = w.substr(1);
			end_paren_c = e_paren - s_paren;
			if (pfuncs.count(w[0]) > 0) {
				pfuncs[w[0]](wsub, addScope);
			}
			else if (w[0] == '%') {
				int temp_val = addScope.parseName(wsub);
				return_stack.push(temp_val);
				return true;
			}
			else if (w[0] == '$') {
				funcName = w.substr(1, s_paren-1);
				func_content = w.substr(s_paren+1,end_paren_c-1);
				funcSeeking = true;
				continue;
			}
			else if (s_paren != w.npos && e_paren != w.npos) {
				string test = w.substr(0, s_paren);
				
				function_id fid = global.get_function(test);
				vector<string> args = get<1>(fid);
				vector<string> params = splitString(w.substr(s_paren + 1,end_paren_c-1), ',');
				lwc_Scope ds;
				//cout << w << endl;
				ds["$"] = 0; /*This line of code adds a single "impossible" variable to the 
			   "addscope" such that the assignment interpreter recognizes we will not be in the global scope*/
				for (int iter = 0; iter < args.size();iter++) {
					ds[args[iter]] = addScope.parseName(params[iter]);
				}
				compute(get<0>(fid), ds); //runs function
				string rvar = w.substr(e_paren + 1); //gets name after '%' symbol
				if (!return_stack.empty()) {
					if (rvar.size() > 0) {
						if (addScope.size() < 1) {
							global[rvar] = return_stack.top();
							return_stack.pop();
						}
						else {
							addScope[rvar] = return_stack.top();
							return_stack.pop();
						}
					}
				}
				continue;
			}
			
			else {
				string ps = "";
				string val = "";
				string var = "";
				bool foundVar = false;
				bool foundAdd = false;
				bool foundIf = false;
				bool foundSub = false;
				for (char c : w) {

					if (c == '=') {
						foundVar = true;
					}
					else if (c == '+') {
						foundAdd = true;
					}
					else if (c == '-' && foundAdd == false && foundSub==false) {
						foundSub = true;
					}
					else if (c == '?') {
						foundIf = true;
						var += c;
					}
					else {
						var += c;
						if (!foundVar && !foundAdd && !foundIf && !foundSub)
							ps += c;
						else {
							val += c;
						}
					}
				}

				int ival = atoi(val.c_str());

				if (foundVar) {
					//The following if statementis a trick to determine if we are in the global scope or inside a function. 
					//When inside a function even if there are no args, an impossible variable "$" is passed through addScope
					stringstream ss;
					ss << "@";
					if (addScope.size() < 1) { 
						global.initializeVar(ps, addScope.parseName(val));
					}
					else {
						addScope.initializeVar(ps, addScope.parseName(val));
					}
					//words[line_num].replace(0, ps.length(), ss.str());
					//cout << "set " << ps << " to " << val << endl;
				}
				else if (foundAdd) {
					int ppdebug = addScope.parseName(val);
					*parseVar(ps, addScope) += ppdebug;
				}
				else if (foundSub) {
					*parseVar(ps, addScope) -= addScope.parseName(val);
				}
				else if (foundIf) {
					//cout << "HELP" << endl;
					if (ps.at(0) == '@') {
						ps = ps.substr(1);
						inLoop = true;
						origps = ps;
						origval = val;
					}
					if (parseConditional(ps, val, addScope)) {
						cSeeking = true;
						cSeeki++;
						condTrue = true;
					}
					else {
						cSeeking = true;
						cSeeki++;
						condTrue = false;
					}
				}
			}

		}
		else if(cSeeking) {

			if (w == "endc") {
				cSeeki--;
				if (cSeeki == 0) {
					if (condTrue) {
						//cout << "Ya" << endl;
						if (!inLoop) {
							if (compute(newvec, addScope)) { //if end of loop found, compute the vector of internal code
								cSeeking = false;
								return true;
							}
						}
						else {
							while (parseConditional(origps, origval, addScope))
							{
								if (compute(newvec, addScope)) {
									inLoop = false;
									return true;
								}
							}
							inLoop = false;
						}
					}
					cSeeking = false;
					newvec.clear();
					continue;
				}
				else {
					newvec.push_back(w);
				}

				
			}
			else if (w.find('?') != w.npos) {
				newvec.push_back(w);
				cSeeki++;
			}
			else {
				newvec.push_back(w);
			}
		}
		else if (funcSeeking) {
			if (w == "endf") {
				function_id fid = make_tuple(newvec, splitString(func_content, ','));
				global.declare_function(funcName, fid);
				funcSeeking = false;
				newvec.clear();
				continue;
			}
			else {
				//cout << w << endl;
				newvec.push_back(w);
			}
		}

	}
	return false;
}

/*vector<char> SP_SYMBOLS = { '+', '-', '?', '>', '<', '~', '@' };
bool isSP_SYM(const char &a) {
	for (const char &b : SP_SYMBOLS) {
		if (a == b)
			return true;
	}
	return false;
}

void myReplace(string &s, const string &rep, int )

vector<string> memProcess(vector<string> &words, string &key, string &addr) {
	for (string &word : words) {
		string ps;
		string as;
		char found_sym = 0;
		for (const char &c : word) {
			if (!isSP_SYM(c)) {
				if (!found_sym) {
					ps += c;
				}
				else if (found_sym) {
					as += c;
				}
			}
			else {
				found_sym++;
			}
		}
		if (ps == key) {
			word.replace(0,ps.length(),)
		}
	}
}*/

int main()
{ 
	//Original file opening code from a sample provided to teach fstreams in an Advanced C++ class
	//Thank you Professor Yates!
	//I began working on this language as I played around with the fstream example
	string fileName = "littlescript.txt"; 
	fstream fs;  
	string s; 
	vector<string> words; 

	cout << "Reading " << fileName << "...." << endl;

	fs.open(fileName);
	while (fs >> s) { 
		words.push_back(s);
	}

	fs.flush();

	cout << "Done Reading!" << endl << endl; // let the user know we are done
	lwc_Scope empty_scope;
	compute(words, empty_scope);
	string ends;
	//cin >> ends;
	return 0;
}