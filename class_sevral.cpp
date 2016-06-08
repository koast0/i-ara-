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
#include <queue>
#include <unistd.h>

#define m_p(a, b) make_pair(a, b)
using namespace std;
const int inf = 1e8+7;

//Логи для визувлизации
static ofstream logging;
static ofstream paths;


//Исходный граф
class IARA{
 private:
    //Возможные соседи в лабиринте
    vector<pair<int, int>> neigh = {m_p(0, 1), m_p(0, -1), m_p(1, 0), m_p(-1, 0)};
    double eps_max = 1.8;
    double eps = eps_max;
    //Начальный эпсилон I-ARA*

    //Шаг I-ara*
    double step = 0.3;
    // числа вершин лабиринта, переменные старта, последней посещеной
    int size, start, goal, last_start, real_goal, agent_count;
    //Размер лабиринта. Изменяется после ввода.
    int n =0;
    //Текстовый граф 
    vector<string> lab;
    int m = 0;
    int path_len=inf;
    int old_goal = inf;

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
    int id;
    //
    mt19937 rd;

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
    // Вычисление приоритета вершины (эвристики на итерации)
    double GetPriority(int i) {
        return min((double) inf, g[i] + eps*Heruestic(i, goal));
    }
    // Мы храним все вершины сетов в таком формате
    // Для каждой вершины храним ее номер и приоритет. Сравнение в первую очередь по приоритетам
    struct SetElem {
        int id;
        double priority;
        SetElem(int id, IARA* owner):id(id) {
            priority = owner->GetPriority(id);
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

    // Итерация алгоритма ARA*
    bool ImprovePath() {
        while (OPEN.size() and GetPriority(goal) > GetPriority(OPEN.begin()->id)) {
            SetElem s = *OPEN.begin();
            CLOSED.insert(s);
            OPEN.erase(OPEN.begin());
            v[s.id] = g[s.id];
            for(auto new_s: graph[s.id]) {
                if (g[new_s] > v[s.id] + 1) {
                    parents[new_s] = s.id;
                    auto next = SetElem(new_s, this);
                    if (CLOSED.find(next) == CLOSED.end()) {
                        OPEN.erase(next);
                        g[new_s] = v[s.id] + 1;
                        if (INCONS.find(next) == INCONS.end()) {
                            OPEN.insert(SetElem(new_s, this));
                        } else {
                            INCONS.erase(next);
                            INCONS.insert(SetElem(new_s, this));
                        }

                    } else {
                        CLOSED.erase(SetElem(new_s, this));
                        g[new_s] = v[s.id] + 1; 
                        INCONS.insert(SetElem(new_s, this));
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

        set<SetElem> save;
        for (auto i:s) {
            save.insert(SetElem(i.id, this));
        }
        s = save;
    }

    // Функция, запускающая итерацию ARA*, меняя при этом эпсилон итерации
    bool ComputePath() {
        while(true) {
            auto result = ImprovePath();
            if (result == false){
                return false;
            }
            if (eps == 1) {
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
            OPEN.erase(SetElem(start, this));
            INCONS.erase(SetElem(start, this));
            g[start] = v[start];
        }
    }
    // Удаление Step2 ведется этим dfs-ом
    void DfsDelete(int current) {
        INCONS.erase(SetElem(current, this));
        OPEN.erase(SetElem(current, this));
        v[current] = inf;
        g[current] = inf;
        parents[current] = -1;
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
        }
    }
    // Добавляем от нового пути DELETED в новые OPEN.
    void Step3() {
        for (auto i:DELETED) {
            for(auto j:graph[i]) {
                if (g[i] > v[j] + 1) {
                    g[i] = v[j] + 1;
                    parents[i]=j;
                }
            }
            if (g[i]!=inf) {
                OPEN.insert(SetElem(i, this));
            }
        }
        UpdatePriorities(INCONS);
        // OPEN.insert(INCONS.begin(), INCONS.end());
        // for (auto i:CLOSED) {
        //     closes << i.id <<' ';
        // }
        // closes << endl;
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
        path.push_back(current);
        while (current!=start) {
            current = parents[current];
            paths << current<<' ' ;
            path.push_back(current);
        }
        path_len = path.size();
    }
    //Генерируем движение цели
    char GetSell(int ind) {
        return lab[ind/m][ind%n];
    }
    set<int> visited;

    int GetNextGoal(int current_goal) {
        visited.insert(current_goal);
        // return graph[current_goal][rd()%graph[current_goal].size()];
        if (current_goal-1>0 and current_goal-1<size and (current_goal%m!=0) and
            GetSell(current_goal-1) == '.' and !visited.count(current_goal-1)==1) {
            return current_goal-1;
        } else if (current_goal-n>0 and current_goal-n<size and GetSell(current_goal-n) =='.' and !visited.count(current_goal-n)==1) {
            return current_goal-n;
        } else if (current_goal+1>0 and current_goal+1<size and (current_goal%m!=m-1) and
                     GetSell(current_goal+1) == '.' and !visited.count(current_goal+1)==1) {
            return current_goal+1;
        } else if (current_goal+n>0 and current_goal+n<size and GetSell(current_goal+n) == '.' and !visited.count(current_goal+n)==1) {
            return current_goal+n;
        }
        return graph[current_goal][rd()%graph[current_goal].size()];
    }


 public:
    vector <vector<int>> graph;
    IARA() : rd(2016) {}
    int total = 0;


    int GetStart() {
        return start;
    }
    int GetGoal() {
        return real_goal;
    }
    int GetFakeGoal() {
        return real_goal;
    }
    int GetPathSize() {
        return path.size();
    }
    int GetId() {
        return id;
    }
    void SetId(int i) {
        id = i;
    }
    void SetAgentCount(int i){
        agent_count = i;
    }

    int GetHeight(){
        return n;
    }

    int GetWidth() {
        return m;
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
        real_goal = goal;
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

    bool StartIAra() {
        last_start = -1;
        //Заполняем v-value g-value бесконечностями
        v.resize(size, inf);
        g.resize(size, inf);
        parents.resize(size, -1);
        // Запускаем алгоритм
        g[start] = 0;
        OPEN.insert(SetElem(start, this));
    }
    void SetNewGoal(int x) {
        goal = x;
    }

    int AgentStep(int vision) {
        if (id==agent_count-1)
            logging <<start<<' ' <<goal<<' '<<real_goal<<endl;
        else 
            logging <<start<<' '<< goal<<' ';
        time_cur = clock();
        if (ComputePath() == false) {
            return false;
        }
        //Получаем найденый путь
        GetCurentPath();
        old_goal = goal;
        last_start = start;
        if (id ==agent_count-1)
            paths<<'\n';
        int old_path = vision;
        // cout << path[0]<<endl;
        auto next_start = path.rbegin();
        while (start!=real_goal and old_path!=0) {
            ++next_start;
            if (next_start==path.rend())
                break;           
            start=*next_start;
            --old_path;
            real_goal= GetNextGoal(real_goal);
            ++total;
        }
    }
    void AfterStep() {
        UpdatePriorities(OPEN);
        Step1();
        Step2();
        Step3();
        Step4();
        times.push_back(clock()-time_cur);
    }
};

class AimMove {
 private:
    int hunters;
    int n, m;
    vector<vector<int>> graph;
    vector<int> goals;
    int Heruestic(int x, int y) {
        int x_x = x%m;
        int x_y = x/n;
        int y_x = y%m;
        int y_y = y/n;
        return (abs(x_x-y_x) + abs(x_y-y_y));
    }
 public:
    AimMove(int hunter_count, vector<vector<int>> &graph_, int n_, int m_) {
        hunters = hunter_count;
        graph = graph_;
        goals.resize(hunters);
        n = n_;
        m = m_;
    }
    
    vector<int> GetTargets(int goal_, int width, vector<int>& positions) {
        fill(goals.begin(), goals.end(), 0);
        goals[0]=goal_;
        unordered_set<int> used;
        int current_priority = 0;
        int max_j=-1;
        int max_j_val = -1;
        if (width<5) {
            fill(goals.begin(), goals.end(), goal_);
            return goals;
        }
        for (int it = 1; it< hunters;++it) {
            used.clear();
            int current = goal_;
            for (int i=0; i< width; ++i) {
                max_j=-1;
                max_j_val = -1;
                used.insert(current);
                for (auto j: graph[current]) {
                    current_priority = 0;
                    for (int k =0; k<it ;++k) {
                        current_priority+=Heruestic(j,goals[k]);
                    }
                    if (!used.count(j)==1 and current_priority>=max_j_val){
                        max_j_val = current_priority;
                        max_j=j;
                    }
                }
                if (max_j==-1) {
                    for (auto j: graph[current]) {
                        current_priority = 0;
                        for (int k =0; k<it ;++k) {
                            current_priority+=Heruestic(j,goals[k]);
                        }
                        if (current_priority>=max_j_val){
                            max_j_val = current_priority;
                            max_j=j;
                        }
                    }
                }
                current = max_j;
            }
            goals[it]=current;
        }
        return goals;
    }
};

int main(int argc, char** argv) {
    int agent_count = atoi(argv[2]);
    int next_path_length = 0;
    logging.open("log.txt");
    paths.open("paths.txt");
    vector<int> current_goals(agent_count);
    IARA agents[agent_count];
    for (int i = 0; i<agent_count;++i) {
        agents[i].GetGraphFromTXT(argv[1]);
        agents[i].StartIAra();
        agents[i].AgentStep(0);
        agents[i].SetId(i);
        agents[i].SetAgentCount(agent_count);
    }
    vector<int> positions(agent_count);
    AimMove GetAim(agent_count, agents[0].graph, agents[0].GetHeight(), agents[0].GetWidth());
    while(true) {
        bool terminate = false;
        for (int i = 0; i<agent_count;++i) {
            int s=agents[i].GetStart();
            int f = agents[i].GetGoal();
            if (s == f) {
                terminate = true;
                // cout <<terminate;
            }
        }
        if (terminate) {
            break;
        }
        next_path_length = inf;
        cout << "\nGOALS:  "<< agents[0].GetGoal() <<"|| ";
        for (int i=0; i<agent_count; ++i)
            next_path_length = min(agents[0].GetPathSize()/10+1,next_path_length);
        positions.resize(agent_count);
        for (int i=0; i<agent_count; ++i) {
            positions[i]=agents[i].GetStart();
        }
        current_goals = GetAim.GetTargets(agents[0].GetGoal(), next_path_length, positions);

        for (int i=0; i<agent_count; ++i) {
            agents[i].SetNewGoal(current_goals[i]);
            cout <<current_goals[i]<<' ';
        }

        for (int i = 0; i < agent_count; ++i) {
            agents[i].AgentStep(next_path_length);
            if (agents[i].GetStart()!=agents[i].GetGoal())
                agents[i].AfterStep();
        }
        // sleep(1);
        // for (int i=0; i<agent_count; ++i) {
        //     cout<< agents[i].GetStart()<<' '<< agents[i].GetGoal()<<endl;
        // }
    }
    cout << agents[0].total<<endl;
    return 0;
}
