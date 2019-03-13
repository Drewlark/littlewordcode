#include "lwc_Scope.h"
#include "lwc_Utility.h"
#include <iostream>
using namespace std;

lwc_Scope lwc_Scope::global_scope;

int lwc_Scope::declare_function(string s, function_id &fid) {
	s = '$' + s;
	int addr = ordered_values.size();
	ordered_values.push_back(s);
	values[s] = addr;
	funcs[addr] = fid;
	return addr;
}


function_id lwc_Scope::get_function(string &s) {
	return funcs[values['$' + s]];
}

int lwc_Scope::parseName(string &s) {
	int val;
	//s = scope+"."+ s;
	//cout << s << endl;
	if (isNum(s)) {
		val = atoi(s.c_str());
	}
	else {
		if ((*this).count(s) > 0) {
			val = (*this)[s];
		}
		else if (global_scope.count(s) > 0) {
			val = global_scope[s];
		}
		else {
			cout << "Undefined variable used " << s << endl;
			throw exception();
		}
	}
	return val;
}