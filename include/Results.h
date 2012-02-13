#ifndef RESULTS_H
#define RESULTS_H

#include <list>
#include <map>
#include <string>

using namespace std;

#define REPORT_DISTINCT_URLS 0

#if REPORT_DISTINCT_URLS
class Results {
	private:
	map<string, list<pair<string, string> > > urlDefensesMap;
	map<string, list<pair<string, string> > > defenseUrlsMap;
	public:
	Results();
	map<string, list<pair<string, string> > > GetUrlDefensesMap(void);
	map<string, list<pair<string, string> > > GetDefenseUrlsMap(void);
	void AddUrlDefense(string url, const string policy, const string value);
	void AddDefenseUrl(string policy, const string url, const string value);
	void MergeUrlDefensesMaps(map<string, list<pair<string, string> > > mapA);
	void MergeDefenseUrlsMaps(map<string, list<pair<string, string> > > mapA);
	void PrintUrlsMap(void);
	void PrintDefensesMap(void);
};

#else
class Results {
	private:
	map<string, map<string, string> > urlDefensesMap;
	map<string, map<string, string> > defenseUrlsMap;
	public:
	Results();
	map<string, map<string, string> > GetUrlDefensesMap(void);
	map<string, map<string, string> > GetDefenseUrlsMap(void);
	void AddUrlDefense(string url, const string policy, const string value);
	void AddDefenseUrl(string policy, string url, const string value);
	void MergeUrlDefensesMaps(map<string, map<string, string> > mapA);
	void MergeDefenseUrlsMaps(map<string, map<string, string> > mapA);
	void PrintUrlsMap(void);
	void PrintDefensesMap(void);
};

#endif

#endif

