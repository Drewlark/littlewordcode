#include <iostream>
#include <fstream>
#include <string>
#include <forward_list>
#include <vector>
#include <unordered_map>
#include <stack>
/*

  PRACTICE:
  Read one word at a time from datafile.txt
  Add each word to a STL forward_list container, any way you like
  After all words have been stored in the container…
  Use the range-based for loop to print the contents of the container
  Rerun your program and have it process your main.cpp file as an input file!

*/

using namespace std;
typedef unordered_map<string, int> dataset;
typedef void(*print_func)(string, dataset);

typedef tuple<vector<string>, vector<string>> function_id;
unordered_map<string, int> values;
unordered_map<char,print_func> pfuncs;
unordered_map<string, function_id > funcs;
unordered_map<string, int> func_scope;
bool isNum(string &s) {
	return to_string(atoi(s.c_str())) == s;
}

bool isNum(string &val, int &ival) {
	return to_string(ival) == val;
}

int parseName(string s, dataset &addScope) {
	int val;
	//s = scope+"."+ s;
	//cout << s << endl;
	if (isNum(s)) {
		val = atoi(s.c_str());
	}
	else {
		if (addScope.count(s) > 0) {
			val = addScope[s];
		}
		else if (values.count(s) > 0) {
			val = values[s];
		}
		else {
			cout << "Undefined variable used " << s << endl;
			throw exception();
		}
	}
	return val;
}

int* parseVar(string s, dataset &addScope) {
	if (addScope.count(s) > 0) {
		return &addScope[s];
	}
	else if (values.count(s) > 0) {
		return &values[s];
	}
}

bool parseConditional(string pre, string post, dataset &addScope) {
	string sp = "";
	char q = pre.at(pre.length() - 1);
	pre = pre.substr(0, pre.length()-1);
	//cout << pre << endl;
	int val1 = parseName(pre, addScope);
	int val2 = parseName(post, addScope);
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

void _printLn(string s, dataset addScope) {
	if (s[0] == '*') {
		s = s.substr(1);
		char value = parseName(s, addScope);
		cout << value << endl;
	}
	else {
		cout << parseName(s,addScope) << endl;
	}
	
		
	
}

void _print(string s, dataset addScope) {
	if (s[0] == '*') {
		s = s.substr(1);
		char value = parseName(s, addScope);
		cout << value;
	}
	else {
		cout << parseName(s, addScope);
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
stack<int> return_stack;

bool compute(vector<string> words, dataset addScope) {
	/*cout << "+++START+++" << endl;
	for (string w : words) {
		cout << w << endl;
	}
	cout << "+++END+++" << endl;*/
	bool cSeeking = false;
	bool funcSeeking = false;
	int cSeeki = 0;
	bool condTrue = false;
	vector<string> newvec;
	bool inLoop = false;
	string origps, origval;
	string funcName;
	string func_content;
	int s_paren;
	int e_paren;
	int end_paren_c;
	int retval = 0;
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
			end_paren_c = e_paren - s_paren;
			if (pfuncs.count(w[0]) > 0) {
				pfuncs[w[0]](w.substr(1),addScope);
			}
			else if (w[0] == '%') {
				int temp_val = parseName(w.substr(1), addScope);
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
				
				unordered_map<string, function_id > f_test_f = funcs;
				function_id fid = funcs[test];
				vector<string> args = get<1>(fid);
				vector<string> params = splitString(w.substr(s_paren + 1,end_paren_c-1), ',');
				dataset ds;
				//cout << w << endl;
				for (int iter = 0; i < args.size();i++) {
					ds[args[i]] = parseName(params[i], addScope);
				}
				compute(get<0>(fid), ds);
				string rvar = w.substr(e_paren + 1);
				if (!return_stack.empty()) {
					if (rvar.size() > 0) {
						if (addScope.size() < 1) {
							values[rvar] = return_stack.top();
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
					else if (c == '-') {
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
					if (addScope.size() < 1) {
						values[ps] = parseName(val, addScope);
					}
					else {
						addScope[ps] = parseName(val, addScope);
					}
					//cout << "set " << ps << " to " << val << endl;
				}
				else if (foundAdd) {
					*parseVar(ps, addScope) += parseName(val, addScope);
				}
				else if (foundSub) {
					*parseVar(ps, addScope) -= parseName(val, addScope);
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
							if (compute(newvec, addScope)) {
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
				
				funcs[funcName] = make_tuple(newvec, splitString(func_content, ','));
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

int main()
{

	string fileName = "littlescript.txt"; // this could also be main.cpp
	fstream fs;  // a handle for the file we will open
	string s; // a word that we will get from the file... delimited by white space
	vector<string> words; // our collection of words (strings)
	pfuncs[':'] = _printLn;
	pfuncs['\''] = _print;

	// note below:
	// we can get the "c" string version from and STL string by using the .c_str() method
	// some functions require a simple string and cannot convert from and STL string properly.

	cout << "Reading " << fileName << "...." << endl;

	fs.open(fileName.c_str());
	while (fs >> s) { // read one word at a time
		words.push_back(s);
	}

	fs.flush();

	cout << "Done Reading!" << endl << endl; // let the user know we are done
	compute(words,dataset());
	return 0;
}


