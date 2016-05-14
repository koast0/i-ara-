#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <set>
#include <algorithm>
#include <unordered_set>
#include <unistd.h>
#include <random>

#define m_p(a, b) make_pair(a, b)

using namespace std;
const int inf = 1e9+7;

class IARA {
private:

	vector <vector<int>> graph;
	vector<pair<int, int>> neigh = {m_p(0, 1), m_p(0, -1), m_p(1, 0), m_p(-1, 0)};
	double eps = 1.7;
	double eps_max = 1.7;
	double step = 0.4;
	int n, size, start, goal, last_start;
	int m = 0;
	vector<int> v;
	vector<int> g;
	vector<int> parents;
	vector<int> path;
	random_device rd;


	inline int GetIndex(int x, int y) {
		return (x*m+y);
	}

	inline int Heruestic(int x, int y) {
		int x_x = x%m;
		int x_y = x/n;
		int y_x = y%m;
		int y_y = y/n;
		return (abs(x_x-y_x) + abs(x_y-y_y));
	}

	inline double GetPriority(int i) {
		return min((double) inf, g[i] + eps*Heruestic(i, start));
	}

	struct SetElem {
		int id;
		double priority;
		SetElem(int id):id(id) {
			priority = GetPriority(id);
		}
		SetElem() {
			id = -1;
			priority = inf;
		}
		bool operator <(const SetElem& d) const {
			return (priority != d.priority) ? (priority < d.priority) : (id < d.id);
		}
	};

	set<SetElem> OPEN;
	set<SetElem> CLOSED;
	set<SetElem> INCONS;
	set<SetElem> DELETED;

	bool ImprovePath() {
		while (not(OPEN.empty()) and GetPriority(goal) > GetPriority(OPEN.begin()->id)) {
			SetElem s = *OPEN.begin();
			CLOSED.insert(s);
			OPEN.erase(s);
			v[s.id] = g[s.id];
			for(auto new_s: graph[s.id]) {
				if (g[new_s] > v[s.id] + 1) {
					g[new_s] = v[s.id] + 1;
					parents[new_s] = s.id;
					if (CLOSED.find(SetElem(new_s)) == CLOSED.end()) {
						if (OPEN.find(SetElem(new_s)) == OPEN.end() and INCONS.find(SetElem(new_s)) == INCONS.end()) {
							OPEN.insert(SetElem(new_s));
						}
					} else {
						CLOSED.erase(SetElem(new_s));
						INCONS.insert(SetElem(new_s));
					}
				}
			}
		}
		if (g[goal] == inf) {
			return false;
		}
		return true; 
	}

	void UpdatePriorities(set<SetElem>& s) {
		set<SetElem> save;
		for (auto i:s) {
			save.insert(SetElem(i.id));
		}
		s.swap(save);
	}

	bool ComputePath() {
		while(true) {
			if (ImprovePath() == false) 
				return false;
			if (eps == 1)
				return true;
			OPEN.insert(INCONS.begin(), INCONS.end());
			INCONS.clear();
			CLOSED.clear();
			eps = fmax(1, eps-step);
			UpdatePriorities(OPEN);
		}
	}

	void Step1() {
		if (g[start]!=v[start]) {
			g[start] = v[start];
			OPEN.erase(SetElem(start));
			INCONS.erase(SetElem(start));
		}
	}

	void DfsDelete(int current) {
		v[current] = inf;
		g[current] = inf;
		parents[current] = -1;
		OPEN.erase(SetElem(current));
		INCONS.erase(SetElem(current));
		DELETED.insert(SetElem(current));
		for (auto i:graph[current]) {
			if (parents[i]==current) {
				DfsDelete(i);
			}
		}
	}

	void Step2() {
		if (start!=last_start) {
			parents[start]=-1;
			DfsDelete(last_start);
		}
	}

	void Step3() {
		for (auto it:DELETED) {
			int i = it.id;
			for(auto j:graph[i]) {
				if (g[i] > v[j] + 1) {
					g[i] = v[j] + 1;
					parents[i]=j;
				}
			}
			if (g[i]!=inf) {
				OPEN.insert(SetElem(i));
			}
		}
		OPEN.insert(INCONS.begin(), INCONS.end());
		CLOSED.clear();
		INCONS.clear();
		DELETED.clear();
	}

	void Step4() {
		if (GetPriority(goal) > OPEN.begin()->priority) {
			eps = eps_max;
		}
		else {
			eps = fmax(eps-step, 1);
		}
		UpdatePriorities(OPEN);
	}

	bool GetCurentPath() {
		path.clear();
		path.push_back(goal);
		int current = goal;
		cout << current/n <<' '<< current%m<<endl;
		cout << start/n <<' '<< start%m<<endl<<endl;

		while (current!=start) {
			if (parents[current]==-1) {
				for (auto i=path.rbegin(); i!=path.rend();++i) {
					cout<< *i/n <<' '<<*i%m<<endl;
				}
				exit(0);
			}
			current = parents[current];
			path.push_back(current);
		}
	}

	inline int GetNextGoal(int goal) {
		return graph[goal][rd()%graph[goal].size()];
	}

	void GetGraphFromTXT(const char* name) {
		ifstream myfile (name);
		string input;
		vector<string> lab;
		while (getline (myfile, input)) {
			lab.push_back(input);
			m = max(m, (int) input.size());
		}
		int i_new, j_new;
		n = lab.size();
		cout <<n<< ' '<<m<<endl;
		graph.resize(n*m);
		size = n+m;
		for (int i = 0; i< n; ++i) {
			for (int j = 0; j< m; ++j) {
				if (lab[i][j] != '.') continue;
				for (auto it:neigh) {
					i_new = i+it.first;
					j_new = j+it.second;
					if (i_new>-1 and i_new < n and j_new>-1 and j_new < m) {
						if (lab[i_new][j_new] == '.') {
							graph[GetIndex(i,j)].push_back(GetIndex(i_new, j_new));
						}
					}
				}
			}
		}
	}

public:
	IARA(int st, int fin, const char* name) {
		start = st;
		goal = fin;
		GetGraphFromTXT(name);
	}
	bool StartIAra() {
		last_start = -1;
		v.resize(size, inf);
		g.resize(size, inf);
		parents.resize(size, -1);
		g[start] = 0;
		OPEN.insert(SetElem(start));
		while (start!=goal) {
			if (ComputePath() == false) {
				return false;
			}
			GetCurentPath();
			last_start = start;
			start = *(path.rbegin()+1);
			goal = GetNextGoal(goal);
			if (start == goal) {
				break;
			}
			Step1();
			Step2();
			Step3();
			Step4();
		}
	}
};

int main() {
	IARA lab(0, 3599, "test.txt");
	lab.StartIAra();
	return 0;
}