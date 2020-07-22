#include <stdio.h>
#include <iostream>
#include <string>
using namespace std;
//So sanh a = b(a>=b)
compareTime(string Time1, string Time2)
{
	if (Time1.length() < Time2.length()){return true;}
	else if (Time1.length() > Time2.length()){return false;}
	else{
		for (int i = 0; i < Time1.length(); i++){
			if (Time1[i] > Time2[i]) {return false;}
			if (Time1[i] < Time2[i]) {return true;}
		}
		return true;
	}
}

// Check TIME
bool isTime(string s)
{
	for (int i = 0; i < s.length(); i++)
		if (isdigit(s[i]) == false)
			return false;

	return true;
}

// Check OP, CP, HP, LP
bool isValue(string s)
{
	int count = 0;
	for (int i = 0; i < s.length(); i++) {
		if (s[i] == '.') {
			count++;
			if (count > 1) {
				return false;
			}
		}
		if (isdigit(s[i]) == false && s[i] != '.') {
			return false;
		}
	}
	return true;
}

int main(){
    string time = "012340567";
    cout<<isTime(time)<<endl;
    string num = "11234.";
    cout<<stof(num)<<endl;

    return 0;

}
