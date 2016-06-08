#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <set>
#include <algorithm>
#include <unordered_set>
#include <random>
#include <ctime>
#include <fstream>

#define m_p(a, b) make_pair(a, b)
using namespace std;
const int inf = 1e8+7;
vector<string> lab;

//Исходный граф
vector <vector<int>> graph;
//Возможные соседи в лабиринте
vector<pair<int, int>> neigh = {m_p(0, 1), m_p(0, -1), m_p(1, 0), m_p(-1, 0)};
// числа вершин лабиринта, переменные старта, последней посещеной
int size, start, goal, last_start, old_goal;
//Размер лабиринта. Изменяется после ввода.
int n =0;
int m = 0;

//Пересенная времени работы
clock_t time_cur;
// Вектора v-value и g-value для вершин графа
vector<int> v;
vector<int> g;
// Вектор родителя для восстановления пути
vector<int> parents;
//последний найденый путь
vector<int> path;
//Времена работы итераций
vector<int> times;
//
mt19937 rd(2016);


int GetIndex(int x, int y) {
    return (x*m+y);
}

void GetGraphFromTXT(const char* name) {
    ifstream myfile (name);
    string input;
    
    while (getline (myfile, input)) {
        lab.push_back(input);
        m = max(m, (int) input.size());
    }
    int i_new, j_new;
    n = lab.size();
    // cout <<n<< ' '<<m<<endl;
    graph.resize(n*m);
    size = n*m;
    start = 0;
    goal = n*m-1;
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

//получение Манхетонского расстояния
double Heruestic(int x, int y) {
    int x_x = x%m;
    int x_y = x/n;
    int y_x = y%m;
    int y_y = y/n;
    return (abs(x_x-y_x) + abs(x_y-y_y));
}
// Вычисление приоритета вершины (евристики на итерации)
double GetPriority(int i) {
    return min((double) inf, g[i] + Heruestic(i, goal));
}
// Мы храним все вершины сетов в таком формате
// Для каждой вершины храним ее номер и приоритет. Сравнение в первую очередь по приоритетам
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
set<SetElem> S;

void UpdatePriorities(set<SetElem>& s) {

    static set<SetElem> save;
    save.clear();
    for (auto i:s) {
        save.insert(SetElem(i.id));
    }
    s.clear();
    for (auto i:save) {
        s.insert(i);
    }
}
char GetSell(int ind) {
    return lab[ind/m][ind%n];
}

//Генерируем движение цели
inline int GetNextGoal(int goal) {
    static set<int> visited;
    visited.insert(goal);
    if (goal-1>0 and goal-1<size and GetSell(goal-1) == '.' and !visited.count(goal-1)==1) {
        return goal-1;
    } else if (goal-n>0 and goal-n<size and GetSell(goal-n) =='.' and !visited.count(goal-n)==1) {
        return goal-n;
    } else if (goal+1>0 and goal+1<size and GetSell(goal+1) == '.' and !visited.count(goal+1)==1) {
        return goal+1;
    } else if (goal+n>0 and goal+n<size and GetSell(goal+n) == '.' and !visited.count(goal+n)==1) {
        return goal+n;
    }
    return graph[goal][rd()%graph[goal].size()];
}

int AStar() {
    fill(g.begin(), g.end(), inf);
    g[start] = 0;
	S.clear();
	vector<int> parents(size, -1);
	S.insert(SetElem(start));
	while (true){
		auto next = S.begin()->id;
        S.erase(S.begin());
		for (auto i:graph[next]) {
			if (g[i]==inf) {
				g[i]=g[next]+1;
				S.insert(SetElem(i));
                parents[i]=next;
			}
		}
        if (next==goal){ 
            break;
        }
	}
	int it = goal;
	int res;
    path.clear();
	while (it!=start) {
		res = it;
		it = parents[it];
        path.push_back(it);
	}
    // cout <<start<<' '<< res <<' '<< goal<<endl;
	return res;
}

int StartA() {
	g.resize(size, inf);
	while (start != goal) {
		time_cur = clock();
        last_start = start;
		start = AStar();
        old_goal = goal;
		goal = GetNextGoal(goal);
		times.push_back(clock()-time_cur);
	}
}

int sum(vector<int> &v) {
    int summ = 0;
    for (auto i:v) {
        summ += i;
    }
    return summ;
}

int main(int argc, char** argv) {
    GetGraphFromTXT(argv[1]);
 	StartA();
    // for (auto i:times) {
    //     cout <<((double)i)/CLOCKS_PER_SEC<<' ';
    // }
    cout << times.size()<<endl;
    cout<<endl<<(double)sum(times)/CLOCKS_PER_SEC;
    return 0;
}
