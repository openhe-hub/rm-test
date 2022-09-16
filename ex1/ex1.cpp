#include <iostream>
#include <algorithm>

#define N 1005
#define RIGHT 1
#define LEFT 2
#define UP 3
#define DOWN 4
using namespace std;

int n, m, k;
int **board;


bool dfs(int x, int y, int cnt, int direction) {
    if (x <= 0 || x > n || y <= 0 || y > m || board[x][y]) return false;
    else {
        if (cnt + 1 == n * m - k) return true;
        board[x][y] = 1;
        if (direction == RIGHT) {
            return dfs(x, y + 1, cnt + 1, RIGHT) ||
                   dfs(x + 1, y, cnt + 1, DOWN);
        } else if (direction == LEFT) {
            return dfs(x, y - 1, cnt + 1, LEFT) ||
                   dfs(x - 1, y, cnt + 1, UP);
        } else if (direction == UP) {
            return dfs(x - 1, y, cnt + 1, UP) ||
                   dfs(x, y + 1, cnt + 1, RIGHT);
        } else if (direction == DOWN) {
            return dfs(x + 1, y, cnt + 1, DOWN) ||
                   dfs(x, y - 1, cnt + 1, LEFT);
        }
    }
}

int main() {
    cin >> n >> m >> k;
    board = new int *[n + 1];
    for (int i = 1; i <= n; ++i) {
        board[i] = new int[m + 1];
        for (int j = 1; j <= m; ++j) {
            board[i][j] = 0;
        }
    }
    for (int i = 0; i < k; ++i) {
        int x, y;
        cin >> x >> y;
        board[x][y] = 1;
    }

    cout << (dfs(1, 1, 0, RIGHT) ? "Yes" : "No") << endl;
    return 0;
}