#include <Windows.h>
#include <memory>
#include "IPlasmaGenerator.h"
#include "TestPlasma.h"

namespace
{
	const char *g_pWindowClassName = "PlasmaMainWindow";

	LRESULT PlasmaWindowFunc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	struct WindowState
	{
		static IPlasmaGenerator *m_generator;
	};

	IPlasmaGenerator* WindowState::m_generator;

	void RegisterWindowClass()
	{
		WNDCLASS cls;

		cls.style = CS_DROPSHADOW;
		cls.lpfnWndProc = (WNDPROC)PlasmaWindowFunc;
		cls.cbClsExtra = 0;
		cls.cbWndExtra = 0;
		cls.hInstance = ::GetModuleHandle(NULL);
		cls.hIcon = ::LoadIcon(NULL, IDI_APPLICATION);
		cls.hCursor = NULL;
		cls.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		cls.lpszMenuName = NULL;
		cls.lpszClassName = g_pWindowClassName;

		::RegisterClass(&cls);
	}

	void Draw(HWND hwnd, HDC hdc)
	{
		RECT rect;
		::GetClientRect(hwnd, &rect);

		auto width = rect.right - rect.left;
		auto height = rect.bottom - rect.top;

		auto time = GetTickCount();

		HDC bmpSrc = ::CreateCompatibleDC(hdc);
		HBITMAP bmp = ::CreateCompatibleBitmap(hdc, width, height);
		::SetBitmapBits(bmp, width * height * IPlasmaGenerator::BytesPerPixel, WindowState::m_generator->Generate(time));

		::SelectObject(bmpSrc, bmp);
		::BitBlt(hdc, 0, 0, width, height, bmpSrc, 0, 0, SRCCOPY);
		::DeleteDC(bmpSrc);

		char buffer[128];
		sprintf_s(buffer, sizeof(buffer), "phase: %x", ((unsigned char)(time >> 6)));

		::TextOut(hdc, 10, 10, buffer, strlen(buffer));
	}
		
	LRESULT PlasmaWindowFunc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		PAINTSTRUCT ps;
		HDC hdc;

		switch (message)
		{
		case WM_PAINT:
			hdc = ::BeginPaint(hWnd, &ps);
			Draw(hWnd, hdc);
			::EndPaint(hWnd, &ps);
			break;
		case WM_CLOSE:
			::DestroyWindow(hWnd);
			break;
		case WM_DESTROY:
			::PostQuitMessage(0);
			break;
		case WM_SIZE:
		{
			::InvalidateRect(hWnd, NULL, FALSE);

			RECT rect;
			::GetClientRect(hWnd, &rect);

			auto width = rect.right - rect.left;
			auto height = rect.bottom - rect.top;

			WindowState::m_generator->SetSize(width, height);

			break;
		}
		default:
			return ::DefWindowProc(hWnd, message, wParam, lParam);
		}

		return 0;
	}

	DWORD Timer(LPVOID window)
	{
		HWND hwnd = (HWND)window;

		while (true)
		{
			::Sleep(100);
			::InvalidateRect(hwnd, NULL, FALSE);
		}

		return 0;
	}
}

int main()
{
	WindowState::m_generator = new TestPlasma();

	RegisterWindowClass();

	HWND hwnd = ::CreateWindow(g_pWindowClassName, "plasma", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 200, 200, NULL, NULL, ::GetModuleHandle(NULL), NULL);
	::ShowWindow(hwnd, SW_SHOW);
	::UpdateWindow(hwnd);

	::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&Timer, (LPVOID)hwnd, 0, NULL);

	MSG msg;

	::InvalidateRect(hwnd, NULL, FALSE);

	while (::GetMessage(&msg, NULL, 0, 0) > 0)
	{
		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
	}

	return 0;
}
