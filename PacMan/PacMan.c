#include <stdio.h>
#include <conio.h>
#include <Windows.h>
#include <time.h>
#include <stdlib.h>

// 색상 정의
#define BLACK	0
#define BLUE1	1
#define GREEN1	2
#define CYAN1	3
#define RED1	4
#define MAGENTA1 5
#define YELLOW1	6
#define GRAY1	7
#define GRAY2	8
#define BLUE2	9
#define GREEN2	10
#define CYAN2	11
#define RED2	12
#define MAGENTA2 13
#define YELLOW2	14
#define WHITE	15

#define BLANK "  " // ' ' 로하면 흔적이 지워진다 

#define ESC 0x1b //  ESC 누르면 종료
#define SPACE 0x20

#define SPECIAL1 0xe0 // 특수키는 0xe0 + key 값으로 구성된다.
#define SPECIAL2 0x00 // keypad 경우 0x00 + key 로 구성된다.

#define UP  0x48 // Up key는 0xe0 + 0x48 두개의 값이 들어온다.
#define DOWN 0x50
#define LEFT 0x4b
#define RIGHT 0x4d

#define WIDTH 25
#define HEIGHT 30

#define LIFE "♥"
#define PACMAN "●"
#define GHOST "＠"

int Delay = 10; // 100 msec delay, 이 값을 줄이면 속도가 빨라진다.
int keep_moving = 1; // 1:계속이동, 0:한칸씩이동.
int score = 0;
int life = 3;
int called[2] = { 0 };
int time_out = 3 * CLOCKS_PER_SEC; // 제한시간
int frame_count = 0;
int pacman_frame_sync = 10;
int ghost_frame_sync = 10;
int game_stage = 1; //게임의 round

typedef struct ghost {
	int x;
	int y;
	int dx; //x축으로 움직일 방향 (-1 : 왼쪽, 0 : X, 1 : 오른쪽)
	int dy; //y축으로 움직일 방향 (-1 : 위쪽, 0 : X, 1 : 아래쪽)
}Ghost;

Ghost ghost[3] = { {20, 14, 0, 0}, {24, 14, 0, 0}, {28, 14, 0, 0} };  //고스트 초기 위치

// 0 = · /  1 = ─ / 2 = │ / 3 = ┌ / 4 = ┐/ 5 = ┘ / 6 = └ / 7 =    / 8 = ◎ / 99 = PACMAN
int map[HEIGHT][WIDTH] = {
	{3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 4, 7, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 4},//0
	{2, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 7, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 2},//1
	{2, 0, 3, 1, 4, 0, 3, 1, 1, 4, 0, 2, 7, 2, 0, 3, 1, 1, 4, 0, 3, 1, 4, 0, 2},//2
	{2, 0, 2, 7, 2, 0, 2, 7, 7, 2, 0, 2, 7, 2, 0, 2, 7, 7, 2, 0, 2, 7, 2, 0, 2},//3
	{2, 0, 6, 1, 5, 0, 6, 1, 1, 5, 0, 6, 1, 5, 0, 6, 1, 1, 5, 0, 6, 1, 5, 0, 2},//4
	{2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2},//5
	{2, 0, 3, 1, 4, 0, 3, 4, 0, 3, 1, 1, 1, 1, 1, 4, 0, 3, 4, 0, 3, 1, 4, 0, 2},//6
	{2, 0, 6, 1, 5, 0, 2, 2, 0, 6, 1, 4, 7, 3, 1, 5, 0, 2, 2, 0, 6, 1, 5, 0, 2},//7
	{2, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 2, 7, 2, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 2},//8
	{6, 1, 1, 1, 4, 0, 2, 6, 1, 4, 7, 2, 7, 2, 7, 3, 1, 5, 2, 0, 3, 1, 1, 1, 5},//9
	{7, 7, 7, 7, 2, 0, 2, 3, 1, 5, 7, 6, 1, 5, 7, 6, 1, 4, 2, 0, 2, 7, 7, 7, 7},//10
	{7, 7, 7, 7, 2, 0, 2, 2, 7, 7, 7, 7, 7, 7, 7, 7, 7, 2, 2, 0, 2, 7, 7, 7, 7},//11
	{7, 7, 7, 7, 2, 0, 2, 2, 7, 3, 1, 7, 7, 7, 1, 4, 7, 2, 2, 0, 2, 7, 7, 7, 7},//12
	{1, 1, 1, 1, 5, 0, 6, 5, 7, 2, 7, 7, 7, 7, 7, 2, 7, 6, 5, 0, 6, 1, 1, 1, 1},//13
	{7, 7, 7, 7, 7, 0, 7, 7, 9, 2, 7, 7, 7, 7, 7, 2, 9, 7, 7, 0, 7, 7, 7, 7, 7},//14
	{1, 1, 1, 1, 4, 0, 3, 4, 7, 6, 1, 1, 1, 1, 1, 5, 7, 3, 4, 0, 3, 1, 1, 1, 1},//15
	{7, 7, 7, 7, 2, 0, 2, 2, 7, 7, 7, 7, 7, 7, 7, 7, 7, 2, 2, 0, 2, 7, 7, 7, 7},//16
	{7, 7, 7, 7, 2, 0, 2, 2, 7, 3, 1, 1, 1, 1, 1, 4, 7, 2, 2, 0, 2, 7, 7, 7, 7},//17
	{3, 1, 1, 1, 5, 0, 6, 5, 7, 6, 1, 4, 7, 3, 1, 5, 7, 6, 5, 0, 6, 1, 1, 1, 4},//18
	{2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 7, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2},//19
	{2, 0, 3, 1, 1, 4, 0, 3, 1, 4, 0, 2, 7, 2, 0, 3, 1, 4, 0, 3, 1, 1, 4, 0, 2},//20
	{2, 0, 6, 1, 4, 2, 0, 6, 1, 5, 0, 6, 1, 5, 0, 6, 1, 5, 0, 2, 3, 1, 5, 0, 2},//21
	{2, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 2},//22
	{6, 1, 4, 0, 2, 2, 0, 3, 4, 0, 3, 1, 1, 1, 4, 0, 3, 4, 0, 2, 2, 0, 3, 1, 5},//23
	{3, 1, 5, 0, 6, 5, 0, 2, 2, 0, 6, 4, 7, 3, 5, 0, 2, 2, 0, 6, 5, 0, 6, 1, 4},//24
	{2, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 2, 7, 2, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 2},//25
	{2, 0, 3, 1, 1, 1, 1, 5, 6, 4, 0, 2, 7, 2, 0, 3, 5, 6, 1, 1, 1, 1, 4, 0, 2},//26
	{2, 0, 6, 1, 1, 1, 1, 1, 1, 5, 0, 6, 1, 5, 0, 6, 1, 1, 1, 1, 1, 1, 5, 0, 2},//27
	{2, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 2},//28
	{6, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 5} //29
};

void removeCursor(void) { // 커서를 안보이게 한다

	CONSOLE_CURSOR_INFO curInfo;
	GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
	curInfo.bVisible = 0;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
}

void gotoxy(int x, int y) //내가 원하는 위치로 커서 이동
{
	COORD pos = { x, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);// WIN32API 함수입니다. 이건 알필요 없어요
}

void textcolor(int fg_color, int bg_color)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), fg_color | bg_color << 4);
}

void cls(int bg_color, int text_color)
{
	char cmd[100];
	system("cls");
	sprintf(cmd, "COLOR %x%x", bg_color, text_color);
	system(cmd);
}

void draw_box(int x1, int y1, int x2, int y2, char* ch)
{
	int len = strlen(ch);
	int i, j;
	for (i = x1; i <= x2; i += len) {
		gotoxy(i, y1);
		printf("%s", ch);
		gotoxy(i, y2);
		printf("%s", ch);
	}
	for (j = y1; j <= y2; j++) {
		gotoxy(x1, j);
		printf("%s", ch);
		gotoxy(x2, j);
		printf("%s", ch);
	}
}

void print_PACMAN(int x, int y, char* ch)
{
	int i, j;

	int p[5][5] = {
					{1,1,1,1,1},
					{1,0,0,0,1},
					{1,1,1,1,1},
					{1,0,0,0,0},
					{1,0,0,0,0} };

	int a[5][5] = {
					{0,0,1,0,0},
					{0,1,0,1,0},
					{0,1,1,1,0},
					{1,0,0,0,1},
					{1,0,0,0,1} };

	int c[5][5] = {
					{0,1,1,1,1},
					{1,0,0,0,0},
					{1,0,0,0,0},
					{1,0,0,0,0},
					{0,1,1,1,1} };

	int m[5][5] = {
					{1,0,0,0,1},
					{1,1,0,1,1},
					{1,0,1,0,1},
					{1,0,0,0,1},
					{1,0,0,0,1} };

	int n[5][5] = {
					{1,0,0,0,1},
					{1,1,0,0,1},
					{1,0,1,0,1},
					{1,0,0,1,1},
					{1,0,0,0,1} };

	textcolor(rand() % 15 + 1, BLACK);
	for (i = 0; i < 5; i++) {
		for (j = 0; j < 5; j++) {
			gotoxy(i * 2 + x, j + y);
			if (p[j][i] == 1)
				printf("%s", ch);
		}
	}

	textcolor(rand() % 15 + 1, BLACK);
	for (i = 0; i < 5; i++) {
		for (j = 0; j < 5; j++) {
			gotoxy(i * 2 + x + 12, j + y);
			if (a[j][i] == 1)
				printf("%s", ch);
		}
	}

	textcolor(rand() % 15 + 1, BLACK);
	for (i = 0; i < 5; i++) {
		for (j = 0; j < 5; j++) {
			gotoxy(i * 2 + x + 24, j + y);
			if (c[j][i] == 1)
				printf("%s", ch);
		}
	}

	textcolor(rand() % 15 + 1, BLACK);
	for (i = 0; i < 5; i++) {
		for (j = 0; j < 5; j++) {
			gotoxy(i * 2 + x + 36, j + y);
			if (m[j][i] == 1)
				printf("%s", ch);
		}
	}

	textcolor(rand() % 15 + 1, BLACK);
	for (i = 0; i < 5; i++) {
		for (j = 0; j < 5; j++) {
			gotoxy(i * 2 + x + 48, j + y);
			if (a[j][i] == 1)
				printf("%s", ch);
		}
	}

	textcolor(rand() % 15 + 1, BLACK);
	for (i = 0; i < 5; i++) {
		for (j = 0; j < 5; j++) {
			gotoxy(i * 2 + x + 60, j + y);
			if (n[j][i] == 1)
				printf("%s", ch);
		}
	}
}

int start_screen()
{
	unsigned char ch;
	cls(BLACK, WHITE);
	removeCursor();
	draw_box(0, 0, 78, 30, "▦");

	gotoxy(30, 15);
	printf("Press the SPACE key");

	gotoxy(10, 17); printf("┌────────────────────────────┐");
	gotoxy(10, 18); printf("│                        START                           │");
	gotoxy(10, 19); printf("└────────────────────────────┘");

	gotoxy(10, 21); printf("  ↑   : UP");
	gotoxy(10, 22); printf("←  → : LEFT / RIGHT");
	gotoxy(10, 23); printf("  ↓   : DOWN");
	gotoxy(10, 24); printf("  ◎   : PACMAN SPEED UP FOR 3 sec");
	gotoxy(10, 25); printf("  ★   : GHOST SPEED DOWN FOR 3 sec");

	srand((unsigned)time(NULL));
	while (1) {
		print_PACMAN(4, 5, "■");

		if (kbhit() == 1) {
			char c1;
			c1 = getche();
			if (c1 == '\0')
				continue;
			else
				ch = c1;

			if (ch == SPACE)
				return 1;
			else
				return 0;
		}
		Sleep(200);
	}
}

void draw_map()
{
	for (int i = 0; i < WIDTH; i++) {
		for (int j = 0; j < HEIGHT; j++) {
			gotoxy(i * 2, j);
			textcolor(BLUE2, BLACK);

			switch (map[j][i]) {
			case 0:
				textcolor(WHITE, BLACK);
				printf("·");
				break;
			case 1:
				printf("─");
				break;
			case 2:
				printf("│");
				break;
			case 3:
				printf("┌");
				break;
			case 4:
				printf("┐");
				break;
			case 5:
				printf("┘");
				break;
			case 6:
				printf("└");
				break;
			case 7:
				break;
			case 8:
				textcolor(WHITE, BLACK);
				printf("◎");
				break;
			case 9:
				textcolor(MAGENTA2, BLACK);
				printf("★");
				break;
			}
		}
		printf("\n");
	}
}

void resetmap() // 재시작 or stage up일 때 맵 초기화
{
	int samemap[HEIGHT][WIDTH] = {
		{3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 4, 7, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 4},//0
		{2, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 7, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 2},//1
		{2, 0, 3, 1, 4, 0, 3, 1, 1, 4, 0, 2, 7, 2, 0, 3, 1, 1, 4, 0, 3, 1, 4, 0, 2},//2
		{2, 0, 2, 7, 2, 0, 2, 7, 7, 2, 0, 2, 7, 2, 0, 2, 7, 7, 2, 0, 2, 7, 2, 0, 2},//3
		{2, 0, 6, 1, 5, 0, 6, 1, 1, 5, 0, 6, 1, 5, 0, 6, 1, 1, 5, 0, 6, 1, 5, 0, 2},//4
		{2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2},//5
		{2, 0, 3, 1, 4, 0, 3, 4, 0, 3, 1, 1, 1, 1, 1, 4, 0, 3, 4, 0, 3, 1, 4, 0, 2},//6
		{2, 0, 6, 1, 5, 0, 2, 2, 0, 6, 1, 4, 7, 3, 1, 5, 0, 2, 2, 0, 6, 1, 5, 0, 2},//7
		{2, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 2, 7, 2, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 2},//8
		{6, 1, 1, 1, 4, 0, 2, 6, 1, 4, 7, 2, 7, 2, 7, 3, 1, 5, 2, 0, 3, 1, 1, 1, 5},//9
		{7, 7, 7, 7, 2, 0, 2, 3, 1, 5, 7, 6, 1, 5, 7, 6, 1, 4, 2, 0, 2, 7, 7, 7, 7},//10
		{7, 7, 7, 7, 2, 0, 2, 2, 7, 7, 7, 7, 7, 7, 7, 7, 7, 2, 2, 0, 2, 7, 7, 7, 7},//11
		{7, 7, 7, 7, 2, 0, 2, 2, 7, 3, 1, 7, 7, 7, 1, 4, 7, 2, 2, 0, 2, 7, 7, 7, 7},//12
		{1, 1, 1, 1, 5, 0, 6, 5, 7, 2, 7, 7, 7, 7, 7, 2, 7, 6, 5, 0, 6, 1, 1, 1, 1},//13
		{7, 7, 7, 7, 7, 0, 7, 7, 9, 2, 7, 7, 7, 7, 7, 2, 9, 7, 7, 0, 7, 7, 7, 7, 7},//14
		{1, 1, 1, 1, 4, 0, 3, 4, 7, 6, 1, 1, 1, 1, 1, 5, 7, 3, 4, 0, 3, 1, 1, 1, 1},//15
		{7, 7, 7, 7, 2, 0, 2, 2, 7, 7, 7, 7, 7, 7, 7, 7, 7, 2, 2, 0, 2, 7, 7, 7, 7},//16
		{7, 7, 7, 7, 2, 0, 2, 2, 7, 3, 1, 1, 1, 1, 1, 4, 7, 2, 2, 0, 2, 7, 7, 7, 7},//17
		{3, 1, 1, 1, 5, 0, 6, 5, 7, 6, 1, 4, 7, 3, 1, 5, 7, 6, 5, 0, 6, 1, 1, 1, 4},//18
		{2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 7, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2},//19
		{2, 0, 3, 1, 1, 4, 0, 3, 1, 4, 0, 2, 7, 2, 0, 3, 1, 4, 0, 3, 1, 1, 4, 0, 2},//20
		{2, 0, 6, 1, 4, 2, 0, 6, 1, 5, 0, 6, 1, 5, 0, 6, 1, 5, 0, 2, 3, 1, 5, 0, 2},//21
		{2, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 2},//22
		{6, 1, 4, 0, 2, 2, 0, 3, 4, 0, 3, 1, 1, 1, 4, 0, 3, 4, 0, 2, 2, 0, 3, 1, 5},//23
		{3, 1, 5, 0, 6, 5, 0, 2, 2, 0, 6, 4, 7, 3, 5, 0, 2, 2, 0, 6, 5, 0, 6, 1, 4},//24
		{2, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 2, 7, 2, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 2},//25
		{2, 0, 3, 1, 1, 1, 1, 5, 6, 4, 0, 2, 7, 2, 0, 3, 5, 6, 1, 1, 1, 1, 4, 0, 2},//26
		{2, 0, 6, 1, 1, 1, 1, 1, 1, 5, 0, 6, 1, 5, 0, 6, 1, 1, 1, 1, 1, 1, 5, 0, 2},//27
		{2, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 2},//28
		{6, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 5}//29
	};

	for (int i = 0; i < HEIGHT; i++)
		for (int j = 0; j < WIDTH; j++)
			map[i][j] = samemap[i][j];
}

void erase_pacman(int x, int y)
{
	map[y][x / 2] = 7;
	gotoxy(x, y);
	printf("  ");
}

void erase_ghost(int x, int y) {
	gotoxy(x, y);
	textcolor(WHITE, BLACK);

	// 원래 있던 자리의 아이템들 표시
	if (map[y][x / 2] == 0)
		printf("·");
	else if (map[y][x / 2] == 7)
		printf("  ");
	else if (map[y][x / 2] == 8)
		printf("◎");
	else if (map[y][x / 2] == 9) {
		textcolor(MAGENTA2, BLACK);
		printf("★");
	}
}

void put_pacman(int x, int y)
{
	map[y][x / 2] = 99;
	gotoxy(x, y);
	textcolor(YELLOW1, BLACK);
	printf("%s", PACMAN);
	textcolor(BLUE2, BLACK);
}

void put_ghost(int n, int x, int y)
{
	//팩맨과 달리 int map[][]에 고스트를 따로 숫자로 지정하는 것이 아닌
	//지도 위에 표시해주는 것이다.
	if (n == 0) textcolor(CYAN2, BLACK);
	else if (n == 1) textcolor(RED2, BLACK);
	else if (n == 2) textcolor(YELLOW2, BLACK);

	gotoxy(x, y);
	printf("%s", GHOST);
	textcolor(BLUE2, BLACK);
}

void print_score_life(int score, int life)
{
	gotoxy(50, 2);
	textcolor(WHITE, BLACK);
	printf("SCORE : %d\n", score);

	gotoxy(50, 4);
	textcolor(RED1, BLACK);
	printf("LIFE : ");
	for (int i = 0; i < life; i++)
		printf("%s", LIFE);
	for (int i = 1; i <= 3 - life; i++)
		printf("%s", "♡");
	printf("\n");

	gotoxy(50, 6);
	textcolor(GREEN2, BLACK);
	printf("%d STAGE", game_stage);
	textcolor(BLUE2, BLACK);
}

int is_wall(int x, int y) 
{
	//벽이 있다면 return 1 아니면 0
	switch (map[y][x]) {
	case 1: case 2: case 3: case 4: case 5: case 6:
		return 1;
	}
	return 0;
}

int is_ghost(int g, int x, int y)
{
	//고스트들끼리 겹쳐지지 않도록 설정
	for (int i = 0; i < 3; i++) {
		if (i == g) //자기 자신 제외
			continue;
		if (x == ghost[i].x && y == ghost[i].y)
			return 1;
	}
	return 0;
}

int newx = 24, oldx = 24; //실질적으로 12
int newy = 16, oldy = 16;

void player(unsigned char ch)
{

	int move_flag = 0;
	static unsigned char last_ch = 0;
	int i;
	static int check_item_circle = 0, check_item_star = 0;
	static clock_t start_circle, now_circle, duration_circle;
	static clock_t start_star, now_star, duration_star;

	if (called[0] == 0) { //처음 또는 Restart
		oldx = 24; oldy = 16; newx = 24; newy = 16;
		map[newy][newx / 2] = 99;
		put_pacman(newx, newy);
		called[0] = 1;
		ch = 0;
	}
	// 같은 방향으로 key 가 들어오면 무시한다.
	if (last_ch == ch && frame_count % pacman_frame_sync != 0)
		return;

	if (keep_moving && ch == 0)
		ch = last_ch;
	last_ch = ch;

	switch (ch) {
	case UP:
		if (!is_wall(oldx / 2, oldy - 1)) {
			newy = oldy - 1;
		}
		move_flag = 1;
		break;
	case DOWN:
		if (!is_wall(oldx / 2, oldy + 1)) {
			newy = oldy + 1;
		}
		move_flag = 1;
		break;
	case LEFT:
		if (!is_wall(oldx / 2 - 1, oldy)) {
			newx = oldx - 2;
		}
		if (oldy == 14 && oldx - 2 < 0) //옆 통로 통과하기
			newx = (WIDTH - 1) * 2;
		move_flag = 1;
		break;
	case RIGHT:
		if (!is_wall(oldx / 2 + 1, oldy)) {
			newx = oldx + 2;
		}
		if (oldy == 14 && oldx + 2 > (WIDTH - 1) * 2)//옆 통로 통과하기
			newx = 0;
		move_flag = 1;
		break;
	}

	if (map[newy][newx / 2] == 0) { //· 먹었을 때
		score += 10;
	}

	if (map[newy][newx / 2] == 8) { //◎ 먹었을 때
		pacman_frame_sync = 5;
		start_circle = clock();
		check_item_circle = 1;
	}
	
	if (check_item_circle) {
		now_circle = clock();
		duration_circle = time_out - (now_circle - start_circle);
		if (duration_circle <= 0) {
			pacman_frame_sync = 10;
			check_item_circle = 0;
		}
	}

	if (map[newy][newx / 2] == 9) { //★ 먹었을 때
		ghost_frame_sync = 15;
		start_star = clock();
		check_item_star = 1;
	}

	if (check_item_star) {
		now_star = clock();
		duration_star = time_out - (now_star - start_star);
		if (duration_star <= 0) {
			ghost_frame_sync = 10;
			check_item_star = 0;
		}
	}

	erase_pacman(oldx, oldy);
	put_pacman(newx, newy);
	oldx = newx;
	oldy = newy;
}

void play_ghost()
{
	static unsigned char last_ch = 0;
	int i, j, rand_dir;
	int check = 0;

	//possible_dir[0] = 위 / [1] = 아래 / [2] = 왼쪽 / [3] = 오른쪽
	//고스트가 갈 수 있는 방향을 1 과 0으로 표현
	int possible_dir[4] = { 0 };

	if (called[1] == 0) {
		for (int i = 0; i < 3; i++) {
			ghost[i].x = 20 + i * 4;
			ghost[i].y = 14;
			ghost[i].dx = 0;
			ghost[i].dy = 0;
			put_ghost(i, ghost[i].x, ghost[i].y);
		}
		called[1] = 1;
	}

	srand((unsigned)time(NULL));
	for (i = 0; i < 3; i++) {
		//다 0으로 초기화 해주기
		for (j = 0; j < 4; j++) {
			possible_dir[j] = 0;
		}

		if (!is_wall(ghost[i].x / 2, ghost[i].y - 1) && !is_ghost(i, ghost[i].x, ghost[i].y - 1))
			possible_dir[0] = 1;
		if (!is_wall(ghost[i].x / 2, ghost[i].y + 1) && !is_ghost(i, ghost[i].x, ghost[i].y + 1))
			possible_dir[1] = 1;
		if (!is_wall(ghost[i].x / 2 - 1, ghost[i].y) && !is_ghost(i, ghost[i].x - 2, ghost[i].y))
			possible_dir[2] = 1;
		if (!is_wall(ghost[i].x / 2 + 1, ghost[i].y) && !is_ghost(i, ghost[i].x + 2, ghost[i].y))
			possible_dir[3] = 1;

		//고스트는 왔던 길을 다시 가지 않는다.
		int cnt = 0;
		for (j = 0; j < 4; j++) {
			if (possible_dir[j] == 1)
				cnt++;
		}

		if (cnt > 1) {
			if (ghost[i].dy == -1)
				possible_dir[1] = 0;
			else if (ghost[i].dy == 1)
				possible_dir[0] = 0;
			else if (ghost[i].dx == -1)
				possible_dir[3] = 0;
			else if (ghost[i].dx == 1)
				possible_dir[2] = 0;
		}

		if ((newx - ghost[i].x <= 14 && newx - ghost[i].x >= 0) || (ghost[i].x - newx <= 14 && ghost[i].x - newx >= 0)
			|| (newy - ghost[i].y <= 7 && newy - ghost[i].y >= 0) || (ghost[i].y - newy <= 7 && ghost[i].y - newy >= 0)) {

			if ((ghost[i].y - newy <= 7 && ghost[i].y - newy >= 0) && possible_dir[0] == 1) { //위쪽으로가기
				ghost[i].dx = 0;
				ghost[i].dy = -1;
				check = 1;
			}
			else if ((newy - ghost[i].y <= 7 && newy - ghost[i].y >= 0) && possible_dir[1] == 1) { //아래로 가기
				ghost[i].dx = 0;
				ghost[i].dy = 1;
				check = 1;
			}
			else if ((ghost[i].x - newx <= 14 && ghost[i].x - newx >= 0) && possible_dir[2] == 1) { //왼쪽으로 가기
				ghost[i].dx = -1;
				ghost[i].dy = 0;
				check = 1;
			}
			else if ((newx - ghost[i].x <= 14 && newx - ghost[i].x >= 0) && possible_dir[3] == 1) { //오른쪽으로 가기
				ghost[i].dx = 1;
				ghost[i].dy = 0;
				check = 1;
			}
		}
		
		if (check == 0) {
			do {
				rand_dir = rand() % 4;
				if (possible_dir[rand_dir] == 1) {
					if (rand_dir == 0) {
						ghost[i].dx = 0;
						ghost[i].dy = -1;
						check = 1;
					}
					else if (rand_dir == 1) {
						ghost[i].dx = 0;
						ghost[i].dy = 1;
						check = 1;
					}
					else if (rand_dir == 2) {
						ghost[i].dx = -1;
						ghost[i].dy = 0;
						check = 1;
					}
					else if (rand_dir == 3) {
						ghost[i].dx = 1;
						ghost[i].dy = 0;
						check = 1;
					}
				}

				else {
					if (newy < ghost[i].y && possible_dir[0] == 1) {
						ghost[i].dx = 0;
						ghost[i].dy = -1;
						check = 1;
					}
					else if (newy > ghost[i].y && possible_dir[1] == 1) {
						ghost[i].dx = 0;
						ghost[i].dy = 1;
						check = 1;
					}
					else if (newx < ghost[i].x && possible_dir[2] == 1) {
						ghost[i].dx = -1;
						ghost[i].dy = 0;
						check = 1;
					}
					else if (newx > ghost[i].x && possible_dir[3] == 1) {
						ghost[i].dx = 1;
						ghost[i].dy = 0;
						check = 1;
					}
				}

				if (possible_dir[0] == 0 && possible_dir[1] == 0 && possible_dir[2] == 0 && possible_dir[3] == 0) {
					ghost[i].dx = 0;
					ghost[i].dy = 0;
					check = 1;
				}

			} while (possible_dir[rand_dir] == 0 && check == 0);
		}
		check = 0;
		erase_ghost(ghost[i].x, ghost[i].y);

		if (ghost[i].x <= 0 && ghost[i].y == 14) {
			ghost[i].x = (WIDTH - 1) * 2;
			ghost[i].dx = -1;
			ghost[i].dy = 0;
		}
		else if (ghost[i].x >= (WIDTH - 1) * 2 && ghost[i].y == 14) {
			ghost[i].x = 0;
			ghost[i].dx = 1;
			ghost[i].dy = 0;
		}

		ghost[i].x = ghost[i].x + 2 * ghost[i].dx;
		ghost[i].y = ghost[i].y + ghost[i].dy;

		put_ghost(i, ghost[i].x, ghost[i].y);

		if (map[ghost[i].y][ghost[i].x / 2] == 99) {
			life--;
			//목숨이 줄어들면 팩맨만 초기 위치로 돌아가기
			erase_pacman(newx, newy);
			newx = oldx = 24;
			newy = oldy = 16;
			put_pacman(newx, newy);
		}
	}
}

void play_game()
{
	unsigned char ch;
	int i, j, check;
	
START:
	cls(BLACK, WHITE);
	draw_map();
	removeCursor();

	while (1) {
		if (kbhit() == 1) { //키보드가 눌려져 있으면
			char c1;
			c1 = getch(); //key 값을 읽는다
			if (c1 == '\0')
				continue;
			else
				ch = c1;
			//ESC 누르면 프로그램 종료
			if (ch == ESC)
				exit(0);
			if (ch == SPECIAL1) {//만약 특수키
				// 예를 들어 UP key의 경우 0xe0 0x48 두개의 문자가 들어온다.
				ch = getch();
				//Player1은 방향키로 움직인다.
				switch (ch) {
				case UP:
				case DOWN:
				case LEFT:
				case RIGHT:
					player(ch);
					if (frame_count % ghost_frame_sync == 0)
						play_ghost();
					break;
				default: //방향이 전환이 아니면
					if (frame_count % pacman_frame_sync == 0)
						player(0);
					if (frame_count % ghost_frame_sync == 0)
						play_ghost();
				}
			}
		}
		else {
			// keyboard 가 눌려지지 않으면 계속 움직인다.
			// 이동중이던 방향으로 계속 이동
			if (frame_count % pacman_frame_sync == 0)
				player(0);
			if (frame_count % ghost_frame_sync == 0)
				play_ghost();
		}

		print_score_life(score, life);

		if (score >= 2150 * game_stage - 450) { //특정 점수가 모이면 고스트, 팩맨 속도 증가
			pacman_frame_sync = 5;
			ghost_frame_sync = 5;
		}

		if (score == 2150 * game_stage) {
			check = 1;
			break;
		}

		if (life == 0) {
			check = -1;
			break;
		}

		Sleep(Delay);
		frame_count++; // frame_count 값으로 속도 조절을 한다.
	}

	if (check == 1) {
		cls(BLACK, WHITE);
		textcolor(BLACK, WHITE);
		gotoxy(10, 12); printf("┌────────────────────────────┐");
		gotoxy(10, 13); printf("│                    STAGE CLEAR!                        │");
		gotoxy(10, 14); printf("└────────────────────────────┘");
		textcolor(WHITE, BLACK);
		Sleep(1000);

		Delay = 10;
		keep_moving = 1;
		/*score = 0;
		life = 3;*/
		called[0] = called[1] = 0;
		frame_count = 0;
		pacman_frame_sync = 10;
		ghost_frame_sync = 10;

		resetmap();
		check = 0;
		game_stage++;
		goto START;
	}

	if (check == -1) {
		cls(BLACK, WHITE);
		textcolor(WHITE, BLACK);
		draw_box(0, 0, 78, 30, "※");
		gotoxy(31, 10);
		textcolor(GREEN2, BLACK);
		printf("YOUR SCORE : %d", score);
		int c1, c2;
		while (1) {
			do {
				c1 = rand() % 16;
				c2 = rand() % 16;
			} while (c1 == c2);
			textcolor(c1, c2);
			gotoxy(32, 10);
			gotoxy(10, 12); printf("┌────────────────────────────┐");
			gotoxy(10, 13); printf("│                     GAME OVER!                         │");
			gotoxy(10, 14); printf("└────────────────────────────┘");
			gotoxy(24, 16);
			textcolor(WHITE, BLACK);
			printf("Hit (R) to Restart (Q) to Quit");
			Sleep(300);
			if (kbhit() == 1) {
				ch = getch();
				if (ch == 'r' || ch == 'q');
				break;
			}
		}
		if (ch == 'r') {
			Delay = 10;
			keep_moving = 1;
			score = 0;
			life = 3;
			called[0] = called[1] = 0;
			frame_count = 0;
			pacman_frame_sync = 10;
			ghost_frame_sync = 10;
			game_stage = 1;

			resetmap();
			goto START;
		}
		else
			exit(1);
	}
}

int main()
{
	int click;

	while (1) {
		click = start_screen();

		if (click) {
			play_game();
		}
		else
			break;
	}
}