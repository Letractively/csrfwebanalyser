#include "Results.h"	
#include <iostream>
#include <algorithm>

Results::Results(void) {
	urlDefensesMap = map<string, list<pair<string, string> > >();
	defenseUrlsMap = map<string, list<pair<string, string> > >();
}

map<string, list<pair<string, string> > > Results::GetUrlDefensesMap(void) {
	return urlDefensesMap;
}
map<string, list<pair<string, string> > > Results::GetDefenseUrlsMap(void) {
	return defenseUrlsMap;
}

void 	Results::AddUrlDefense(string url, const string policy, const string value) {
								 
	if(urlDefensesMap.count(url) != 0) {
		urlDefensesMap[url].push_back(make_pair(policy, value));
	}
	else {
		list< pair<string,string> > policyValueList;
		policyValueList.push_back(make_pair(policy, value));
		urlDefensesMap.insert(make_pair(url, policyValueList) );
	}
}
void Results::AddDefenseUrl(string policy, const string url, const string value) {
	transform(policy.begin(), policy.end(), 
						policy.begin(), (int(*)(int)) ::toupper);
	
	if(defenseUrlsMap.count(policy) != 0) {
		defenseUrlsMap[policy].push_back(make_pair(url, value));
	}
	else {
		list< pair<string,string> > urlValueList;
		urlValueList.push_back(make_pair(url, value));
		defenseUrlsMap.insert(make_pair(policy, urlValueList) );
	}
}
	
void Results::PrintUrlsMap(void){
	for(map <string, list< pair<string, string > > >::iterator it=urlDefensesMap.begin(); it != urlDefensesMap.end(); it++){
		cout<< "****** " <<it->first << " ******" <<endl;
		cout << "total defenses: " << it->second.size() << endl;
		cout << (it->second.size()!=0 ? "  Defenses : \n" : "") ;
		for(list< pair<string, string > >::iterator list_it = it->second.begin(); list_it != it->second.end(); list_it++){
			cout << "  - " <<  list_it->first << " : " << list_it->second << endl;
		}
	}
}
	
void Results::PrintDefensesMap(void){
	for(map <string, list< pair<string, string > > >::iterator it=defenseUrlsMap.begin(); it != defenseUrlsMap.end(); it++){
		cout << "****** " <<it->first << " ******" <<endl;
		cout << "total urls: " << it->second.size() << endl;
		cout << (it->second.size()!=0 ? "  URLs : \n" : "") ;
		for(list< pair<string, string > >::iterator list_it = it->second.begin(); list_it != it->second.end(); list_it++){
			cout << "  - " <<  list_it->first << " : " << list_it->second << endl;
		}
	}
}

void Results::MergeUrlDefensesMaps(map<string, list<pair<string, string> > > mapA) 
{
	map<string, list<pair<string, string> > >::iterator itA = mapA.begin();
	string curr_key;
	while(itA != mapA.end()) {
		curr_key = itA->first;
		if(urlDefensesMap.count(curr_key) != 0) {
			urlDefensesMap[curr_key].merge(mapA[curr_key]);
		}
		else {
			urlDefensesMap[curr_key] = list<pair<string, string> >(mapA[curr_key]);
		}	
		itA++;
	}
	//urlDefensesMap = map<string, list<pair<string, string> > >(mapA);
}

													
void Results::MergeDefenseUrlsMaps(map<string, list<pair<string, string> > > mapA)
{
	map<string, list<pair<string, string> > >::iterator itA = mapA.begin();
	string curr_key;
	while(itA != mapA.end()) {
		curr_key = itA->first;
		if(defenseUrlsMap.count(curr_key) != 0) {
			defenseUrlsMap[curr_key].merge(mapA[curr_key]);
		}
		else {
			defenseUrlsMap[curr_key] = list<pair<string, string> >(mapA[curr_key]);
		}	
		itA++;
	}
}