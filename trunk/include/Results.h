#ifndef RESULTS_H
#define RESULTS_H

#include <list>
#include <map>
#include <string>

using namespace std;

class Results {
	private:
	map<string, list<pair<string, string> > > urlDefensesMap;
	map<string, list<pair<string, string> > > defenseUrlsMap;
	public:
	Results();
	map<string, list<pair<string, string> > > GetUrlDefensesMap(void);
	map<string, list<pair<string, string> > > GetDefenseUrlsMap(void);
	void AddUrlDefense(const string url, const string policy, const string value);
	void AddDefenseUrl(const string policy, const string url, const string value);
	void MergeUrlDefensesMaps(map<string, list<pair<string, string> > > mapA);
	void MergeDefenseUrlsMaps(map<string, list<pair<string, string> > > mapA);
	void PrintUrlsMap(void);
	void PrintDefensesMap(void);
};

#endif