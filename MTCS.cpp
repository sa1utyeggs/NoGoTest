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

bool dfs_air(int fx, int fy, int tempBoard[9][9])
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
			if (tempBoard[dx][dy] == 0)
				flag = true;
			//如果相邻同色，且没有被访问
			if (tempBoard[dx][dy] == tempBoard[fx][fy] && !dfs_air_visit[dx][dy])
			{
				//判断连着的同色子是否有气
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
	//下子
	tempBoard[fx][fy] = col;
	//将数字以单个字节逐个拷贝的方式放到指定的内存中去
	memset(dfs_air_visit, 0, sizeof(dfs_air_visit));
	if (!dfs_air(fx, fy, tempBoard))
	{
		//如果没气
		tempBoard[fx][fy] = 0;
		return false;
	}
	//判断下完这个棋之后，其周围4个棋是否会没气
	for (int dir = 0; dir < 4; dir++)
	{
		int dx = fx + cx[dir], dy = fy + cy[dir];

		if (inBorder(dx, dy))
		{
			//周围是否有子，且没有被dfs_air访问过
			if (tempBoard[dx][dy] && !dfs_air_visit[dx][dy])
				if (!dfs_air(dx, dy, tempBoard))
				{
					//如果周围没气，这个子就不能下
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

//fx，fy是可下的点
//获得某个点的进攻权值
double getOffensiveValue(int fx, int fy, int col) {
	double offensiveValue = 0;
	bool available_list[12];


	//去除在下这步棋之前，下了就会输的点，这样的点不算在考虑范围内
	for (int dir = 0; dir < 12; dir++) {
		int dx = fx + cx[dir], dy = fy + cy[dir];
		if (inBorder(dx, dy) && board[dx][dy] == 0 && !judgeAvailable(dx, dy, col,board)) {
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

	//下完这步棋之后，对手的合法位置数
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

//获得进攻权-防守权值，最大的XY，值
XYWithValue getMyMaxValueXY() {
	int col = -1;
	vector<int> available_list; //合法位置表 
	int tmpx = -1;
	int tmpy = -1;

	//获得目前col的合法位置表
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
			if (judgeAvailable(i, j, col,board) && judgeAvailable(i, j, -col,board)) {
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

class Node {
private:
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


	//floor用来这一步模拟的是自己还是对方
	int floor;

	//如果这个节点，模拟出来
	//胜率太小，就放弃给他子节点
	bool giveSon;

	double UCBValue;

public:
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
		giveSon = true;
		for (int i = 0; i < 81; i++) {
			sons[i] = NULL;
		}
		floor = 0;
	}

	//在调用 NodeExpansion方法时，调用这个构造函数
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

	//返回UCB函数的值
	double getUCBResult() {
		int c = 1;
		int N = originalNode.getExploreTimes();
		UCBValue = value / (exploreTimes + 0.0000000001) + c * sqrt(2 * log(N)/ (exploreTimes + 0.0000000001));
		return UCBValue;
	}

	void nodeExpansion() {
		vector<int> available_list; //合法位置表 

		int col = -1;

		//floor是奇数
		//代表着，这一层做出的选择是己方的
		//那么下一层中做出的选择应该是对面的
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

	//无论为哪一方模拟，最终返回的都是
	//相对于我方的结果
	bool getRandomResult() {
		int tempBoard[9][9];

		for (int i = 0; i < 9; i++) {
			for (int j = 0; j < 9; j++) {
				tempBoard[i][j] = nodeBoard[i][j];
			}
		}

		bool flag = false;

		//col 代表着是相对于谁在模拟
		int col = -1;
		if (floor % 2 == 0) {
			col = 1;
		}
		// 逻辑是：
		// 如果floor为奇数，这一层的节点的 nodeBoard下的应该是 -1 
		// 那模拟的是 在 自己下了这个点之后，对面先手，然后继续下的结果

		while (true) {

			vector<int> available_list_foropp; //合法位置表 

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

			vector<int> available_list_forme; //合法位置表 

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
		//去除胜率过低的限制
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

	//times是迭代次数
	Node* iterate(int times, int rollTimes, clock_t begin_time) {

		//&& float(clock() - begin_time) < 3000

		//共迭代times次
		while (1.0 * (clock() - begin_time) / CLOCKS_PER_SEC <= 0.80) {
			Node* selectedNode = originalNode.select();
			//如果是叶节点，且探索次数为0
			//就对其进行RollOut
			if (selectedNode->getSonNumber() == 0 && selectedNode->getExploreTimes() == 0) {
				selectedNode->rollOut(rollTimes);
			}
			//如果是叶节点，但探索次数不为0
			else if(selectedNode->getSonNumber() == 0){
				selectedNode->nodeExpansion();
			}
			//不是叶节点，且探索次数不为0
			//就，通过UCB来寻找UCB值最高的节点
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
	// 读入JSON
	getline(cin, str);

	//记录程序运行时间
	const clock_t begin_time = clock();

	Json::Reader reader;
	Json::Value input;
	reader.parse(str, input);
	// 分析自己收到的输入和自己过往的输出，并恢复状态
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
	// 输出决策JSON
	Json::Value ret;
	Json::Value action;


	Node* maxNode = NULL;

	Node::setNodeBoard();
	Node::originalNode.nodeExpansion();

	int blankNum = 0;

	vector<int> available_list; //合法位置表 

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