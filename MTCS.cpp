#include <iostream>
#include <cstdio>
#include <string>
#include <cstring>
#include<cmath>
#include "jsoncpp/json.h"
using namespace std;

//1��������µ�
//-1���Լ��µ�
int board[9][9];

bool dfs_air_visit[9][9];

//cx �� cy ������������� ���������ϡ��������ϣ����ϣ����£����£��������ң����ϣ�����
const int cx[] = { -1, 0, 1, 0, -1, 1, -1, 1, -2, 2, 0, 0 };
const int cy[] = { 0, -1, 0, 1, 1, 1, -1, -1, 0, 0, 2, -2 };

bool inBorder(int x, int y) { return x >= 0 && y >= 0 && x < 9 && y < 9; }

bool dfs_air(int fx, int fy, int tempBoard[9][9])
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
			if (tempBoard[dx][dy] == 0)
				flag = true;
			//�������ͬɫ����û�б�����
			if (tempBoard[dx][dy] == tempBoard[fx][fy] && !dfs_air_visit[dx][dy])
			{
				//�ж����ŵ�ͬɫ���Ƿ�����
				if (dfs_air(dx, dy, tempBoard))
					flag = true;
			}
		}
	}
	return flag;
}

//true: available
bool judgeAvailable(int fx, int fy, int col, int tempBoard[9][9])
{
	if (tempBoard[fx][fy] != 0) {
		return false;
	}
	//����
	tempBoard[fx][fy] = col;
	//�������Ե����ֽ���������ķ�ʽ�ŵ�ָ�����ڴ���ȥ
	memset(dfs_air_visit, 0, sizeof(dfs_air_visit));
	if (!dfs_air(fx, fy, tempBoard))
	{
		//���û��
		tempBoard[fx][fy] = 0;
		return false;
	}
	//�ж����������֮������Χ4�����Ƿ��û��
	for (int dir = 0; dir < 4; dir++)
	{
		int dx = fx + cx[dir], dy = fy + cy[dir];

		if (inBorder(dx, dy))
		{
			//��Χ�Ƿ����ӣ���û�б�dfs_air���ʹ�
			if (tempBoard[dx][dy] && !dfs_air_visit[dx][dy])
				if (!dfs_air(dx, dy, tempBoard))
				{
					//�����Χû��������ӾͲ�����
					tempBoard[fx][fy] = 0;
					return false;
				}
		}
	}
	tempBoard[fx][fy] = 0;
	return true;
}


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

//fx��fy�ǿ��µĵ�
//���ĳ����Ľ���Ȩֵ
double getOffensiveValue(int fx, int fy, int col) {
	double offensiveValue = 0;
	bool available_list[12];


	//ȥ�������ⲽ��֮ǰ�����˾ͻ���ĵ㣬�����ĵ㲻���ڿ��Ƿ�Χ��
	for (int dir = 0; dir < 12; dir++) {
		int dx = fx + cx[dir], dy = fy + cy[dir];
		if (inBorder(dx, dy) && board[dx][dy] == 0 && !judgeAvailable(dx, dy, col,board)) {
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
			if (judgeAvailable(p, q, -col,board)) {
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
			if (judgeAvailable(p, q, -col,board)) {
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
				else if (!judgeAvailable(dx, dy, col,board)) {
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
			if (judgeAvailable(i, j, col,board)) {
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
			if (judgeAvailable(i, j, col,board) && judgeAvailable(i, j, -col,board)) {
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

class Node {
private:
	//��ʱ��UCBֵ
	double value;
	//Ŀǰ��̽������
	int exploreTimes;
	//����һ�������ŵ� xy ֵ
	int x;
	int y;
	//����ӽڵ�
	Node* sons[81];
	//���ӽڵ�ĸ���
	int sonNumber;
	//���׽ڵ�
	Node* father;


	//floor������һ��ģ������Լ����ǶԷ�
	int floor;

	//�������ڵ㣬ģ�����
	//ʤ��̫С���ͷ��������ӽڵ�
	bool giveSon;

	double UCBValue;

public:
	//�������ⲽ��ʱ�������
	int nodeBoard[9][9];
	//���ڵ�
	Node static originalNode;

	//�޲εĹ��캯��ר�Ÿ����ڵ�ʹ��
	Node() {
		value = 0;
		exploreTimes = 1;
		x = -1;
		y = -1;
		father = NULL;
		sonNumber = 0;
		giveSon = true;
		for (int i = 0; i < 81; i++) {
			sons[i] = NULL;
		}
		floor = 0;
	}

	//�ڵ��� NodeExpansion����ʱ������������캯��
	Node(int x1, int y1, Node* father, int tempBoard[9][9]) {
		x = x1;
		y = y1;

		this->father = father;
		giveSon = true;

		for (int i = 0; i < 9; i++) {
			for (int j = 0; j < 9; j++) {
				nodeBoard[i][j] = tempBoard[i][j];
			}
		}
		nodeBoard[x][y] = -1;

		value = 0;
		exploreTimes = 0;
		sonNumber = 0;
		for (int i = 0; i < 81; i++) {
			sons[i] = NULL;
		}
		floor = father->getFloor() + 1;
	}

	double getValue() {
		return value;
	}

	int getExploreTimes() {
		return exploreTimes;
	}

	int getSonNumber() {
		return sonNumber;
	}

	Node** getSons() {
		return sons;
	}

	int getX() {
		return x;
	}

	int getY() {
		return y;
	}

	bool getGiveSon() {
		return giveSon;
	}

	Node* getFather() {
		return father;
	}

	int getFloor() {
		return floor;
	}

	void addValue(double increase) {
		value += increase;
	}

	void addExploreTimes() {
		exploreTimes++;
	}

	//����UCB������ֵ
	double getUCBResult() {
		int c = 1;
		int N = originalNode.getExploreTimes();
		UCBValue = value / (exploreTimes + 0.0000000001) + c * sqrt(2 * log(N)/ (exploreTimes + 0.0000000001));
		return UCBValue;
	}

	void nodeExpansion() {
		vector<int> available_list; //�Ϸ�λ�ñ� 

		int col = -1;

		//floor������
		//�����ţ���һ��������ѡ���Ǽ�����
		//��ô��һ����������ѡ��Ӧ���Ƕ����
		if (floor % 2 == 0) {
			col = 1;
		}

		for (int i = 0; i < 9; i++) {
			for (int j = 0; j < 9; j++) {
				if (judgeAvailable(i, j, -col, nodeBoard)) {
					available_list.push_back(i * 9 + j);
				}
			}
		}
		sonNumber = available_list.size();

		for (int i = 0; i < sonNumber; i++) {
			sons[i] = new Node(available_list[i] / 9, available_list[i] % 9, this, this->nodeBoard);
		}
	}

	//����Ϊ��һ��ģ�⣬���շ��صĶ���
	//������ҷ��Ľ��
	bool getRandomResult() {
		int tempBoard[9][9];

		for (int i = 0; i < 9; i++) {
			for (int j = 0; j < 9; j++) {
				tempBoard[i][j] = nodeBoard[i][j];
			}
		}

		bool flag = false;

		//col �������������˭��ģ��
		int col = -1;
		if (floor % 2 == 0) {
			col = 1;
		}
		// �߼��ǣ�
		// ���floorΪ��������һ��Ľڵ�� nodeBoard�µ�Ӧ���� -1 
		// ��ģ����� �� �Լ����������֮�󣬶������֣�Ȼ������µĽ��

		while (true) {

			vector<int> available_list_foropp; //�Ϸ�λ�ñ� 

			for (int i = 0; i < 9; i++)
				for (int j = 0; j < 9; j++)
					if (judgeAvailable(i, j, -col, tempBoard))
						available_list_foropp.push_back(i * 9 + j);

			if (available_list_foropp.size() != 0) {
				int result = available_list_foropp[rand() % available_list_foropp.size()];
				tempBoard[result / 9][result % 9] = -col;
			}
			else {
				flag = true;
				break;
				//return true;
			}

			vector<int> available_list_forme; //�Ϸ�λ�ñ� 

			for (int i = 0; i < 9; i++)
				for (int j = 0; j < 9; j++)
					if (judgeAvailable(i, j, col, tempBoard))
						available_list_forme.push_back(i * 9 + j);

			if (available_list_forme.size() != 0) {
				int result = available_list_forme[rand() % available_list_forme.size()];
				tempBoard[result / 9][result % 9] = col;
			}
			else {
				flag = false;
				break;
				//return false;
			}

		}


		/*for (int i = 0; i < 9; i++) {
			for (int j = 0; j < 9; j++) {
				cout << tempBoard[i][j] << " ";
			}
			cout << endl;
		}
		cout << endl;*/
		
		if (col == -1) {
			return flag;
		}
		else {
			return !flag;
		}

	}

	void rollOut(int times) {
		double increase = 0;
		double win = 0;
		double lose = 0;
		for (int i = 0; i < times; i++) {
			if (getRandomResult()) {
				win++;
			}
			else {
				lose++;
			}
		}
		//ȥ��ʤ�ʹ��͵�����
		/*if (win / (lose + win) < 0.1) {
			giveSon = false;
		}*/
		increase = (win * 10 / (lose + win));
		value += increase;
		exploreTimes++;

		Node* temp = father;
		while (temp != NULL) {
			temp->addValue(increase);
			temp->addExploreTimes();
			temp = temp->getFather();
		}
	}

	Node* select() {
		if (sonNumber == 0) {
			return this;
		}
		else {
			Node* maxNode = sons[0];
			double maxUCB = maxNode->getUCBResult();
			for (int i = 1; i < sonNumber; i++) {
				double tempUCB = sons[i]->getUCBResult();
				if (tempUCB > maxUCB) {
					maxUCB = tempUCB;
					maxNode = sons[i];
				}
			}

			return maxNode->select();
		}
	}

	//times�ǵ�������
	Node* iterate(int times, int rollTimes, clock_t begin_time) {

		//&& float(clock() - begin_time) < 3000

		//������times��
		while (1.0 * (clock() - begin_time) / CLOCKS_PER_SEC <= 0.80) {
			Node* selectedNode = originalNode.select();
			//�����Ҷ�ڵ㣬��̽������Ϊ0
			//�Ͷ������RollOut
			if (selectedNode->getSonNumber() == 0 && selectedNode->getExploreTimes() == 0) {
				selectedNode->rollOut(rollTimes);
			}
			//�����Ҷ�ڵ㣬��̽��������Ϊ0
			else if(selectedNode->getSonNumber() == 0){
				selectedNode->nodeExpansion();
			}
			//����Ҷ�ڵ㣬��̽��������Ϊ0
			//�ͣ�ͨ��UCB��Ѱ��UCBֵ��ߵĽڵ�
			else {
				selectedNode = selectedNode->select();
			}

		}

		int maxValue = 0;
		Node* maxNode = originalNode.sons[0];
		for (int i = 1; i < originalNode.sonNumber; i++) {
			if (originalNode.sons[i]->getValue() > maxValue) {
				maxValue = originalNode.sons[i]->getValue();
				maxNode = originalNode.sons[i];
			}
		}

		return maxNode;
		
	}

	void static setNodeBoard() {
		for (int i = 0; i < 9; i++) {
			for (int j = 0; j < 9; j++) {
				Node::originalNode.nodeBoard[i][j] = board[i][j];
			}
		}
	}
};

Node Node::originalNode;

int main() {
	srand((unsigned)time(0));
	string str;
	int x, y;
	// ����JSON
	getline(cin, str);

	//��¼��������ʱ��
	const clock_t begin_time = clock();

	Json::Reader reader;
	Json::Value input;
	reader.parse(str, input);
	// �����Լ��յ���������Լ���������������ָ�״̬
	int turnID = input["responses"].size();
	for (int i = 0; i < turnID; i++)
	{
		x = input["requests"][i]["x"].asInt(), y = input["requests"][i]["y"].asInt();
		if (x != -1) board[x][y] = 1;
		x = input["responses"][i]["x"].asInt(), y = input["responses"][i]["y"].asInt();
		if (x != -1) board[x][y] = -1;
	}
	x = input["requests"][turnID]["x"].asInt(), y = input["requests"][turnID]["y"].asInt();
	if (x != -1) board[x][y] = 1;
	// �������JSON
	Json::Value ret;
	Json::Value action;


	Node* maxNode = NULL;

	Node::setNodeBoard();
	Node::originalNode.nodeExpansion();

	int blankNum = 0;

	vector<int> available_list; //�Ϸ�λ�ñ� 

	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			if (judgeAvailable(i, j, -1, board)) {
				available_list.push_back(i * 9 + j);
			}
			if (board[i][j] == 0) {
				blankNum++;
			}
		}
	}

	int times = available_list.size();

	//cout << times << endl;

	if (times > 50) {
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
	}
	else {
		maxNode = Node::originalNode.iterate((int)(times * 3), 50, begin_time);
		action["x"] = maxNode->getX(); action["y"] = maxNode->getY();
		ret["response"] = action;
		Json::FastWriter writer;

		cout << writer.write(ret) << endl;
	}
	
	maxNode = Node::originalNode.iterate((int)(times * 3), 5, begin_time);
	action["x"] = maxNode->getX(); action["y"] = maxNode->getY();
	ret["response"] = action;
	Json::FastWriter writer;

	cout << writer.write(ret) << endl;
	

	/*for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			cout << Node::originalNode.nodeBoard[i][j] << " ";
		}
		cout << endl;
	}*/

	//for (int i = 0; i < Node::originalNode.getSonNumber(); i++) {
	//	cout << Node::originalNode.getSons()[i]->getX() << " ";
	//	cout << Node::originalNode.getSons()[i]->getY() << ": ";
	//	cout << Node::originalNode.getSons()[i]->getValue() << endl;
	//}



	//float seconds = float(clock() - begin_time);
	//cout << seconds / CLOCKS_PER_SEC << endl;
	//

	return 0;
}