
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
	
private:
	dataset values;
	vector<string> ordered_values;
	dataset invrs_ord_val;
	unordered_map<int, function_id> funcs;
public:
	//static vector<lwc_Scope&> scopes;
	lwc_Scope() {
	}
	int& operator[](const string s) {
		if (values.count(s) == 0) {
			values[s] = 0;
			ordered_values.push_back(s);
			invrs_ord_val[s] = ordered_values.size() - 1;
		}
		return values[s];
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
		return invrs_ord_val[s];
	}

	int declare_function(string s, function_id fid);

	function_id get_function(string s);
};


#endif
