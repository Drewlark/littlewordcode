#ifndef LWC_SCOPE
#define LWC_SCOPE
#include <unordered_map>
#include <string>
#include <tuple>
#include <vector>

using namespace std;
struct lwc_Scope;
typedef unordered_map<string, int> dataset;

typedef tuple<vector<string>, vector<string>> function_id;



struct lwc_Scope{
	static lwc_Scope global_scope;
private:
	dataset values;
	vector<string> ordered_values;
	dataset invrs_ord_val;
	unordered_map<int, function_id> funcs;
	
public:
	
	//static vector<lwc_Scope&> scopes;
	lwc_Scope() {}
	int& operator[](const string s) {
		if (values.count(s) < 1) {
			values[s] = 0;
		}
		return values[s];
	}
	int initializeVar(const string &s, int i) { //initializes variable then returns pointer
		return (int)(values[s] = i);
	}
	int count(const string s) {
		return values.count(s);
	}

	int size() {
		return values.size();
	}

	bool empty() {
		return values.empty();
	}

	string dereference(int ptr)
	{
		return ordered_values[ptr];
	}

	int get_address(string s)
	{
		return (int)values[s];
	}

	int declare_function(string s, function_id &fid);

	function_id get_function(string &s);

	int parseName(string &s);

};



#endif
