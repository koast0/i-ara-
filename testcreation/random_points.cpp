#include <iostream>
#include <vector>
#include <stack>
#include <algorithm>


using std::vector;
using std::cin;
using std::cout;
using std::stack;
using std::random_shuffle;

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
	srand(time(0));
	while (k!= points) {
		++k;
		i = rand()%n;
		j = rand()%m;
		//cout << i <<' '<< j << "//";
		if (lab[i][j]=='#') {
			continue;
		}
		if ((i==n-1 and j == m-1) or (i==0 and j == 0)) {
			continue;
		}
		//cout << i <<' '<< j << std::endl;
		lab[i][j] = '#';
		
	}
	for (int i = 0; i < n; ++i) {
		for (int j = 0; j < m; ++j) {
			cout<<lab[i][j];
		}
		cout <<'\n';
	}
	return 0;
}