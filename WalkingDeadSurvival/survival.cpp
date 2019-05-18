#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <graphics.h>
#include <time.h>
#include <math.h>

#define SCREEN_HEIGHT 500 //�]�w�C���������� 
#define SCREEN_WIDTH 500 //�]�w�C�������e��
#define GRID_SIDE 40 //�]�w�C����}�C���l�ƶq 
#define LEFT_MARGINE 30 //�]�w����� 
#define TOP_MARGINE 40 //�]�w�W��� 
#define RESOURCE_AMOUNT 1 //�]�w�C�����͸귽�ƶq 
#define PER_RESOURCE_KILL 5 //�]�w�h�ָ귽�ƶq�i�H�����@����� 
#define INIT_SPEED  80 //�]�w��l���ʳt�� 
#define MAX_QUEUE_SIZE 1600 //�]�w�W�C�j�p 

//�ŧi�e�i��V�C�|��� 
enum Direction {
	RIGHT, 
	LEFT, 
	UP, 
	DOWN 
};

//�ŧi�C�����X�{����C�|��� 
enum Object {
	EMPTY, //�ť� 
	WALL, //��λ�ê�� 
	RESOURCE //��� 
};

//�ŧi��ͨ���`�I���c 
struct Node {
	int row; //�`�I��b�ĴX�� 
	int col; //�`�I��b�ĴX�C 
	Direction direct; //�Ӹ`�I���e�i��V 
	struct Node *next;	//���V�U�@�Ӹ`�I 
}; 

//�w�q���V�`�I���c�������ܼ� 
typedef struct Node *NodePointer;

//�w�q�y�е��c 
struct Location {
	int row;
	int col;
};

typedef struct PathNode *PathPointer;

//�w�q���|�`�I���c�A�Ψӫإ߲��ʸ��| 
struct PathNode {
	int cost; //�Z�����I���B�� 
	int steps; //�Z���ؼЪ��B�� 
	Location loc;
	PathPointer parent;
	PathPointer next;
};

void openWindow(); //�}�Ҵ������� 
void closeGame(NodePointer dragon); //�B�z�C�������޿� 
int playGame(int field[][GRID_SIDE], NodePointer dragon, NodePointer player); //�C���i���޿� 
bool IsGameOver(NodePointer zombie, NodePointer player, int field[][GRID_SIDE]); //(�ͦs�̦��`����G����M������) 
int showGameOverMsg(); //�C�������T��
void showInfo(); //��ܹC��������T 
void drawGameField(int field[][GRID_SIDE]); //ø�s�C���ϰ� 
void drawSquare(int row, int col, int color); //ø�s��� 
void controlZombieDirection(int field[][GRID_SIDE], NodePointer zombie, NodePointer player); //Ū��AI��J�A�ó]�w��Ҧ���͸`�I 
void controlPlayerDirection(int field[][GRID_SIDE], NodePointer player, NodePointer zombie); //Ū����L��V��J�A�Ϊ�AI��J 
void moveZombie(int field[][GRID_SIDE], NodePointer zombie); //ø�s��͸s�e�i�@�B������  
void movePlayer(NodePointer player); //ø�s�ͦs�̫e�i�@�B������  
void createResource(int field[][GRID_SIDE], NodePointer zombie); //���͸귽
bool IsAtWall(int field[][GRID_SIDE], int row, int col); //�P�_�O�_������  
bool IsAtZombie(NodePointer zombie, int row, int col); //�P�_�O�_�����ͪ�����
void playerCollectResource(int field[][GRID_SIDE], NodePointer player, NodePointer zombie); //�B�z�ͦs�̦�����귽�޿� 
void addZombie(int field[][GRID_SIDE], NodePointer zombie, NodePointer player); //�W�[��ͼƶq 
void killZombie(NodePointer zombie); //�H�������@����� 
Location nextStepLoc(NodePointer node, Direction direct); //�p��U�@�B���y�� 
Location findNearestResource(int field[][GRID_SIDE], NodePointer zombie); //�M��̱���귽���y�� 
//��ͦp�G�L�k��즳�ĸ��|�A�ȮɨM�w�@�Ӧw����V
Direction safeDirect4Zombie(int field[][GRID_SIDE], NodePointer zombie);

//��ʹM����I�����i��F�����|�A���ݦҼ{�|���|�����L��ͩΪ̥ͦs�̡A�u�ݦҼ{���༲���� 
PathPointer zombieFindPath(int field[][GRID_SIDE], Location startLoc, Location goalLoc);  
void addPathQueue(PathNode pathNode); //�N����n���X���`�I��J��C�� 
PathPointer popPathQueue(); //�Ǧ^���|��C���������A�ñN���q��C���R�� 
bool isPathQueueEmpty(); //�P�_��C�O�_���� 
void resetPathQueue(); //���]��C 
void sortPathQueue(); //���C���������i��Ƨ� 
bool IsInPathQueue(PathNode pathNode); //�P�_�Ӥ����O�_�b��C���� 
PathPointer buildPath(PathPointer goal); //�^�Ǩ�ؼЦ�m�����|��C 
int calcSteps(Location start, Location goal); //�p����I�����ݭn���ʪ��B�� 
bool visited(Location loc); //�P�_�O�_�Ӹ`�I�w�g���X�L 
Direction getDirectionByPath(NodePointer zombie, PathPointer path); //�q���|��ƧP�_�U�@�B��V 

Direction zombieAI(int field[][GRID_SIDE], NodePointer zombie, Location target); //���AI 
Direction playerAI(int field[][GRID_SIDE], NodePointer player, NodePointer zombie); //�ͦs��AI 

struct PathNode pathQueue[MAX_QUEUE_SIZE]; //�ŧi�N�n���X���`�I�W�C 
int front; //queue �Ĥ@�Ӥ������e�@�Ӧ�m 
int rear; //queue �̫�@�Ӥ�������m

int speed; //�C�����ʳt�� 
int scoreSum = 0; //�������� 
int killedCount = 0; //������ͼƶq 
int totalTime = 0; //�����C���ɶ� 
int stepCount = 0; //�B�ƭp�ƾ� 
int const scorePerResource = 1; //�C�@���귽�i�o���� 
bool IFPlayAI = false; //�O�_�}��AI�Ҧ� 

// �D�{��      
int main(){  	
	openWindow();
	
	while(TRUE){
		
		//�]�w�C�����M��ê�� 
	   	int field[GRID_SIDE][GRID_SIDE] = {{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
		   								   {1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1},
										   {1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1},
										   {1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1},
										   {1,0,0,0,1,0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,1,0,0,0,1},
										   {1,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1},
										   {1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1},
										   {1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1},
										   {1,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0,1},
										   {1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,2,0,0,0,0,0,0,0,1},
										   {1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1},
										   {1,0,0,0,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1},
										   {1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
										   {1,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,1,0,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1},
										   {1,0,0,1,1,1,1,0,0,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,1,1,1,1,0,0,1},
										   {1,0,0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1},
										   {1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1},
										   {1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1},
										   {1,0,0,1,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,0,0,1},
										   {1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1},
										   {1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1},
										   {1,0,0,1,1,1,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,1,1,1,0,0,1},
										   {1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
										   {1,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,1,0,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1},
										   {1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
										   {1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1,0,0,2,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
										   {1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1},
										   {1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1},
										   {1,0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,0,0,1},
										   {1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1},
										   {1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1},
										   {1,0,0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1},
										   {1,0,0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1},
										   {1,0,0,1,1,1,1,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,1,1,1,1,0,0,0,1},
										   {1,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1},
										   {1,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1},
										   {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
										   {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
										   {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
										   {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}}; 
	   	
		Node headPlayer = {2, 4, RIGHT, NULL}; //�]�w�i�̪�l��m�M��V 
		Node headZombie = {15, 15, RIGHT, NULL}; //�]�w��ͫ��Y��l��m�M��V 
		NodePointer zombie = &headZombie;
		NodePointer player = &headPlayer;
		
		char key;
		key = playGame(field, zombie, player); //�i��C��
		if (key == 'q' || key == 'Q')
			closeGame(zombie); //�p�G�ͦs�̿�J'q'���}�C��	
		else if (key == 's' || key == 'S')
			continue; //�p�G�ͦs�̿�J's' �~��C�� 		    
	}
}

//�}�Ҵ�������
void openWindow(){
	initwindow(SCREEN_WIDTH + LEFT_MARGINE * 3, SCREEN_HEIGHT + TOP_MARGINE * 3, "Hungry zombie Game");
}

//�B�z�C�������޿� 
void closeGame(NodePointer zombie){
	free(zombie);
	exit(0);
}

//�C���i���޿�
int playGame(int field[][GRID_SIDE], NodePointer zombie, NodePointer player) {
	speed = INIT_SPEED;
	stepCount = 0;
	killedCount = 0; 
	bool killed = true;
	srand((unsigned)time(NULL)); //���ثe�t�ήɶ��@���üƺؤl 
	drawGameField(field); //ø�s�C���ϰ� 
	createResource(field, zombie); //���ͲĤ@���귽 
	
	while(true){
				
		int key = 0;
		controlPlayerDirection(field, player, zombie);//Ū���ͦs�̿�J��V��A�ñN�s��V�]�w��U��͸`�I 
		movePlayer(player); //�̾ڸ`�I����V�Aø�s�s����ͦ�m
		
		if(stepCount % 2 == 0){
			controlZombieDirection(field, zombie, player); 
			moveZombie(field, zombie); //�̾ڸ`�I����V�Aø�s�s����ͦ�m			
		}

		//�s�W��ͼƶq 
		if(stepCount % 30 == 0)
			addZombie(field, zombie, player);

		playerCollectResource(field, player, zombie); //�P�_�ͦs�̬O�_��������귽�A�p�G���W�[���� 
		
		showInfo(); //��ܮɶ��M���Ƹ�T 
		
		if(IsGameOver(zombie, player, field)) //�P�_�O�_�ŦX�C����������A 
			return showGameOverMsg(); //��ܹC�������T���A�õ��ݥͦs�̿�J�ﶵ 
		
		//���F������귽�|���ͷs�귽�A�t�Τ]�H�����ͷs�귽
		if (rand() % 20 == 0)
			createResource(field, zombie);
			
		if(key != 0)
			return key;
		
		delay(speed); //�M�w�ͦs�̻P��Ͳ��ʳt�סAspeed�V�p���ʶV�� 
		stepCount++;
		//Ū���D��V�䪺��L��L��J 
	   	if(kbhit()) {
	   		char key;
			key = getch();	
			
			//�u����J�j�p�g��a, q �M s�A�t�Τ~�������ñN��J���G�ߨ�~�h���ݳB�z 
			if (key == 'q' || key == 'Q' || key == 's' || key == 'S')
				return key; 				
			else if (key == 'a') //�M�w�O�_���ܼҦ� �A�D�n���ͦs�̼Ҧ��M�q���ޱ���AI�Ҧ� 
				IFPlayAI = !IFPlayAI;
		}
	}
}

//ø�s�C���ϰ�A�̾ڹC�����x�}�]�wø�s���� 
void drawGameField(int field[][GRID_SIDE]){
   	int row = 0, col = 0;
   	cleardevice(); //�M�z�ù��e�� 
	for(row = 0; row < GRID_SIDE; row ++){
   		for(col = 0; col <  GRID_SIDE; col++){
				switch(field[row][col])	{
					case WALL: //��b�x�}�����ȬO1 
						drawSquare(row, col, YELLOW);
						break;
					case RESOURCE: //�귽�b�x�}�����ȬO2
						drawSquare(row, col, GREEN);
						break;	
				}	
		   }
	}
}

//ø�s���
void drawSquare(int row, int col, int color){
	
	int squareHeight = SCREEN_HEIGHT / GRID_SIDE;
   	int SquareWidth = SCREEN_WIDTH / GRID_SIDE;
	int left = 0, right = 0, bottom = 0, top = 0;
	left = LEFT_MARGINE + col * SquareWidth + col;
   	top =  TOP_MARGINE + row * squareHeight + row;
	right = left + SquareWidth;
   	bottom = top + squareHeight;
   	 
	setfillstyle(SOLID_FILL,color); //�]�wø�s���󪺬���ߩM�C�� 
   	bar(left , top, right, bottom);	//�]�wø�s�������m 
}

//ø�s��ͨC�e�i�@�B������ 
void moveZombie(int field[][GRID_SIDE], NodePointer zombie) {
	int currRow, currCol;
	
	while (zombie != NULL){
		currRow = zombie->row;
		currCol = zombie->col;
		
		if(field[currRow][currCol] == RESOURCE)
			drawSquare(currRow, currCol, GREEN);
		else
			drawSquare(currRow, currCol, BLACK);	
		
		//�̾ڸ`�I����V�ݩʡA�]�w���ʤU�@�B����m	
		switch(zombie->direct){
			case RIGHT:
				zombie->col++;
				break;			
			case LEFT:
				zombie->col--;
				break;
			case UP:
				zombie->row--;
				break;			
			case DOWN:
				zombie->row++;
				break;
		}

		drawSquare(zombie->row, zombie->col, RED);
			
		zombie = zombie -> next;	
	}
}

//ø�s�ͦs�̨C�e�i�@�B������ 
void movePlayer(NodePointer player) {
	int currRow, currCol;
	if (player != NULL){
		currRow = player->row;
		currCol = player->col;
		
		switch(player->direct){
			case RIGHT:
				player->col++;
				break;			
			case LEFT:
				player->col--;
				break;
			case UP:
				player->row--;
				break;			
			case DOWN:
				player->row++;
				break;
		}
		drawSquare(player->row, player->col, BLUE);
		drawSquare(currRow, currCol, BLACK);
	}	
}

//�P�_�ͦs�̬O�_���`(���`����G����M����ۤv����) 
bool IsGameOver(NodePointer zombie, NodePointer player, int field[][GRID_SIDE]){
	NodePointer head = zombie;
	
	//�P�_�O�_������ 
	if (IsAtWall(field, head->row, head->col))
		return true;
	if (IsAtWall(field, player->row, player->col))
		return true;
	
	//�ˬd�O�_AI������ 
	if(player != NULL) {
		if(IsAtZombie(zombie, player->row, player->col))
			return true;
	}
	
	return false;
}

//�P�_�O�_������
bool IsAtWall(int field[][GRID_SIDE], int row, int col){
		if (field[row][col] == WALL)
			return true;
		return false;
}

//�P�_�O�_������
bool IsAtZombie(NodePointer zombie, int row, int col){

	while(zombie != NULL){
		if (row == zombie->row && col == zombie->col)
			return true;
		zombie = zombie -> next;
	}
	return false;
}

//�C�������T��
int showGameOverMsg(){
   	//cleardevice(); //�M�z�Ҧ��ù���ơA�p�G�Ʊ�u��ܰT���ɡA�������� 
	int i = 0;
	char msg1[15] = "Game Over!!";
	char msg2[40] = "press [q] to quit or [s] to restart!!";
   	for(; ; i++){
	   	setcolor(i%14);
	   	settextstyle(TRIPLEX_FONT, HORIZ_DIR , 7);
	   	outtextxy(0, SCREEN_HEIGHT / 2, msg1);
	   	
	   	setcolor(WHITE);
	   	settextstyle(TRIPLEX_FONT, HORIZ_DIR , 2);
	   	outtextxy(20, SCREEN_HEIGHT / 2 + 70, msg2);
	   	
	   	delay(200);
	   	
	   	setcolor(BLACK);
	   	settextstyle(TRIPLEX_FONT, HORIZ_DIR , 7);
	   	outtextxy(0, SCREEN_HEIGHT / 2, msg1);	   
		   	
	   	if(kbhit()) {
	   		char key;
			key = getch();	
			if (key == 'q' || key == 'Q' || key == 's' || key == 'S') {
				return key; 				
			}
		}
	}	
}

//��ܹC��������T
void showInfo(){
	totalTime += speed;
	char timeMsg[45] = " Time:";
	char scoreMsg[45] = "Score:";
	char killedMsg[50] = "Killed Zombie:";
	char modeMsg[20] = "";
	char optMsg1[50] = "press [q] to quit, [s] to restart or";
	char optMsg2[50] = "press [a] to change mode.";
	
	char time[10];
	char score[10];
	char killed[10];
	
	sprintf(time, "%5d", totalTime/1000);
	strcat(timeMsg, time);
	strcat(timeMsg, " sec.");
	
	setcolor(WHITE); //�]�w��r�C�� 
   	settextstyle(COMPLEX_FONT, HORIZ_DIR , 1); //�]�w�r���A�����Ϋ����M�r���j�p 
   	outtextxy(0, 0, timeMsg); //�̾ڧ��п�X��r��ù�	
	
	sprintf(score, "%5d", scoreSum);
	strcat(scoreMsg, score);
	strcat(scoreMsg, " point.");

	setcolor(WHITE);
   	settextstyle(COMPLEX_FONT, HORIZ_DIR , 1);
   	outtextxy(0, 19, scoreMsg);
   	
 	sprintf(killed, "%3d", killedCount);
	strcat(killedMsg, killed);  	
   	
   	setcolor(WHITE);
   	settextstyle(COMPLEX_FONT, HORIZ_DIR , 1);
   	outtextxy(250, 19, killedMsg);
   	
   	if(IFPlayAI){
   		strcat(modeMsg, "AI Mode    ");
	}else{
   		strcat(modeMsg, "Player Mode");
	}
			
   	setcolor(LIGHTMAGENTA);
   	settextstyle(COMPLEX_FONT, HORIZ_DIR , 1);
   	outtextxy(SCREEN_HEIGHT - LEFT_MARGINE * 2, 0, modeMsg);
   	
   	setcolor(LIGHTGREEN);
   	settextstyle(COMPLEX_FONT, HORIZ_DIR , 1);
   	outtextxy(0, TOP_MARGINE + (GRID_SIDE + 2) * SCREEN_HEIGHT / GRID_SIDE, optMsg1);
	outtextxy(0, TOP_MARGINE + (GRID_SIDE + 2) * SCREEN_HEIGHT / GRID_SIDE + 20, optMsg2);	      	
}

//Ū����L��V��J�A�ó]�w��ͦs�̸`�I 
void controlPlayerDirection(int field[][GRID_SIDE], NodePointer player, NodePointer zombie) {
	
	Direction playerDirect = player -> direct;
	
	//get key code by pressing keybord
	int key;
	if(kbhit()) 
		key = getch();
		
	//decide zombie's moving direction	
	switch(key){
		case KEY_RIGHT:
			playerDirect = RIGHT;
			break;			
		case KEY_LEFT:
			playerDirect = LEFT;
			break;		
		case KEY_UP:
			playerDirect = UP;
			break;				
		case KEY_DOWN:
			playerDirect = DOWN;
			break;				
	}
		
	if (IFPlayAI)
		playerDirect = playerAI(field, player, zombie);
	
	
	player -> direct = playerDirect;
			
}


//Ū����L��V��J�A�ó]�w��Ҧ���͸`�I 
void controlZombieDirection(int field[][GRID_SIDE], NodePointer zombie, NodePointer player) {
	int count = 0;
	while(zombie != NULL){
		Location target = {player -> row + count, player -> col + count};
		Direction zombieDirect = zombieAI(field, zombie, target);
		zombie -> direct = zombieDirect;
		zombie = zombie -> next;
		count += 2;
	}	
	
}

//���͸귽
void createResource(int field[][GRID_SIDE], NodePointer zombie){
	int row, col, i, amount = RESOURCE_AMOUNT;

	for(i=0; i< amount; i++){
		//�p�G�üƲ��ͪ���m�O�b��M��ͨ��魫�|�A�h���s���͡A����ŦX���󬰤� 
		do{
			row = rand() % GRID_SIDE;
			col = rand() % GRID_SIDE;	
		}while(IsAtWall(field, row, col) || IsAtZombie(zombie, row, col));
	
		field[row][col] = RESOURCE;
		drawSquare(row,col, GREEN);			
	}
}


//�t�γB�z�ͦs�̦�����귽�޿�
void playerCollectResource(int field[][GRID_SIDE], NodePointer player, NodePointer zombie){
	//�p�G�ͦs�̻P�귽��m���|�A�N�O������귽
	if(field[player->row][player->col] == RESOURCE){
		field[player->row][player->col] = EMPTY; //�N�Ӹ귽�M�� 
		printf("The player has eaten food at row: %d, col: %d\n", player->row, player->col);
		scoreSum += scorePerResource; //�������� 
		createResource(field, player); //���ͷs���귽
		
		//�����@�w�ƶq���귽�i�H�����@����� 
		if(scoreSum % PER_RESOURCE_KILL == 0) 
			killZombie(zombie);
	}
}

//�W�[��ͼƶq 
void addZombie(int field[][GRID_SIDE], NodePointer zombie, NodePointer player){
	int row, col;
	NodePointer tail = NULL;
	NodePointer newNode = (NodePointer) malloc(sizeof(Node)); //��l�Ƥ@�ӷs�`�I 
	
	//�M��̫�@�ӳ�͸`�I 
	tail = zombie;
	while(tail->next != NULL){
		tail = tail -> next;
	}
	//�N�̫�@���ͪ���V�ݩʵ��s�`�I 
	newNode -> direct = tail -> direct;
	
	do{
		row = rand() % GRID_SIDE;
		col = rand() % GRID_SIDE;	
	}while(IsAtWall(field, row, col) || IsAtZombie(zombie, row, col) || (player->row == row && player->col == col));	
	
	newNode -> row = row;
	newNode -> col = col;
	
	tail -> next = newNode; //�N���ڸ`�I�s����s�`�I 
	newNode -> next = NULL;		
}

//�����@�����
void killZombie(NodePointer zombie){
	int i;
	NodePointer temp, killed;
	temp = zombie;

	killed = zombie;
	
	//���|�����Ҧ���͡A�ܤַ|�O�d�@�� 
	if(zombie->next == NULL)
		return;
	while(killed->next != NULL){
		temp = killed;
		killed = killed -> next;
	}
	temp -> next = killed -> next;
	drawSquare(killed->row, killed->col, BLACK);
	printf("\n(%d, %d) is killed\n", killed->row, killed->col);
	free(killed);
	killedCount ++;
}

//��ͪ�AI���� 
Direction zombieAI(int field[][GRID_SIDE], NodePointer zombie, Location target) {
	Direction zombieDirect = zombie -> direct;
	Location start = {zombie -> row, zombie -> col};


	PathPointer path = zombieFindPath(field, start, target);
	if(path){
		zombieDirect = getDirectionByPath(zombie, path);
	}	
	else
		zombieDirect = safeDirect4Zombie(field, zombie);	

	return zombieDirect;
}

//�q���|��ƧP�_�U�@�B��V 
Direction getDirectionByPath(NodePointer head, PathPointer path){
	PathPointer nextPath = path->next;
	int horizontal = nextPath->loc.col - head->col;
	int vertical = nextPath->loc.row - head->row;
	if(horizontal == 1)
		return RIGHT;
	else if(horizontal == -1)
		return LEFT;
	
	if(vertical == 1)
		return DOWN;
	else if(vertical == -1)
		return UP;
	return 	head -> direct;		
}

//��ͦp�G�L�k��즳�ĸ��|�A�ȮɨM�w�@�Ӧw����V 
Direction safeDirect4Zombie(int field[][GRID_SIDE], NodePointer zombie){
	int i;
	int dirSize = 4;
	Location loc;
	
	loc = nextStepLoc(zombie, UP);
	if(!IsAtWall(field, loc.row, loc.col))
		return UP;
	loc = nextStepLoc(zombie, DOWN);
	if(!IsAtWall(field, loc.row, loc.col))
		return DOWN;
	loc = nextStepLoc(zombie, RIGHT);
	if(!IsAtWall(field, loc.row, loc.col))
		return RIGHT;
	loc = nextStepLoc(zombie, LEFT);
	if(!IsAtWall(field, loc.row, loc.col))
		return LEFT;						
	return zombie->direct;
}

//��ʹM����I�����i��F�����|�A���ݦҼ{�|���|�����L��ͩΪ̥ͦs��
PathPointer zombieFindPath(int field[][GRID_SIDE], Location startLoc, Location goalLoc){
	resetPathQueue();
	int steps = calcSteps(startLoc, goalLoc);
	PathNode start = {0, steps, startLoc, NULL, NULL};
	addPathQueue(start);
	while(!isPathQueueEmpty()){
		sortPathQueue();
		PathPointer current = popPathQueue();
		if(current->loc.row == goalLoc.row && current->loc.col == goalLoc.col)
			return buildPath(current);
		int dirSize = 4;
		int iDir[] = {1, 0, -1, 0};
		int jDir[] = {0, 1, 0, -1};
		int i,j;
		for(i=0, j=0; i<dirSize; i++, j++){
			Location neighborLoc = {current->loc.row + iDir[i], current->loc.col + jDir[j]};
			if(!visited(neighborLoc) && !IsAtWall(field, neighborLoc.row, neighborLoc.col)){
				int steps = calcSteps(neighborLoc, goalLoc);
				int cost = 	current->cost + 1;
				PathNode neighbor = {cost, steps, neighborLoc, current, NULL};
				if(!IsInPathQueue(neighbor)){
					addPathQueue(neighbor);
				}				
			}
		}
	}
	return NULL;
}

//�N����n���X���`�I��J��C��
void addPathQueue(PathNode pathNode){
	if(rear == MAX_QUEUE_SIZE - 1){
		printf("the queue is full");
		return;
	}
	rear += 1;
	pathQueue[rear]	= pathNode;
}

//�Ǧ^��C�������|�y�и`�I�A�ñN���q��C���R��
PathPointer popPathQueue(){
	if(front == rear){
		printf("the queue is empty");
		return NULL;
	}
	front ++;
	PathPointer node = (PathPointer)malloc(sizeof(PathNode));
	node->cost = pathQueue[front].cost;
	node->steps = pathQueue[front].steps;
	node->loc = pathQueue[front].loc;
	node->parent = pathQueue[front].parent;
	node->next = pathQueue[front].next;
	return node;
}

//�P�_��C�O�_����
bool isPathQueueEmpty(){
	return front == rear;
}

//���]��C 
void resetPathQueue(){
	front = -1;
	rear = -1;	
}

//���C���������i��Ƨ� 
void sortPathQueue(){
	if(front == rear)
		return;
	int i, j;
	int nowTotal, nextTotal;	
	for(i=front + 1; i<rear; i++){
		for(j=i+1; j<=rear; j++){

			nowTotal = pathQueue[i].cost + pathQueue[i].steps;
			nextTotal = pathQueue[j].cost + pathQueue[j].steps;

			if(nowTotal > nextTotal){
				PathNode temp =  pathQueue[i];
				pathQueue[i] = pathQueue[j];
				pathQueue[j] = temp;
			}
		}
	}		
}

//�P�_�Ӥ����O�_�b��C����
bool IsInPathQueue(PathNode pathNode){
	int i;
	if(front == rear)
		return false;
	for(i=front;i<=rear;i++){
		if(pathQueue[i].loc.row == pathNode.loc.row && pathQueue[i].loc.col == pathNode.loc.col)
			return true;
	}
	return false;
}

//�^�Ǩ�ؼЦ�m�����|��C 
PathPointer buildPath(PathPointer goal){
	printf("buildPath ");
	printf("(%d, %d)\n", goal->loc.row, goal->loc.col);
	if(goal == NULL || goal->parent == NULL)
		return NULL;
	PathPointer head = goal;
	head->next = NULL;
	PathPointer temp = head;
	
	while(head->parent){
		if(head == NULL)
			return NULL;
		printf("node (%d, %d)->", head->loc.row, head->loc.col);	
		head = head->parent; 
		head->next = temp;
		temp = head;
	}
	printf("NULL\n");
	return head;
}

//�p����I�����ݭn���ʪ��B�� 
int calcSteps(Location start, Location goal){
	int steps = abs(start.row - goal.row) + abs(start.col - goal.col);
	return steps;
}

//�P�_�O�_�Ӹ`�I�w�g���X�L 
bool visited(Location loc){
	int i;
	for(i=0;i<=front;i++){
		if(pathQueue[i].loc.row == loc.row && pathQueue[i].loc.col == loc.col)
			return true;
	}
	return false;
}

//�M��̱���귽
Location findNearestResource(int field[][GRID_SIDE], NodePointer me){
	int rowDis, colDis, row, col, nearest = 100000;
	Location nearestFood = {-1, -1};
	for(row = 0; row < GRID_SIDE; row++){
		for(col = 0; col < GRID_SIDE; col++){
			if(field[row][col] == RESOURCE){
				rowDis = abs(row - me -> row);
				colDis = abs(col - me -> col);
				if(nearest > (rowDis + colDis)){
					nearest = (rowDis + colDis);
					nearestFood.row = row;
					nearestFood.col = col;
				}
			}
		}
	}
	return nearestFood;
}

//�p��U�@�B���y�� 
Location nextStepLoc(NodePointer node, Direction direct){
	int currRow =  node -> row;
	int currCol = node -> col;
	int nextRow, nextCol;
	Location loc;
	switch(direct){
		case RIGHT:
			nextRow = currRow;
			nextCol = currCol + 1;
			break;	
		case LEFT:
			nextRow = currRow;
			nextCol = currCol - 1;
			break;
		case UP:
			nextRow = currRow - 1;
			nextCol = currCol;
			break;				
		case DOWN:
			nextRow = currRow + 1;
			nextCol = currCol;		
			break;	
	}
	loc.row = nextRow;
	loc.col = nextCol;
	return loc;
}


//��@�ͦs��AI�A�Цb��function��@�XAI�\�� 
Direction playerAI(int field[][GRID_SIDE], NodePointer player, NodePointer zombie){
	Direction playerDirect = player -> direct;

	
	return playerDirect;
}

