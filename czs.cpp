#include "jsoncpp/json.h"
#include <cstdio>
#include <cstring>
#include <iostream>
#include <random>
#include <string>
#include <vector>

using namespace std;

#define TIME_OUT_SET 0.98
#define EXPLORE 0.1

int board[9][9] = { 0 };
int node_count = 0;

bool dfs_air_visit[9][9] = { 0 };
const int cx[] = { -1, 0, 1, 0 };
const int cy[] = { 0, -1, 0, 1 };

inline bool inBorder(int x, int y) { return x >= 0 && y >= 0 && x < 9 && y < 9; }


struct Action
{
    int x = -1;
    int y = -1;
};

class Node
{
    //Node������״̬��¼����������ֵcurrent_value,����current_board
    //ʵ�����ж��Ƿ��terminal��simulation�׶εĵ���default policy
public:
    Node();
    signed char current_board[9][9] = { 0 };
    int col = 0;
    Node* parent = NULL;
    Node* children[81];
    int visit_times = 0;
    int countChildrenNum = 0;
    int maxChildrenNum = 0;
    double quality_value = 0.0;
    int available_choices[81];
    void getAviliableAction();           //�õ����е��ж�
    bool dfsAir(int fx, int fy);         //�ж��Ƿ�����
    bool judgeAvailable(int fx, int fy); //�ж��Ƿ����
    double quickEvaluate();              //���ٹ�ֵ
    Node* bestChild(double C);
    Node* expand();
    Node* treePolicy();
    double defaultPolicy();
    void backup(double reward);
    int static exploreTimes ;
};

int Node::exploreTimes = 0;

Node::Node()
{
    memset(current_board, 0, sizeof(current_board));
    memset(available_choices, 0, sizeof(available_choices));
}

double Node::quickEvaluate()
{
    int n1 = 0, n2 = 0;
    for (int i = 0; i < 9; i++)
        for (int j = 0; j < 9; j++)
        {
            bool f1 = judgeAvailable(i, j);
            col = -col;
            bool f2 = judgeAvailable(i, j);
            col = -col;
            if (f1 && !f2)
                n1++;
            else if (!f1 && f2)
                n2++;
        }
    cout << n2 - n1 << endl;
    return n2 - n1;
}

bool Node::dfsAir(int fx, int fy)
{
    dfs_air_visit[fx][fy] = true;
    bool flag = false;
    for (int dir = 0; dir < 4; dir++) //ĳһλ������������Ϊ��Χ�пո������һ���ڽ���ͬɫ����(��֤����һ��֤���ڽ�����һ��������������)
    {
        int dx = fx + cx[dir], dy = fy + cy[dir];
        if (inBorder(dx, dy))
        {
            if (current_board[dx][dy] == 0)
                flag = true;
            if (current_board[dx][dy] == current_board[fx][fy] && !dfs_air_visit[dx][dy])
                if (dfsAir(dx, dy))
                {
                    flag = true;

                }
        }
    }
    return flag;
}

bool Node::judgeAvailable(int fx, int fy)
{
    if (current_board[fx][fy])
        return false;
    current_board[fx][fy] = col;
    memset(dfs_air_visit, 0, sizeof(dfs_air_visit));
    if (!dfsAir(fx, fy)) //�ж���ɱ
    {
        current_board[fx][fy] = 0;
        return false;
    }
    for (int dir = 0; dir < 4; dir++)
    {
        int dx = fx + cx[dir], dy = fy + cy[dir];
        if (inBorder(dx, dy))
        {
            if (current_board[dx][dy] && !dfs_air_visit[dx][dy])
            {
                if (!dfsAir(dx, dy))
                {
                    current_board[fx][fy] = 0;
                    return false;
                }
            }
        }
    }
    current_board[fx][fy] = 0;
    return true;
}

void Node::getAviliableAction()
{
    memset(available_choices, 0, sizeof(available_choices));
    for (int i = 0; i < 9; i++)
        for (int j = 0; j < 9; j++)
            if (judgeAvailable(i, j))
            {
                int act = i * 9 + j;
                available_choices[maxChildrenNum++] = act;
            }
}

//ʹ��UCB�㷨��Ȩ��exploration��exploitation��ѡ��÷���ߵ��ӽڵ㣬ע�������Ԥ��׶�ֱ��ѡ��ǰQֵ�÷���ߵġ�
Node* Node::bestChild(double C)
{
    double max_score = -2e50; //������ʼ�����2e-50���ѹֻ᷵��NULL
    Node* best_child = NULL;
    for (int i = 0; i < countChildrenNum; i++)
    {
        Node* p = children[i];
        double score = p->quality_value / (p->visit_times) + 2 * C * sqrt(log(2 * visit_times) / (p->visit_times));
        if (score > max_score)
        {
            max_score = score;
            best_child = p;
        }
    }
    return best_child;
}

Node* Node::expand()
{

    int a = available_choices[countChildrenNum];
    int x = a / 9;
    int y = a % 9;
    Node* new_node = new Node;
    children[countChildrenNum++] = new_node;
    new_node->parent = this;
    for (int i = 0; i < 9; i++)
        for (int j = 0; j < 9; j++)
            new_node->current_board[i][j] = current_board[i][j];
    new_node->col = -col;
    new_node->current_board[x][y] = col;
    new_node->getAviliableAction();
    return new_node;
}

Node* Node::treePolicy()
{
    //Selection��Expansion�׶Ρ����뵱ǰ��Ҫ��ʼ�����Ľڵ㣬����UCB1ֵ������õ���Ҫexpend�Ľڵ㣬ע������ڵ���Ҷ�ӽ��ֱ�ӷ��ء�
    //�������������ҵ�ǰδѡ������ӽڵ㣬����ж�������ѡ�������ѡ�������Ȩ���exploration/exploitation��UCBֵ���ģ����UCBֵ��������ѡ��
    if (maxChildrenNum == 0) //��treePolicy����Ҷ�ڵ�ʱ(node->state.available_choices.empty() && node->children.empty())
    {
        //cout << "leaf";
        return this;
    }
    if (countChildrenNum >= maxChildrenNum)
    {
        Node* p = bestChild(EXPLORE);
        return p->treePolicy();
    }
    else
        return expand();
}

//Simulation�׶Σ��ӵ�ǰ�ڵ��������ģ���������վ֣�����reward
double Node::defaultPolicy()
{
    return quickEvaluate();
}

//���ؿ�����������Backpropagation�׶Σ�����ǰ���ȡ��Ҫexpend�Ľڵ����ִ��Action��reward��������expend�ڵ���������нڵ㲢���¶�Ӧ���ݡ�
void Node::backup(double reward)
{
    Node* p = this;
    while (p)
    {
        p->visit_times++;
        p->quality_value += reward;
        reward = -reward;
        p = p->parent;
    }
}

//MCTS�����岿��
/*ʵ�����ؿ����������㷨������һ�����ڵ㣬�����޵�ʱ���ڸ���֮ǰ�Ѿ�̽���������ṹexpand�½ڵ�͸������ݣ�Ȼ�󷵻�ֻҪexploitation��ߵ��ӽڵ㡣
  ���ؿ��������������ĸ����裬Selection��Expansion��Simulation��Backpropagation��
  ǰ����ʹ��tree policy�ҵ�ֵ��̽���Ľڵ㡣
  ������ʹ��default policyҲ������ѡ�еĽڵ�������㷨ѡһ���ӽڵ㲢����reward��
  ���һ��ʹ��backupҲ���ǰ�reward���µ����о�����ѡ�нڵ�Ľڵ��ϡ�
  ����Ԥ��ʱ��ֻ��Ҫ����Qֵѡ��exploitation���Ľڵ㼴�ɣ��ҵ���һ�����ŵĽڵ㡣*/

int main()
{
    srand((unsigned)time(0));
    string str;
    int x, y;
    // ����JSON
    getline(cin, str);
    //getline(cin, str);

    //��ʱ��ʼ������ʱ��
    int start = clock();
    int timeout = (int)(TIME_OUT_SET * (double)CLOCKS_PER_SEC);

    Json::Reader reader;
    Json::Value input;
    reader.parse(str, input);

    //��ʼ���ڵ�
    int first = 0;
    Node* node = new Node;
    if (input["requests"][first]["x"].asInt() == -1) //ȷ����ɫ
        node->col = 1;                               //����
    else
        node->col = -1;

    int color = node->col;
    // �����Լ��յ���������Լ���������������ָ�״̬
    int turnID = input["responses"].size();
    for (int i = 0; i < turnID; i++)
    {
        x = input["requests"][i]["x"].asInt(), y = input["requests"][i]["y"].asInt();
        if (x != -1)
        {
            board[x][y] = -color;
        }
        x = input["responses"][i]["x"].asInt(), y = input["responses"][i]["y"].asInt();
        if (x != -1)
        {
            board[x][y] = color;
        }
    }
    x = input["requests"][turnID]["x"].asInt(), y = input["requests"][turnID]["y"].asInt();
    if (x != -1) //�Է�Ϊ����
    {
        board[x][y] = -color;
        node_count++;
    }

    cout << "color: " <<color << endl;

    for (int i = 0; i < 9; i++) { //��������
        for (int j = 0; j < 9; j++) {
            cout << board[i][j] << " ";
            node->current_board[i][j] = board[i][j];
        }
        cout << endl;
    }
    node->getAviliableAction();

    //��ʼ���ؿ���������
    while (clock() - start < timeout)
    {
        node_count++;
        Node* expand_node = node->treePolicy();
        double reward = expand_node->defaultPolicy();
        expand_node->backup(reward);
    }

    //������
    Json::Value ret;
    Json::Value action;

    Node* best_child = node->bestChild(0);
    for (int i = 0; i < 9; i++)
        for (int j = 0; j < 9; j++)
            if (board[i][j] != best_child->current_board[i][j])
            {
                action["x"] = i;
                action["y"] = j;
                break;
            }
    ret["response"] = action;
    Json::FastWriter writer;
    char buffer[4096];
    sprintf(buffer, "???:%d", node_count);
    ret["debug"] = buffer;
    cout << writer.write(ret) << endl;
    //system("pause");
    return 0;
}


