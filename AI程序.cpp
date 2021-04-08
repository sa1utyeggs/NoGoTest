#include <iostream>
#include <cstdio>
#include <string>
#include <cstring>
#include "jsoncpp/json.h"
using namespace std;

//1代表对面下的
//-1是自己下的
int board[9][9];


bool dfs_air_visit[9][9];

//cx 和 cy 组合起来代表着 “左下右上”
const int cx[] = { -1, 0, 1, 0 };
const int cy[] = { 0, -1, 0, 1 };

bool inBorder(int x, int y) { return x >= 0 && y >= 0 && x < 9 && y < 9; }


//-1 为判断自己的棋是否有气
//1 则是对手的棋
bool dfs_air(int fx, int fy)
{
	//判断是否已经被dfs_air方法访问过了
	dfs_air_visit[fx][fy] = true;
	bool flag = false;
	for (int dir = 0; dir < 4; dir++)
	{
		int dx = fx + cx[dir], dy = fy + cy[dir];
		if (inBorder(dx, dy))
		{
			//周围有空，有气
			if (board[dx][dy] == 0)
				flag = true;
			//如果相邻同色，且没有被访问
			if (board[dx][dy] == board[fx][fy] && !dfs_air_visit[dx][dy])
			{
				//判断连着的同色子是否有气
				if (dfs_air(dx, dy))
					flag = true;
			}
		}
	}
	return flag;
}

//true: available
bool judgeAvailable(int fx, int fy, int col)
{
	if (board[fx][fy] != 0) {
		return false;
	}
	//下子
	board[fx][fy] = col;
	//将数字以单个字节逐个拷贝的方式放到指定的内存中去
	memset(dfs_air_visit, 0, sizeof(dfs_air_visit));
	if (!dfs_air(fx, fy))
	{
		//如果没气
		board[fx][fy] = 0;
		return false;
	}
	//判断下完这个棋之后，其周围4个棋是否会没气
	for (int dir = 0; dir < 4; dir++)
	{
		int dx = fx + cx[dir], dy = fy + cy[dir];

		if (inBorder(dx, dy))
		{
			//周围是否有子，且没有被dfs_air访问过
			if (board[dx][dy] && !dfs_air_visit[dx][dy])
				if (!dfs_air(dx, dy))
				{
					//如果周围没气，这个子就不能下
					board[fx][fy] = 0;
					return false;
				}
		}
	}
	board[fx][fy] = 0;
	return true;
}

double getMaxValue(int col, int floor, int maxFloor, double weight) {
	if (floor >= maxFloor) {
		return 0;
	}
	vector<int> available_list; //合法位置表 
	vector<double> value_list;
	int tmpx = -1;
	int tmpy = -1;
	double max = 1 << 31;
	//获得目前自己的合法位置表
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			if (judgeAvailable(i, j, col)) {
				available_list.push_back(i * 9 + j);
				value_list.push_back(0);
			}
		}
	}
	int legalNumForOpp = 0;
	//获得对手目前的合法位置个数
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			if (judgeAvailable(i, j, -col)) {
				legalNumForOpp++;
			}
		}
	}
	//对合法位置表中的点进行价值评估
	for (int i = 0; i < available_list.size(); i++) {
		tmpx = available_list[i] / 9;
		tmpy = available_list[i] % 9;

		board[tmpx][tmpy] = col;

		//下完这步棋之后，对手的合法位置数
		int newLegalNumForOpp = 0;
		for (int p = 0; p < 9; p++) {
			for (int q = 0; q < 9; q++) {
				if (judgeAvailable(p, q, -col)) {
					newLegalNumForOpp++;
				}
			}
		}

		value_list[i] = legalNumForOpp - newLegalNumForOpp - 1;

		value_list[i] -= weight * getMaxValue(-col, ++floor, maxFloor, weight);

		board[tmpx][tmpy] = 0;

		if (value_list[i] > max) {
			max = value_list[i];
		}
	}
	return max;
}

int getMaxValueXY(int col, int maxFloor, double weight) {
	vector<int> available_list; //合法位置表 
	vector<double> value_list;
	int tmpx = -1;
	int tmpy = -1;
	double max = 1 << 31;
	int maxXY = -1 ;
	//获得目前自己的合法位置表
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			if (judgeAvailable(i, j, col)) {
				available_list.push_back(i * 9 + j);
				value_list.push_back(0);
			}
		}
	}
	//获得对手目前的合法位置个数
	int legalNumForOpp = 0;
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			if (judgeAvailable(i, j, -col)) {
				legalNumForOpp++;
			}
		}
	}
	//对合法位置表中的点进行价值评估
	for (int i = 0; i < available_list.size(); i++) {
		tmpx = available_list[i] / 9;
		tmpy = available_list[i] % 9;

		board[tmpx][tmpy] = -1;

		//下完这步棋之后，对手的合法位置数
		int newLegalNumForOpp = 0;
		for (int p = 0; p < 9; p++) {
			for (int q = 0; q < 9; q++) {
				if (judgeAvailable(p, q, -col)) {
					newLegalNumForOpp++;
				}
			}
		}

		value_list[i] = legalNumForOpp - newLegalNumForOpp;

		if (maxFloor > available_list.size() - 1) {

		}

		value_list[i] -= weight * getMaxValue(-col, 1, maxFloor, weight);

		board[tmpx][tmpy] = 0;

		cout << "x:" << available_list[i] / 9 << " y:" << available_list[i] % 9 << " " << value_list[i] << endl;

		if (value_list[i] > max) {
			max = value_list[i];
			maxXY = tmpx * 9 + tmpy;
		}
	}
	return maxXY;
}

int main()
{
	int c = 1;
	int N = 11;
	cout << 0 / (0 + 0.0000000001) + c * sqrt(2 * log(N) / (0 + 0.0000000001));
	return 0;
}