// 지나가는 block을 피하는 게임
#define _CRT_SECURE_NO_WARNINGS //sprintf 사용을 위함
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <conio.h>
#include <Windows.h>

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

#define STAR "@"
#define BLANK " "
#define ITEM_STAR "$"
#define ITEM_HEART "&"

#define ESC 0x1b

#define SPECIAL1 0xe0 // 특수키는 0xe0 + key 값으로 구성된다.
#define SPECIAL2 0x00 // keypad 경우 0x00 + key 로 구성된다.

#define UP  0x48 // Up key는 0xe0 + 0x48 두개의 값이 들어온다.
#define DOWN 0x50
#define LEFT 0x4b
#define RIGHT 0x4d

#define WIDTH 85
#define HEIGHT 21
int player[HEIGHT][WIDTH] = { 0 };
int map[HEIGHT][WIDTH] = { 0 };
int item1[HEIGHT][WIDTH] = { 0 }; //star
int item2[HEIGHT][WIDTH] = { 0 }; //heart
int Delay = 60; // 100 msec delay, 이 값을 줄이면 속도가 빨라진다.
int cnt = 0;
int keep_moving = 0;
int score = 0;
int life = 1;

int bottom_flag = 1;
int jump_flag = 0;

void removeCursor(void) { // 커서를 안보이게 한다

	CONSOLE_CURSOR_INFO curInfo;
	GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
	curInfo.bVisible = 0;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
}

void gotoxy(int x, int y) //내가 원하는 위치로 커서 이동
{
	COORD pos = { x, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);// WIN32API 함수
}

void textcolor(int fg_color, int bg_color)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), fg_color | bg_color << 4);
}

// 화면 지우고 원하는 배경색으로 설정한다.
void cls(int bg_color, int text_color)
{
	char cmd[100];
	system("cls");
	sprintf(cmd, "COLOR %x%x", bg_color, text_color);
	system(cmd);

}
void putstar(int x, int y)
{
	gotoxy(x, y);
	printf("%s", STAR);
	player[y][x] = 1;
}
void erasestar(int x, int y)
{
	gotoxy(x, y);
	printf(BLANK);
	player[y][x] = 0;
}

int x1 = 30, y1 = 10, x2 = 30, y2 = 10;
void show_item(int w, int x, int y)
{
	switch (w) {
	case 1:
		if (item2[y][x]) break;
		gotoxy(x, y);
		textcolor(YELLOW1, WHITE);
		printf("%s", ITEM_STAR);
		item1[y][x] = 1;
	case 2:
		if (item1[y][x]) break;
		gotoxy(x, y);
		textcolor(MAGENTA1, WHITE);
		printf("%s", ITEM_HEART);
		item2[y][x] = 1;
	}
	textcolor(BLACK, WHITE);
}

void move_item(int x, int y)
{

	if (item1[y1][x1] == 1) {
		gotoxy(x1, y1);
		printf("  ");
		item1[y1][x1] = 0;
		y1 += y; x1 += x;
		gotoxy(x1, y1);
		item1[y1][x1] = 1;
		show_item(1, x1, y1);
	}
	if (item2[y2][x2] == 1) {
		gotoxy(x2, y2);
		printf("  ");
		item2[y2][x2] = 0;
		y2 += y; x2 += x;
		gotoxy(x2, y2);
		item2[y2][x2] = 1;
		show_item(2, x2, y2);
	}
	if (x1 <= 6) { item1[y1][x1] = 0; gotoxy(x1, y1); printf("  "); }
	if (x2 <= 6) { item2[y2][x2] = 0; gotoxy(x2, y2); printf("  "); }
}

void showscore()
{
	gotoxy(10, 3);
	printf("Score : ");
	textcolor(GREEN2, GRAY2);
	gotoxy(18, 3);
	printf("%d", score);
	textcolor(BLACK, WHITE);
}

void showlife(int life) {

	gotoxy(30, 3);
	printf("               ");
	gotoxy(30, 3);
	printf("Life : ");
	textcolor(MAGENTA1, WHITE);
	gotoxy(37, 3);
	for (int i = 1; i <= life; i++) {
		printf("%s", ITEM_HEART);
	}
	textcolor(BLACK, WHITE);
}

void player1(unsigned char ch)
{
	static int oldx = 30, oldy = 20, newx = 30, newy = 20;

	if (keep_moving && ch == 0)
		keep_moving = 0;

	switch (ch) {
	case UP: //끝까지 올라간 후, 내려가기
		if (bottom_flag == 1) { //바닥일 때
			bottom_flag = 0;
			jump_flag = 1;
		}

		if (jump_flag == 1) {
			newy -= 1;
		}
		else if (jump_flag == 0) {
			newy += 1;
		}

		if (newy <= 14) { //천장을 찍으면 점프 상태를 끝냄
			jump_flag = 0;
		}

		if (newy >= 20) { //바닥 이하로 내려가면 바닥으로 고정, bottom_flag를 바꿔줌
			newy = 20;
			bottom_flag = 1; keep_moving = 0;
		}
		break;
	}

	if (map[oldy][oldx] == 1 && player[oldy][oldx] == 1) { // 장애물에 닿으면 라이프 감소
		life--; Delay = 60;
	}
	if (item1[oldy][oldx] == 1 && player[oldy][oldx] == 1) {
		score += 1000; showscore();
		item1[oldy][oldx] = 0;
		printf("  ");
	}
	else if (item2[oldy][oldx] == 1 && player[oldy][oldx] == 1) {
		if (life < 3)
			life++;
		item2[oldy][oldx] = 0;
		printf("  ");
		showlife(life);
	}
	erasestar(oldx, oldy); // 마지막 위치의 * 를 지우고
	putstar(newx, newy); // 새로운 위치에서 * 를 표시한다.
	oldx = newx; // 마지막 위치를 기억한다.
	oldy = newy;
}

// box 그리기 함수, ch 문자열로 (x1,y1) ~ (x2,y2) box를 그린다.
void draw_box(int x1, int y1, int x2, int y2, char* ch)
{
	int x;
	int len = strlen(ch);
	for (x = x1; x <= x2; x += 1) {
		gotoxy(x, y1);
		printf("%s", ch);
		gotoxy(x, y2);
		printf("%s", ch);
	}
}
void draw_box2(int x1, int y1, int x2, int y2, char* ch)
{
	int y;
	int len = strlen(ch);
	for (y = y1; y <= y2; y += 1) {
		gotoxy(x1, y);
		printf("%s", ch);
		gotoxy(x2, y);
		printf("%s", ch);
	}
}
int block_x = 84;
int block_y = 20;
int rdom = 0;
void init_block()
{

	map[block_y][block_x] = 1;
	if (block_x == 84) {
		srand((unsigned int)time(NULL));
		rdom = rand() % 4 + 1;//1~4
	}
	for (int i = 0; i <= rdom; i++) {
		map[block_y - i][block_x] = 1;
	}
}
void show_block()
{
	int x, y;
	for (x = 0; x < WIDTH - 1; x++) {
		for (y = 0; y < HEIGHT; y++) {
			if (map[y][x] == 1) {
				gotoxy(x, y);
				textcolor(BLACK, WHITE);
				printf("#");
			}
		}
	}
}
void erase_block()
{
	int x, y;
	for (x = 0; x < WIDTH - 1; x++) {
		for (y = 0; y < HEIGHT; y++) {
			if (map[y][x] == 1) {
				gotoxy(x, y);
				printf(" ");
			}
			if (map[y][x] == 1 && item1[y][x] == 1)
				item1[y][x] = 0;
			if (map[y][x] == 1 && item2[y][x] == 1)
				item2[y][x] = 0;
		}
	}
	for (int i = 0; i <= rdom; i++) {
		map[block_y - i][block_x] = 0;
	}
}
void move_block(dx, dy)
{
	erase_block();
	block_x += dx;
	if (block_x == 6) {
		block_x = 84;
	}
	block_y += dy;
	init_block();
	show_block();
}
void main()
{
	unsigned char ch; //특수키 0xe0을 입력받으려면 unsigned char 로 선언해야 함
	int oldx, oldy, newx, newy;
	int start_time, time_score;
	int item = 0, ix = 0;
	char c;

RESTART:
	newx = oldx = 30;
	newy = oldy = HEIGHT - 1;
	life = 1; item = 0;
	score = 0; cnt = 0; Delay = 60;
	erase_block();
	for (int i = 0; i <= 4; i++) {
		map[block_y - i][block_x] = 0;
	}
	removeCursor();
	cls(WHITE, BLACK);
	while (1) {
		int c1, c2;
		do {
			c1 = rand() % 16;
			c2 = rand() % 16;
		} while (c1 == c2);
		gotoxy(37, 7);
		textcolor(c1, c2);
		printf("  장애물 넘기 게임  ");
		gotoxy(21, 10);
		textcolor(CYAN1, WHITE);
		printf(" 이동 : ↑  $ : 보너스 점수  & : 생명력 증가 ");
		gotoxy(29, 12);
		textcolor(BLACK, YELLOW2);
		printf(" START -> \"S\"       EXIT -> \"ESC\" \n");
		Sleep(300);
		if (_kbhit()) {
			ch = _getch();
			if (ch == 's')
				break;
			if (ch == ESC)
				exit(0);
		}
	}

	cls(WHITE, BLACK);
	draw_box(6, 5, 84, 21, "━");
	draw_box2(4, 6, 85, 20, "┃");
	draw_box(4, 5, 4, 5, "┏");
	draw_box(4, 21, 4, 21, "┗");
	draw_box(85, 5, 85, 5, "┓");
	draw_box(85, 21, 85, 21, "┛");
	textcolor(BLACK, WHITE);
	putstar(oldx, oldy);
	init_block();
	show_block();
	ch = 0; // 초기값 정지상태
	showscore();
	showlife(life);
	keep_moving = 0;
	start_time = time(NULL);

	while (1) {
		time_score = (time(NULL) - start_time) % 10;
		if (rand() % 100 == 10) {
			x1 = 83; y1 = 15; item = 1;
			show_item(item, x1, y1);
		}
		else if (rand() % 100 == 50) {
			x2 = 83; y2 = 15; item = 2;
			show_item(item, x2, y2);
		}
		if (_kbhit() == 1) {  // 키보드가 눌려져 있으면
			c = _getch(); // key 값을 읽는다
			if (c == '\0') continue;
			else ch = c;
			if (ch == ESC) break;

			if (ch == SPECIAL1 || ch == SPECIAL2) { // 만약 특수키
				//UP key의 경우 0xe0 0x48 두개의 문자가 들어온다.
				ch = _getch();
				switch (ch) {
				case UP:
					keep_moving = 1; break;
				}
			}
		}
		if (keep_moving && cnt % 2 == 0) { // 움직이고 있으면
			keep_moving = 1; //1:계속이동, 0:한번에 한칸씩이동
			player1(ch);
		}
		if (keep_moving == 0) ch = 0;
		
		move_item(-1, 0);
		move_block(-1, 0); // x 좌표를 -1씩 , --> 왼쪽으로 이동
		if (life == 0) break;
		Sleep(Delay); // Delay를 줄이면 속도가 빨라진다.
		if (cnt % 100 == 0) Delay -= 1;
		cnt++;
		score += time_score;
		showscore();
		showlife(life);
	}
	cls(WHITE, BLACK);
	while (1) {
		char RE = 0;
		int c1, c2;

		gotoxy(37, 10);
		do { // 색을 변경하면서 Game Over 출력
			c1 = rand() % 16;
			c2 = rand() % 16;
		} while (c1 == c2);
		textcolor(c1, c2);
		printf(" GAME OVER ");
		gotoxy(27, 12);
		textcolor(BLACK, YELLOW2);
		printf("  RESTART -> r       EXIT -> q  ");
		Sleep(250);
		if (_kbhit()) {
			RE = _getch();
			if (RE == 'q') {
				gotoxy(25, 14);
				return 0; //break;
			}
			else if (RE == 'r')
				goto RESTART;
		}
	}
}