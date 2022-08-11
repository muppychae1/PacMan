#include<iostream>
#include<string>
#include<cstdlib>
#include<conio.h>
#include <Windows.h>
using namespace std;

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
public:
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

void textcolor(int fg_color, int bg_color) //text의 색상 지정
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), fg_color | bg_color << 4);
}

void cls(int bg_color, int text_color) // 화면 초기화
{
	char cmd[100];
	system("cls");
	sprintf(cmd, "COLOR %x%x", bg_color, text_color);
	system(cmd);
}

void print_PACMAN(int x, int y, string ch)
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
				cout << ch;
		}
	}

	textcolor(rand() % 15 + 1, BLACK);
	for (i = 0; i < 5; i++) {
		for (j = 0; j < 5; j++) {
			gotoxy(i * 2 + x + 12, j + y);
			if (a[j][i] == 1)
				cout << ch;
		}
	}

	textcolor(rand() % 15 + 1, BLACK);
	for (i = 0; i < 5; i++) {
		for (j = 0; j < 5; j++) {
			gotoxy(i * 2 + x + 24, j + y);
			if (c[j][i] == 1)
				cout << ch;
		}
	}

	textcolor(rand() % 15 + 1, BLACK);
	for (i = 0; i < 5; i++) {
		for (j = 0; j < 5; j++) {
			gotoxy(i * 2 + x + 36, j + y);
			if (m[j][i] == 1)
				cout << ch;
		}
	}

	textcolor(rand() % 15 + 1, BLACK);
	for (i = 0; i < 5; i++) {
		for (j = 0; j < 5; j++) {
			gotoxy(i * 2 + x + 48, j + y);
			if (a[j][i] == 1)
				cout << ch;
		}
	}

	textcolor(rand() % 15 + 1, BLACK);
	for (i = 0; i < 5; i++) {
		for (j = 0; j < 5; j++) {
			gotoxy(i * 2 + x + 60, j + y);
			if (n[j][i] == 1)
				cout << ch;
		}
	}

}

void draw_box(int x1, int y1, int x2, int y2, string ch)
{
	int len = ch.length();
	int i;
	for (i = x1; i <= x2; i += len) {
		gotoxy(i, y1);
		cout << ch;
		gotoxy(i, y2);
		cout << ch;
	}
	for (i = y1; i <= y2; i++) {
		gotoxy(x1, i);
		cout << ch;
		gotoxy(x2, i);
		cout << ch;
	}
}

int start_screen() //타이틀 화면 출력
{
	unsigned char ch;
	cls(BLACK, WHITE);
	removeCursor();
	draw_box(0, 0, 78, 30, "▦");

	gotoxy(30, 15);
	cout << "Press the SPACE key";

	gotoxy(10, 17); cout << "┌────────────────────────────┐";
	gotoxy(10, 18); cout << "│                        START                           │";
	gotoxy(10, 19); cout << "└────────────────────────────┘";

	gotoxy(10, 21); cout << "  ↑   : UP";
	gotoxy(10, 22); cout << "←  → : LEFT / RIGHT";
	gotoxy(10, 23); cout << "  ↓   : DOWN";
	gotoxy(10, 24); cout << "  ◎   : PACMAN SPEED UP FOR 3 sec";
	gotoxy(10, 25); cout << "  ★   : GHOST SPEED DOWN FOR 3 sec";

	srand((unsigned)time(NULL));
	while (1) {
		print_PACMAN(4, 5, "■");

		if (kbhit() == 1) {
			char c1;
			cin >> c1;
			if (c1 == '\0')
				continue;
			else
				ch = c1;

			if (ch == SPACE) return 1;
			else return 0;
		}
		Sleep(200);
	}
}

class PacMan {
private:
	static int newx, newy;
	static int oldx, oldy;
	static unsigned char last_ch;
	static int check_item_circle, check_item_star;
	static clock_t start_circle, now_circle, duration_circle;
	static clock_t start_star, now_star, duration_star;
public:
	PacMan(); //생성자
	PacMan(int x, int y); // 생성자
	void put_pacman();
	void setXY(int x, int y);
	unsigned char is_first(unsigned char ch);
	bool is_wall(int x, int y);
	void moving(unsigned char ch);
	void eat_item();
	void earase_pacman();
	void setting_oldxy();
};

int PacMan::newx = 24; int PacMan::oldx = 24;
int PacMan::newy = 16; int PacMan::oldy = 16;
unsigned char PacMan::last_ch;
int PacMan::check_item_circle; int PacMan::check_item_star;
clock_t PacMan::start_circle; clock_t PacMan::now_circle; clock_t PacMan::duration_circle;
clock_t PacMan::start_star; clock_t PacMan::now_star; clock_t PacMan::duration_star;

PacMan::PacMan() {
	newx = oldx = 24;
	newy = oldy = 16;
	check_item_circle = 0;
	check_item_star = 0;
}

PacMan::PacMan(int x, int y) {
	newx = oldx = x;
	newy = oldy = y;
	check_item_circle = 0;
	check_item_star = 0;
}

void PacMan::put_pacman() {
	map[newy][newx / 2] = 99;
	gotoxy(newx, newy);
	textcolor(YELLOW1, BLACK);
	cout << PACMAN;
	textcolor(BLUE2, BLACK);
}

void PacMan::setXY(int x, int y) {
	newx = oldx = x;
	newy = oldy = y;
}

unsigned char PacMan::is_first(unsigned char ch) {
	if (called[0] == 0) { //처음 또는 Restart
		setXY(24, 16);
		called[0] = 1;
		return 0;
	}

	else {
		setXY(newx, newy);
		return ch;
	}
}

bool PacMan::is_wall(int x, int y) {
	//벽이 있다면 return 1 아니면 0
	switch (map[y][x]) {
	case 1: case 2: case 3: case 4: case 5: case 6:
		return true;
	}
	return false;
}

void PacMan::moving(unsigned char ch) {
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
		break;
	case DOWN:
		if (!is_wall(oldx / 2, oldy + 1)) {
			newy = oldy + 1;
		}
		break;
	case LEFT:
		if (!is_wall(oldx / 2 - 1, oldy)) {
			newx = oldx - 2;
		}
		if (oldy == 14 && oldx - 2 < 0) //옆 통로 통과하기
			newx = (WIDTH - 1) * 2;
		break;
	case RIGHT:
		if (!is_wall(oldx / 2 + 1, oldy)) {
			newx = oldx + 2;
		}
		if (oldy == 14 && oldx + 2 > (WIDTH - 1) * 2)//옆 통로 통과하기
			newx = 0;
		break;
	}
}

void PacMan::eat_item() {
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
}

void PacMan::earase_pacman() {
	map[oldy][oldx / 2] = 7;
	gotoxy(oldx, oldy);
	printf("  ");
}

void PacMan::setting_oldxy() {
	oldx = newx;
	oldy = newy;
}

/////////SetGame class 시작///////////
class SetGame {
public:
	SetGame(); //생성자
	void draw_map();
	void show_score_life_round();
};

SetGame::SetGame() { //생성자
	cls(BLACK, WHITE);
	draw_map();
	removeCursor();
}

void SetGame::draw_map() {
	for (int i = 0; i < WIDTH; i++) {
		for (int j = 0; j < HEIGHT; j++) {
			gotoxy(i * 2, j);
			textcolor(BLUE2, BLACK);

			switch (map[j][i]) {
			case 0:
				textcolor(WHITE, BLACK);
				cout << "·";
				break;
			case 1:
				cout << "─";
				break;
			case 2:
				cout << "│";
				break;
			case 3:
				cout << "┌";
				break;
			case 4:
				cout << "┐";
				break;
			case 5:
				cout << "┘";
				break;
			case 6:
				printf("└");
				break;
			case 7:
				break;
			case 8:
				textcolor(WHITE, BLACK);
				cout << "◎";
				break;
			case 9:
				textcolor(MAGENTA2, BLACK);
				cout << "★";
				break;
			}
		}
		cout << endl;
	}
}

void SetGame::show_score_life_round() {
	gotoxy(50, 2);
	textcolor(WHITE, BLACK);
	cout << "SCORE : " << score << endl;

	gotoxy(50, 4);
	textcolor(RED1, BLACK);
	cout << "LIFE : ";
	for (int i = 0; i < life; i++)
		cout << LIFE;
	for (int i = 1; i <= 3 - life; i++)
		cout << "♡";
	cout << endl;

	gotoxy(50, 6);
	textcolor(GREEN2, BLACK);
	cout << game_stage << " STAGE" << endl;
	textcolor(BLUE2, BLACK);
}

void player(unsigned char ch, PacMan& user)
{
	ch = user.is_first(ch); // 첫 턴(Restart) or !첫턴
	user.moving(ch);

	user.earase_pacman();
	user.put_pacman();
	user.setting_oldxy();
}

void play_game()
{
	//START:
	SetGame sg;
	PacMan user;
	unsigned char ch;
	int i, j, check;

	while (1) {
		if (kbhit() == 1) {
			char c1;
			cin >> c1;
			if (c1 == '\0')
				continue;
			else
				ch = c1;
			//ESC 누르면 프로그램 종료
			if (ch == ESC)
				exit(0);
			if (ch == SPECIAL1) { //만약 특수키
				//예를들어 UP key의 경우 0xe0 0x48 두개의 문자가 들어온다.
				cin >> ch;
				switch (ch) {
				case UP:
				case DOWN:
				case LEFT:
				case RIGHT:
					player(ch, user);
					break;
				default:
					if (frame_count % pacman_frame_sync == 0)
						player(0, user);
				}

			}

		}
		else {
			if (frame_count % pacman_frame_sync == 0)
				player(0, user);
		}

		sg.show_score_life_round();

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
}

int main()
{
	int click;

	while (1) {
		click = start_screen();

		//게임 시작
		if (click)
			play_game();

		else
			break;
	}
	return 0;
}