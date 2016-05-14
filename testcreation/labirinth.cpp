#include <iostream>
#include <vector>
#include <stack>
#include <algorithm>


using std::vector;
using std::cin;
using std::cout;
using std::stack;
using std::random_shuffle;

int height;
int width;

struct cell {
	int x;
	int y;
	cell(int x_, int y_) {
		x = x_;
		y = y_;
	}
	cell() {
		x = 0;
		y = 0;
	}
};
struct Par_Ch {
	cell parent;
	cell child;
	Par_Ch(cell x_, cell y_) {
		parent = x_;
		child = y_;
	}
};

const cell Nlist[4]= {cell(0,2), cell(-2, 0), cell(2, 0), cell(0, -2)};



void build(vector<vector<char>> &maze) {
	height = maze.size();
	width = maze[0].size();
	for(int i = 0; i < height; i++){
        for(int j = 0; j < width; j++){
            if((i % 2 != 0  && j % 2 != 0) && 
               (i < height-1 && j < width-1)) 
                   maze[i][j] = '0';    

            else maze[i][j] = '#';   
            //cout<<maze[i][j];         
        }
        //cout <<'\n';
    }
}

vector<cell> GetNeighbours(cell current) {
	vector<cell> neigh= {};
	for (auto i:Nlist) {
		if (current.x+i.x<height && current.x+i.x>0 &&
			current.y+i.y<width&& current.y+i.y>0) {
			neigh.push_back(cell(current.x+i.x,current.y+i.y));
		}
	}
	return neigh;
}



void dfs(vector<vector<char>> &maze, cell start) {
	stack<Par_Ch> dfs_order;
	vector<cell> neigh;
	dfs_order.push(Par_Ch(start, start));
	cell current;
	cell last = start;
	while (!dfs_order.empty()) {
		current = dfs_order.top().child;
		last = dfs_order.top().parent;
		if (maze[current.x][current.y] != '0') {
			dfs_order.pop();
			continue;
		}
		maze[(current.x+last.x)/2][(current.y+last.y)/2] = '.';
		maze[current.x][current.y] = '.';
		dfs_order.pop();

		neigh = GetNeighbours(current);
		random_shuffle(neigh.begin(), neigh.end());
		for (auto i:neigh) {
			if (maze[i.x][i.y] == '0')  {
				dfs_order.push(Par_Ch(current, i));
			}
		}
	}
}

int main() {
	int n, m;
	cin >> n >> m;
	double p = 0.3;
	n = n*2+1;
	m = m*2+1;
	vector<vector<char>> lab(n);
	for (int i= 0; i <n; ++i) {
		lab[i].resize(m);
	}
	srand(time(0));
	build(lab);
	cell start(1,1);
	dfs(lab, start);
	for (int i = 1; i < n-1; ++i) {
		for (int j = 1; j < m-1; ++j) {
			if (lab[i][j] =='#') {
				if (((double)rand())/RAND_MAX < p) {
					lab[i][j] = '.';
				}
			}
		}
	}
	for (int i = 1; i < n-1; ++i) {
		for (int j = 1; j < m-1; ++j) {
			cout<<lab[i][j];
		}
		cout <<'\n';
	}

	return 0;
}