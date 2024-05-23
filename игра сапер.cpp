#include <cmath>
#include <iostream>
#include <random>
#include"glut.h"
const int MINE = 9;
const int TILE_SIZE = 22;//pазмер одной плитки в пикселях.
const int MARGIN = 40;//oтступ от края окна до игрового поля
const int PADDING = 10;//oтступ между плитками
const int BOARD_SIZE = 11;
const int MINE_COUNT = 7;
enum Color {
    RED, DARKRED, BLUE, DARKBLUE, GREEN, DARKGREEN, CYAN, DARKCYAN, YELLOW, DARKYELLOW, WHITE, MAGENTA, BLACK, DARKGRAY, LIGHTGRAY, ULTRALIGHTGRAY
};
const struct
{
    float r, g, b;
}
colors[] =
{
    { 1, 0, 0 },// red
    { 0.5f, 0, 0 },// dark red
    { 0, 0, 1 }, // blue
    { 0, 0, 0.5f }, // dark blue
    { 0, 1, 0 }, // green
    { 0, 0.5f, 0 }, // dark green
    { 0, 1, 1 }, // cyan
    { 0, 0.5f, 0.5f }, // dark  cyan
    { 1, 1, 0 },//yellow
    { 0.5f, 0.5f, 0 },//dark yellow
    { 1, 1, 1 },// White
    { 1, 0, 1 }, // magenta
    { 0, 0, 0 }, // black
    { 0.25, 0.25, 0.25 }, // dark gray
    { 0.5, 0.5, 0.5 }, // light gray
    { 0.75, 0.75, 0.75 }, // ultra-light gray
};
struct cell
{
    int type;
    bool flag;
    bool open;
};
cell board[BOARD_SIZE * BOARD_SIZE];
int death;
int WIDTH;
int HEIGHT;
bool clicked;
int num_opened;
int rand_int(int low, int high)
{
    std::random_device rd;  // Стандартное устройство генерации случайных чисел
    std::mt19937 gen(rd()); // Инициализация генератора
    std::uniform_int_distribution<int> distrib(low, high); // Создание равномерного целочисленного распределения
    return distrib(gen); // Генерация случайного числа в диапазоне и возврат
}
//отрисовка клеток поля
void drawRect(int x, int y, float WIDTH, float HEIGHT, const Color& color = DARKGRAY, bool outline = true)
{
    glColor3f(colors[color].r, colors[color].g, colors[color].b);
    glBegin(outline ? GL_LINE_STRIP : GL_TRIANGLE_FAN);
    {
        glVertex2i(x + 0 * WIDTH, y + 0 * HEIGHT);
        glVertex2i(x + 1 * WIDTH, y + 0 * HEIGHT);
        glVertex2i(x + 1 * WIDTH, y + 1 * HEIGHT);
        glVertex2i(x + 0 * WIDTH, y + 1 * HEIGHT);
    }
    glEnd();
}

void drawCircle(int cx, int cy, float radius, const Color& color = WHITE, bool outline = true)
{
    glColor3f(colors[color].r, colors[color].g, colors[color].b);
    glBegin(outline ? GL_LINE_LOOP : GL_TRIANGLE_FAN);
    for (int i = 0; i <= 32; i++) {
        float angle = 2 * 3.14159 * i / 32.0f;
        float x = radius * cosf(angle);
        float y = radius * sinf(angle);
        glVertex2f(x + cx, y + cy);
    }
    glEnd();
}
//функция рисования флага
void drawFlag(int x, int y)
{
    glColor3f(colors[BLACK].r, colors[BLACK].g, colors[BLACK].b);
    x = (x * TILE_SIZE) + PADDING + 6;
    y = (y * TILE_SIZE) + PADDING + 3;
    glBegin(GL_POLYGON);
    {
        glVertex2i(x + 0, y + 2);
        glVertex2i(x + 9, y + 2);
        glVertex2i(x + 9, y + 3);
        glVertex2i(x + 7, y + 3);
        glVertex2i(x + 7, y + 4);
        glVertex2i(x + 3, y + 4);
        glVertex2i(x + 3, y + 3);
        glVertex2i(x + 0, y + 3);
    }
    glEnd();
    glBegin(GL_LINES);
    {
        glVertex2i(x + 5, y + 5);
        glVertex2i(x + 5, y + 8);
    }
    glEnd();
    glColor3f(colors[DARKRED].r, colors[DARKRED].g, colors[DARKRED].b);
    glBegin(GL_TRIANGLES);
    {
        glVertex2i(x + 6, y + 8);
        glVertex2i(x + 6, y + 13);
        glVertex2i(x + 0, y + 9);
    }
    glEnd();
}
//рисование мины
void drawMine(int x, int y, bool dead)
{
    if (dead)
    {
        drawRect(x * TILE_SIZE + PADDING, y * TILE_SIZE + PADDING, TILE_SIZE, TILE_SIZE, DARKGRAY, false);
    }
    x = (x * TILE_SIZE) + PADDING + 4;
    y = (y * TILE_SIZE) + PADDING + 4;
    glColor3f(colors[BLACK].r, colors[BLACK].g, colors[BLACK].b);
    glBegin(GL_LINES);
    {
        glVertex2i(x + 5, y - 1);
        glVertex2i(x + 5, y + 12);
        glVertex2i(x - 1, y + 5);
        glVertex2i(x + 12, y + 5);
        glVertex2i(x + 1, y + 1);
        glVertex2i(x + 10, y + 10);
        glVertex2i(x + 1, y + 10);
        glVertex2i(x + 10, y + 1);
    }
    glEnd();
    glBegin(GL_POLYGON);
    {
        glVertex2i(x + 3, y + 1);
        glVertex2i(x + 1, y + 4);
        glVertex2i(x + 1, y + 7);
        glVertex2i(x + 3, y + 10);
        glVertex2i(x + 8, y + 10);
        glVertex2i(x + 10, y + 7);
        glVertex2i(x + 10, y + 4);
        glVertex2i(x + 8, y + 1);
    }
    glEnd();
    drawRect(x + 3, y + 5, 2, 2, WHITE, false);
}
//рисование цифр в клеточках
void drawNum(int x, int y, int v)
{

    glColor3f(colors[v].r, colors[v].g, colors[v].b);
    glRasterPos2i((x + 0) * TILE_SIZE + PADDING + 6, (y + 0) * TILE_SIZE + PADDING + 5);
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, '0' + v);
}

//отрисовка рамки
void drawFrame(float x, float y, float WIDTH, float HEIGHT, bool doubleFrame = true)
{

    glColor3f(colors[ULTRALIGHTGRAY].r, colors[ULTRALIGHTGRAY].g, colors[ULTRALIGHTGRAY].b);
    glBegin(GL_LINE_LOOP);
    {
        glVertex2f((x + 0) + 0 * WIDTH, (y - 0) + 0 * HEIGHT);
        glVertex2f((x - 0) + 0 * WIDTH, (y - 1) + 1 * HEIGHT);
        glVertex2f((x - 1) + 1 * WIDTH, (y - 1) + 1 * HEIGHT);
        glVertex2f((x - 2) + 1 * WIDTH, (y - 2) + 1 * HEIGHT);
        glVertex2f((x + 1) + 0 * WIDTH, (y - 2) + 1 * HEIGHT);
        glVertex2f((x + 1) + 0 * WIDTH, (y + 1) + 0 * HEIGHT);
    }
    glEnd();

    glColor3f(colors[LIGHTGRAY].r, colors[LIGHTGRAY].g, colors[LIGHTGRAY].b);
    glBegin(GL_LINE_LOOP);
    {
        glVertex2f((x - 2) + 1 * WIDTH, (y - 2) + 1 * HEIGHT);
        glVertex2f((x - 2) + 1 * WIDTH, (y + 1) + 0 * HEIGHT);
        glVertex2f((x + 1) + 0 * WIDTH, (y + 1) + 0 * HEIGHT);
        glVertex2f((x - 0) + 0 * WIDTH, (y - 0) + 0 * HEIGHT);
        glVertex2f((x - 1) + 1 * WIDTH, (y - 0) + 0 * HEIGHT);
        glVertex2f((x - 1) + 1 * WIDTH, (y - 1) + 1 * HEIGHT);
    }
    glEnd();
    if (!doubleFrame) return;
    WIDTH = WIDTH - 2 * PADDING;
    HEIGHT = HEIGHT - 2 * PADDING;
    glBegin(GL_LINE_LOOP);
    {
        glVertex2f((x - 0 + PADDING) + 0 * WIDTH, (y + PADDING - 0) + 0 * HEIGHT);
        glVertex2f((x - 0 + PADDING) + 0 * WIDTH, (y + PADDING - 1) + 1 * HEIGHT);
        glVertex2f((x - 1 + PADDING) + 1 * WIDTH, (y + PADDING - 1) + 1 * HEIGHT);
        glVertex2f((x - 2 + PADDING) + 1 * WIDTH, (y + PADDING - 2) + 1 * HEIGHT);
        glVertex2f((x + 1 + PADDING) + 0 * WIDTH, (y + PADDING - 2) + 1 * HEIGHT);
        glVertex2f((x + 1 + PADDING) + 0 * WIDTH, (y + PADDING + 1) + 0 * HEIGHT);
    }
    glEnd();
    glColor3f(colors[WHITE].r, colors[WHITE].g, colors[WHITE].b);
    glBegin(GL_LINE_LOOP);
    {
        glVertex2i((x + PADDING - 2) + 1 * WIDTH, (y + PADDING - 2) + 1 * HEIGHT);
        glVertex2i((x + PADDING - 2) + 1 * WIDTH, (y + PADDING + 1) + 0 * HEIGHT);
        glVertex2i((x + PADDING + 1) + 0 * WIDTH, (y + PADDING + 1) + 0 * HEIGHT);
        glVertex2i((x + PADDING - 0) + 0 * WIDTH, (y + PADDING - 0) + 0 * HEIGHT);
        glVertex2i((x + PADDING - 1) + 1 * WIDTH, (y + PADDING - 0) + 0 * HEIGHT);
        glVertex2i((x + PADDING - 1) + 1 * WIDTH, (y + PADDING - 1) + 1 * HEIGHT);
    }
    glEnd();
}