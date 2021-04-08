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

//1��������µ�
//-1���Լ��µ�
int board[9][9];


bool dfs_air_visit[9][9];

//cx �� cy ������������� ���������ϡ��������ϣ����ϣ����£����£��������ң����ϣ�����
const int cx[] = { -1, 0, 1, 0, -1, 1, -1, 1, -2, 2, 0, 0 };
const int cy[] = { 0, -1, 0, 1, 1, 1, -1, -1, 0, 0, 2, -2 };

bool inBorder(int x, int y) { return x >= 0 && y >= 0 && x < 9 && y < 9; }


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
	int legalNumForOpp = 0;
	//��ö���Ŀǰ�ĺϷ�λ�ø���
	//���Ŀǰ�Լ��ĺϷ�λ�ñ�
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

		//���������һ�����µ���Ϊ0ʱ��������Ӧ
		if (newLegalNumForOpp == 0) {
			//floorΪż��ʱ��˵�����ҷ��ľ�ʤ��
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

//fx��fy�ǿ��µĵ�
//���ĳ����Ľ���Ȩֵ
double getOffensiveValue(int fx, int fy, int col) {
	double offensiveValue = 0;
	bool available_list[12];

	
	//ȥ�������ⲽ��֮ǰ�����˾ͻ���ĵ㣬�����ĵ㲻���ڿ��Ƿ�Χ��
	for (int dir = 0; dir < 12; dir++) {
		int dx = fx + cx[dir], dy = fy + cy[dir];
		if (inBorder(dx, dy) && board[dx][dy] == 0 && !judgeAvailable(dx, dy, col)) {
			available_list[dir] = false;
		}
		else {
			available_list[dir] = true;
		}
	}

	//�����ⲽ��֮ǰ�����ֵĺϷ�λ����
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

	//�����ⲽ��֮�󣬶��ֵĺϷ�λ����
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
	//ĳ�������ϣ�����Ҳ���Ƿ���Ȩֵ
	offensiveValue += (legalNumForOpp - newLegalNumForOpp - 1);
	//cout << "off:" << offensiveValue << " ";



	for (int dir = 0; dir < 4; dir++) {
		if (available_list[dir]) {
			int dx = fx + cx[dir], dy = fy + cy[dir];
			if (inBorder(dx, dy)) {
				//��������������ж��ֵ��壬ȨֵӦ�ý���
				if (board[dx][dy] == -col) {
					offensiveValue -= 0.8;
					//cout << "-0.8" << " ";
				}
				//�����Լ����壬ȨֵҲӦ���ͣ� �ҿ۵ĸ���
				else if (board[dx][dy] == col) {
					offensiveValue -= 1;
					//cout << "-1" << " ";
				}
			}
			else {
				//�����Χ�ĵ��в��������ڵģ�
				//������������²��˵ģ�����Ȩֵ�ý���
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
				//�����Χ�е������˻�����ģ�Ȩֵ�ý���
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

//��ý���Ȩ-����Ȩֵ������XY��ֵ
XYWithValue getMyMaxValueXY() {
	int col = -1;
	vector<int> available_list; //�Ϸ�λ�ñ� 
	int tmpx = -1;
	int tmpy = -1;

	//���Ŀǰcol�ĺϷ�λ�ñ�
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

	//�ԺϷ�λ�ñ��еĵ㣬���н���Ȩֵ����
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
	vector<int> available_list; //�Ϸ�λ�ñ� 
	int tmpx = -1;
	int tmpy = -1;

	//���Ŀǰcol�ĺϷ�λ�ñ�
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			//��֤�����Է����£����ҷ�Ҳ����
			if (judgeAvailable(i, j, col) && judgeAvailable(i, j, -col)) {
				available_list.push_back(i * 9 + j);
			}
		}
	}

	double maxOffensiveValue = -1000;
	double tempOffensiveValue = -1000;
	int OffensiveX = -1;
	int OffensiveY = -1;

	//�ԺϷ�λ�ñ��еĵ㣬���н���Ȩֵ����
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
	vector<int> available_list; //�Ϸ�λ�ñ� 
	vector<double> value_list;
	int tmpx = -1;
	int tmpy = -1;
	double max = 1 << 31;
	int maxXY = -1;
	int legalNumForOpp = 0;
	//��ö���Ŀǰ�ĺϷ�λ�ø���
	//���Ŀǰ�Լ��ĺϷ�λ�ñ�
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
	//1 Ϊ�� ��-1Ϊ��
	string str;
	int x, y;
	// ����JSON
	getline(cin, str);
	//getline(cin, str);
	Json::Reader reader;
	Json::Value input;
	reader.parse(str, input);
	// �����Լ��յ���������Լ���������������ָ�״̬
	int turnID = input["responses"].size();
	for (int i = 0; i < turnID; i++)
	{
		//ֻҪrequests����x = -1 �����˵���ҷ��Ǻ���
		x = input["requests"][i]["x"].asInt(), y = input["requests"][i]["y"].asInt();
		if (x != -1) board[x][y] = 1;

		x = input["responses"][i]["x"].asInt(), y = input["responses"][i]["y"].asInt();
		if (x != -1) board[x][y] = -1;
	}
	x = input["requests"][turnID]["x"].asInt(), y = input["requests"][turnID]["y"].asInt();
	if (x != -1) {
		board[x][y] = 1;
	}


	// �������JSON
	Json::Value ret;
	Json::Value action;

	//maxFloor�����ŵݹ�Ĳ���
	//maxFloor�����ŵݹ�Ĳ���
	/*int maxXY = getMaxValueXY(-1, 7, 1.2222);
	action["x"] = maxXY / 9; action["y"] = maxXY % 9;*/

	XYWithValue myDot = getMyMaxValueXY();
	//��ȡ����Ľ���Ȩֵ���ĵ�
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