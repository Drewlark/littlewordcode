#include "lwc_Scope.h"

using namespace std;


int lwc_Scope::declare_function(string s, function_id fid) {
	s = '$' + s;
	int addr = ordered_values.size();
	ordered_values.push_back(s);
	values[s] = addr;
	funcs[addr] = fid;
	return addr;
}


function_id lwc_Scope::get_function(string s) {
	return funcs[values['$' + s]];
}