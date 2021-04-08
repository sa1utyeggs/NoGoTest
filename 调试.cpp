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


class Node {
public:
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
	//��������ڵ��µ��ӵ���ɫ
	int col;

	double UCBValue;
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
		for (int i = 0; i < 81; i++) {
			sons[i] = NULL;
		}
		col = 0;
	}

	//�ڵ��� NodeExpansion����ʱ������������캯��
	Node(int x1, int y1, Node* father) {
		x = x1;
		y = y1;

		this->father = father;

		for (int i = 0; i < 9; i++) {
			for (int j = 0; j < 9; j++) {
				nodeBoard[i][j] = father->nodeBoard[i][j];
			}
		}
		col = -(father->col);
		nodeBoard[x][y] = col;

		value = 0;
		exploreTimes = 0;
		sonNumber = 0;
		for (int i = 0; i < 81; i++) {
			sons[i] = NULL;
		}
	}


	void addValue(double increase) {
		value += increase;
	}

	void addExploreTimes() {
		exploreTimes++;
	}

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
				if (nodeBoard[dx][dy] == 0)
					flag = true;
				//�������ͬɫ����û�б�����
				if (nodeBoard[dx][dy] == nodeBoard[fx][fy] && !dfs_air_visit[dx][dy])
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
	bool judgeAvailable(int fx, int fy)
	{
		if (nodeBoard[fx][fy] != 0) {
			return false;
		}
		//����
		nodeBoard[fx][fy] = col;
		//�������Ե����ֽ���������ķ�ʽ�ŵ�ָ�����ڴ���ȥ
		memset(dfs_air_visit, 0, sizeof(dfs_air_visit));
		if (!dfs_air(fx, fy))
		{
			//���û��
			nodeBoard[fx][fy] = 0;
			return false;
		}
		//�ж����������֮������Χ4�����Ƿ��û��
		for (int dir = 0; dir < 4; dir++)
		{
			int dx = fx + cx[dir], dy = fy + cy[dir];

			if (inBorder(dx, dy))
			{
				//��Χ�Ƿ����ӣ���û�б�dfs_air���ʹ�
				if (nodeBoard[dx][dy] && !dfs_air_visit[dx][dy])
					if (!dfs_air(dx, dy))
					{
						//�����Χû��������ӾͲ�����
						nodeBoard[fx][fy] = 0;
						return false;
					}
			}
		}
		nodeBoard[fx][fy] = 0;
		return true;
	}

	//����UCB������ֵ
	double getUCBResult(double C) {
		int c = 2;
		//int N = originalNode.exploreTimes;
		int N = father->exploreTimes;
		UCBValue = value / (exploreTimes + 0.000000000000001) + c * C * sqrt(2 * log(N) / (exploreTimes + 0.000000000000001));
		return UCBValue;
	}

	void nodeExpansion() {
		vector<int> available_list; //�Ϸ�λ�ñ� 

		//��ʱ��Ӧ�ô����father���ֵĺϷ�λ�ñ�
		col = -col;

		for (int i = 0; i < 9; i++) {
			for (int j = 0; j < 9; j++) {
				if (judgeAvailable(i, j)) {
					available_list.push_back(i * 9 + j);
				}
			}
		}
		col = -col;

		sonNumber = available_list.size();



		for (int i = 0; i < sonNumber; i++) {
			sons[i] = new Node(available_list[i] / 9, available_list[i] % 9, this);
		}
	}

	double quickEvaluate() {
		int myGoodDot = 0;
		int oppGoodDot = 0;

		for (int i = 0; i < 9; i++) {
			for (int j = 0; j < 9; j++) {
				//������ҷ����£��ҶԷ�������
				bool f1 = judgeAvailable(i, j);
				col = -col;
				bool f2 = judgeAvailable(i, j);
				col = -col;
				if (f1 && !f2) {
					myGoodDot++;
				}
				else if (!f1 && f2) {
					oppGoodDot++;
				}
			}
		}
		//cout << myGoodDot - oppGoodDot << endl;
		return myGoodDot - oppGoodDot;
	}

	void rollOut() {
		double increase = 0;

		increase = quickEvaluate();

		value += increase;
		exploreTimes++;

		Node* temp = father;
		while (temp != NULL) {
			increase = -increase;
			temp->addValue(increase);
			temp->addExploreTimes();
			temp = temp->father;
		}
	}

	Node* select() {
		if (sonNumber == 0) {
			return this;
		}
		else {
			Node* maxNode = sons[0];
			double maxUCB = maxNode->getUCBResult(0.1);
			for (int i = 1; i < sonNumber; i++) {
				double tempUCB = sons[i]->getUCBResult(0.1);
				if (tempUCB > maxUCB) {
					maxUCB = tempUCB;
					maxNode = sons[i];
				}
			}

			return maxNode->select();
		}
	}

	//times�ǵ�������
	Node* iterate(clock_t begin_time) {

		Node* selectedNode = originalNode.select();

		//1.0 * (clock() - begin_time) / CLOCKS_PER_SEC <= 0.90
		while (1.0 * (clock() - begin_time) / CLOCKS_PER_SEC <= 0.90) {
			//�����Ҷ�ڵ㣬��̽������Ϊ0
			//�Ͷ������RollOut
			if (selectedNode->sonNumber == 0 && selectedNode->exploreTimes == 0) {
				selectedNode->rollOut();
				selectedNode = originalNode.select();
			}
			//�����Ҷ�ڵ㣬��̽��������Ϊ0
			else if (selectedNode->sonNumber == 0) {
				selectedNode->nodeExpansion();
				selectedNode = selectedNode->select();
			}
			//����Ҷ�ڵ㣬��̽��������Ϊ0
			//��ͨ��UCB��Ѱ��UCBֵ��ߵĽڵ�
			else {
				selectedNode = selectedNode->select();
			}

		}

		double maxValue = originalNode.sons[0]->getUCBResult(0);
		Node* maxNode = originalNode.sons[0];
		for (int i = 1; i < originalNode.sonNumber; i++) {
			if (originalNode.sons[i]->getUCBResult(0) > maxValue) {
				maxValue = originalNode.sons[i]->getUCBResult(0);
				maxNode = originalNode.sons[i];
			}
		}

		/*vector<Node*> randomDot;

		for (int i = 0; i < originalNode.sonNumber; i++) {
			if (originalNode.sons[i]->value == maxValue) {
				randomDot.push_back(originalNode.sons[i]);
			}
		}
		if (randomDot.size() != 0) {
			maxNode = randomDot[rand() % randomDot.size()];
		}*/

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

	int color = 0;
	int first = 0;
	if (input["requests"][first]["x"].asInt() == -1) {
		//��ɫΪ -1 
		color = -1;
	}
	else {
		color = 1;
	}


	for (int i = 0; i < turnID; i++)
	{
		x = input["requests"][i]["x"].asInt(), y = input["requests"][i]["y"].asInt();
		if (x != -1) board[x][y] = -color;
		x = input["responses"][i]["x"].asInt(), y = input["responses"][i]["y"].asInt();
		if (x != -1) board[x][y] = color;
	}
	x = input["requests"][turnID]["x"].asInt(), y = input["requests"][turnID]["y"].asInt();
	if (x != -1) board[x][y] = -color;
	// �������JSON
	Json::Value ret;
	Json::Value action;


	Node* maxNode = NULL;

	Node::setNodeBoard();


	Node::originalNode.col = -color;

	Node::originalNode.nodeExpansion();


	maxNode = Node::originalNode.iterate(begin_time);
	action["x"] = maxNode->x; action["y"] = maxNode->y;
	ret["response"] = action;
	Json::FastWriter writer;

	cout << writer.write(ret) << endl;




	/*for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			cout << Node::originalNode.nodeBoard[i][j] << " ";
		}
		cout << endl;
	}*/

	/*for (int i = 0; i < Node::originalNode.sonNumber; i++) {
		cout << (Node::originalNode.sons[i])->x << " ";
		cout << (Node::originalNode.sons()[i])->y << ": ";
		cout << Node::originalNode.sons()[i]->value << endl;
	}*/



	//float seconds = float(clock() - begin_time);
	//cout << seconds / CLOCKS_PER_SEC << endl;
	//

	return 0;
}