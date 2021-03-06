#include "appsystem.h"
#include "util.h"
#include "resource.h"

AppSystem::AppSystem() { };

AppSystem::~AppSystem() { };

bool AppSystem::Init()
{
	m_AppWidth = SYSTEM_VIDEO_WIDTH;
	m_AppHeight = SYSTEM_VIDEO_HEIGHT;

	// initialize the app window
	InitializeWindows(m_AppWidth, m_AppHeight);

	// initialize Direct3D11 rendering core
	m_RenderCore = new RenderCore();
	if (!m_RenderCore->Init(m_AppWidth, m_AppHeight, m_hWnd)) {
		DebugOut("Fatal Error: Render Core initialization failed!\n");
		return false;
	}

	// init COM
	HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

	return true;
};

void AppSystem::Shutdown()
{
	m_RenderCore->Shutdown();
	delete m_RenderCore;
	m_RenderCore = nullptr;
	
	CoUninitialize();
}; 

void AppSystem::Refresh()
{
	MSG msg;
	bool done = false;

	ZeroMemory(&msg, sizeof(MSG));

	while (!done) {
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (msg.message == WM_QUIT) {
			done = true;
		}
		else {
			if (!Frame()) {
				done = true;
			}
		}
	}	
};

bool AppSystem::Frame()
{
	if (m_RenderCore) {
		m_RenderCore->Render();		
	}
	else {
		DebugOut("Rendercore failed abruptly!\n");
		return false;
	}
	return true;
}

LRESULT CALLBACK AppSystem::MessageHandler(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
		case WM_KEYDOWN:
		{
			// placeholder for input object keydown handler
			return 0;
		}
		case WM_KEYUP:
		{
			// placeholder for input object keyup handler
			return 0;
		}

		default:
		{
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
		}
	}
};

void AppSystem::InitializeWindows(int screenWidth, int screenHeight)
{
	WNDCLASSEX wc;
	//DEVMODE dmScreenSettings;
	
	int posX = 0;
	int posY = 0;

	g_ApplicationHandle = this;

	// instance of this application
	m_hInstance = GetModuleHandle(nullptr);
	// name of this application
	m_AppName = SYSTEM_APP_NAME;

	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hInstance;
	wc.hIcon = LoadIcon(nullptr, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);	
	//wc.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = m_AppName;
	wc.cbSize = sizeof(WNDCLASSEX);
		
	RegisterClassEx(&wc);
	
	posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
	posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;

	m_hWnd = CreateWindowEx(	WS_EX_APPWINDOW, m_AppName, m_AppName, 
								WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP, 
								posX, posY, screenWidth, screenHeight, 
								nullptr, nullptr, m_hInstance, nullptr);

	ShowWindow(m_hWnd, SW_SHOW);
	SetForegroundWindow(m_hWnd);
	SetFocus(m_hWnd);
	ShowCursor(true);
	
	HMENU mainMenu = CreateMenu();

	DebugOut("Application %ls started\n", m_AppName);
}; 

void AppSystem::ShutdownWindows()
{
	DestroyWindow(m_hWnd);
	m_hWnd = nullptr;

	UnregisterClass(m_AppName, m_hInstance);
	m_hInstance = nullptr;
	g_ApplicationHandle = nullptr;	
};

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}

		case WM_CLOSE:
		{
			PostQuitMessage(0);
			return 0;
		}

		default:
		{
			return g_ApplicationHandle->MessageHandler(hwnd, uMsg, wParam, lParam);
		}
	}
};