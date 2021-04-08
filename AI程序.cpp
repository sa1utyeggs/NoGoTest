#include <iostream>
#include <cstdio>
#include <string>
#include <cstring>
#include "jsoncpp/json.h"
using namespace std;

//1��������µ�
//-1���Լ��µ�
int board[9][9];


bool dfs_air_visit[9][9];

//cx �� cy ������������� ���������ϡ�
const int cx[] = { -1, 0, 1, 0 };
const int cy[] = { 0, -1, 0, 1 };

bool inBorder(int x, int y) { return x >= 0 && y >= 0 && x < 9 && y < 9; }


//-1 Ϊ�ж��Լ������Ƿ�����
//1 ���Ƕ��ֵ���
bool dfs_air(int fx, int fy)
{
	//�ж��Ƿ��Ѿ���dfs_air�������ʹ���
	dfs_air_visit[fx][fy] = true;
	bool flag = false;
	for (int dir = 0; dir < 4; dir++)
	{
		int dx = fx + cx[dir], dy = fy + cy[dir];
		if (inBorder(dx, dy))
		{
			//��Χ�пգ�����
			if (board[dx][dy] == 0)
				flag = true;
			//�������ͬɫ����û�б�����
			if (board[dx][dy] == board[fx][fy] && !dfs_air_visit[dx][dy])
			{
				//�ж����ŵ�ͬɫ���Ƿ�����
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
	//����
	board[fx][fy] = col;
	//�������Ե����ֽ���������ķ�ʽ�ŵ�ָ�����ڴ���ȥ
	memset(dfs_air_visit, 0, sizeof(dfs_air_visit));
	if (!dfs_air(fx, fy))
	{
		//���û��
		board[fx][fy] = 0;
		return false;
	}
	//�ж����������֮������Χ4�����Ƿ��û��
	for (int dir = 0; dir < 4; dir++)
	{
		int dx = fx + cx[dir], dy = fy + cy[dir];

		if (inBorder(dx, dy))
		{
			//��Χ�Ƿ����ӣ���û�б�dfs_air���ʹ�
			if (board[dx][dy] && !dfs_air_visit[dx][dy])
				if (!dfs_air(dx, dy))
				{
					//�����Χû��������ӾͲ�����
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
	vector<int> available_list; //�Ϸ�λ�ñ� 
	vector<double> value_list;
	int tmpx = -1;
	int tmpy = -1;
	double max = 1 << 31;
	//���Ŀǰ�Լ��ĺϷ�λ�ñ�
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			if (judgeAvailable(i, j, col)) {
				available_list.push_back(i * 9 + j);
				value_list.push_back(0);
			}
		}
	}
	int legalNumForOpp = 0;
	//��ö���Ŀǰ�ĺϷ�λ�ø���
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			if (judgeAvailable(i, j, -col)) {
				legalNumForOpp++;
			}
		}
	}
	//�ԺϷ�λ�ñ��еĵ���м�ֵ����
	for (int i = 0; i < available_list.size(); i++) {
		tmpx = available_list[i] / 9;
		tmpy = available_list[i] % 9;

		board[tmpx][tmpy] = col;

		//�����ⲽ��֮�󣬶��ֵĺϷ�λ����
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
	vector<int> available_list; //�Ϸ�λ�ñ� 
	vector<double> value_list;
	int tmpx = -1;
	int tmpy = -1;
	double max = 1 << 31;
	int maxXY = -1 ;
	//���Ŀǰ�Լ��ĺϷ�λ�ñ�
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			if (judgeAvailable(i, j, col)) {
				available_list.push_back(i * 9 + j);
				value_list.push_back(0);
			}
		}
	}
	//��ö���Ŀǰ�ĺϷ�λ�ø���
	int legalNumForOpp = 0;
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			if (judgeAvailable(i, j, -col)) {
				legalNumForOpp++;
			}
		}
	}
	//�ԺϷ�λ�ñ��еĵ���м�ֵ����
	for (int i = 0; i < available_list.size(); i++) {
		tmpx = available_list[i] / 9;
		tmpy = available_list[i] % 9;

		board[tmpx][tmpy] = -1;

		//�����ⲽ��֮�󣬶��ֵĺϷ�λ����
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