#include <cmath>
#include <iostream>
#include <random>
#include"glut.h"
const int MINE = 9;
const int TILE_SIZE = 22;//pазмер одной плитки в пикселях.
const int MARGIN = 40;//oтступ от края окна до игрового поля
const int PADDING = 10;//oтступ между плитками
const int BOARD_SIZE = 11;
const int MINE_COUNT = 25;
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
//закрытая ячейка
void drawClosedDim(int x, int y)
{
    drawFrame(x * TILE_SIZE + PADDING, y * TILE_SIZE + PADDING, TILE_SIZE, TILE_SIZE, false);
}
//Рисует открытую ячейку без числа или символа
void drawOpenDim(int x, int y)
{
    drawRect(x * TILE_SIZE + PADDING, y * TILE_SIZE + PADDING, TILE_SIZE, TILE_SIZE);
}
//Рисует верхнюю рамку игрового поля
void drawUpperFrame(int x = 0, int y = 0)
{
    const float upper_frame_outter_WIDTH = WIDTH;
    const float upper_frame_outter_HEIGHT = 2 * MARGIN;
    const float offset = HEIGHT - upper_frame_outter_HEIGHT;

    drawFrame(0, offset, upper_frame_outter_WIDTH, upper_frame_outter_HEIGHT);
}
//Рисует нижнюю рамку игрового поля
void drawLowerFrame(int x = 0, int y = 0)
{
    const float lower_frame_outter_size = WIDTH;
    drawFrame(0, 0, lower_frame_outter_size, lower_frame_outter_size);
}
//Рисует смайлик над полем
void drawIcon(int x = 0, int y = 0)
{
    const float icon_size = 2 * TILE_SIZE;
    if (clicked)
    {
        int x = 0, y = 0;
        const float cx = (WIDTH - icon_size) / 2.0f;
        const float cy = (HEIGHT - MARGIN) - icon_size / 2.0f;
        drawRect(cx, cy, 2 * TILE_SIZE, 2 * TILE_SIZE, ULTRALIGHTGRAY, false);
    }
    drawFrame((WIDTH - icon_size) / 2.0f, (HEIGHT - MARGIN) - icon_size / 2.0f, icon_size, icon_size, false);

    const float cx = WIDTH / 2.0f;
    const float cy = (HEIGHT - MARGIN);

    // face
    drawCircle(x + cx, y + cy, TILE_SIZE * 0.707f, GREEN, false);
    drawCircle(x + cx, y + cy, TILE_SIZE * 0.707f, BLACK);

    // eyes
    glBegin(GL_POINTS);
    glVertex2f(-4.707 + cx, 1.707 + cy);
    glVertex2f(4.707 + cx, 1.707 + cy);
    glEnd();

    // mouth
    glBegin(GL_LINES);
    {
        glVertex2f(-3.707 + cx, -8.707 + cy);
        glVertex2f(3.707 + cx, -8.707 + cy);
    }
    glEnd();
}
int index(int x, int y)
{
    return x + (y * BOARD_SIZE);
}

bool isOpen(int x, int y)
{
    return board[index(x, y)].open;
}

//Возвращает тип ячейки с координатами (x, y) (пустая, мина и т. д.).
int getType(int x, int y)
{
    return board[index(x, y)].type;
}
//Устанавливает тип ячейки с координатами (x, y) в значение v
void setType(int x, int y, int v)
{
    board[index(x, y)].type = v;
}

bool isMine(int x, int y)
{
    if (x < 0 || y < 0 || x > BOARD_SIZE - 1 || y > BOARD_SIZE - 1)
        return false;

    if (getType(x, y) == MINE)
        return true;
    return false;
}

int calcMine(int x, int y)
{
    return isMine(x - 1, y - 1)
        + isMine(x, y - 1)
        + isMine(x + 1, y - 1)
        + isMine(x - 1, y)
        + isMine(x + 1, y)
        + isMine(x - 1, y + 1)
        + isMine(x, y + 1)
        + isMine(x + 1, y + 1);
}
bool isFlag(int x, int y)
{
    return board[index(x, y)].flag;
}

bool gameOver()
{
    return death != -1;
}
//Проверяет, является ли ячейка с координатами (x, y) той ячейкой, которая привела к поражению
bool isDead(int x, int y)
{
    return death == index(x, y);
}
//Проверяет, выиграл ли игрок
bool hasWon()
{
    return num_opened == MINE_COUNT;
}

void openMines(bool open = true)
{
    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++) {
            if (isMine(x, y))
                board[index(x, y)].open = open;
        }
    }
}

void openCell(int x, int y)
{
    if (x < 0 || y < 0 || y > BOARD_SIZE - 1 || x > BOARD_SIZE - 1)
        return;
    if (isOpen(x, y))
        return;
    num_opened--;
    board[index(x, y)].open = true;
    if (isMine(x, y))
    {
        death = index(x, y);
        openMines();
        return;
    }
    //пустая ячейка
    if (getType(x, y) == 0)
    {
        openCell(x - 1, y + 1);
        openCell(x, y + 1);
        openCell(x + 1, y + 1);
        openCell(x - 1, y);
        openCell(x + 1, y);
        openCell(x - 1, y - 1);
        openCell(x, y - 1);
        openCell(x + 1, y - 1);
    }
}

void toggleFlag(int x, int y)
{
    board[index(x, y)].flag = !isFlag(x, y);
}
//Рисует открытую ячейку с количеством соседних мин n. 
// Если игрок проиграл, то ячейка, приведшая к поражению, рисуется особой.
void drawOpen(int x, int y, int n, bool dead)
{
    switch (n) {
    case 0:
        drawOpenDim(x, y);
        break;
    case 9:
        if (!dead) {
            drawOpenDim(x, y);
        }
        drawMine(x, y, dead);
        break;
    default:
        drawOpenDim(x, y);
        drawNum(x, y, n);
    }
}
//Рисует закрытую ячейку.
void drawClosed(int x, int y)
{
    drawClosedDim(x, y);
    if (isFlag(x, y))
        drawFlag(x, y);
}

void draw()
{
    for (int y = 0; y < BOARD_SIZE; y++)
    {
        for (int x = 0; x < BOARD_SIZE; x++)
        {
            if (isOpen(x, y))
                drawOpen(x, y, getType(x, y), isDead(x, y));
            else
                drawClosed(x, y);
        }
    }
}
//Проверяет, нажал ли игрок на кнопку перезапуска
bool requestRestart(int x, int y)
{
    return (x >= 5 && x <= 7 && y >= 12 && y <= 14);
}

void init()
{
    for (int i = 0; i < BOARD_SIZE * BOARD_SIZE; i++) {
        board[i].type = 0;
        board[i].flag = false;
        board[i].open = false;
    }
    for (int i = 0; i < MINE_COUNT; i++)
    {
        bool tmp = true;
        do
        {
            int x = rand_int(0, BOARD_SIZE - 1);
            int y = rand_int(0, BOARD_SIZE - 1);
            if (!isMine(x, y))
            {
                tmp = false;
                setType(x, y, MINE);
            }
        } while (tmp);
    }
    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++) {
            if (!isMine(x, y)) {
                setType(x, y, calcMine(x, y));
            }
        }
    }
    death = -1;
    clicked = true;
    num_opened = BOARD_SIZE * BOARD_SIZE;
    glClearColor(0.8f, 0.8f, 0.8f, 1.f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, WIDTH, 0, HEIGHT, -1.f, 1.f);
    glPointSize(5.0);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glEnable(GL_POINT_SMOOTH);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

// базовая функция для экрана используя глут
void display()
{
    glClear(GL_COLOR_BUFFER_BIT);
    drawLowerFrame();
    drawUpperFrame();
    drawIcon();
    draw();

    glutSwapBuffers();
}
void key(unsigned char key, int x, int y)
{
    switch (key) {
    case 27: exit(0); break;
    }
    //glutPostRedisplay();
}
void mouse(int b, int s, int x, int y)
{
    x = (x + PADDING) / TILE_SIZE - 1;
    y = (HEIGHT - y + PADDING) / TILE_SIZE - 1;

    switch (b)
    {
    case GLUT_LEFT_BUTTON:
        if (s == GLUT_DOWN)
        {
            if (requestRestart(x, y))
            {
                init();
            }
            else if (!gameOver() && !hasWon()) {
                openCell(x, y);
            }
        }
        break;
    case GLUT_RIGHT_BUTTON:
        if (s == GLUT_DOWN)
        {
            if (gameOver() || hasWon()) {
                break;
            }
            toggleFlag(x, y);
        }
        break;
    }

}

int main(int argc, char** argv)
{
    WIDTH = (BOARD_SIZE * TILE_SIZE + 2 * PADDING);
    HEIGHT = (BOARD_SIZE * TILE_SIZE + 2 * PADDING + 2 * MARGIN);
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_MULTISAMPLE);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutInitWindowPosition((glutGet(GLUT_SCREEN_WIDTH) - WIDTH) / 2, (glutGet(GLUT_SCREEN_HEIGHT) - HEIGHT) / 2);
    glutCreateWindow("MiNeSwEePeR");
    glutIdleFunc(display);
    glutDisplayFunc(display);
    glutKeyboardFunc(key);
    glutMouseFunc(mouse);
    init();
    glutMainLoop();
    return 0;
}