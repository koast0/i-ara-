#include <iostream>
#include <vector>
#include <stack>
#include <algorithm>
#include <random>

using std::vector;
using std::cin;
using std::cout;
using std::stack;
using std::random_shuffle;

int X=3;
int Y=2;
int main() {
	int n, m;
	double p;
	cin >> n >> m >> p;
	int points = round(n*m*p);
	vector<vector<char>> lab(n);
	for (int i= 0; i <n; ++i) {
		lab[i].resize(m, '.');
	}
	int k =0;
	int i = 0;
	int j = 0;
	int nY, nX;
	int type = 0;
	srand(time(0));
	std::default_random_engine generator;
	std::normal_distribution<double> distribution(X,Y);
	while (k!= points) {
		++k;
		int nX = round(distribution(generator));
		if (!(nX >= 0 && nX <=2*X)) continue;
		++nX;
		i = rand()%n;
		j = rand()%m;
		type = rand()%2;
		if (type==1) {
			for(int t = i -(nX/2); t<i +(nX/2); ++t) {
				if ((t<0 or t>m-1) or (t==n-1 and j == m-1) or (t==0 and j == 0)) continue;
				lab[t][j] = '#';
			}
		}
		if (type==0) {
			for(int t = j -(nX/2); t<j +(nX/2); ++t) {
				if ((t<0 or t>n-1) or (t==m-1 and i == n-1) or (t==0 and i == 0)) continue;
				lab[i][t] = '#';
			}
		}

	}
	for (int i = 0; i < n; ++i) {
		for (int j = 0; j < m; ++j) {
			cout<<lab[i][j];
		}
		cout <<'\n';
	}
	return 0;
}