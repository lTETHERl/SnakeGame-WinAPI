#include "headers.h"

using namespace std;

//Глобальные переменные
HWND hMainWindow;

HBRUSH brush = CreateSolidBrush(RGB(0, 0, 0));
HBRUSH snake_brush = CreateSolidBrush(RGB(0, 255, 0));
HBRUSH snake_head_brush = CreateSolidBrush(RGB(0, 128, 0));
HBRUSH apple_brush = CreateSolidBrush(RGB(255, 0, 0));

HDC hdc;
vector<vector<RECT>> allRects(10, vector<RECT>(10));

vector<vector<int>> snake{ {5,5} };

int apple_x = 1 + rand() % 10;
int apple_y = 1 + rand() % 10;

enum directions { left = 'l', right = 'r', down = 'd', up = 'u' };
char direction = left;

int score;

//Функция перересовки экрана
void RedrawWindow(HWND hwnd) {
	RECT rect;
	GetClientRect(hwnd, &rect);
	InvalidateRect(hwnd, &rect, TRUE);
}

//Функиця отрисовки змеи
void DrawSnake()
{
	for (int i = 0; i < snake.size(); ++i)
	{
		if (i == 0)
		{
			FillRect(hdc, &allRects[snake[i][0]][snake[i][1]], snake_head_brush);
		}
		else
		{
			FillRect(hdc, &allRects[snake[i][0]][snake[i][1]], snake_brush);
		}
	}
}

//Функция перемещения змеи
void MoveSnake(char direction)
{
	for (auto& i : snake)
	{
		FillRect(hdc, &allRects[i[0]][i[1]], brush);
	}

	vector<vector<int>> temp = snake;


	for (int i = 1; i < snake.size(); ++i)
	{
		temp[i] = snake[i - 1];
	}

	snake = temp;
	switch (direction)
	{
	case left:
		--snake[0][0];
		break;

	case right:
		++snake[0][0];
		break;

	case up:
		--snake[0][1];
		break;

	case down:
		++snake[0][1];
		break;
	}
}

//Функиця увеличения змеи
void IncreaseSnake()
{
	if (snake.size() == 1)
	{
		switch (direction)
		{
		case left:
			snake.push_back({ snake[0][0] + 1, snake[0][1] });
			break;

		case right:
			snake.push_back({ snake[0][0] - 1, snake[0][1] });
			break;

		case up:
			snake.push_back({ snake[0][0], snake[0][1] + 1 });
			break;

		case down:
			snake.push_back({ snake[0][0], snake[0][1] - 1 });
			break;
		}
	}
	else
	{
		//вертикальный хвост
		if (snake[snake.size() - 1][0] - snake[snake.size() - 2][0] == 0) 
		{
			if (snake[snake.size() - 1][1] - snake[snake.size() - 2][1] > 0) {
				snake.push_back({ snake[snake.size() - 1][0], max(0, snake[snake.size() - 1][1] + 1) });
			}
			else {
				snake.push_back({ snake[snake.size() - 1][0], min(9, snake[snake.size() - 1][1] - 1) });
			}
		}

		//горизонтальный хвост
		else 
		{
			if (snake[snake.size() - 1][0] - snake[snake.size() - 2][0] > 0) {
				snake.push_back({ max(0, snake[snake.size() - 1][0] + 1), snake[snake.size() - 1][1] });
			}
			else {
				snake.push_back({ min(9, snake[snake.size() - 1][0] - 1), snake[snake.size() - 1][1] });
			}
		}
	}
}

//Функция проверки
void Check()
{
	if (snake[0][0] < 0 || snake[0][0] > 9 || snake[0][1] < 0 || snake[0][1] > 9 || (count(snake.begin(), snake.end(), snake[0]) > 1))
	{

		vector<vector<int>> temp{ {5,5} };
		snake = temp;

		apple_x = 1 + rand() % 10;
		apple_y = 1 + rand() % 10;

		direction = left;
		SetTimer(hMainWindow, 1, 500, nullptr);
		score = 0;
	}

	if (snake[0][0] == apple_x && snake[0][1] == apple_y)
	{
		FillRect(hdc, &allRects[apple_x][apple_y], brush);
		apple_x = 0 + rand() % 9;
		apple_y = 0 + rand() % 9;
		IncreaseSnake();
		++score;
		SetTimer(hMainWindow, 1, 500 - score*20, nullptr);

	}
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {

	case WM_KEYDOWN: {
		int key = wParam;


		switch (key) {

		//Проверка нажатия стрелочки влево
		case VK_LEFT:
		{
			if (direction != right)
				direction = left;

			return 0;
		}

		//Проверка нажатия стрелочки вправо
		case VK_RIGHT:
		{
			if (direction != left)
				direction = right;

			return 0;
		}

		//Проверка нажатия стрелочки вверх
		case VK_UP:
		{
			if (direction != down)
				direction = up;

			return 0;
		}

		//Проверка нажатия стрелочки вниз
		case VK_DOWN:
		{
			if (direction != up)
				direction = down;

			return 0;
		}
		}
	}

	case WM_TIMER:
	{
		MoveSnake(direction);
		Check();
		RedrawWindow(hMainWindow);
		return 0;
	}

	case WM_PAINT: {
		PAINTSTRUCT ps;
		hdc = BeginPaint(hwnd, &ps);

		HPEN hPen = CreatePen(PS_SOLID, 2, RGB(255, 255, 255));
		SelectObject(hdc, hPen);

		FillRect(hdc, &allRects[apple_x][apple_y], apple_brush);

		DrawSnake();

		for (int i = 0; i <= 10; ++i)
		{
			MoveToEx(hdc, 0, i * 50, NULL);
			LineTo(hdc, 500, i * 50);

			MoveToEx(hdc, i * 50, 0, NULL);
			LineTo(hdc, i * 50, 500);
		}

		DeleteObject(hPen);
		EndPaint(hwnd, &ps);
		return 0;
	}
	case WM_CLOSE: {
		PostQuitMessage(0);
		return 0;
	}
	default: {
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	}
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	srand(static_cast<unsigned int>(time(nullptr)));

	for (int i = 0; i < 10; ++i)
	{
		for (int j = 0; j < 10; ++j)
		{
			allRects[i][j] = RECT{ i * 50, j * 50, i * 50 + 50, j * 50 + 50 };
		}
	}
	
	WNDCLASS wc = {};
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = GetModuleHandle(nullptr);
	wc.lpszClassName = L"Snake";
	wc.hbrBackground = reinterpret_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);

	RegisterClass(&wc);
	hMainWindow = CreateWindowExW(
		0, L"Snake", L"Snake",
		WS_OVERLAPPEDWINDOW | WS_THICKFRAME, CW_USEDEFAULT, CW_USEDEFAULT, 520, 545,
		nullptr, nullptr, GetModuleHandle(nullptr), nullptr
	);

	SetTimer(hMainWindow, 1, 500, nullptr);

	ShowWindow(hMainWindow, nCmdShow);

	MSG msg;
	while (GetMessage(&msg, nullptr, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return static_cast<int>(msg.wParam);
}
