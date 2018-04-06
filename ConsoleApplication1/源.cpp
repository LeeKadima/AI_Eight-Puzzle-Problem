#include <iostream>
#include <math.h>
#include <time.h>
#include <windows.h>
using namespace std;

#define SUDOKU_SIZE  9
#define ZERO_SIGN 0
#define OPEN_LIST
#define CLOSED_LIST
#define ROW 3	//ÐÐ
#define COL 3	//ÁÐ
#define UP		-3
#define DOWN	 3
#define LEFT	-1
#define RIGHT	 1

const bool k_open_list = 0;
const bool k_closed_list = 1;
enum dirction{	up, down, e_left, e_right,none = 10};

int manhattan[9][9] = //The Manhattan path length from coordinate x to coordinate y.
{
	{ 0, 1, 2, 1, 2, 3, 2, 3, 4 },
	{ 1, 0, 1, 2, 1, 2, 3, 2, 3 },
	{ 2, 1, 0, 3, 2, 1, 4, 3, 2 },
	{ 1, 2, 3, 0, 1, 2, 1, 2, 3 },
	{ 2, 1, 2, 1, 0, 1, 2, 1, 2 },
	{ 3, 2, 1, 2, 1, 0, 3, 2, 1 },
	{ 2, 3, 4, 1, 2, 3, 0, 1, 2 },
	{ 3, 2, 3, 2, 1, 2, 1, 0, 1 },
	{ 4, 3, 2, 3, 2, 1, 2, 1, 0 }
};
int position[9] = { -1, 0, 1, 2, 5, 8, 7, 6, 3 };	//Convert number to the postion in sudoku.
struct Sudoku
{
	int numbers[ROW][COL];
	unsigned char sum;
	unsigned char value;
	unsigned char depth;
	unsigned char zero_position;
	unsigned char last_position;
	int id;
	dirction dir = none;
	Sudoku* next;
};

Sudoku* g_open_head = new Sudoku;			//The head of open list.
Sudoku* g_closed_head = new Sudoku;			//The head of close list.
Sudoku start_sudoku;						//The initial sudoku
Sudoku ultimate_sudoku;						//The ultimate sudoku.

/*
//Function pre declaration
*/
void PrintSudoku(const Sudoku& sudoku);

/**
* Name		:JudgeErrorPos
* Function	:Though the comparison of parameter sourseState and ultimateState,return the numbers of error.
* Parameter	:const Sudoku& sourseState, const Sudoku& ultimateState
* Return	:The numbers of error.
**/
int JudgeErrorPos(const Sudoku& sourseState, const Sudoku& ultimateState)
{
	int error_rate = 0;
	for (int i = 0; i < ROW; i++)
		for (int j = 0; j < COL; j++)
	{
			if (sourseState.numbers[i][j] != ultimateState.numbers[i][j])
			{
				error_rate++;
			}
	}
	return error_rate;
}

/**
* Name		:Manhattan_distance
* Function	:Though the comparison of parameter sourseState and ultimateState,return the mahattan distance of all numbers.
* Parameter	:const Sudoku& sourseState, const Sudoku& ultimateState
* Return	:The manhattan distance of all soursestate sudoku numbers. 
**/
int Manhattan_distance(const Sudoku& sourseState, const Sudoku& ultimateState)
{
	int error_rate = 0;
	for (int i = 0; i < ROW; i++)
	for (int j = 0; j < COL; j++)
	{
		if (sourseState.numbers[i][j] != ultimateState.numbers[i][j])
		{
			int x = i*COL + j;
			error_rate += manhattan[x][position[sourseState.numbers[i][j]]];
		}
	}
	return error_rate;
}


/**
* Name		:GenerateNodeId
* Function	:Generate the unique id of every sudoku.The id equal to the factorial of 10 mul every number in sudoku.
* Parameter	:const Sudoku& node
* Return	:The unique id of parameter node.
**/
int GenerateNodeId(const Sudoku& node)
{
	int multiplier = 1;
	int sum = 0;
	for (int i = 0; i < ROW; i++)
	for (int j = 0; j< COL; j++)
	{
		sum = node.numbers[i][j] * multiplier+sum;
		multiplier = multiplier * 10;
	}
	return sum;
}

/**
* Name		:InitSudoku
* Function	:Initialize the state of start sudoku and ultimate sudoku..
* Parameter	:Sudoku* startState, Sudoku* ultimateState
* Return	:void
**/
void InitSudoku(Sudoku* startState, Sudoku* ultimateState)
{
	//unsigned int test_start[3][3] = { { 3, 7, 2 }, { 8, 1, 5 }, { 4, 6, 0 } };
	unsigned int test_ult[3][3] = { { 1, 2, 3 }, { 8, 0, 4 }, { 7, 6, 5 } };
	cout << "Please input the initial state:\n";
	for (int i = 0; i < ROW; i++)
	{
		for (int j = 0; j < COL; j++)
		{
			cin>>startState->numbers[i][j];
		}
	}
	cout << "Please input the ultimate state:\n";
	for (int i = 0; i < ROW; i++)
	{
		for (int j = 0; j < COL; j++)
		{
			ultimateState->numbers[i][j] = test_ult[i][j];
		}
	}

	startState->value = Manhattan_distance(*startState, *ultimateState);		//	W(n) = valude of all numbers manhattan distance.
	startState->id = GenerateNodeId(*startState);
	startState->depth = 0;														//	d(n) = depth. 
	startState->sum = startState->value + startState->depth;			//value = f(n) = d(n) + W(n)
	startState->next = NULL;

	ultimateState->id = GenerateNodeId(*ultimateState);
	ultimateState->value = -1;
	ultimateState->depth = 0;
	ultimateState->sum = ultimateState->value + ultimateState->depth;
	ultimateState->next = NULL;
	
	cout << "The initial state: \n";
	PrintSudoku(*startState);
	cout << "The ultimate state: \n";
	PrintSudoku(*ultimateState);
}

/**
* Name		:PrintSudoku
* Function	:Print sudoku.
* Parameter	:const Sudoku& sudoku
* Return	:void
**/
void PrintSudoku(const Sudoku& sudoku)
{
	for (int i = 0; i < COL; i++)
	{
		for (int j = 0; j < ROW; j++)
		{
			if (sudoku.numbers[i][j]>0)
				cout << sudoku.numbers[i][j] << " ";
			else
				cout << "  ";
		}
		cout << endl;
	}
	cout << endl;
}

/**
* Name		:IsSecurityNode
* Function	:In order to make the program run without cycle.
*			 Every times the father generates a son node,which must be checked in open and close lists.
*			 '1'	 --Traversal the open list.
*			 If the node appears in the open list,compare the value of node and node in open list.The smaller one is linked in open list.
*			 If not,the node is linked in position after of open list head.
*			 '2'	--Traversal the close list.
*			 If the node appears in the close list,which means the node is used.Abandon the son node.
* Parameter	:Sudoku* node, Sudoku* g_openHead, Sudoku* g_closeHead
* Return	:The state of check result, '1' means security node. '0' means not security node.
**/
int IsSecurityNode(Sudoku* node, Sudoku* g_openHead, Sudoku* g_closeHead)
{
	Sudoku* pLink = g_openHead;
	Sudoku* pnode = NULL;
	while (pLink->next != NULL)				//Traversal the open list
	{
		if (pLink->next->id == node->id)
		{
			if (node->sum < pLink->next->sum)
			{
				node->next = pLink->next->next;
				free(pLink->next);
				pLink->next = node;
				return 0;
			}
		}
		pLink = pLink->next;
	}
	pLink = g_closeHead;
	while (pLink->next != NULL)				////Traversal the close list
	{
		if (pLink->next->id == node->id)
		{
			return 0;
		}
		pLink = pLink->next;
	}
	return 1;
}

/**
* Name		:MakeSudoku
* Function	:Though the father node,make the son node.
			 If the son node id security,put it after the open list head.
			 If not abandon.
* Parameter	:const Sudoku& fatherNode		--father node.
			 Sudoku* g_openHead				--global open list.
			 const int zeroPos				--The index of zero position
			 const int lastPos				--The index of moving position
			 dirction dirc					--The moving dirction
* Return	:The state of check result, '1' means security node. '0' means not security node.
**/
void MakeSudoku(const Sudoku& fatherNode, Sudoku* g_openHead, const int zeroPos, const int lastPos,dirction dirc)
{
	Sudoku* node = new Sudoku;
	for (int i = 0; i < ROW; i++)
	{
		for (int j = 0; j < COL; j++)
		{
			node->numbers[i][j] = fatherNode.numbers[i][j];
		}
	}
	int fx = zeroPos / ROW;
	int fy = zeroPos % COL;

	int sx = lastPos / ROW;
	int sy = lastPos % COL;

	node->dir = dirc;
	int swap = node->numbers[fx][fy];
	node->numbers[fx][fy]  = node->numbers[sx][sy];
	node->numbers[sx][sy] = swap;
	
	node->value = Manhattan_distance(*node, ultimate_sudoku);
	node->id = GenerateNodeId(*node);
	node->depth = fatherNode.depth + 1;
	node->sum = node->value + node->depth;
	node->zero_position = zeroPos;
	node->last_position = lastPos;

	if (1 == IsSecurityNode(node, g_openHead, g_closed_head))
	{
		node->next = g_openHead->next;
		g_openHead->next = node;

	}
}

/**
* Name		:GetDirctionValue
* Function	:Convert the dirction to index value.
* Parameter	:dirction dir
* Return	:The index value.
**/
int GetDirctionValue(dirction dir)
{
	switch (dir)
	{
	case up:	return	UP;
	case down:	return	DOWN;
	case e_left:return	LEFT;
	case e_right:return	RIGHT;
	default:	 return 0;
	}
}

/**
* Name		:GenerateNode
* Function	:Though the father node, genetating its son node/s.
* Parameter	:const Sudoku* fathernode,Sudoku* openHead
* Return	:void
**/
void GenerateNode(const Sudoku* fathernode,Sudoku* openHead)
{
	int zero_pos;
	int reversnode = fathernode->dir;
	int flag = 0;
	for (int i = 0; i < COL; i++)
	{

		for (int j = 0; j < ROW; j++)
		{
			if (fathernode->numbers[i][j] == ZERO_SIGN)				//Find the zero position of the father node.
			{
				zero_pos = i*COL + j;
				flag = 1;
				break;
			}
			if (fathernode->numbers[i][j] < 0)
				PrintSudoku(*fathernode);
		}
		if (flag == 1)
			break;
	}
	reversnode = zero_pos + GetDirctionValue(fathernode->dir);		//Calculating the index of moving index.
	//reversnode = the zero index of father node + the index of dirction.

	//Generating the four position's son node though father node.
	if ((zero_pos + 1 < SUDOKU_SIZE)&&(zero_pos + 1) / ROW == (zero_pos / ROW) && (zero_pos + 1 != reversnode))
	{
		MakeSudoku(*fathernode, openHead, zero_pos, zero_pos + 1, e_left);
	}
	if (((zero_pos  - 1)>=0) &&(zero_pos - 1) / ROW == (zero_pos / ROW) && (zero_pos - 1 != reversnode))
	{
		MakeSudoku(*fathernode, openHead, zero_pos, zero_pos - 1, e_right);
	}
	if (((zero_pos + ROW) < SUDOKU_SIZE) && (zero_pos + ROW != reversnode))
	{
		MakeSudoku(*fathernode, openHead, zero_pos, zero_pos + ROW, up);
	}
	if (((zero_pos - ROW) >= 0) && (zero_pos - ROW != reversnode))
	{
		MakeSudoku(*fathernode, openHead, zero_pos, zero_pos - ROW, down);
	}
}

/**
* Name		:PutInClose
* Function	:Delete a node from open list,added in close list.
* Parameter	:Sudoku* node, Sudoku* g_openHead, Sudoku* g_closeHead
* Return	:void
**/
int PutInClose(Sudoku* node, Sudoku* g_openHead, Sudoku* g_closeHead)
{
	Sudoku* pOpen = g_openHead;
	Sudoku* pClose = g_closeHead;
	while (pOpen->next !=node)
	{
		pOpen = pOpen->next;
	}
	pOpen->next = pOpen->next->next;
	
	//put in the head of close
	node->next = g_closeHead->next;
	g_closeHead->next = node;
	
	return 1;
}

/**
* Name		:PutInList
* Function	:Put a node in list
* Parameter	:Sudoku* node					//The node
			 Sudoku* g_openHead				//global open list head
			 Sudoku* g_closeHead			//global close list head
			 const bool& list				//'1' means put in close list,'0' means in open list
* Return	:void
**/
void PutInList(Sudoku* node, Sudoku* g_openHead, Sudoku* g_closeHead,const bool& list)
{
	Sudoku* pList = NULL;
	if (list == 0)
		pList = g_openHead;
	else
		pList = g_closeHead;
	
	pList->next = node;
	node->next = NULL;
}

/**
* Name		:FindMinSum
* Function	:Find the node of minimumest sum in open list.
* Parameter	:Sudoku* g_openHead
* Return	:Sudoku* node		//The min sum node.
**/
Sudoku* FindMinSum(Sudoku* g_openHead)
{
	int sum = 0xffff;
	Sudoku* minNode = NULL;
	Sudoku* open = NULL;
	minNode = g_openHead->next;

	open = g_openHead->next->next;
	while (open != NULL)
	{
		if (minNode->sum > open->sum)
		{
			minNode = open;
		}
		open = open->next;
	}
	return minNode;
}

/**
* Name		:IsUltimateState
* Function	:Judging if the node is ultimate state. 
* Parameter	:const Sudoku* node
* Return	:'1' means yes!!! '0' means no.U_U
**/
int IsUltimateState(const Sudoku* node)
{
	if (node->id == ultimate_sudoku.id)
		return 1;
	return 0;
}

/**
* Name		:PrintSuccessWay
* Function	:According the ultimate state and dirction,calculating the last node's id.
			 Find the last node though last node's id,delelte the others node,and change the poin dirction.
			 Print the success way though the rever node
* Result    :head <- node  <- node  <- node  <- node  <- node  <- node...  <- The start state <- NULL
			 head -> node  -> node  -> node  -> node  -> node  -> node...  -> The start state -> NULL
* Parameter	:Sudoku* g_closeHead
* Return	:void
**/
void PrintSuccessWay(Sudoku* g_closeHead)
{
	Sudoku* pLink = g_closeHead->next;
	Sudoku* node = new Sudoku;
	g_closeHead->next = NULL;
	node = pLink;

	Sudoku* pnode = NULL;
	Sudoku*	pnext = NULL;
	Sudoku* now = NULL;
	Sudoku* next = NULL;

	int sucessArry[50];
	int successNumber = 0;
	int zx, zy;		//zero-x,y
	int rx, ry;		//reverse-x,y

	sucessArry[successNumber++] = node->id;

	pLink = pLink->next;
	pnext = pLink->next;
	node->next = g_closeHead;
	while (pnext != NULL)
	{
		//Find the last node's id
		zx = node->zero_position / ROW; zy = node->zero_position % COL;
		rx = node->last_position / ROW; ry = node->last_position% COL;

		int x = node->numbers[zx][zy] * pow(10, node->zero_position);
		int y = node->numbers[zx][zy] * pow(10, node->last_position);
		node->id = node->id - x + y;

		sucessArry[successNumber++] = node->id;							//Storing the id in a array.
		while (node->id != pLink->id && pnext != NULL)					//Find the last node though last node's id
		{																//If not the last node,delete the redundant node	
			delete(pLink);
			pLink = pnext;
			pnext = pLink->next;
		}												//If is,change the last node'pointer to the node.
		pLink->next = node;
		node = pLink;
		if (pnext != NULL){
			pLink = pnext;
			pnext = pLink->next;
		}
	}

	int step = 0;
	cout << "step: " << step++ << endl;									
	PrintSudoku(*pLink);								//Print the first node.
	node = node->next;
	while (node->next != NULL)							//Print the others node though the reverse close list.
	{
		cout << "step: " << step++ << endl;
		PrintSudoku(*node);
		now = node;
		node = node->next;
		delete(now);
	} 

	now = g_open_head->next;							//Deleting the evert node in open list.Preventing memory leaks
	next = now->next;
	while (next != NULL)
	{
		delete(now);
		now = next;
		next = next->next;
	}
}

/**
* Name		:PlaySudoku
* Function	:The frame of the sudoku pragma.
* Parameter	:Sudoku* startSudoku, Sudoku* ultimateSudoku
* Return	:void
**/
void PlaySudoku(Sudoku* startSudoku, Sudoku* ultimateSudoku)
{
	Sudoku* minNode = NULL;
	minNode = startSudoku;
	//Put the start node in close list,meaning the node is already used.
	PutInList(startSudoku, g_open_head, g_closed_head ,k_open_list);		
	while (0 == IsUltimateState(minNode))					//Judging if the minest mode is the ultimate node.
	{
		minNode = FindMinSum(g_open_head);					//Find the minest node from open list.
		PutInClose(minNode, g_open_head, g_closed_head);	//Deleted the minest node from open list,put it in close list.
		GenerateNode(minNode, g_open_head);					//Generating the minest node's son node.
	}
	minNode = g_closed_head;
	PrintSuccessWay(minNode);								//Print the success way.
}

int IsAvailable(const Sudoku& node)
{
	int array[10];
	int arrayNumber = 0, reverseNumber = 0;

	for (int i = 0; i < ROW; i++)
	for (int j = 0; j < COL; j++)
	{
		array[arrayNumber++] = node.numbers[i][j];
	}
	for (int i = 0; i < arrayNumber; i++)
	for (int j = 0; j < i; j++)
	{
		if (array[i] > array[j])
			reverseNumber++;
	}
	if (reverseNumber % 2 == 1)
		return 1;
	return 0;
}
int main()
{
	double start = GetTickCount();							//Count time

	g_open_head->next = NULL;
	g_closed_head->next = NULL;

	InitSudoku(&start_sudoku, &ultimate_sudoku);			//Initilize start and ultimate sudoku.
	
	if (1 == IsAvailable(start_sudoku))
	{
	PlaySudoku(&start_sudoku, &ultimate_sudoku);			//The frame of progma.

	delete(g_open_head);
	delete(g_closed_head);

	double  end = GetTickCount();
	cout << "Evaluation function:" << "Manhattan distance." << endl;
	cout << "GetTickCount:" << end - start <<" ms"<< endl;
	}
	else {
		cout << "The sudoku is unsolveble\n";
		system("pause");
	}return 0;
}
//1 3 0 8 2 4 7 6 5
//2 8 0 1 6 3 7 5 4
//3 7 2 8 1 5 4 6 0