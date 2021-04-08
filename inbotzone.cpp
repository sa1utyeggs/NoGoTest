#include <iostream>
#include <cstdio>
#include <string>
#include <cstring>
#include "jsoncpp/json.h"
using namespace std;


struct XYWithValue {
public:
	int x;
	int y;
	double value;

	XYWithValue(int x1, int y1, double value1) {
		x = x1;
		y = y1;
		value = value1;
	}
};

//1代表对面下的
//-1是自己下的
int board[9][9];


bool dfs_air_visit[9][9];

//cx 和 cy 组合起来代表着 “左下右上”，“左上，右上，左下，右下，左左，右右，上上，下下
const int cx[] = { -1, 0, 1, 0, -1, 1, -1, 1, -2, 2, 0, 0 };
const int cy[] = { 0, -1, 0, 1, 1, 1, -1, -1, 0, 0, 2, -2 };

bool inBorder(int x, int y) { return x >= 0 && y >= 0 && x < 9 && y < 9; }


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
	int legalNumForOpp = 0;
	//获得对手目前的合法位置个数
	//获得目前自己的合法位置表
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			if (judgeAvailable(i, j, col)) {
				available_list.push_back(i * 9 + j);
				value_list.push_back(0);
			}

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

		//如果发现有一方能下的棋为0时，作出反应
		if (newLegalNumForOpp == 0) {
			//floor为偶数时，说明是我方的决胜步
			if (floor % 2 == 0) {
				return 100;
			}
			else {
				return -100;
			}
		}

		value_list[i] -= weight * getMaxValue(-col, floor + 1, maxFloor, weight);

		board[tmpx][tmpy] = 0;

		if (value_list[i] > max) {
			max = value_list[i];
		}
	}
	return max;
}

//fx，fy是可下的点
//获得某个点的进攻权值
double getOffensiveValue(int fx, int fy, int col) {
	double offensiveValue = 0;
	bool available_list[12];

	
	//去除在下这步棋之前，下了就会输的点，这样的点不算在考虑范围内
	for (int dir = 0; dir < 12; dir++) {
		int dx = fx + cx[dir], dy = fy + cy[dir];
		if (inBorder(dx, dy) && board[dx][dy] == 0 && !judgeAvailable(dx, dy, col)) {
			available_list[dir] = false;
		}
		else {
			available_list[dir] = true;
		}
	}

	//下完这步棋之前，对手的合法位置数
	int legalNumForOpp = 0;
	for (int p = 0; p < 9; p++) {
		for (int q = 0; q < 9; q++) {
			if (judgeAvailable(p, q, -col)) {
				legalNumForOpp++;
			}
			/*else if (judgeAvailable(p, q, col)) {
				cout << "P:" << p << " Q:" << q << " ";
			}*/
		}
	}
	//cout << endl;

	board[fx][fy] = col;

	//下完这步棋之后，对手的合法位置数
	int newLegalNumForOpp = 0;
	for (int p = 0; p < 9; p++) {
		for (int q = 0; q < 9; q++) {
			if (judgeAvailable(p, q, -col)) {
				newLegalNumForOpp++;
			}
			/*else if (judgeAvailable(p, q, col)) {
				cout << "P:" << p << " Q:" << q << " ";
			}*/
		}
	}
	//某种意义上，这样也算是防守权值
	offensiveValue += (legalNumForOpp - newLegalNumForOpp - 1);
	//cout << "off:" << offensiveValue << " ";



	for (int dir = 0; dir < 4; dir++) {
		if (available_list[dir]) {
			int dx = fx + cx[dir], dy = fy + cy[dir];
			if (inBorder(dx, dy)) {
				//如果上下左右是有对手的棋，权值应该降低
				if (board[dx][dy] == -col) {
					offensiveValue -= 0.8;
					//cout << "-0.8" << " ";
				}
				//若是自己的棋，权值也应降低， 且扣的更多
				else if (board[dx][dy] == col) {
					offensiveValue -= 1;
					//cout << "-1" << " ";
				}
			}
			else {
				//如果周围的点有不在棋盘内的，
				//由于这个点是下不了的，所以权值得降低
				offensiveValue -= 0.5;
				//cout << "-0.5" << " ";
			}
		}
	}

	for (int dir = 4; dir < 12; dir++) {
		if (available_list[dir]) {
			int dx = fx + cx[dir], dy = fy + cy[dir];
			if (inBorder(dx, dy)) {
				if (board[dx][dy] == col) {
					offensiveValue += 0.9;
					//cout << "+0.9" << " ";
				}
				else if (board[dx][dy] == -col)
				{
					offensiveValue -= 0.2;
					//cout << "-0.2" << " ";
				}
				//如果周围有点是下了会输棋的，权值得降低
				else if (!judgeAvailable(dx, dy, col)) {
					//cout << "lose:";
					offensiveValue--;
					//cout << "-1" << " ";
				}
			}
			else {
				offensiveValue += 0.71;
				//cout << "+0.71" << " ";
			}
		}
	}

	board[fx][fy] = 0;

	return offensiveValue;
}

//获得进攻权-防守权值，最大的XY，值
XYWithValue getMyMaxValueXY() {
	int col = -1;
	vector<int> available_list; //合法位置表 
	int tmpx = -1;
	int tmpy = -1;

	//获得目前col的合法位置表
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			if (judgeAvailable(i, j, col)) {
				available_list.push_back(i * 9 + j);
			}
		}
	}

	double maxOffensiveValue = -1000;
	double tempOffensiveValue = -1000;
	int OffensiveX = -1;
	int OffensiveY = -1;

	//对合法位置表中的点，进行进攻权值运算
	for (int i = 0; i < available_list.size(); i++) {
		tmpx = available_list[i] / 9;
		tmpy = available_list[i] % 9;
		tempOffensiveValue = getOffensiveValue(tmpx, tmpy, col);
		//cout << tmpx << " " << tmpy << " " << tempOffensiveValue << " " <<maxOffensiveValue << endl;
		if (tempOffensiveValue > maxOffensiveValue) {
			maxOffensiveValue = tempOffensiveValue;
			OffensiveX = tmpx;
			OffensiveY = tmpy;
		}
	}
	return XYWithValue(OffensiveX, OffensiveY, maxOffensiveValue);
}

XYWithValue getOppMaxValueXY() {
	int col = 1;
	vector<int> available_list; //合法位置表 
	int tmpx = -1;
	int tmpy = -1;

	//获得目前col的合法位置表
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			//保证这个点对方能下，且我方也能下
			if (judgeAvailable(i, j, col) && judgeAvailable(i, j, -col)) {
				available_list.push_back(i * 9 + j);
			}
		}
	}

	double maxOffensiveValue = -1000;
	double tempOffensiveValue = -1000;
	int OffensiveX = -1;
	int OffensiveY = -1;

	//对合法位置表中的点，进行进攻权值运算
	for (int i = 0; i < available_list.size(); i++) {
		tmpx = available_list[i] / 9;
		tmpy = available_list[i] % 9;
		tempOffensiveValue = getOffensiveValue(tmpx, tmpy, col);
		//cout << tmpx << " " << tmpy << " " << tempOffensiveValue << " " <<maxOffensiveValue << endl;
		if (tempOffensiveValue > maxOffensiveValue) {
			maxOffensiveValue = tempOffensiveValue;
			OffensiveX = tmpx;
			OffensiveY = tmpy;
		}
	}
	return XYWithValue(OffensiveX, OffensiveY, maxOffensiveValue);
}

int getMaxValueXY(int col, int maxFloor, double weight) {
	vector<int> available_list; //合法位置表 
	vector<double> value_list;
	int tmpx = -1;
	int tmpy = -1;
	double max = 1 << 31;
	int maxXY = -1;
	int legalNumForOpp = 0;
	//获得对手目前的合法位置个数
	//获得目前自己的合法位置表
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			if (judgeAvailable(i, j, col)) {
				available_list.push_back(i * 9 + j);
				value_list.push_back(0);
			}

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

		value_list[i] -= getMaxValue(-col, 1, maxFloor, weight);

		board[tmpx][tmpy] = 0;

		if (value_list[i] > max) {
			max = value_list[i];
			maxXY = tmpx * 9 + tmpy;
		}
	}
	return maxXY;
}

int main()
{
	srand((unsigned)time(0));
	//1 为黑 ，-1为白
	string str;
	int x, y;
	// 读入JSON
	getline(cin, str);
	//getline(cin, str);
	Json::Reader reader;
	Json::Value input;
	reader.parse(str, input);
	// 分析自己收到的输入和自己过往的输出，并恢复状态
	int turnID = input["responses"].size();
	for (int i = 0; i < turnID; i++)
	{
		//只要requests中有x = -1 的项，就说明我方是黑子
		x = input["requests"][i]["x"].asInt(), y = input["requests"][i]["y"].asInt();
		if (x != -1) board[x][y] = 1;

		x = input["responses"][i]["x"].asInt(), y = input["responses"][i]["y"].asInt();
		if (x != -1) board[x][y] = -1;
	}
	x = input["requests"][turnID]["x"].asInt(), y = input["requests"][turnID]["y"].asInt();
	if (x != -1) {
		board[x][y] = 1;
	}


	// 输出决策JSON
	Json::Value ret;
	Json::Value action;

	//maxFloor代表着递归的层数
	//maxFloor代表着递归的层数
	/*int maxXY = getMaxValueXY(-1, 7, 1.2222);
	action["x"] = maxXY / 9; action["y"] = maxXY % 9;*/

	XYWithValue myDot = getMyMaxValueXY();
	//获取对面的进攻权值最大的点
	XYWithValue oppDot = getOppMaxValueXY();

	if (myDot.value >= oppDot.value) {
		action["x"] = myDot.x; action["y"] = myDot.y;
		//cout << "my dot" << endl;
	}
	else {
		action["x"] = oppDot.x; action["y"] = oppDot.y;
		//cout << "opp dot" << endl;
	}



	ret["response"] = action;
	Json::FastWriter writer;

	cout << writer.write(ret) << endl;
	return 0;
}