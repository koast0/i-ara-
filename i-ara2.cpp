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

//Исходный граф
vector <vector<int>> graph;
//Возможные соседи в лабиринте
vector<pair<int, int>> neigh = {m_p(0, 1), m_p(0, -1), m_p(1, 0), m_p(-1, 0)};

double eps = 1.7;
//Начальный эпсилон I-ARA*
double eps_max = 1.7;
//Шаг I-ara*
double step = 0.3;
// числа вершин лабиринта, переменные старта, последней посещеной
int size, start, goal, last_start;
//Размер лабиринта. Изменяется после ввода.
int n =0;
int m = 0;
//Логи для визувлизации
ofstream logging;
ofstream paths;
ofstream opens;
ofstream closes;
ofstream inconses;
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
random_device rd;

//Перевод координат в лабиринте в номер вершины
int GetIndex(int x, int y) {
    return (x*m+y);
}
//получение Манхетонского расстояния
int Heruestic(int x, int y) {
    int x_x = x%m;
    int x_y = x/n;
    int y_x = y%m;
    int y_y = y/n;
    return (abs(x_x-y_x) + abs(x_y-y_y));
}
// Вычисление приоритета вершины (евристики на итерации)
double GetPriority(int i) {
    return min((double) inf, g[i] + eps*Heruestic(i, goal));
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

// Создаем множества, необходимые алгоритму
set<SetElem> OPEN;
set<SetElem> CLOSED;
set<SetElem> INCONS;
set<int> DELETED;


//Ввод графа из текстового файла и получение его размеров
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
// Итерация алгоритма ARA*
bool ImprovePath() {
    while (GetPriority(goal) > GetPriority(OPEN.begin()->id)) {
        SetElem s = *OPEN.begin();
        CLOSED.insert(s);
        OPEN.erase(s);
        v[s.id] = g[s.id];
        for(auto new_s: graph[s.id]) {
            if (g[new_s] > v[s.id] + 1) {
                parents[new_s] = s.id;
                if (CLOSED.find(SetElem(new_s)) == CLOSED.end()) {
                    if (OPEN.find(SetElem(new_s)) == OPEN.end() and INCONS.find(SetElem(new_s)) == INCONS.end()) {
                        g[new_s] = v[s.id] + 1;
                        OPEN.insert(SetElem(new_s));
                    }
                } else {
                    CLOSED.erase(SetElem(new_s));
                    g[new_s] = v[s.id] + 1; 
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

//Функция, обновляющаяя приоритеты выбраного множества SetElem
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
// Функция, запускающая итерацию ARA*, меняя при этом эпсилон итерации
bool ComputePath() {
    while(true) {
        auto result = ImprovePath();
        if (result == false){
            return false;
        }
        if (eps == 1) {
            // cout << CLOSED.size()<<endl;
            return true;
        }
        OPEN.insert(INCONS.begin(), INCONS.end());
        INCONS.clear();
        CLOSED.clear();
        eps = fmax(1, eps-step);
        UpdatePriorities(OPEN);
    }
}

// Нам не нужно пересчитывать путь для стартовой вершины. Поэтому удаляем ее из Open и делаем ее 
// g-value равным v-value
void Step1() {
    if (g[start]!=v[start]) {
        OPEN.erase(SetElem(start));
        INCONS.erase(SetElem(start));
        g[start] = v[start];
    }
}
// Удаление Step2 ведется этим dfs-ом
void DfsDelete(int current) {
    if (OPEN.count(current)==1 or INCONS.count(current)==1) {
    	v[current] = v[current]+1;
    	g[current] = g[current]+1;
    } else {
    	v[current] = inf;
    	g[current] = inf;
        INCONS.erase(SetElem(current));
   		OPEN.erase(SetElem(current));
    }
    DELETED.insert(current);
    for (auto i:graph[current]) {
        if (parents[i]==current) {
            DfsDelete(i);
        }
    }
}
// Удаляем дерево поиска, которое пораждено старой стартовой вершиной, но не достижимо из новой
void Step2() {
    if (start!=last_start) {
        parents[start]=-1;
        DfsDelete(last_start);
        parents[last_start]=start;
    }
}
// Добавляем от нового пути DELETED в новые OPEN.
void Step3() {
    for (auto i:DELETED) {
        for(auto j:graph[i]) {
            if (g[i] > v[j] + 1) {
            	OPEN.erase(SetElem(i));
            	INCONS.erase(SetElem(i));
                g[i] = v[j] + 1;
                parents[i]=j;
            }
        }
        if (g[i]!=inf) {
            OPEN.insert(SetElem(i));
        }
    }
    UpdatePriorities(INCONS);
    OPEN.insert(INCONS.begin(), INCONS.end());
    for (auto i:CLOSED) {
        closes << i.id <<' ';
    }
    closes << endl;
    CLOSED.clear();
    INCONS.clear();
    DELETED.clear();
}

// Получаем эпсилон для следующей итерации. 
void Step4() {
    if (GetPriority(goal) > OPEN.begin()->priority) {
        eps = eps_max;
    }
    else {
        eps = fmax(eps-step, 1);
    }
    UpdatePriorities(OPEN);
}
// Восстанавливаем путь по ходам
bool GetCurentPath() {
    path.clear();
    int current = goal;

    while (current!=start) {
        current = parents[current];
        paths << current<<' ' ;
        path.push_back(current);
    }
}
//Генерируем движение цели
inline int GetNextGoal(int goal) {
    return graph[goal][rd()%graph[goal].size()];
}

bool StartIAra() {
    last_start = -1;
    //Заполняем v-value g-value бесконечностями
    v.resize(size, inf);
    g.resize(size, inf);
    parents.resize(size, -1);
    // Запускаем алгоритм
    g[start] = 0;
    OPEN.insert(SetElem(start));
    closes << endl;
    while (start!=goal) {

        time_cur = clock();
        logging <<start<<' ' <<goal<<endl;
        for (auto i:OPEN) {
            opens << i.id <<' ';
        }
        opens <<endl;
        if (ComputePath() == false) {
            return false;
        }
        //Получаем найденый путь
        GetCurentPath();
        paths<<'\n';
        last_start = start;
        start = *(path.rbegin()+1);
        int old_goal = goal;
        goal = GetNextGoal(goal);
        if (start == goal) {
            break;
        }
        UpdatePriorities(OPEN);
        
        Step1();
        Step2();
        Step3();
        Step4();
        
        //cout<<OPEN.size()<<' ';
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
    //Запись в логи
    logging.open("log.txt");
    paths.open("paths.txt");
    opens.open("opens.txt");
    closes.open("closes.txt");
    inconses.open("inconses.txt");

    StartIAra();
    opens <<endl;
    logging <<start<<' ' <<goal<<endl;
    closes << endl;
    for (auto i:times) {
        cout <<((double)i)/CLOCKS_PER_SEC<<' ';
    }
    cout<<endl<<(double)sum(times)/CLOCKS_PER_SEC;
    return 0;
}
