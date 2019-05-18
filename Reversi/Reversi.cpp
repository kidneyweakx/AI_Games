#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <graphics.h>
#include <time.h>
#include <math.h>

#define SCREEN_HEIGHT 500 //設定遊戲視窗高度 
#define SCREEN_WIDTH 500 //設定遊戲視窗寬度
#define GRID_NUM 8 //設定遊戲方陣每邊格子數量 
#define LEFT_MARGINE 30 //設定左邊界 
#define TOP_MARGINE 45 //設定上邊界 
#define INIT_SPEED  80 //設定初始速度 
#define PLAYONE_COLOR  GREEN //設定玩家一顏色
#define PLAYTWO_COLOR  RED //設定玩家二顏色

//宣告棋子種類 
enum Chess {
	EMPTY, //空白 
	PLAY_ONE, //玩家一 
	PLAY_TWO //玩家二 
};

enum Mode {
	PLAYER, //玩家模式 
	AI_SINGLE, //玩家一, AI 玩家二 
	AI_BOTH //玩家一, 玩家二 都是AI 
};

//宣告棋子位子 
struct Location{
	int row; 
	int col;
};

//宣告紀錄落子位置的節點結構 
struct Node {
	Location loc; //棋盤位置 
	struct Node *next;	//指向下一個節點 
};

//定義指向節點結構的指標變數 
typedef struct Node *NodePointer;

void openWindow(); //開啟游戲視窗 
void closeGame(); //處理遊戲結束邏輯 
void playGame(int field[][GRID_NUM]); //遊戲進行邏輯 
void initChessboard(int field[][GRID_NUM]); //初始化棋盤狀態 
Location* getChessDown(int field[][GRID_NUM], Location *focusPtr); //取得玩家落子位置 
void setChess(int field[][GRID_NUM], Location chessLoc); //設定落子 
void drawChessboard(int field[][GRID_NUM]); //繪製遊戲區域 
void printBoardState(int field[][GRID_NUM]); //印出棋盤狀態 
bool checkValidation(NodePointer list, Location loc); //檢查該位置是否為合法的落子位置 
NodePointer getValidatedLocs(int field[][GRID_NUM]); //獲得所有合法下子的所有座標節點 
NodePointer addNode(NodePointer list, Location loc); //新增合法下子的座標資訊節點
void drawValidatedLocs(NodePointer list); //標記出所有該玩家所有合法的下棋位置 
void cleanValidatedLocs(NodePointer list); //清理標記 
void doReverse(int field[][GRID_NUM], Location chessLoc); //進行黑白棋翻轉 
int reverse(int field[][GRID_NUM], Location chessLoc, bool checkOnly); //計算某下子座標所有可以翻轉的棋子數量， check_only = true 只進行計算，false 會進行棋子的翻轉 
void drawSquare(int row, int col, int color); //繪製色塊 
void drawFocus(int field[][GRID_NUM], Location focus); //繪製目標十字  
void cleanFocus(int field[][GRID_NUM], Location focus);//清除十字 
void drawCircle(int row, int col, int color); //繪製圓點 
void showGameWinMsg(); //遊戲結束訊息
void showInfo(); //顯示遊戲相關資訊 

Location* PLAYONE_AI(int field[][GRID_NUM], Location *focusPtr, NodePointer validated_locs);//實作電腦控制邏輯 

char key; //操作按鍵 
int speed; //移動速度 
int currentPlayer = PLAY_ONE; //設定玩家 
int modeCount = 0; //控制模式紀錄 
int totalTime = 0; //紀錄遊戲時間 
int playMode = PLAYER; // 設定控制模式，玩家模式或AI模式 
int play1Score = 0; //玩家一分數 
int play2Score = 0; //玩家二分數 
int stepCount = 0; //回合計數器 

int main( )
{  	
	openWindow();
	while(TRUE){
	
		speed = INIT_SPEED;
		
		//設定遊戲場
	   	int field[GRID_NUM][GRID_NUM] = {0};

		playGame(field); //進行遊戲
		if (key == 'q' || key == 'Q')
			closeGame(); //如果玩家輸入'q'離開遊戲	
		else if (key == 's' || key == 'S')
			continue; //如果玩家輸入's' 繼續遊戲 		    
	}
}

//開啟游戲視窗
void openWindow(){
	initwindow(SCREEN_WIDTH + LEFT_MARGINE * 3, SCREEN_HEIGHT + TOP_MARGINE * 3, "Backgammon");
}

//處理遊戲結束邏輯 
void closeGame(){
	exit(0);
}

//遊戲進行邏輯
void playGame(int field[][GRID_NUM]){
	
	drawChessboard(field); //繪製遊戲區域 
	
	NodePointer validated_locs = NULL;
	play1Score = 0, play2Score = 0;
	stepCount = 0;
	
	initChessboard(field); //初始化棋盤狀態 
   	printBoardState(field); //印出棋盤狀態，用於debug使用 
   	
   	validated_locs = getValidatedLocs(field); //計算所有合法座標  
   	drawValidatedLocs(validated_locs); //在棋盤上繪製這回合合法的座標 
	   	
	Location focus = {0, 0};
	Location* chessLocPtr;
	key = 0;
	int endPoint = 0;
	
	while(true){
		delay(speed); //決定每回合下棋速度 
		showInfo(); //顯示時間和分數資訊
		
		if(playMode == PLAYER){
			chessLocPtr = getChessDown(field, &focus);
		}else if(playMode == AI_SINGLE){
			if(currentPlayer == PLAY_ONE)
				chessLocPtr = getChessDown(field, &focus);
			else
				chessLocPtr = PLAYONE_AI(field, &focus, validated_locs);			
		}
		else { // playMode == AI_BOTH
			getChessDown(field, &focus);
			chessLocPtr = PLAYONE_AI(field, &focus, validated_locs);
		}
		
		if(chessLocPtr == NULL){
			//只有輸入大小寫的a, q 和 s，系統才有反應並將輸入結果拋到外層等待處理 
			if (key == 'q' || key == 'Q' || key == 's' || key == 'S')
				return;				
			else if (key == 'a' || key == 'A'){ 
				//決定是否改變模式 ，主要有玩家模式和電腦操控的AI模式 
				modeCount++;
				playMode = modeCount % 3;
				key = 0;
			}
			
			if (endPoint == 0)
				continue;
		}
		
		//玩家只能下在合法位置 
		if(chessLocPtr != NULL && checkValidation(validated_locs, *chessLocPtr) == false)
			continue;
		

		cleanValidatedLocs(validated_locs); //清理合法位置標記 
		if(chessLocPtr != NULL)
			setChess(field, *chessLocPtr); //如果玩家按下space，就在目標十字座標繪製棋子，並改變棋盤狀態 
 
 		//如果不是賽末點，進行吃子判斷，並翻轉棋子 
		if(endPoint == 0)
			doReverse(field, *chessLocPtr);
		
		if(currentPlayer == PLAY_ONE){
			currentPlayer = PLAY_TWO;
		}else{
			currentPlayer = PLAY_ONE;
		}
		
		printBoardState(field);
		
		// 取得下一回合的合法位置 
		validated_locs = getValidatedLocs(field);
		//如果該回合無法獲得合法的下子位置，賽末點+1 
		if(validated_locs == NULL)
			endPoint++;
		else{
			endPoint = 0;	
		}
		
		
		//連續兩回合都無法獲得合法座標，或者已經沒有可以下子的位置，遊戲結束 
		if(endPoint == 2 || (play1Score + play2Score) == GRID_NUM * GRID_NUM){ //判斷是否符合遊戲結束條件， 
			showGameWinMsg(); //顯示遊戲結束訊息，並等待玩家輸入選項
			return;
		}
		
		//繪製合法座標標記 
		drawValidatedLocs(validated_locs);
		printf("end point: %d\n", endPoint);
							
	}		
}

// 初始化棋盤狀態 
void initChessboard(int field[][GRID_NUM]){
	Location loc;
   	currentPlayer = PLAY_ONE;
   	loc.row = GRID_NUM/2 - 1, loc.col = GRID_NUM/2 - 1;
   	setChess(field, loc);
   	
   	currentPlayer = PLAY_TWO;
   	loc.row = GRID_NUM/2, loc.col = GRID_NUM/2 - 1;
   	setChess(field, loc);
   	
   	currentPlayer = PLAY_ONE;
   	loc.row = GRID_NUM/2, loc.col = GRID_NUM/2;
   	setChess(field, loc);
   	
   	currentPlayer = PLAY_TWO;
   	loc.row = GRID_NUM/2 - 1, loc.col = GRID_NUM/2;
   	setChess(field, loc);
   	currentPlayer = PLAY_ONE;	
}

//繪製遊戲區域，依據遊戲場矩陣設定繪製物件 
void drawChessboard(int field[][GRID_NUM]){
   	int row = 0, col = 0;
   	int startX = 0, startY = 0, endX = 0, endY = 0;
   	cleardevice(); //清理螢幕畫面 
   	int squareHeight = SCREEN_HEIGHT / GRID_NUM;
   	int squareWidth = SCREEN_WIDTH / GRID_NUM;
   	setlinestyle(SOLID_LINE, 0, 1); 
   	setcolor(YELLOW);
	for(row = 0; row <= GRID_NUM; row ++){
		startX = LEFT_MARGINE + 0 * squareWidth + 0;
		startY =  TOP_MARGINE + row * squareHeight;
		endX = startX + GRID_NUM * squareWidth;
		line(startX, startY, endX, startY);
	}
	
	for(col = 0; col <=  GRID_NUM; col++){
		startX = LEFT_MARGINE + col * squareWidth;
		startY =  TOP_MARGINE + 0 * squareHeight + 0;
		endY = startY + GRID_NUM * squareHeight;
		line(startX, startY, startX, endY);
	}
}

//列印棋盤狀態 
void printBoardState(int field[][GRID_NUM]){
	int i,j;
	
	for(i=0;i<GRID_NUM;i++){
		for(j=0;j<GRID_NUM;j++)
			printf("%d ", field[i][j]);
		printf("\n");	
	}
}

//取得玩家落子位置
Location* getChessDown(int field[][GRID_NUM], Location *focusPtr){
		
	char keyPress;
	if(kbhit()) 
		keyPress = getch();
		
	if(keyPress == ' ' && field[focusPtr->row][focusPtr->col] == EMPTY){
		return focusPtr;
	}
		
	cleanFocus(field, *focusPtr);
	//decide focus moving direction	
	switch(keyPress){
		case KEY_RIGHT:
			if(focusPtr->col < GRID_NUM - 1)
				focusPtr->col = focusPtr->col + 1;
			break;			
		case KEY_LEFT:
			if(focusPtr->col > 0)
				focusPtr->col = focusPtr->col - 1;
			break;		
		case KEY_UP:
			if(focusPtr->row > 0)
				focusPtr->row = focusPtr->row - 1;
			break;				
		case KEY_DOWN:
			if(focusPtr->row < GRID_NUM - 1)
				focusPtr->row = focusPtr->row + 1;
			break;				
	}
	
	drawFocus(field, *focusPtr);
	//只有輸入大小寫的a, q 和 s，系統才有反應並將輸入結果拋到外層等待處理 
	if (keyPress == 'q' || keyPress == 'Q' || keyPress == 's' || keyPress == 'S' || keyPress == 'a' || keyPress == 'A'){
		key = keyPress;
	}
	return NULL;
}

//設定落子
void setChess(int field[][GRID_NUM], Location chessLoc){
	
	if(currentPlayer == PLAY_ONE){
		drawSquare(chessLoc.row, chessLoc.col, PLAYONE_COLOR);
		play1Score++;
		if(field[chessLoc.row][chessLoc.col]==PLAY_TWO)
			play2Score--;
	}else{
		drawSquare(chessLoc.row, chessLoc.col, PLAYTWO_COLOR);
		play2Score++;
		if(field[chessLoc.row][chessLoc.col]==PLAY_ONE)
			play1Score--;		
	}
	field[chessLoc.row][chessLoc.col] = currentPlayer;
	stepCount++;
}

//檢查該位置是否為合法的落子位置 
bool checkValidation(NodePointer list, Location loc){
	
	while(list != NULL){
		if(list->loc.row == loc.row && list->loc.col == loc.col)
			return true;
		list = list -> next;	
	}
	return false;
}

//獲得所有合法下子的所有座標節點 
NodePointer getValidatedLocs(int field[][GRID_NUM]){
	int row, col;
	NodePointer reversible_list = NULL;
	Location checkLoc;
	for(row=0; row < GRID_NUM; row++){
		for(col=0; col < GRID_NUM; col++){
			if(field[row][col] != EMPTY)
				continue;
			checkLoc.row = row, checkLoc.col = col;
			if(reverse(field, checkLoc, true) > 0){
				reversible_list = addNode(reversible_list, checkLoc);
			}		
		}
	}
	return reversible_list;
}

//新增合法下子的座標資訊節點
NodePointer addNode(NodePointer list, Location loc){
	NodePointer tail = NULL;
	NodePointer newNode = (NodePointer) malloc(sizeof(Node)); //初始化一個新節點 
	newNode -> loc = loc;
	newNode -> next = NULL;	
	if(list == NULL)
		return newNode;
	else{
		//尋找串列的尾巴 
		tail = list;
		while(tail -> next != NULL){
			tail = tail -> next;
		}
		tail -> next = newNode; //將尾巴節點連接到新節點 
		return list;
	}
}

//標記出所有該玩家所有合法的下棋位置 
void drawValidatedLocs(NodePointer list){
	
	if (list==NULL){
		printf("validated location do not exist!!\n");
		return;
	}

	printf("validated location: ");
	while(list != NULL){
		printf("(%d, %d) ",  list -> loc.row, list -> loc.col);
		
		if(currentPlayer == PLAY_ONE)
			drawCircle(list -> loc.row, list -> loc.col, PLAYONE_COLOR);
		else
			drawCircle(list -> loc.row, list -> loc.col, PLAYTWO_COLOR);
		
		list = list -> next;
	}
	printf("\n");

}

//清理標記 
void cleanValidatedLocs(NodePointer list){

	while(list != NULL){
		drawCircle(list -> loc.row, list -> loc.col, BLACK);
		list = list -> next;
	}
	printf("\n");

}

//進行黑白棋翻轉 
void doReverse(int field[][GRID_NUM], Location chessLoc){
	reverse(field, chessLoc, false);
}

//計算某下子座標所有可以翻轉的棋子數量， check_only = true 只進行計算，false 會進行棋子的翻轉 
int reverse(int field[][GRID_NUM], Location chessLoc, bool checkOnly){
	int dirSize = 8;
	int rowDir[] = {-1, -1, -1,  0, 0,  1, 1, 1};
	int colDir[] = {-1,  0,  1, -1, 1, -1, 0, 1};
	int i, rowNext, colNext;
	int totalReversed = 0;
	Location loc;
	
	for(i = 0; i<dirSize; i++){
		rowNext = chessLoc.row;
		colNext = chessLoc.col;
		int reverseCount = 0, count = 0;
		while(true){
			rowNext += rowDir[i];
			colNext += colDir[i];		
			if(field[rowNext][colNext] == EMPTY || rowNext < 0 || colNext < 0 || rowNext > GRID_NUM-1 || colNext > GRID_NUM-1){
				break;
			} else if(field[rowNext][colNext] != currentPlayer){
				count++;
			} else if(field[rowNext][colNext] == currentPlayer){
				reverseCount = count;
				break;
			}	
		}
		
		if(reverseCount > 0 && checkOnly == false){
			int j;
			rowNext = chessLoc.row;
			colNext = chessLoc.col;
			for(j = 0; j < reverseCount; j++){
				rowNext += rowDir[i];
				colNext += colDir[i];
				loc.row = rowNext;
				loc.col = colNext;
				setChess(field, loc);
			}
		}
		
		totalReversed += reverseCount; 
	}
	return totalReversed;
}

//繪製目標十字
void drawFocus(int field[][GRID_NUM], Location focus){
	int focusX = 0, focusY = 0;
	int squareHeight = SCREEN_HEIGHT / GRID_NUM;
   	int squareWidth = SCREEN_WIDTH / GRID_NUM;
   	focusX = LEFT_MARGINE + focus.col * squareWidth + squareWidth / 2;
   	focusY =  TOP_MARGINE + focus.row * squareHeight + squareHeight / 2;
   	setlinestyle(SOLID_LINE, 0, 1); 
   	setcolor(YELLOW);
   	line(focusX -5, focusY, focusX + 5, focusY);
   	line(focusX, focusY - 5, focusX, focusY + 5);
}

//清除目標十字
void cleanFocus(int field[][GRID_NUM], Location focus){
	if(field[focus.row][focus.col] == PLAY_ONE)
		setcolor(PLAYONE_COLOR);
	else if(field[focus.row][focus.col] == PLAY_TWO)
		setcolor(PLAYTWO_COLOR);	
	else
		setcolor(BLACK);
			
	int focusX = 0, focusY = 0;
	int squareHeight = SCREEN_HEIGHT / GRID_NUM;
   	int squareWidth = SCREEN_WIDTH / GRID_NUM;
   	focusX = LEFT_MARGINE + focus.col * squareWidth + squareWidth / 2;
   	focusY =  TOP_MARGINE + focus.row * squareHeight + squareHeight / 2;
   	setlinestyle(SOLID_LINE, 0, 1); 
   	
   	line(focusX -5, focusY, focusX + 5, focusY);
   	line(focusX, focusY - 5, focusX, focusY + 5);
}

//繪製方塊
void drawSquare(int row, int col, int color){
	
	int squareHeight = SCREEN_HEIGHT / GRID_NUM;
   	int SquareWidth = SCREEN_WIDTH / GRID_NUM;
	int left = 0, right = 0, bottom = 0, top = 0;
	left = LEFT_MARGINE + col * SquareWidth + 1;
   	top =  TOP_MARGINE + row * squareHeight + 1;
	right = left + SquareWidth - 1;
   	bottom = top + squareHeight - 1;
   	 
	setfillstyle(SOLID_FILL,color); //設定繪製物件的為實心和顏色 
   	bar(left , top, right, bottom);	//設定繪製方塊的位置 
}

//繪製圓點 
void drawCircle(int row, int col, int color){
	int squareHeight = SCREEN_HEIGHT / GRID_NUM;
   	int SquareWidth = SCREEN_WIDTH / GRID_NUM;
	int left = 0, right = 0, bottom = 0, top = 0;
	left = LEFT_MARGINE + col * SquareWidth + 3;
   	top =  TOP_MARGINE + row * squareHeight + 3;
	right = left + SquareWidth - 3;
	bottom = top + squareHeight - 3;
	int lr_center = (left + right)/2;
	int td_center = (top + bottom)/2;

	int r = 10;   	
   	setcolor(color);
//	setfillstyle(SOLID_FILL,color); //設定繪製物件的為實心和顏色 
	circle(lr_center,td_center,r);
}

//遊戲結束訊息
void showGameWinMsg(){
   	//cleardevice(); //清理所有螢幕資料，如果希望只顯示訊息時，取消註解 
	int i = 0;
	char msg1[20] = "";
	if(play1Score == play2Score){
		strcat(msg1, "End In A Draw!!");
	}else if(play1Score > play2Score){
		strcat(msg1, "Player One Win!!");
	}else
		strcat(msg1, "Player Two Win!!");
			
	char msg2[40] = "press [q] to quit or [s] to restart!!";
   	for(; ; i++){
	   	setcolor(i%14);
	   	settextstyle(TRIPLEX_FONT, HORIZ_DIR , 5);
	   	outtextxy(0, SCREEN_HEIGHT / 2, msg1);
	   	
	   	setcolor(WHITE);
	   	settextstyle(TRIPLEX_FONT, HORIZ_DIR , 2);
	   	outtextxy(20, SCREEN_HEIGHT / 2 + 50, msg2);
	   	
	   	delay(200);
	   	
	   	setcolor(BLACK);
	   	settextstyle(TRIPLEX_FONT, HORIZ_DIR , 5);
	   	outtextxy(0, SCREEN_HEIGHT / 2, msg1);	   
		   	
	   	if(kbhit()) {
			key = getch();	
			if (key == 'q' || key == 'Q' || key == 's' || key == 'S') {
				return; 				
			}
		}
	}	
}

//顯示遊戲相關資訊
void showInfo(){
	totalTime += speed;
	char timeMsg[45] = " Time:";
	char modeMsg[20] = "";
	char optMsg1[50] = "press [q] to quit, [s] to restart or";
	char optMsg2[50] = "press [a] to change mode.";
	char optMsg3[50] = "press [space] to set chess.";
	char play1RoundMsg[20] = "PLAYER ONE ROUND";
	char play2RoundMsg[20] = "PLAYER TWO ROUND";	
	char stepCountMsg[10] = "STEP:";
	
	char time[10];
	char step[10];
	char score1Msg[10];
	char score2Msg[10];
	int left, top;
	sprintf(time, "%5d", totalTime/1000);
	strcat(timeMsg, time);
	strcat(timeMsg, " sec.");
	
	sprintf(score1Msg, "%3d", play1Score);
	sprintf(score2Msg, "%3d", play2Score);
	sprintf(step, "%3d", stepCount);
	strcat(stepCountMsg, step);
	
	setcolor(WHITE); //設定文字顏色 
   	settextstyle(COMPLEX_FONT, HORIZ_DIR , 1); //設定字型，水平或垂直和字型大小 
   	outtextxy(0, 0, timeMsg); //依據坐標輸出文字到螢幕
   	
   	if(currentPlayer == PLAY_ONE){
   		outtextxy(30, 20, play1RoundMsg);
		setfillstyle(SOLID_FILL,PLAYONE_COLOR);  	  
	}else{
		outtextxy(30, 20, play2RoundMsg); 
		setfillstyle(SOLID_FILL,PLAYTWO_COLOR);  
	}
	
	left = 5, top = 20;
   	bar(left , top, left + 20, top + 20);		
	   		
   	if(playMode == PLAYER){
   		strcat(modeMsg, "Player Mode      ");
   				
	}else if(playMode == AI_SINGLE){
   		strcat(modeMsg, "P1 player, P2 AI");    		
	}else
		strcat(modeMsg, "P1, P2 AI both   ");

   	setcolor(LIGHTMAGENTA);
   	settextstyle(COMPLEX_FONT, HORIZ_DIR , 1);
   	outtextxy(SCREEN_WIDTH - LEFT_MARGINE * 4, 0, modeMsg);
	
	setcolor(WHITE); //設定文字顏色
	left = LEFT_MARGINE * 2 + GRID_NUM * SCREEN_WIDTH / GRID_NUM / 3;
	top = 20;	      	
	outtextxy(left + 25, 20, stepCountMsg);	
	
	left = LEFT_MARGINE * 4 + GRID_NUM * SCREEN_WIDTH / GRID_NUM / 2;
	top = 20;
	setfillstyle(SOLID_FILL,PLAYONE_COLOR);
	bar(left , top, left + 20, top + 20);	      	
	outtextxy(left + 25, 20, score1Msg);
	
	left = LEFT_MARGINE * 4 + GRID_NUM * SCREEN_WIDTH / GRID_NUM / 2 + 100;
	setfillstyle(SOLID_FILL,PLAYTWO_COLOR);
	bar(left, top, left + 20, top + 20);	      	
	outtextxy(left + 25, 20, score2Msg);
   	
   	setcolor(LIGHTGREEN);
   	settextstyle(COMPLEX_FONT, HORIZ_DIR , 1);
   	outtextxy(0, TOP_MARGINE + (GRID_NUM) * SCREEN_HEIGHT / GRID_NUM, optMsg1);
	outtextxy(0, TOP_MARGINE + (GRID_NUM) * SCREEN_HEIGHT / GRID_NUM + 20, optMsg2);	      	
	outtextxy(0, TOP_MARGINE + (GRID_NUM) * SCREEN_HEIGHT / GRID_NUM + 40, optMsg3);
}

//電腦玩家，請在此function實作AI功能 (stupid ai)
Location* PLAYONE_AI(int field[][GRID_NUM],  Location *focusPtr, NodePointer validated_locs){
	Location tmp= {0, 0}; int maxx = 0;//定義及初始化最優解	
	for (int i = 0; i < GRID_NUM; ++i)
		for (int j = 0; j < GRID_NUM; ++j)
		{
			tmp = {i, j};
			if (reverse(field, tmp, true) >= maxx && checkValidation(validated_locs, tmp) == true )
			{
				maxx = reverse(field, tmp, true);
				focusPtr->row = i ;focusPtr->col = j ;
			}
		}
	return focusPtr;
}
