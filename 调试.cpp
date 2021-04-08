#include <iostream>
#include <cstdio>
#include <string>
#include <cstring>
#include<cmath>
#include "jsoncpp/json.h"
using namespace std;

//1代表对面下的
//-1是自己下的
int board[9][9];

bool dfs_air_visit[9][9];

//cx 和 cy 组合起来代表着 “左下右上”，“左上，右上，左下，右下，左左，右右，上上，下下
const int cx[] = { -1, 0, 1, 0, -1, 1, -1, 1, -2, 2, 0, 0 };
const int cy[] = { 0, -1, 0, 1, 1, 1, -1, -1, 0, 0, 2, -2 };

bool inBorder(int x, int y) { return x >= 0 && y >= 0 && x < 9 && y < 9; }


class Node {
public:
	//此时的UCB值
	double value;
	//目前的探索次数
	int exploreTimes;
	//存这一步代表着的 xy 值
	int x;
	int y;
	//存儿子节点
	Node* sons[81];
	//儿子节点的个数
	int sonNumber;
	//父亲节点
	Node* father;
	//储存这个节点下的子的颜色
	int col;

	double UCBValue;
	//存下了这步棋时，的棋局
	int nodeBoard[9][9];

	//根节点
	Node static originalNode;

	//无参的构造函数专门给根节点使用 
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

	//在调用 NodeExpansion方法时，调用这个构造函数
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
		//判断是否已经被dfs_air方法访问过了
		dfs_air_visit[fx][fy] = true;
		bool flag = false;
		for (int dir = 0; dir < 4; dir++)
		{
			int dx = fx + cx[dir], dy = fy + cy[dir];
			if (inBorder(dx, dy))
			{
				//周围有空，有气
				if (nodeBoard[dx][dy] == 0)
					flag = true;
				//如果相邻同色，且没有被访问
				if (nodeBoard[dx][dy] == nodeBoard[fx][fy] && !dfs_air_visit[dx][dy])
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
	bool judgeAvailable(int fx, int fy)
	{
		if (nodeBoard[fx][fy] != 0) {
			return false;
		}
		//下子
		nodeBoard[fx][fy] = col;
		//将数字以单个字节逐个拷贝的方式放到指定的内存中去
		memset(dfs_air_visit, 0, sizeof(dfs_air_visit));
		if (!dfs_air(fx, fy))
		{
			//如果没气
			nodeBoard[fx][fy] = 0;
			return false;
		}
		//判断下完这个棋之后，其周围4个棋是否会没气
		for (int dir = 0; dir < 4; dir++)
		{
			int dx = fx + cx[dir], dy = fy + cy[dir];

			if (inBorder(dx, dy))
			{
				//周围是否有子，且没有被dfs_air访问过
				if (nodeBoard[dx][dy] && !dfs_air_visit[dx][dy])
					if (!dfs_air(dx, dy))
					{
						//如果周围没气，这个子就不能下
						nodeBoard[fx][fy] = 0;
						return false;
					}
			}
		}
		nodeBoard[fx][fy] = 0;
		return true;
	}

	//返回UCB函数的值
	double getUCBResult(double C) {
		int c = 2;
		//int N = originalNode.exploreTimes;
		int N = father->exploreTimes;
		UCBValue = value / (exploreTimes + 0.000000000000001) + c * C * sqrt(2 * log(N) / (exploreTimes + 0.000000000000001));
		return UCBValue;
	}

	void nodeExpansion() {
		vector<int> available_list; //合法位置表 

		//此时，应该存的是father对手的合法位置表
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
				//这个点我方能下，且对方不能下
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

	//times是迭代次数
	Node* iterate(clock_t begin_time) {

		Node* selectedNode = originalNode.select();

		//1.0 * (clock() - begin_time) / CLOCKS_PER_SEC <= 0.90
		while (1.0 * (clock() - begin_time) / CLOCKS_PER_SEC <= 0.90) {
			//如果是叶节点，且探索次数为0
			//就对其进行RollOut
			if (selectedNode->sonNumber == 0 && selectedNode->exploreTimes == 0) {
				selectedNode->rollOut();
				selectedNode = originalNode.select();
			}
			//如果是叶节点，但探索次数不为0
			else if (selectedNode->sonNumber == 0) {
				selectedNode->nodeExpansion();
				selectedNode = selectedNode->select();
			}
			//不是叶节点，且探索次数不为0
			//就通过UCB来寻找UCB值最高的节点
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
	// 读入JSON
	getline(cin, str);

	//记录程序运行时间
	const clock_t begin_time = clock();

	Json::Reader reader;
	Json::Value input;
	reader.parse(str, input);
	// 分析自己收到的输入和自己过往的输出，并恢复状态
	int turnID = input["responses"].size();

	int color = 0;
	int first = 0;
	if (input["requests"][first]["x"].asInt() == -1) {
		//黑色为 -1 
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
	// 输出决策JSON
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