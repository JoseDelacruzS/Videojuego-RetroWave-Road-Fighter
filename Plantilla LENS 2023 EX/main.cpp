#include <windows.h>
#include <cmath>
#include <ctime>
#include <cstdlib> 
#include <iostream> //Libreria para entrada y salida de datos
#include <fstream> //Libreria para manejar archivos
#include <string> //Libreria para utilizar tipos de datos string
#include "Librerias/Dibujar bitmaps/gdipload.h"
#include "Librerias/Musica/libzplay.h"
#include "Librerias/Gamepad/GamePadRR.h"
#include <memory.h>
#pragma comment(lib, "XInput.lib")

GamePadRR* Gpad;

using namespace libZPlay;//Se declara que se usara la libreria de libZPlay para reproductor de musica
using namespace std; //Utiliza el espacio de nombres de la libreria STD de c++

ZPlay* playerEfecto = CreateZPlay();

struct Musica
{
	string Dir;
	string Nombre;
};
Musica Cancion[3];

struct FrameArray{
	int x,y;
	int ancho,alto;
};//Similar a la estructura rect de winapi

struct Input
{
	enum Keys
	{
		Backspace = 0x08, Tab,
		Clear = 0x0C, Enter,
		Shift = 0x10, Control, Alt,
		Escape = 0x1B,
		Space = 0x20, PageUp, PageDown, End, Home, Left, Up, Right, Down,
		Zero = 0x30, One, Two, Three, Four, Five, Six, Seven, Eight, Nine,
		A = 0x41, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
		NumPad0 = 0x60, NumPad1, NumPad2, NumPad3, NumPad4, NumPad5, NumPad6, NumPad7, NumPad8, NumPad9,
		F1 = 0x70, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12
	};
}input;
bool KEYS[256];

enum Animaciones
{
	Idle,
	inmune,
	Running_Left,
	Jump,
	Dash,
};
enum Stages
{
	Inicio,
	FirstStage,
	GameOver
};
enum Frames {
	Frame0,
	Frame1,
	Frame2,
	Frame3,
	Frame4,
	Frame5,
	Frame6,
	Frame7,
	Frame8,
	Frame9,
	Frame10,
	Frame11,
	Frame12,
	Frame13,
	Frame14,
	Frame15,
	Frame16,
	Frame17,
	Frame18,
	Frame19,
	Frame20
};
struct DatosPersonaje{
	//Ruta donde esta nuestra imagen
	WCHAR BmpW[MAX_PATH] = L"Recursos/Principal.bmp";
	WCHAR BmpW2[MAX_PATH] = L"Recursos/Carros2.bmp";
	WCHAR BmpW3[MAX_PATH] = L"Recursos/Carros3.bmp";
	WCHAR BmpW4[MAX_PATH] = L"Recursos/Carros4y5.bmp";
	WCHAR BmpW5[MAX_PATH] = L"Recursos/Carros4y5.bmp";
	WCHAR BmpW6[MAX_PATH] = L"Recursos/Carros6.bmp";
	WCHAR BmpW7[MAX_PATH] = L"Recursos/Carros7.bmp";
	WCHAR BmpW8[MAX_PATH] = L"Recursos/Carros8.bmp";
	miBitmap AutoSprite,AutoSprite2, AutoSprite3, AutoSprite4, AutoSprite5, AutoSprite6, AutoSprite7, AutoSprite8;

	//Posicion relativa de nuestro Sprite en la ventana
	int XCurrentCoordDraw;
	int YCurrentCoordDraw;

	//Dimensiones en pixeles que se requieren para dibujarlo en la ventana
	int HeightDraw;
	int WeightDraw;

	int Animaciones = 4;
	int idAnimacionIdle = 0;
	int FramesAnimacionIdle = 5;

	int idAnimacionInmune = 1;
	int FramesAnimacionInmune = 4;

	int idAnimacionSaltando = 2;
	int FramesAnimacionSaltando = 0;

	int idAnimacionGameOver = 3;
	int FramesAnimacionGameOver = 0;

	int HitboxAncho;
	int HitboxAlto;
	int HitboxX = XCurrentCoordDraw;
	int HitboxY = YCurrentCoordDraw;
	int HitboxScaleX;
	int HitboxScaleY;

	FrameArray** FrameSpriteArray;
}miPersonaje,Auto2, Auto3, Auto4, Auto5, Auto6, Auto7, Auto8;
struct DatosStage{
	int Escenarios = 4;
	miBitmap ImagenEscenario1;
	WCHAR Bmp1[MAX_PATH] = L"Recursos/zeros12.bmp";
	miBitmap ImagenEscenario2;
	WCHAR Bmp2[MAX_PATH] = L"Recursos/Inicio.bmp";
	miBitmap ImagenEscenario3;
	WCHAR Bmp3[MAX_PATH] = L"Recursos/GameOver.bmp";
	miBitmap ImagenEscenario4;
	WCHAR Bmp4[MAX_PATH] = L"";
}miStage;

struct DatosGas {
	//Ruta donde esta nuestra imagen
	WCHAR BmpW[MAX_PATH] = L"Recursos/Gas.bmp";
	miBitmap GasSprite;

	//Posicion relativa de nuestro Sprite en la ventana
	int XCurrentCoordDraw;
	int YCurrentCoordDraw;

	//Dimensiones en pixeles que se requieren para dibujarlo en la ventana
	int HeightDraw;
	int WeightDraw;

	int Animaciones = 4;
	int idAnimacionIdle = 0;
	int FramesAnimacionIdle = 5;

	int idAnimacionCaminando = 1;
	int FramesAnimacionCaminando = 0;

	int idAnimacionSaltando = 2;
	int FramesAnimacionSaltando = 0;

	int idAnimacionGameOver = 3;
	int FramesAnimacionGameOver = 0;

	int HitboxAncho;
	int HitboxAlto;
	int HitboxX = XCurrentCoordDraw;
	int HitboxY = YCurrentCoordDraw;
	int HitboxScaleX;
	int HitboxScaleY;

	FrameArray** FrameSpriteArray;
}Gas;

//PUNTUACIONES
struct Puntuaciones
{
	int puntajesArr=0;
	char nombresArr[3] = { 'A','A', 'A' };

}punts[3];
int cont;

//registro nombre
bool RegName = false;
bool Pletra = true;
bool Sletra = false;
bool Tletra = false;

wchar_t LetraChar[1];
wchar_t PLetraAux[1];
wchar_t SLetraAux[1];
wchar_t TLetraAux[1];
wchar_t Auxnombre[3];
enum puntuacionNombre
{
	A = L'A', B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z
};
int Letraint = A;

void guardararchivo(int newRecord);
void Leerarchivo();
bool compararPuntos(int newRecord);

void resetMemory();

//Mostrar puntaje
long long puntos = 0;
void dibujar_puntos(HDC hdc) {
	// Configurar el color del texto y el tamaño de fuente
	COLORREF textColor = RGB(255, 255, 255); // Blanco
	COLORREF bgColor = RGB(0, 0, 0); // Negro
	int fontSize = 30;
	HFONT font = CreateFont(fontSize, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH | FF_DONTCARE, TEXT("Fixedsys"));
	SelectObject(hdc, font);
	SetTextColor(hdc, textColor);
	SetBkColor(hdc, bgColor);
	SetBkMode(hdc, TRANSPARENT);

	// Dibujar los puntos
	wstring puntoss = to_wstring(puntos);
	TextOutW(hdc, 720, 115, puntoss.c_str(), puntoss.size());

	// Liberar recursos
	DeleteObject(font);
}


//Variables Globales
const int ANCHO_VENTANA = 870;
const int ALTO_VENTANA = 800;
const int BPP = 4;
const int TICK = 99;//99
const unsigned int TRANSPARENCY = 0xFF00FF00;
ZPlay* player = CreateZPlay();//Generamos un objeto puntero para nuestro reproductor
TStreamStatus status;
bool pausa = false;
int* ptrBufferPixelsWindow;
int AnimacionActual;
int FrameActualJugador = 0;
int FrameActual = 0;
int FrameActualA2 = 0;
int FrameActualA3 = 0;
int FrameActualA4 = 0;
int FrameActualGas = 0;
int DelayFrameAnimation=0;

//Pantallas
bool pantallaInicial = true;
bool pantallaGameOver = false;

int posicion_y = 44000;
int autoY=0;
int YInicialAuto = -100;

//Variables para las vidas
int vida = 3;
void dibujar_vidas(HDC hdc) {
	wstring vidas = to_wstring(vida);
	TextOutW(hdc, 730, 280, vidas.c_str(), vidas.size());
}

//Vriables de colision
bool colision = false;
clock_t inicioTiempo = 0;

//Definicion de funciones
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
void LimpiarFondo(int *ptrBuffer, unsigned int color, int area);
void TranScaleblt(int* punteroDestino, int* punteroOrigen, int inicioXDestino, int inicioYDestino, int inicioXOrigen, int inicioYOrigen, int ancho, int alto, int anchodefondo, int anchodeorigen, int escalaX, int escalaY);
void DibujaHitbox(int* ptrBuffer, unsigned int color, int anchoWnd, int altoWnd, int HitboxX, int HitboxY, int HitboxAncho, int HitboxAlto, int escalaX, int escalaY);
bool DetectarColision(int x1, int y1, int ancho1, int alto1, int x2, int y2, int ancho2, int alto2);
void MainRender(HWND hWnd);
void Init();
void KeysEvents(HWND hWnd);

//Reproductor
void ReproductorPausa();
void ReproductorReproduce();
void ReproductorInicializaYReproduce();
void ReproductorCambiarCancionYReproduce(int);
void ReproductorEfecto(string dir);

//Sprites
void CargaFramesSprite();
void CargaFramesSprite2();
void CargaFramesSprite3();
void CargaFramesSprite4();
void CargaFramesSprite5();
void CargaFramesSprite6();
void CargaFramesSprite7();
void CargaFramesSprite8();
void CargaFramesGas();

//Dibujar
void DibujarAuto2();
void DibujarAuto3();
void DibujarAuto4();
void DibujarAuto5();
void DibujarAuto6();
void DibujarAuto7();
void DibujarAuto8();
//Dibujar Gas
void DibujarGas();

int WINAPI wWinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,PWSTR pCmdLine,int nCmdShow)
{
	WNDCLASSEX wc;									// Windows Class Structure
	HWND hWnd;
	MSG msg;

	TCHAR szAppName[] = TEXT("MyWinAPIApp");		
	TCHAR szAppTitle[] = TEXT("Retro wave road fighter");

	hInstance = GetModuleHandle(NULL);				// Grab An Instance For Our Window

	wc.cbSize			= sizeof(WNDCLASSEX);
	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// Redraw On Size, And Own DC For Window.
	wc.lpfnWndProc		= (WNDPROC) WndProc;					// WndProc Handles Messages
	wc.cbClsExtra		= 0;									// No Extra Window Data
	wc.cbWndExtra		= 0;									// No Extra Window Data
	wc.hInstance		= hInstance;							// Set The Instance	
	wc.hIcon			= LoadIcon(NULL, IDI_WINLOGO);			// Load The Default Icon
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);			// Load The Arrow Pointer
	wc.hbrBackground	= (HBRUSH) (COLOR_WINDOW + 1);			// No Background Required For GL
	wc.lpszMenuName		= NULL;									// We Don't Want A Menu	
	wc.lpszClassName	= szAppName;							// Set The Class Name
	wc.hIconSm			= LoadIcon(NULL, IDI_APPLICATION);
	
	if (!RegisterClassEx(&wc))									// Attempt To Register The Window Class
	{
		MessageBox(NULL,
			"Fallo al registrar clase (Failed To Register The Window Class).",
			"ERROR",
			MB_OK|MB_ICONEXCLAMATION);
		return 0;
	}

	hWnd = CreateWindowEx(	
		WS_EX_CLIENTEDGE | WS_EX_APPWINDOW | WS_EX_WINDOWEDGE,	// Extended Style For The Window
		szAppName,							// Class Name
		szAppTitle,							// Window Title
		WS_OVERLAPPEDWINDOW |				// Defined Window Style
		WS_CLIPSIBLINGS |					// Required Window Style
		WS_CLIPCHILDREN,					// Required Window Style
		0, 0,								// Window Position
		ANCHO_VENTANA,						// Calculate Window Width
		ALTO_VENTANA,						// Calculate Window Height
		NULL,								// No Parent Window
		NULL,								// No Menu
		hInstance,							// Instance
		NULL);								// Pass this class To WM_CREATE								

	if(hWnd == NULL) {
		MessageBox(NULL, 
			"Error al crear ventana (Window Creation Error).", 
			"ERROR", 
			MB_OK|MB_ICONEXCLAMATION);
		return 0;
	}
		
	SetForegroundWindow(hWnd);						// Slightly Higher Priority
	Init();
	ShowWindow(hWnd, nCmdShow);
	SetFocus(hWnd);

	SetTimer(hWnd, TICK, TICK, NULL);
	ZeroMemory(&msg, sizeof(MSG));
	Gpad = new GamePadRR(1);

	while(GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return(int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)									// Check For Windows Messages
	{
		case WM_CREATE:
		Leerarchivo();
		break;
		case WM_KEYDOWN:
		{
			KEYS[wParam] = true;
		}
			break;
		case WM_KEYUP:
		{
			KEYS[wParam] = false;
			switch (wParam) {
				case VK_F1://F1
				{
					ReproductorPausa();
				}
				return 0;
				case VK_F2://F1
				{
					ReproductorPausa();
				}
				return 0;
			}
		}
			break;
		case WM_TIMER:
			if(wParam == TICK)
			{
				MainRender(hWnd);
				player->GetStatus(&status);
				if (!pausa && status.fPlay == 0){
					ReproductorReproduce();
				}
				if (!pantallaInicial&&!pantallaGameOver) {
					//puntos++;

				}
				if (pantallaInicial) {
					puntos = 0;
				}
			}
			break;
		case WM_PAINT:
			{
				HDC hdc; 
				PAINTSTRUCT ps;
				hdc = BeginPaint(hWnd, &ps);
				HBITMAP h_CMC = CreateBitmap(ANCHO_VENTANA, ALTO_VENTANA, 1, 32, ptrBufferPixelsWindow);
				HDC hdcMem = CreateCompatibleDC(hdc);
				HBITMAP hbmOld = (HBITMAP) SelectObject(hdcMem, h_CMC);//Creacion de marco para proyecto

				BitBlt(hdc, 0, 0, ANCHO_VENTANA, ALTO_VENTANA, hdcMem, 0, 0, SRCCOPY);
				if (!pantallaInicial && !pantallaGameOver) {
					dibujar_puntos(hdc);
					wchar_t texto[] = L"PUNTUACION";
					TextOutW(hdc, 655, 90, texto, wcslen(texto));

					wchar_t vidas[] = L"VIDAS";
					TextOutW(hdc, 700, 250, vidas, wcslen(vidas));
					dibujar_vidas(hdc);

					COLORREF textColor = RGB(255, 255, 255); // Blanco
					COLORREF bgColor = RGB(0, 0, 0); // Negro

					//int fontSize = 14;
					//HFONT font2 = CreateFont(fontSize, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
					//	OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH | FF_DONTCARE, TEXT("Fixedsys"));
					//SelectObject(hdc, font2);
					//SetTextColor(hdc, textColor);
					//SetBkMode(hdc, TRANSPARENT); // Hacer el fondo transparente
					//TextOutW(hdc, 650, 730, L"Presiona Esc para salir", 24);

				}
				if (pantallaGameOver) {
					
					COLORREF textColor = RGB(255, 255, 255); // Blanco
					COLORREF bgColor = RGB(0, 0, 0); // Negro

					int Fuente2 = 50;
					HFONT fuente2 = CreateFont(Fuente2, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
						OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH | FF_DONTCARE, TEXT("Fixedsys"));
					SelectObject(hdc, fuente2);
					SetTextColor(hdc, textColor);
					SetBkMode(hdc, TRANSPARENT); // Hacer el fondo transparente
					TextOutW(hdc, 180, 190, L"TU PUNTAJE FUE", 15);

					wstring puntaje = to_wstring(puntos);
					TextOutW(hdc, 550, 190, puntaje.c_str(), puntaje.size());

					string auxpuntos1 = to_string(punts[0].puntajesArr);
					string auxpuntos2 = to_string(punts[1].puntajesArr);
					string auxpuntos3 = to_string(punts[2].puntajesArr);
					TextOut(hdc, 320, 380, (LPCSTR)punts[0].nombresArr, 3);
					TextOut(hdc, 480, 380, (LPCSTR)auxpuntos1.c_str(), auxpuntos1.size());

					TextOut(hdc, 320, 440, (LPCSTR)punts[1].nombresArr, 3);
					TextOut(hdc, 480, 440, (LPCSTR)auxpuntos2.c_str(), auxpuntos2.size());

					TextOut(hdc, 320, 500, (LPCSTR)punts[2].nombresArr, 3);
					TextOut(hdc, 480, 500, (LPCSTR)auxpuntos3.c_str(), auxpuntos3.size());

					if (compararPuntos(puntos)) {
						LetraChar[0] = static_cast<wchar_t>(Letraint);
						TextOutW(hdc, 280, 570, L"NUEVO RECORD!!", 15);
						if (Pletra)
						{
							TextOutW(hdc, 320, 610, L"^", 1);
							TextOutW(hdc, 320, 630, (LPCWSTR)LetraChar, 1);
							TextOutW(hdc, 320, 650, L"_", 1);
						}
						else if (Sletra)
						{
							TextOutW(hdc, 400, 610, L"^", 1);
							TextOutW(hdc, 320, 630, (LPCWSTR)&PLetraAux, 1);
							TextOutW(hdc, 400, 630, (LPCWSTR)&LetraChar, 1);
							TextOutW(hdc, 400, 650, L"_", 1);
						}
						else if (Tletra)
						{
							TextOutW(hdc, 320, 630, (LPCWSTR)&PLetraAux, 1);
							TextOutW(hdc, 480, 610, L"^", 1);
							TextOutW(hdc, 400, 630, (LPCWSTR)&SLetraAux, 1);
							TextOutW(hdc, 480, 630, (LPCWSTR)&LetraChar, 1);
							TextOutW(hdc, 480, 650, L"_", 1);
						}
						else {
							TextOutW(hdc, 370, 630, (LPCWSTR)Auxnombre, 3);
							guardararchivo(puntos);
							resetMemory();
						}
					}
					DeleteObject(fuente2);
				}

				DeleteObject(h_CMC);
				//SelectObject(hdcMem, hbmOld);
				DeleteDC(hdcMem);
				//DeleteObject(hbmOld);
			}
			break;		
		/*case WM_KEYDOWN:							
			{
				KEYS[ wParam ] = true;
			}
			break;*/
		case WM_CLOSE: 
			{
				DestroyWindow(hWnd);
			}
			break;
		case WM_DESTROY: //Send A Quit Message
			{
				KillTimer(hWnd, TICK);
				PostQuitMessage(0);
			}
			break;
		default:
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return 0;
}
/* Inicializacion de variables y reserva de memoria.
	*/
void Init()
{
	////Empezamos a reproducir musica
	ReproductorInicializaYReproduce();
	ReproductorCambiarCancionYReproduce(1);
	 if (!pantallaInicial&&!pantallaGameOver)
	{
		ReproductorCambiarCancionYReproduce(0);
	}

	CargaFramesSprite();
	CargaFramesSprite2();
	CargaFramesSprite3();
	CargaFramesSprite4();
	CargaFramesSprite5();
	CargaFramesSprite6();
	CargaFramesSprite7();
	CargaFramesSprite8();
	CargaFramesGas();


	//Cargamos imagen bitmap de nuestro escenario
	miStage.ImagenEscenario1 = gdipLoad(miStage.Bmp1);
	miStage.ImagenEscenario2 = gdipLoad(miStage.Bmp2);
	miStage.ImagenEscenario3 = gdipLoad(miStage.Bmp3);

	//Definimos un puntero del total de pixeles que tiene nuestra ventana
	ptrBufferPixelsWindow = new int[ANCHO_VENTANA * ALTO_VENTANA];

	// Definimos la animacion inicial
	AnimacionActual = Idle;

}

void CargaFramesSprite(){
	//Cargamos primero las hojas de sprite
	miPersonaje.AutoSprite = gdipLoad(miPersonaje.BmpW);

	//Definiendo las coordenadas iniciales en pantalla donde iniciaremos
	miPersonaje.XCurrentCoordDraw = 260;
	miPersonaje.YCurrentCoordDraw = 500;
	//Definiendo los tamaños de nuestro sprite para renderizarlo en la ventana
	miPersonaje.HeightDraw = 3;
	miPersonaje.WeightDraw = 41;

	//Hitbox
	miPersonaje.HitboxAncho = 19;
	miPersonaje.HitboxAlto = 45;
	miPersonaje.HitboxX = miPersonaje.XCurrentCoordDraw+16;
	miPersonaje.HitboxY = miPersonaje.YCurrentCoordDraw+8;
	miPersonaje.HitboxScaleX = 4;
	miPersonaje.HitboxScaleY = 4;

	//Definiendo las dimenciones en base al # de Animaciones
	miPersonaje.FrameSpriteArray = new FrameArray * [miPersonaje.Animaciones];
	//Definiendo la cantidad de Frames en base a cada Animacion
	miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionIdle] = new FrameArray[miPersonaje.FramesAnimacionIdle];
	miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionInmune] = new FrameArray[miPersonaje.FramesAnimacionInmune];

	//Cargando Frames a nuestro arreglo del sprite
	// ------ - Animacion 1 Idle -------- //
	miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionIdle][Frame0].x = 3; miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionIdle][Frame0].y = 9;
	miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionIdle][Frame0].ancho = 27; miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionIdle][Frame0].alto = 49;

	miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionIdle][Frame1].x = 31; miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionIdle][Frame1].y = 9;
	miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionIdle][Frame1].ancho = 27; miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionIdle][Frame1].alto = 49;

	miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionIdle][Frame2].x = 59; miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionIdle][Frame2].y = 9;
	miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionIdle][Frame2].ancho = 27; miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionIdle][Frame2].alto = 49;

	miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionIdle][Frame3].x = 87; miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionIdle][Frame3].y = 9;
	miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionIdle][Frame3].ancho = 27; miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionIdle][Frame3].alto = 49;

	//Cargando Frames a nuestro arreglo del sprite
	// ------ - Animacion 2 Inmune -------- //
	miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionInmune][Frame0].x = 117; miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionInmune][Frame0].y = 9;
	miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionInmune][Frame0].ancho = 27; miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionInmune][Frame0].alto = 49;

	miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionInmune][Frame1].x = 145; miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionInmune][Frame1].y = 9;
	miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionInmune][Frame1].ancho = 27; miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionInmune][Frame1].alto = 49;

	miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionInmune][Frame2].x = 173; miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionInmune][Frame2].y = 9;
	miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionInmune][Frame2].ancho = 27; miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionInmune][Frame2].alto = 49;

	miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionInmune][Frame3].x = 201; miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionInmune][Frame3].y = 9;
	miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionInmune][Frame3].ancho = 27; miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionInmune][Frame3].alto = 49;

}
void CargaFramesSprite2() {
	//Cargamos primero las hojas de sprite
	Auto2.AutoSprite2 = gdipLoad(Auto2.BmpW2);

	//Definiendo las coordenadas iniciales en pantalla donde iniciaremos
	Auto2.XCurrentCoordDraw = 425;
	Auto2.YCurrentCoordDraw = 280;
	//Definiendo los tamaños de nuestro sprite para renderizarlo en la ventana
	Auto2.HeightDraw = 3;
	Auto2.WeightDraw = 41;
	//Hitbox
	Auto2.HitboxAncho = 14;
	Auto2.HitboxAlto = 37;
	//Auto2.HitboxX = 700;
	//Auto2.HitboxY = 400;
	Auto2.HitboxScaleX = 3;
	Auto2.HitboxScaleY = 3;

	//Definiendo las dimenciones en base al # de Animaciones
	Auto2.FrameSpriteArray = new FrameArray * [Auto2.Animaciones];
	//Definiendo la cantidad de Frames en base a cada Animacion
	Auto2.FrameSpriteArray[Auto2.idAnimacionIdle] = new FrameArray[Auto2.FramesAnimacionIdle];/*
	miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionCaminando] = new FrameArray[miPersonaje.FramesAnimacionCaminando];*/

	//Cargando Frames a nuestro arreglo del sprite
	// ------ - Animacion 1 Idle -------- //
	Auto2.FrameSpriteArray[Auto2.idAnimacionIdle][Frame0].x = 125; Auto2.FrameSpriteArray[Auto2.idAnimacionIdle][Frame0].y = 22;
	Auto2.FrameSpriteArray[Auto2.idAnimacionIdle][Frame0].ancho = 18; Auto2.FrameSpriteArray[Auto2.idAnimacionIdle][Frame0].alto = 37;

	Auto2.FrameSpriteArray[Auto2.idAnimacionIdle][Frame1].x = 100; Auto2.FrameSpriteArray[Auto2.idAnimacionIdle][Frame1].y = 22;
	Auto2.FrameSpriteArray[Auto2.idAnimacionIdle][Frame1].ancho = 18; Auto2.FrameSpriteArray[Auto2.idAnimacionIdle][Frame1].alto = 37;

	Auto2.FrameSpriteArray[Auto2.idAnimacionIdle][Frame2].x = 125; Auto2.FrameSpriteArray[Auto2.idAnimacionIdle][Frame2].y = 22;
	Auto2.FrameSpriteArray[Auto2.idAnimacionIdle][Frame2].ancho = 18; Auto2.FrameSpriteArray[Auto2.idAnimacionIdle][Frame2].alto = 37;

	Auto2.FrameSpriteArray[Auto2.idAnimacionIdle][Frame3].x = 53; Auto2.FrameSpriteArray[Auto2.idAnimacionIdle][Frame3].y = 22;
	Auto2.FrameSpriteArray[Auto2.idAnimacionIdle][Frame3].ancho = 18; Auto2.FrameSpriteArray[Auto2.idAnimacionIdle][Frame3].alto = 37;


}
void CargaFramesSprite3() {
	//Cargamos primero las hojas de sprite
	Auto3.AutoSprite3 = gdipLoad(Auto3.BmpW3);

	//Definiendo las coordenadas iniciales en pantalla donde iniciaremos
	Auto3.XCurrentCoordDraw = 370;
	Auto3.YCurrentCoordDraw = 10;
	//Definiendo los tamaños de nuestro sprite para renderizarlo en la ventana
	Auto3.HeightDraw = 3;
	Auto3.WeightDraw = 41;
	//Hitbox
	Auto3.HitboxAncho = 48;
	Auto3.HitboxAlto = 122;
	//Auto3.HitboxX = 700;
	//Auto3.HitboxY = 400;
	Auto3.HitboxScaleX = 3;
	Auto3.HitboxScaleY = 3;

	//Definiendo las dimenciones en base al # de Animaciones
	Auto3.FrameSpriteArray = new FrameArray * [Auto3.Animaciones];
	//Definiendo la cantidad de Frames en base a cada Animacion
	Auto3.FrameSpriteArray[Auto3.idAnimacionIdle] = new FrameArray[Auto3.FramesAnimacionIdle];/*
	miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionCaminando] = new FrameArray[miPersonaje.FramesAnimacionCaminando];*/

	//Cargando Frames a nuestro arreglo del sprite
	// ------ - Animacion 1 Idle -------- //
	Auto3.FrameSpriteArray[Auto3.idAnimacionIdle][Frame0].x = 269; Auto3.FrameSpriteArray[Auto3.idAnimacionIdle][Frame0].y = 14;
	Auto3.FrameSpriteArray[Auto3.idAnimacionIdle][Frame0].ancho = 54; Auto3.FrameSpriteArray[Auto3.idAnimacionIdle][Frame0].alto = 125;

	Auto3.FrameSpriteArray[Auto3.idAnimacionIdle][Frame1].x = 209; Auto3.FrameSpriteArray[Auto3.idAnimacionIdle][Frame1].y = 14;
	Auto3.FrameSpriteArray[Auto3.idAnimacionIdle][Frame1].ancho = 54; Auto3.FrameSpriteArray[Auto3.idAnimacionIdle][Frame1].alto = 125;

	Auto3.FrameSpriteArray[Auto3.idAnimacionIdle][Frame2].x = 149; Auto3.FrameSpriteArray[Auto3.idAnimacionIdle][Frame2].y = 14;
	Auto3.FrameSpriteArray[Auto3.idAnimacionIdle][Frame2].ancho = 54; Auto3.FrameSpriteArray[Auto3.idAnimacionIdle][Frame2].alto = 125;

	Auto3.FrameSpriteArray[Auto3.idAnimacionIdle][Frame3].x = 87; Auto3.FrameSpriteArray[Auto3.idAnimacionIdle][Frame3].y = 14;
	Auto3.FrameSpriteArray[Auto3.idAnimacionIdle][Frame3].ancho = 54; Auto3.FrameSpriteArray[Auto3.idAnimacionIdle][Frame3].alto = 125;

	Auto3.FrameSpriteArray[Auto3.idAnimacionIdle][Frame4].x = 22; Auto3.FrameSpriteArray[Auto3.idAnimacionIdle][Frame3].y = 14;
	Auto3.FrameSpriteArray[Auto3.idAnimacionIdle][Frame4].ancho = 54; Auto3.FrameSpriteArray[Auto3.idAnimacionIdle][Frame3].alto = 125;
}
void CargaFramesSprite4() {
	//Cargamos primero las hojas de sprite
	Auto4.AutoSprite4 = gdipLoad(Auto4.BmpW4);

	//Definiendo las coordenadas iniciales en pantalla donde iniciaremos
	Auto4.XCurrentCoordDraw = 390;
	Auto4.YCurrentCoordDraw = 80;
	//Definiendo los tamaños de nuestro sprite para renderizarlo en la ventana
	Auto4.HeightDraw = 3;
	Auto4.WeightDraw = 41;
	//Hitbox
	Auto4.HitboxAncho = 24;
	Auto4.HitboxAlto = 50;
	//Auto4.HitboxX = 700;
	//Auto4.HitboxY = 400;
	Auto4.HitboxScaleX = 4;
	Auto4.HitboxScaleY = 4;

	//Definiendo las dimenciones en base al # de Animaciones
	Auto4.FrameSpriteArray = new FrameArray * [Auto4.Animaciones];
	//Definiendo la cantidad de Frames en base a cada Animacion
	Auto4.FrameSpriteArray[Auto4.idAnimacionIdle] = new FrameArray[Auto4.FramesAnimacionIdle];

	//Cargando Frames a nuestro arreglo del sprite
	// ------ - Animacion 1 Idle -------- //
	Auto4.FrameSpriteArray[Auto4.idAnimacionIdle][Frame0].x = 281; Auto4.FrameSpriteArray[Auto4.idAnimacionIdle][Frame0].y = 10;
	Auto4.FrameSpriteArray[Auto4.idAnimacionIdle][Frame0].ancho = 30; Auto4.FrameSpriteArray[Auto4.idAnimacionIdle][Frame0].alto = 50;

	Auto4.FrameSpriteArray[Auto4.idAnimacionIdle][Frame1].x = 248; Auto4.FrameSpriteArray[Auto4.idAnimacionIdle][Frame1].y = 10;
	Auto4.FrameSpriteArray[Auto4.idAnimacionIdle][Frame1].ancho = 30; Auto4.FrameSpriteArray[Auto4.idAnimacionIdle][Frame1].alto = 50;

	Auto4.FrameSpriteArray[Auto4.idAnimacionIdle][Frame2].x = 214; Auto4.FrameSpriteArray[Auto4.idAnimacionIdle][Frame2].y = 10;
	Auto4.FrameSpriteArray[Auto4.idAnimacionIdle][Frame2].ancho = 30; Auto4.FrameSpriteArray[Auto4.idAnimacionIdle][Frame2].alto = 50;

	Auto4.FrameSpriteArray[Auto4.idAnimacionIdle][Frame3].x = 180; Auto4.FrameSpriteArray[Auto4.idAnimacionIdle][Frame3].y = 10;
	Auto4.FrameSpriteArray[Auto4.idAnimacionIdle][Frame3].ancho = 30; Auto4.FrameSpriteArray[Auto4.idAnimacionIdle][Frame3].alto = 50;

	Auto4.FrameSpriteArray[Auto4.idAnimacionIdle][Frame4].x = 145; Auto4.FrameSpriteArray[Auto4.idAnimacionIdle][Frame3].y = 10;
	Auto4.FrameSpriteArray[Auto4.idAnimacionIdle][Frame4].ancho = 30; Auto4.FrameSpriteArray[Auto4.idAnimacionIdle][Frame3].alto = 50;
}
void CargaFramesSprite5() {
	//Cargamos primero las hojas de sprite
	Auto5.AutoSprite5= gdipLoad(Auto5.BmpW4);

	//Definiendo las coordenadas iniciales en pantalla donde iniciaremos
	Auto5.XCurrentCoordDraw = 395;
	Auto5.YCurrentCoordDraw = 500;
	//Definiendo los tamaños de nuestro sprite para renderizarlo en la ventana
	Auto5.HeightDraw = 3;
	Auto5.WeightDraw = 41;
	//Hitbox
	Auto5.HitboxAncho = 24;
	Auto5.HitboxAlto = 50;
	//Auto5.HitboxX = 700;
	//Auto5.HitboxY = 400;
	Auto5.HitboxScaleX = 4;
	Auto5.HitboxScaleY = 4;

	//Definiendo las dimenciones en base al # de Animaciones
	Auto5.FrameSpriteArray = new FrameArray * [Auto5.Animaciones];
	//Definiendo la cantidad de Frames en base a cada Animacion
	Auto5.FrameSpriteArray[Auto5.idAnimacionIdle] = new FrameArray[Auto5.FramesAnimacionIdle];

	//Cargando Frames a nuestro arreglo del sprite
	// ------ - Animacion 1 Idle -------- //
	Auto5.FrameSpriteArray[Auto5.idAnimacionIdle][Frame0].x = 8; Auto5.FrameSpriteArray[Auto5.idAnimacionIdle][Frame0].y = 9;
	Auto5.FrameSpriteArray[Auto5.idAnimacionIdle][Frame0].ancho = 30; Auto5.FrameSpriteArray[Auto5.idAnimacionIdle][Frame0].alto = 50;

	Auto5.FrameSpriteArray[Auto5.idAnimacionIdle][Frame1].x = 42; Auto5.FrameSpriteArray[Auto5.idAnimacionIdle][Frame1].y = 9;
	Auto5.FrameSpriteArray[Auto5.idAnimacionIdle][Frame1].ancho = 30; Auto5.FrameSpriteArray[Auto5.idAnimacionIdle][Frame1].alto = 50;

	Auto5.FrameSpriteArray[Auto5.idAnimacionIdle][Frame2].x = 75; Auto5.FrameSpriteArray[Auto5.idAnimacionIdle][Frame2].y = 9;
	Auto5.FrameSpriteArray[Auto5.idAnimacionIdle][Frame2].ancho = 30; Auto5.FrameSpriteArray[Auto5.idAnimacionIdle][Frame2].alto = 50;

	Auto5.FrameSpriteArray[Auto5.idAnimacionIdle][Frame3].x = 107; Auto5.FrameSpriteArray[Auto5.idAnimacionIdle][Frame3].y = 9;
	Auto5.FrameSpriteArray[Auto5.idAnimacionIdle][Frame3].ancho = 30; Auto5.FrameSpriteArray[Auto5.idAnimacionIdle][Frame3].alto = 50;

}
void CargaFramesSprite6() {
	//Cargamos primero las hojas de sprite
	Auto6.AutoSprite6 = gdipLoad(Auto6.BmpW6);

	//Definiendo las coordenadas iniciales en pantalla donde iniciaremos
	Auto6.XCurrentCoordDraw = 0;
	Auto6.YCurrentCoordDraw = 520;
	//Definiendo los tamaños de nuestro sprite para renderizarlo en la ventana
	Auto6.HeightDraw = 3;
	Auto6.WeightDraw = 41;
	//Hitbox
	Auto6.HitboxAncho = 22;
	Auto6.HitboxAlto = 48;
	//Auto6.HitboxX = 700;
	//Auto6.HitboxY = 400;
	Auto6.HitboxScaleX = 4;
	Auto6.HitboxScaleY = 4;

	//Definiendo las dimenciones en base al # de Animaciones
	Auto6.FrameSpriteArray = new FrameArray * [Auto6.Animaciones];
	//Definiendo la cantidad de Frames en base a cada Animacion
	Auto6.FrameSpriteArray[Auto6.idAnimacionIdle] = new FrameArray[Auto6.FramesAnimacionIdle];

	//Cargando Frames a nuestro arreglo del sprite
	// ------ - Animacion 1 Idle -------- //
	Auto6.FrameSpriteArray[Auto6.idAnimacionIdle][Frame0].x = 18; Auto6.FrameSpriteArray[Auto6.idAnimacionIdle][Frame0].y = 12;
	Auto6.FrameSpriteArray[Auto6.idAnimacionIdle][Frame0].ancho = 26; Auto6.FrameSpriteArray[Auto6.idAnimacionIdle][Frame0].alto = 48;

	Auto6.FrameSpriteArray[Auto6.idAnimacionIdle][Frame1].x = 51; Auto6.FrameSpriteArray[Auto6.idAnimacionIdle][Frame1].y = 12;
	Auto6.FrameSpriteArray[Auto6.idAnimacionIdle][Frame1].ancho = 26; Auto6.FrameSpriteArray[Auto6.idAnimacionIdle][Frame1].alto = 48;

	Auto6.FrameSpriteArray[Auto6.idAnimacionIdle][Frame2].x = 85; Auto6.FrameSpriteArray[Auto6.idAnimacionIdle][Frame2].y = 12;
	Auto6.FrameSpriteArray[Auto6.idAnimacionIdle][Frame2].ancho = 26; Auto6.FrameSpriteArray[Auto6.idAnimacionIdle][Frame2].alto = 48;

	Auto6.FrameSpriteArray[Auto6.idAnimacionIdle][Frame3].x = 51; Auto6.FrameSpriteArray[Auto6.idAnimacionIdle][Frame3].y = 12;
	Auto6.FrameSpriteArray[Auto6.idAnimacionIdle][Frame3].ancho = 26; Auto6.FrameSpriteArray[Auto6.idAnimacionIdle][Frame3].alto = 48;

}
void CargaFramesSprite7() {
	//Cargamos primero las hojas de sprite
	Auto7.AutoSprite7 = gdipLoad(Auto7.BmpW7);

	//Definiendo las coordenadas iniciales en pantalla donde iniciaremos
	Auto7.XCurrentCoordDraw = 262;
	Auto7.YCurrentCoordDraw = 100;
	//Definiendo los tamaños de nuestro sprite para renderizarlo en la ventana
	Auto7.HeightDraw = 3;
	Auto7.WeightDraw = 41;
	//Hitbox
	Auto7.HitboxAncho = 20;
	Auto7.HitboxAlto = 44;
	//Auto7.HitboxX = 700;
	//Auto7.HitboxY = 300;
	Auto7.HitboxScaleX = 4;
	Auto7.HitboxScaleY = 4;

	//Definiendo las dimenciones en base al # de Animaciones
	Auto7.FrameSpriteArray = new FrameArray * [Auto7.Animaciones];
	//Definiendo la cantidad de Frames en base a cada Animacion
	Auto7.FrameSpriteArray[Auto7.idAnimacionIdle] = new FrameArray[Auto7.FramesAnimacionIdle];/*
	miPersonaje.FrameSpriteArray[miPersonaje.idAnimacionCaminando] = new FrameArray[miPersonaje.FramesAnimacionCaminando];*/

	//Cargando Frames a nuestro arreglo del sprite
	// ------ - Animacion 1 Idle -------- //
	Auto7.FrameSpriteArray[Auto7.idAnimacionIdle][Frame0].x = 21; Auto7.FrameSpriteArray[Auto7.idAnimacionIdle][Frame0].y = 14;
	Auto7.FrameSpriteArray[Auto7.idAnimacionIdle][Frame0].ancho = 26; Auto7.FrameSpriteArray[Auto7.idAnimacionIdle][Frame0].alto = 44;

	Auto7.FrameSpriteArray[Auto7.idAnimacionIdle][Frame1].x = 51; Auto7.FrameSpriteArray[Auto7.idAnimacionIdle][Frame1].y = 14;
	Auto7.FrameSpriteArray[Auto7.idAnimacionIdle][Frame1].ancho = 26; Auto7.FrameSpriteArray[Auto7.idAnimacionIdle][Frame1].alto = 44;

	Auto7.FrameSpriteArray[Auto7.idAnimacionIdle][Frame2].x = 80; Auto7.FrameSpriteArray[Auto7.idAnimacionIdle][Frame2].y = 14;
	Auto7.FrameSpriteArray[Auto7.idAnimacionIdle][Frame2].ancho = 26; Auto7.FrameSpriteArray[Auto7.idAnimacionIdle][Frame2].alto = 44;

	Auto7.FrameSpriteArray[Auto7.idAnimacionIdle][Frame3].x = 110; Auto7.FrameSpriteArray[Auto7.idAnimacionIdle][Frame3].y = 14;
	Auto7.FrameSpriteArray[Auto7.idAnimacionIdle][Frame3].ancho = 26; Auto7.FrameSpriteArray[Auto7.idAnimacionIdle][Frame3].alto = 44;

	Auto7.FrameSpriteArray[Auto7.idAnimacionIdle][Frame4].x = 140; Auto7.FrameSpriteArray[Auto7.idAnimacionIdle][Frame3].y = 14;
	Auto7.FrameSpriteArray[Auto7.idAnimacionIdle][Frame4].ancho = 26; Auto7.FrameSpriteArray[Auto7.idAnimacionIdle][Frame3].alto = 44;
}
void CargaFramesSprite8() {
	//Cargamos primero las hojas de sprite
	Auto8.AutoSprite8 = gdipLoad(Auto8.BmpW8);

	//Definiendo las coordenadas iniciales en pantalla donde iniciaremos
	Auto8.XCurrentCoordDraw = 395;
	Auto8.YCurrentCoordDraw = 300;
	//Definiendo los tamaños de nuestro sprite para renderizarlo en la ventana
	Auto8.HeightDraw = 3;
	Auto8.WeightDraw = 41;
	//Hitbox
	Auto8.HitboxAncho = 20;
	Auto8.HitboxAlto = 42;
	//Auto8.HitboxX = 700;
	//Auto8.HitboxY = 300;
	Auto8.HitboxScaleX = 4;
	Auto8.HitboxScaleY = 4;

	//Definiendo las dimenciones en base al # de Animaciones
	Auto8.FrameSpriteArray = new FrameArray * [Auto8.Animaciones];
	//Definiendo la cantidad de Frames en base a cada Animacion
	Auto8.FrameSpriteArray[Auto8.idAnimacionIdle] = new FrameArray[Auto8.FramesAnimacionIdle];

	//Cargando Frames a nuestro arreglo del sprite
	// ------ - Animacion 1 Idle -------- //
	Auto8.FrameSpriteArray[Auto8.idAnimacionIdle][Frame0].x = 10; Auto8.FrameSpriteArray[Auto8.idAnimacionIdle][Frame0].y = 8;
	Auto8.FrameSpriteArray[Auto8.idAnimacionIdle][Frame0].ancho = 26; Auto8.FrameSpriteArray[Auto8.idAnimacionIdle][Frame0].alto = 42;

	Auto8.FrameSpriteArray[Auto8.idAnimacionIdle][Frame1].x = 46; Auto8.FrameSpriteArray[Auto8.idAnimacionIdle][Frame1].y = 9;
	Auto8.FrameSpriteArray[Auto8.idAnimacionIdle][Frame1].ancho = 26; Auto8.FrameSpriteArray[Auto8.idAnimacionIdle][Frame1].alto = 42;

	Auto8.FrameSpriteArray[Auto8.idAnimacionIdle][Frame2].x = 81; Auto8.FrameSpriteArray[Auto8.idAnimacionIdle][Frame2].y = 8;
	Auto8.FrameSpriteArray[Auto8.idAnimacionIdle][Frame2].ancho = 26; Auto8.FrameSpriteArray[Auto8.idAnimacionIdle][Frame2].alto = 42;

	Auto8.FrameSpriteArray[Auto8.idAnimacionIdle][Frame3].x = 116; Auto8.FrameSpriteArray[Auto8.idAnimacionIdle][Frame3].y = 9;
	Auto8.FrameSpriteArray[Auto8.idAnimacionIdle][Frame3].ancho = 26; Auto8.FrameSpriteArray[Auto8.idAnimacionIdle][Frame3].alto = 42;

	Auto8.FrameSpriteArray[Auto8.idAnimacionIdle][Frame4].x = 152; Auto8.FrameSpriteArray[Auto8.idAnimacionIdle][Frame4].y = 9;
	Auto8.FrameSpriteArray[Auto8.idAnimacionIdle][Frame4].ancho = 26; Auto8.FrameSpriteArray[Auto8.idAnimacionIdle][Frame4].alto = 42;

}

void CargaFramesGas() {
	//Cargamos primero las hojas de sprite
	Gas.GasSprite = gdipLoad(Gas.BmpW);

	//Definiendo las coordenadas iniciales en pantalla donde iniciaremos
	Gas.XCurrentCoordDraw = 420;
	Gas.YCurrentCoordDraw = 300;
	//Definiendo los tamaños de nuestro sprite para renderizarlo en la ventana
	Gas.HeightDraw = 3;
	Gas.WeightDraw = 41;

	//Definiendo valores del hitbox
	Gas.HitboxAncho = 27;
	Gas.HitboxAlto = 33;
	//Gas.HitboxX = Gas.XCurrentCoordDraw;
	//Gas.HitboxY = Gas.YCurrentCoordDraw;
	Gas.HitboxScaleX = 2;
	Gas.HitboxScaleY = 2;

	//Definiendo las dimenciones en base al # de Animaciones
	Gas.FrameSpriteArray = new FrameArray * [Gas.Animaciones];
	//Definiendo la cantidad de Frames en base a cada Animacion
	Gas.FrameSpriteArray[Gas.idAnimacionIdle] = new FrameArray[Gas.FramesAnimacionIdle];

	//Cargando Frames a nuestro arreglo del sprite
	// ------ - Animacion 1 Idle -------- //
	Gas.FrameSpriteArray[Gas.idAnimacionIdle][Frame0].x = 2; Gas.FrameSpriteArray[Gas.idAnimacionIdle][Frame0].y = 9;
	Gas.FrameSpriteArray[Gas.idAnimacionIdle][Frame0].ancho = 27; Gas.FrameSpriteArray[Gas.idAnimacionIdle][Frame0].alto = 33;

	Gas.FrameSpriteArray[Gas.idAnimacionIdle][Frame1].x = 34; Gas.FrameSpriteArray[Gas.idAnimacionIdle][Frame1].y = 9;
	Gas.FrameSpriteArray[Gas.idAnimacionIdle][Frame1].ancho = 27; Gas.FrameSpriteArray[Gas.idAnimacionIdle][Frame1].alto = 33;

	Gas.FrameSpriteArray[Gas.idAnimacionIdle][Frame2].x = 66; Gas.FrameSpriteArray[Gas.idAnimacionIdle][Frame2].y = 9;
	Gas.FrameSpriteArray[Gas.idAnimacionIdle][Frame2].ancho = 27; Gas.FrameSpriteArray[Gas.idAnimacionIdle][Frame2].alto = 33;

	Gas.FrameSpriteArray[Gas.idAnimacionIdle][Frame3].x = 98; Gas.FrameSpriteArray[Gas.idAnimacionIdle][Frame3].y = 9;
	Gas.FrameSpriteArray[Gas.idAnimacionIdle][Frame3].ancho = 27; Gas.FrameSpriteArray[Gas.idAnimacionIdle][Frame3].alto = 33;

	Gas.FrameSpriteArray[Gas.idAnimacionIdle][Frame4].x = 131; Gas.FrameSpriteArray[Gas.idAnimacionIdle][Frame4].y = 9;
	Gas.FrameSpriteArray[Gas.idAnimacionIdle][Frame4].ancho = 27; Gas.FrameSpriteArray[Gas.idAnimacionIdle][Frame4].alto = 33;

	Gas.FrameSpriteArray[Gas.idAnimacionIdle][Frame5].x = 164; Gas.FrameSpriteArray[Gas.idAnimacionIdle][Frame5].y = 9;
	Gas.FrameSpriteArray[Gas.idAnimacionIdle][Frame5].ancho = 27; Gas.FrameSpriteArray[Gas.idAnimacionIdle][Frame5].alto = 33;
}

void DibujaPixeles()
{
	if (pantallaInicial&&!pantallaGameOver)
	{
		//Dibujamos el fondo
		TranScaleblt(ptrBufferPixelsWindow, (miStage.ImagenEscenario2.pixeles),
			0, 0,//Iniciamos a dibujar en la ventana en 0,0
			0, 0,//Indicamos cuales son las coordenadas para dibujar desde nuestra imagen; iniciamos en 0,0 desde nuestro escenario
			ANCHO_VENTANA, ALTO_VENTANA,//Definimos cuantos pixeles dibujaremos de nuestra imagen a la pantalla
			870, miStage.ImagenEscenario2.ancho,
			1, 1);//Si ponemos un numero mayor a 1 estaremos repitiendo 2 veces la linea de pixeles en X o en Y

	}
	else if (!pantallaInicial&&!pantallaGameOver){

		posicion_y -= 20; // actualiza la posición vertical del escenario en cada fotograma
		//Si la posición vertical supera el borde inferior de la imagen del escenario, la ajustamos
		if (posicion_y + ALTO_VENTANA > miStage.ImagenEscenario1.alto) {
			posicion_y = miStage.ImagenEscenario1.alto - ALTO_VENTANA;
		}
		// Si la posición vertical es menor que el borde superior de la ventana, la ajustamos
		if (posicion_y < 0) {
			posicion_y = 0;
		}
		if (posicion_y == 0) {
			posicion_y = miStage.ImagenEscenario1.alto - ALTO_VENTANA;
		}

		//Dibujamos el fondo
		TranScaleblt(ptrBufferPixelsWindow, (miStage.ImagenEscenario1.pixeles),
			0, 0,//Iniciamos a dibujar en la ventana en 0,0
			0, posicion_y,//Indicamos cuales son las coordenadas para dibujar desde nuestra imagen; iniciamos en 0,0 desde nuestro escenario
			ANCHO_VENTANA, ALTO_VENTANA,//Definimos cuantos pixeles dibujaremos de nuestra imagen a la pantalla
			870, miStage.ImagenEscenario1.ancho,
			1, 1);//Si ponemos un numero mayor a 1 estaremos repitiendo 2 veces la linea de pixeles en X o en Y

		//Dibujamos a nuestro personaje
		TranScaleblt(ptrBufferPixelsWindow, (miPersonaje.AutoSprite.pixeles),
			miPersonaje.XCurrentCoordDraw, miPersonaje.YCurrentCoordDraw,
			miPersonaje.FrameSpriteArray[AnimacionActual][FrameActualJugador].x, miPersonaje.FrameSpriteArray[AnimacionActual][FrameActualJugador].y,
			miPersonaje.FrameSpriteArray[AnimacionActual][FrameActualJugador].ancho, miPersonaje.FrameSpriteArray[AnimacionActual][FrameActualJugador].alto,
			870, miPersonaje.AutoSprite.ancho, 4, 4);
		//HitboxPersonaje
		//DibujaHitbox(ptrBufferPixelsWindow, 0xFFFF00FF, ANCHO_VENTANA, ALTO_VENTANA,
		//	miPersonaje.HitboxX, miPersonaje.HitboxY, miPersonaje.HitboxAncho, miPersonaje.HitboxAlto,
		//	miPersonaje.HitboxScaleX, miPersonaje.HitboxScaleY);

		//Dibujamos autos
		DibujarAuto3();
		DibujarAuto2();
		DibujarAuto4();
		DibujarAuto5();
		DibujarAuto6();
		DibujarAuto7();
		DibujarAuto8();
		DibujarGas();


		if (DetectarColision(miPersonaje.HitboxX, miPersonaje.HitboxY, miPersonaje.HitboxAncho * 4, miPersonaje.HitboxAlto * 4, Auto2.HitboxX, Auto2.HitboxY, Auto2.HitboxAncho * 3, Auto2.HitboxAlto * 3))
		{
			colision = true;
			miPersonaje.XCurrentCoordDraw = 260;
			miPersonaje.YCurrentCoordDraw = 500;
			miPersonaje.HitboxX = 700;
			miPersonaje.HitboxY = 500;
			AnimacionActual = inmune;

			ReproductorEfecto("Recursos/Choque.mp3");
			vida = vida - 1;

			inicioTiempo = clock();
		}
		if (DetectarColision(miPersonaje.HitboxX, miPersonaje.HitboxY, miPersonaje.HitboxAncho * 4, miPersonaje.HitboxAlto * 4, Auto3.HitboxX, Auto3.HitboxY, Auto3.HitboxAncho * 3, Auto3.HitboxAlto * 3))
		{
			colision = true;
			miPersonaje.XCurrentCoordDraw = 260;
			miPersonaje.YCurrentCoordDraw = 500;
			miPersonaje.HitboxX = 700;
			miPersonaje.HitboxY = 500;
			AnimacionActual = inmune;

			ReproductorEfecto("Recursos/Choque.mp3");
			vida = vida - 1;

			inicioTiempo = clock();
		}
		if (DetectarColision(miPersonaje.HitboxX, miPersonaje.HitboxY, miPersonaje.HitboxAncho * 4, miPersonaje.HitboxAlto * 4, Auto4.HitboxX, Auto4.HitboxY, Auto4.HitboxAncho * 4, Auto4.HitboxAlto * 4))
		{
			colision = true;
			miPersonaje.XCurrentCoordDraw = 260;
			miPersonaje.YCurrentCoordDraw = 500;
			miPersonaje.HitboxX = 700;
			miPersonaje.HitboxY = 500;
			AnimacionActual = inmune;

			ReproductorEfecto("Recursos/Choque.mp3");
			vida = vida - 1;

			inicioTiempo = clock();
		}
		if (DetectarColision(miPersonaje.HitboxX, miPersonaje.HitboxY, miPersonaje.HitboxAncho * 4, miPersonaje.HitboxAlto * 4, Auto5.HitboxX, Auto5.HitboxY, Auto5.HitboxAncho * 4, Auto5.HitboxAlto * 4))
		{
			colision = true;
			miPersonaje.XCurrentCoordDraw = 260;
			miPersonaje.YCurrentCoordDraw = 500;
			miPersonaje.HitboxX = 700;
			miPersonaje.HitboxY = 500;
			AnimacionActual = inmune;

			ReproductorEfecto("Recursos/Choque.mp3");
			vida = vida - 1;

			inicioTiempo = clock();
		}
		if (DetectarColision(miPersonaje.HitboxX, miPersonaje.HitboxY, miPersonaje.HitboxAncho * 4, miPersonaje.HitboxAlto * 4, Auto6.HitboxX, Auto6.HitboxY, Auto6.HitboxAncho * 4, Auto6.HitboxAlto * 4))
		{
			colision = true;
			miPersonaje.XCurrentCoordDraw = 260;
			miPersonaje.YCurrentCoordDraw = 500;
			miPersonaje.HitboxX = 700;
			miPersonaje.HitboxY = 500;
			AnimacionActual = inmune;

			ReproductorEfecto("Recursos/Choque.mp3");
			vida = vida - 1;

			inicioTiempo = clock();
		}
		if (DetectarColision(miPersonaje.HitboxX, miPersonaje.HitboxY, miPersonaje.HitboxAncho * 4, miPersonaje.HitboxAlto * 4, Auto7.HitboxX, Auto7.HitboxY, Auto7.HitboxAncho * 4, Auto7.HitboxAlto * 4))
		{
			colision = true;
			miPersonaje.XCurrentCoordDraw = 260;
			miPersonaje.YCurrentCoordDraw = 500;
			miPersonaje.HitboxX = 700;
			miPersonaje.HitboxY = 500;
			AnimacionActual = inmune;

			ReproductorEfecto("Recursos/Choque.mp3");
			vida = vida - 1;

			inicioTiempo = clock();
		}
		if (DetectarColision(miPersonaje.HitboxX, miPersonaje.HitboxY, miPersonaje.HitboxAncho * 4, miPersonaje.HitboxAlto * 4, Auto8.HitboxX, Auto8.HitboxY, Auto8.HitboxAncho * 4, Auto8.HitboxAlto * 4))
		{
			colision = true;
			miPersonaje.XCurrentCoordDraw = 260;
			miPersonaje.YCurrentCoordDraw = 500;
			miPersonaje.HitboxX = 700;
			miPersonaje.HitboxY = 500;
			AnimacionActual = inmune;

			ReproductorEfecto("Recursos/Choque.mp3");
			vida = vida - 1;

			inicioTiempo = clock();
		}

		if (colision) {
			clock_t tiempoActual = clock();
			double tiempoTranscurrido = (tiempoActual - inicioTiempo) / (double)CLOCKS_PER_SEC;

			if (tiempoTranscurrido >= 3.0)
			{
				miPersonaje.HitboxX = miPersonaje.XCurrentCoordDraw + 16;
				miPersonaje.HitboxY = miPersonaje.YCurrentCoordDraw+8;
				ReproductorEfecto("Recursos/Revivir.mp3");
				AnimacionActual = Idle;
				colision = false;
			}
		}
		if (vida == 0) {
			pantallaInicial = false;
			pantallaGameOver = true;
			ReproductorCambiarCancionYReproduce(2);
		}
	}
	else if (pantallaGameOver&&!pantallaInicial) {
		TranScaleblt(ptrBufferPixelsWindow, (miStage.ImagenEscenario3.pixeles),
			0, 0,//Iniciamos a dibujar en la ventana en 0,0
			0, 0,//Indicamos cuales son las coordenadas para dibujar desde nuestra imagen; iniciamos en 0,0 desde nuestro escenario
			ANCHO_VENTANA, ALTO_VENTANA,//Definimos cuantos pixeles dibujaremos de nuestra imagen a la pantalla
			870, miStage.ImagenEscenario2.ancho,
			1, 1);//Si ponemos un numero mayor a 1 estaremos repitiendo 2 veces la linea de pixeles en X o en Y
	}
}

void ActualizaAnimacion(HWND hWnd) {
	switch (AnimacionActual) {

	case Idle: {
		if (TICK % TICK == 0 && FrameActualJugador == 0)//125
		{
			DelayFrameAnimation++;
		}
		else if (TICK % TICK == 0 && FrameActualJugador == 2)
		{
			DelayFrameAnimation += 3;
		}
		if (DelayFrameAnimation % 2 == 0)
		{
			FrameActualJugador++;
			if (FrameActualJugador > 3) FrameActualJugador = 0;
		}
		
		if (TICK % TICK == 0 && FrameActual == 0)//125
		{
			DelayFrameAnimation++;
		}
		else if (TICK % TICK == 0 && FrameActual == 2)
		{
			DelayFrameAnimation += 3;
		}
		if (DelayFrameAnimation % 2 == 0)
		{
			FrameActual++;
			if (FrameActual > 3) FrameActual = 0;
		}


		if (TICK % TICK == 0 && FrameActualA3 == 0)//125
		{
			DelayFrameAnimation++;
		}
		else if (TICK % TICK == 0 && FrameActualA3 == 2)
		{
			DelayFrameAnimation += 3;
		}
		if (DelayFrameAnimation % 1 == 0)
		{
			FrameActualA3++;
			if (FrameActualA3 > 3) FrameActualA3 = 0;
		}


		if (TICK % TICK == 0 && FrameActualA2 == 0)//125
		{
			DelayFrameAnimation++;
		}
		else if (TICK % TICK == 0 && FrameActualA2 == 2)
		{
			DelayFrameAnimation += 3;
		}
		if (DelayFrameAnimation % 2 == 0)
		{
			FrameActualA2++;
			if (FrameActualA2 > 3) FrameActualA2 = 0;
		}

		if (TICK % TICK == 0 && FrameActualGas == 0)//125
		{
			DelayFrameAnimation++;
		}
		else if (TICK % TICK == 0 && FrameActualGas == 2)
		{
			DelayFrameAnimation += 4;
		}
		if (DelayFrameAnimation % 1 == 0)
		{
			FrameActualGas++;
			if (FrameActualGas > 4) FrameActualGas = 0;
		}



	}break;
	case inmune: {
		if (TICK % TICK == 0 && FrameActualJugador == 0)//125
		{
			DelayFrameAnimation++;
		}
		else if (TICK % TICK == 0 && FrameActualJugador == 2)
		{
			DelayFrameAnimation += 3;
		}
		if (DelayFrameAnimation % 2 == 0)
		{
			FrameActualJugador++;
			if (FrameActualJugador > 3) FrameActualJugador = 0;
		}

	}break;

	}
	InvalidateRect(hWnd, NULL, FALSE);
	UpdateWindow(hWnd);
}

void MainRender(HWND hWnd) 
{
	LimpiarFondo(ptrBufferPixelsWindow, 0xFFFFFFFF, (ANCHO_VENTANA * ALTO_VENTANA));
	KeysEvents(hWnd);
	DibujaPixeles();
	ActualizaAnimacion(hWnd);

}
void KeysEvents(HWND hWnd)
{
	if (pantallaGameOver)
	{
		if (KEYS[input.W] || KEYS[input.Up] || Gpad->GetState().Gamepad.wButtons == XINPUT_GAMEPAD_DPAD_UP || Gpad->GetState().Gamepad.sThumbLY < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
		{
			Letraint = (Letraint - 1 < A) ? Z : Letraint - 1;
		}
		if (KEYS[input.S] || KEYS[input.Down] || Gpad->GetState().Gamepad.wButtons == XINPUT_GAMEPAD_DPAD_DOWN|| Gpad->GetState().Gamepad.sThumbLY > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
		{
			Letraint = (Letraint + 1 > Z) ? A : Letraint + 1;
		}
		if (KEYS[input.Enter] || Gpad->GetState().Gamepad.wButtons == XINPUT_GAMEPAD_A)
		{
			if (Pletra)
			{
				PLetraAux[0] = static_cast<wchar_t>(Letraint);
				Pletra = false;
				Sletra = true;
			}
			else if (Sletra)
			{
				SLetraAux[0] = static_cast<wchar_t>(Letraint);
				Sletra = false;
				Tletra = true;
			}
			else if (Tletra)
			{
				TLetraAux[0] = static_cast<wchar_t>(Letraint);
				Pletra = false;
				Sletra = false;
				Tletra = false;

				Auxnombre[0] = PLetraAux[0];
				Auxnombre[1] = SLetraAux[0];
				Auxnombre[2] = TLetraAux[0];

				//InvalidateRect(hWnd, NULL, TRUE); // Actualiza la ventana
			}
			else {
				ReproductorCambiarCancionYReproduce(1);
				resetMemory();
			}
		}
	}

	if (!pantallaGameOver && !pantallaInicial) {
		if (KEYS[input.W] || KEYS[input.Up] || Gpad->GetState().Gamepad.wButtons == XINPUT_GAMEPAD_DPAD_UP || Gpad->GetState().Gamepad.sThumbLY < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
		{
			if (miPersonaje.YCurrentCoordDraw >= 200) {
				miPersonaje.YCurrentCoordDraw -= 15;
				miPersonaje.HitboxY -= 15;
			}
		}
		if (KEYS[input.D] || KEYS[input.Right]|| Gpad->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT || Gpad->GetState().Gamepad.sThumbLX > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
		{
			if (miPersonaje.XCurrentCoordDraw <= 398) {
				miPersonaje.XCurrentCoordDraw += 20;
				miPersonaje.HitboxX += 20;
			}
		}
		if (KEYS[input.S] || KEYS[input.Down] || Gpad->GetState().Gamepad.wButtons == XINPUT_GAMEPAD_DPAD_DOWN || Gpad->GetState().Gamepad.sThumbLY > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
		{
			if (miPersonaje.YCurrentCoordDraw < 540)
			{
				miPersonaje.YCurrentCoordDraw += 8;
				miPersonaje.HitboxY += 8;
			}
		}
		if (KEYS[input.A] || KEYS[input.Left] || Gpad->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT || Gpad->GetState().Gamepad.sThumbLX < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
		{
			if (miPersonaje.XCurrentCoordDraw > 120) {
				miPersonaje.XCurrentCoordDraw -= 20;
				miPersonaje.HitboxX -= 20;
			}
		}
	}

	if (pantallaInicial){
		if (KEYS[input.Space] || Gpad->GetState().Gamepad.wButtons == XINPUT_GAMEPAD_START)
		{
			ReproductorEfecto("Recursos/- sonido de start-.mp3");
			pantallaInicial = 0;
			Init();
		}
	}
	//if (!pantallaInicial&&!pantallaGameOver) {
	//	if (KEYS[input.Escape] || Gpad->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_BACK)
	//	{
	//		pantallaInicial = 1;
	//		Init();
	//		//resetMemory();
	//	}
	//}
}

#pragma region LENS_CODE
/* Pinta el fondo de la ventana de acuerdo al color especificado.
	@param *ptrBuffer.	Puntero al area de memoria reservada para el proceso de dibujado.
	@param color.		Color expresado en formato hexadecimal.
	@param area.		Area de la ventana.
	*/
void LimpiarFondo(int *ptrBuffer, unsigned int colorFondo, int area) 
{
	__asm {
			mov edi, ptrBuffer
			mov ecx, area
			mov eax, colorFondo

			draw: 	
				mov [edi], eax
				add edi, BPP
			loop draw

			rep stosd
	}
}

/* Funcion que pinta una figura rectangular en pantalla.
	@param *ptrBuffer.	Puntero al area de memoria reservada para el dibujado.
	@param color.		Color de la figura expresado en formato hexadecimal.
	@param anchoWnd.	Ancho total de la ventana.
	@param altoWnd.		Alto total de la ventana.
	@param dmnFigura.	Especifica las dimensiones de la figura en relacion con la ventana.
	@param posFigura.	Posiciona la figura en la ventana.
	*/

void TranScaleblt(int* punteroDestino, int* punteroOrigen, int inicioXDestino, int inicioYDestino, int inicioXOrigen, int inicioYOrigen, int ancho, int alto, int anchodefondo, int anchodeorigen, int escalaX, int escalaY){
	//blt = block transfer, transferencia de bloque de imagen
	int bytesporlineafondo = anchodefondo * 4;
	int bytesporlineaorigen = anchodeorigen * 4;
	int bytesporlineaimagen = ancho * 4;

	__asm{

			mov edi, punteroDestino //movemos la direccion del bitmap a edi, para poder escribir en el
			//Conseguimos el pixel inicial donde empezaremos a dibujar
			mov eax, inicioYDestino
			mul bytesporlineafondo //inicioY * bytesporlineafondo, asi iniciamos en la linea donde queremos 
			mov ebx, eax //ebx contendra el resultado anterior
			mov eax, 4
			mul inicioXDestino //inicioX*4, para asi encontrar la columna donde queremos empezar a dibujar
			add eax, ebx //posicion de columna + posicion de linea
			add edi, eax //Sumamos el desplazamiento anterior al inicio de nuestra imagen para empezar a trabajar en la posicion deseada
			mov esi, punteroOrigen //movemos la direccion de la imagen a dibujar a esi, para poder escribir de ella
			//Conseguimos el pixel inicial DEL CUAL empezaremos a dibujar
			mov eax, inicioYOrigen
			mul bytesporlineaorigen //inicioY * bytesporlineaorigen, asi iniciamos en la linea donde queremos 
			mov ebx, eax //ebx contendra el resultado anterior
			mov eax, 4
			mul inicioXOrigen //inicioX*4, para asi encontrar la columnda de donde queremos empezar a leer
			add eax, ebx //posicion de columna + posicion de linea
			add esi, eax //Sumamos el desplazamiento anterior al inicio de nuestra imagen para empezar a trabajar en la posicion deseada
			mov eax, [esi]
			mov ecx, alto //movemos a ecx la cantidad de lineas que dibujaremos
			lazollenarY :
				push ecx
				mov ecx, escalaY
				escaladoY :
					push ecx //guardamos el valor anterior de ecx, porque lo reemplazaremos en un nuevo ciclo
					mov ecx, ancho //la cantidad de columnas que dibujaremos
					lazollenarX :
						//mueve un pixel de la direccion apuntada por esi a la apuntada por edi, e incrementa esi y edi por 4
						push ecx
						mov ecx, escalaX
						escaladoX :
							cmp eax, [esi]
							je nodibujar
							mov edx, [esi]
							mov[edi], edx
								nodibujar :
								add edi, 4
						loop escaladoX
						add esi, 4
						pop ecx
					loop lazollenarX
					add edi, bytesporlineafondo //le sumamos la cantidad de bytes de la linea del fondo para pasar a la siguiente linea
					push eax
					mov eax, bytesporlineaimagen
					mul escalaX
					sub edi, eax //y retrocedemos una cantidad igual al a su ancho para dibujar desde la posicion X inicial y que no quede escalonado
					pop eax
					sub esi, bytesporlineaimagen
					pop ecx
				loop escaladoY
			//Lo mismo para esi
			add esi, bytesporlineaorigen
			pop ecx //recuperamos el valor del contador del ciclo exterior
			loop lazollenarY
	}
}	
void DibujaHitbox(int* ptrBuffer, unsigned int color, int anchoWnd, int altoWnd, int HitboxX, int HitboxY, int HitboxAncho, int HitboxAlto, int escalaX, int escalaY)
{
	HitboxAncho = HitboxAncho * escalaX;
	HitboxAlto = HitboxAlto * escalaY;
	__asm {
		mov edi, ptrBuffer

		mov eax, HitboxY
		mul BPP
		mul anchoWnd
		add edi, eax

		mov eax, HitboxX
		mul BPP
		add edi, eax

		mov eax, color

		mov ecx, HitboxAlto
		lazo_alto :
		push ecx
			mov ecx, HitboxAncho
			lazo_ancho :
		mov[edi], eax
			add edi, BPP
			//stosd
			loop lazo_ancho
			push eax
			mov eax, anchoWnd
			sub eax, HitboxAncho
			mul BPP
			add edi, eax
			pop eax
			pop ecx
			loop lazo_alto
	}
}

#pragma endregion

bool DetectarColision(int x1, int y1, int ancho1, int alto1, int x2, int y2, int ancho2, int alto2) {
	bool colisionn = (x1 < (x2 + ancho2)) && ((x1 + ancho1) > x2) && (y1 < (y2 + alto2)) && (y1 > y2 - alto1);
	return colisionn;
}

//Musica
void ReproductorInicializaYReproduce() {
	Cancion[0].Nombre = "Inicio";
	Cancion[0].Dir = "Recursos/Gary Vs. David [8 Bit - Chiptune Remix].mp3";
	Cancion[1].Nombre = "Juego";
	Cancion[1].Dir = "Recursos/A Night Of Dizzy Spells.mp3";
	Cancion[2].Nombre = "Game Over";
	Cancion[2].Dir = "Recursos/Game Over (8-Bit Music).mp3";
	ifstream inputFile(Cancion[0].Dir.c_str());

	if (!inputFile.good()) 
		printf("No file found");
	else 
		player->OpenFile(Cancion[0].Dir.c_str(), sfAutodetect);
	player->Play();
}
void ReproductorCambiarCancionYReproduce(int NumeroCancionAeproducir) {
	player->Stop();
	ifstream inputFile(Cancion[NumeroCancionAeproducir].Dir.c_str());

	if (!inputFile.good())
		printf("No file found");
	else
		player->OpenFile(Cancion[NumeroCancionAeproducir].Dir.c_str(), sfAutodetect);
	player->Play();

}
void ReproductorPausa() {
	player->Pause();
	pausa = true;
}
void ReproductorReproduce() {
	if(pausa)
		player->Resume();
	else 
		player->Play();
	pausa = false;
}

void ReproductorEfecto(string dir) {
	ifstream inputFile(dir.c_str());

	if (!inputFile.good()) {
		printf("No se encontró el archivo de efecto de sonido");
	}
	else {
		// Detiene cualquier reproducción existente antes de cargar y reproducir el efecto de sonido
		playerEfecto->Stop();
		playerEfecto->OpenFile(dir.c_str(), sfAutodetect);
		playerEfecto->Play();
	}
}

//Dibujar Autos enemigos
void DibujarAuto3() {
	static int startX[] = { 110, 250, 370, 110, 370 }; // Posiciones X posibles
	static int numStartX = sizeof(startX) / sizeof(startX[0]); // Número de posiciones X posibles
	static int currentStartX = startX[rand() % numStartX]; // Posición X actual
	static int startY = 0; // Coordenada Y inicial
	static int endY = 400; // Coordenada Y final
	static int spriteHeight = 125; // Altura del sprite
	static bool resetPosition = false; // Bandera para reiniciar la posición
	static bool initialDelay = true; // Bandera para retraso inicial
	static clock_t lastResetTime = 0; // Variable para almacenar el tiempo del último reinicio

		// Verificar si ha transcurrido el tiempo de retraso inicial (8 segundos = 8 * CLOCKS_PER_SEC)
		if (initialDelay && (clock() - lastResetTime) < 34 * CLOCKS_PER_SEC || resetPosition && (clock() - lastResetTime) < 43 * CLOCKS_PER_SEC) {
			return; // Salir de la función si todavía no ha transcurrido el tiempo necesario
		}

		// Reiniciar posición y establecer el tiempo del reinicio
		if (resetPosition) {
			currentStartX = startX[rand() % numStartX];
			resetPosition = false;
		}

		// Establecer el retraso inicial en el primer ciclo
		if (initialDelay) {
			lastResetTime = clock();
			initialDelay = false;
		}

		TranScaleblt(ptrBufferPixelsWindow, (Auto3.AutoSprite3.pixeles),
			currentStartX, startY,
			Auto3.FrameSpriteArray[0][FrameActualA3].x, Auto3.FrameSpriteArray[0][FrameActualA3].y,
			Auto3.FrameSpriteArray[0][FrameActualA3].ancho, spriteHeight,
			870, Auto3.AutoSprite3.ancho, 3, 3);

		Auto3.HitboxX = currentStartX+9;
		Auto3.HitboxY = startY;
		Auto3.HitboxAlto = spriteHeight;
		//HitboxCamion
		//DibujaHitbox(ptrBufferPixelsWindow, 0xFFFF00FF, ANCHO_VENTANA, ALTO_VENTANA,
		//	Auto3.HitboxX, Auto3.HitboxY, Auto3.HitboxAncho, Auto3.HitboxAlto,
		//	Auto3.HitboxScaleX, Auto3.HitboxScaleY);

		startY += 15;
		if (startY > endY) {
			if (spriteHeight > 5) {
				spriteHeight -= 5;
			}
			else {
				// Reiniciar posición
				startY = 0;
				spriteHeight = 125;
				lastResetTime = clock();
				if (puntos < 999) {
					puntos = puntos + 9;
					if (puntos > 999) {
						puntos = 999;
					}
				}
				resetPosition = true;
			}
		}
}
void DibujarAuto2() {
	static int startX[] = { 150, 425, 290, 425, 150, 290 }; // Posiciones X posibles
	static int numStartX = sizeof(startX) / sizeof(startX[0]); // Número de posiciones X posibles
	static int currentStartX = 290; // Posición X actual
	static int startY = 0; // Coordenada Y inicial
	static int endY = 680; // Coordenada Y final
	static int spriteHeight = 37; // Altura del sprite
	static bool resetPosition = false; // Bandera para reiniciar la posición
	static bool initialDelay = true; // Bandera para retraso inicial
	static clock_t lastResetTime = 0; // Variable para almacenar el tiempo del último reinicio

	if (initialDelay && (clock() - lastResetTime) < 4 * CLOCKS_PER_SEC || resetPosition && (clock() - lastResetTime) < 5.5 * CLOCKS_PER_SEC) {
		return; // Salir de la función si todavía no ha transcurrido el tiempo necesario
	}

	// Reiniciar posición y establecer el tiempo del reinicio
	if (resetPosition) {
		currentStartX = startX[rand() % numStartX];
		resetPosition = false;
	}

	// Establecer el retraso inicial en el primer ciclo
	if (initialDelay) {
		lastResetTime = clock();
		initialDelay = false;
	}

	TranScaleblt(ptrBufferPixelsWindow, (Auto2.AutoSprite2.pixeles),
		currentStartX, startY,
		Auto2.FrameSpriteArray[0][FrameActualA2].x, Auto2.FrameSpriteArray[0][FrameActualA2].y,
		Auto2.FrameSpriteArray[0][FrameActualA2].ancho, spriteHeight,
		870, Auto2.AutoSprite2.ancho, 3, 3);

	Auto2.HitboxX = currentStartX+6;
	Auto2.HitboxY = startY;
	Auto2.HitboxAlto = spriteHeight;

	//HitboxMoto
	//DibujaHitbox(ptrBufferPixelsWindow, 0xFFFF00FF, ANCHO_VENTANA, ALTO_VENTANA,
	//	Auto2.HitboxX, Auto2.HitboxY, Auto2.HitboxAncho, Auto2.HitboxAlto,
	//	Auto2.HitboxScaleX, Auto2.HitboxScaleY);

	startY += 26;
	if (startY > endY) {
		if (spriteHeight > 6) {
			spriteHeight -= 8;
		}
		else {
			startY = 0;
			spriteHeight = 38;
			lastResetTime = clock();
			if (puntos < 999) {
				puntos++;
				if (puntos > 999) {
					puntos = 999;
				}
			}
			resetPosition = true;
		}
	}

}
void DibujarAuto4() {
	static int startX[] = { 120, 250, 390, 120, 390 }; // Posiciones X posibles
	static int numStartX = sizeof(startX) / sizeof(startX[0]); // Número de posiciones X posibles
	static int currentStartX = startX[rand() % numStartX]; // Posición X actual
	static int startY = 0; // Coordenada Y inicial
	static int endY = 600; // Coordenada Y final
	static int spriteHeight = 50; // Altura del sprite
	static bool resetPosition = false; // Bandera para reiniciar la posición
	static bool initialDelay = true; // Bandera para retraso inicial
	static clock_t lastResetTime = 0; // Variable para almacenar el tiempo del último reinicio

	if (initialDelay && (clock() - lastResetTime) < 8 * CLOCKS_PER_SEC || resetPosition && (clock() - lastResetTime) < 10 * CLOCKS_PER_SEC) {
		return; // Salir de la función si todavía no ha transcurrido el tiempo necesario
	}

	// Reiniciar posición y establecer el tiempo del reinicio
	if (resetPosition) {
		currentStartX = startX[rand() % numStartX];
		resetPosition = false;
	}

	// Establecer el retraso inicial en el primer ciclo
	if (initialDelay) {
		lastResetTime = clock();
		initialDelay = false;
	}

	TranScaleblt(ptrBufferPixelsWindow, (Auto4.AutoSprite4.pixeles),
		currentStartX, startY,
		Auto4.FrameSpriteArray[0][FrameActualA3].x, Auto4.FrameSpriteArray[0][FrameActualA3].y,
		Auto4.FrameSpriteArray[0][FrameActualA3].ancho, spriteHeight,
		870, Auto4.AutoSprite4.ancho, 4, 4);

	Auto4.HitboxX = currentStartX+12;
	Auto4.HitboxY = startY;
	Auto4.HitboxAlto = spriteHeight;
	//HitboxAuto4
	//DibujaHitbox(ptrBufferPixelsWindow, 0xFFFF00FF, ANCHO_VENTANA, ALTO_VENTANA,
	//	Auto4.HitboxX, Auto4.HitboxY, Auto4.HitboxAncho, Auto4.HitboxAlto,
	//	Auto4.HitboxScaleX, Auto4.HitboxScaleY);

	startY += 17;
	if (startY > endY) {
		if (spriteHeight > 5) {
			spriteHeight -= 5;
		}
		else {
			startY = 0;
			spriteHeight = 50;
			lastResetTime = clock();
			if (puntos < 999) {
				puntos = puntos + 2;
				if (puntos > 999) {
					puntos = 999;
				}
			}
			resetPosition = true;
		}
	}
}
void DibujarAuto5() {
	static int startX[] = { 120, 388, 388, 250 , 120, 388, 120}; // Posiciones X posibles
	static int numStartX = sizeof(startX) / sizeof(startX[0]); // Número de posiciones X posibles
	static int currentStartX = 120; // Posición X actual
	static int startY = 0; // Coordenada Y inicial
	static int endY = 600; // Coordenada Y final
	static int spriteHeight = 50; // Altura del sprite
	static bool resetPosition = false; // Bandera para reiniciar la posición
	static bool initialDelay = true; // Bandera para retraso inicial
	static clock_t lastResetTime = 0; // Variable para almacenar el tiempo del último reinicio

	if (initialDelay && (clock() - lastResetTime) < 8 * CLOCKS_PER_SEC || resetPosition && (clock() - lastResetTime) < 7 * CLOCKS_PER_SEC) {
		return; // Salir de la función si todavía no ha transcurrido el tiempo necesario
	}

	// Reiniciar posición y establecer el tiempo del reinicio
	if (resetPosition) {
		currentStartX = startX[rand() % numStartX];
		resetPosition = false;
	}

	// Establecer el retraso inicial en el primer ciclo
	if (initialDelay) {
		lastResetTime = clock();
		initialDelay = false;
	}

	TranScaleblt(ptrBufferPixelsWindow, (Auto5.AutoSprite5.pixeles),
		currentStartX, startY,
		Auto5.FrameSpriteArray[0][FrameActual].x, Auto5.FrameSpriteArray[0][FrameActual].y,
		Auto5.FrameSpriteArray[0][FrameActual].ancho, spriteHeight,
		870, Auto5.AutoSprite5.ancho, 4, 4);

	//HitboxAuto5
	Auto5.HitboxX = currentStartX + 12;
	Auto5.HitboxY = startY;
	Auto5.HitboxAlto = spriteHeight;

	//DibujaHitbox(ptrBufferPixelsWindow, 0xFFFF00FF, ANCHO_VENTANA, ALTO_VENTANA,
	//	Auto5.HitboxX, Auto5.HitboxY, Auto5.HitboxAncho, Auto5.HitboxAlto,
	//	Auto5.HitboxScaleX, Auto5.HitboxScaleY);

	startY += 21;
	if (startY > endY) {
		if (spriteHeight > 5) {
			spriteHeight -= 5;
		}
		else {
			startY = 0;
			spriteHeight = 50;
			lastResetTime = clock();
			if (puntos < 999) {
				puntos = puntos + 2;
				if (puntos > 999) {
					puntos = 999;
				}
			}
			resetPosition = true;
		}
	}
}
void DibujarAuto6() {
	static int startX[] = { 130, 260, 130, 390, 260, 130 }; // Posiciones X posibles
	static int numStartX = sizeof(startX) / sizeof(startX[0]); // Número de posiciones X posibles
	static int currentStartX = startX[rand() % numStartX]; // Posición X actual
	static int startY = 0; // Coordenada Y inicial
	static int endY = 600; // Coordenada Y final
	static int spriteHeight = 48; // Altura del sprite
	static bool resetPosition = false; // Bandera para reiniciar la posición
	static bool initialDelay = true; // Bandera para retraso inicial
	static clock_t lastResetTime = 0; // Variable para almacenar el tiempo del último reinicio

	if (initialDelay && (clock() - lastResetTime) < 25 * CLOCKS_PER_SEC || resetPosition && (clock() - lastResetTime) < 16.5 * CLOCKS_PER_SEC) {
		return; // Salir de la función si todavía no ha transcurrido el tiempo necesario
	}

	// Reiniciar posición y establecer el tiempo del reinicio
	if (resetPosition) {
		currentStartX = startX[rand() % numStartX];
		resetPosition = false;
	}

	// Establecer el retraso inicial en el primer ciclo
	if (initialDelay) {
		lastResetTime = clock();
		initialDelay = false;
	}

	TranScaleblt(ptrBufferPixelsWindow, (Auto6.AutoSprite6.pixeles),
		currentStartX, startY,
		Auto6.FrameSpriteArray[0][FrameActual].x, Auto6.FrameSpriteArray[0][FrameActual].y,
		Auto6.FrameSpriteArray[0][FrameActual].ancho, spriteHeight,
		870, Auto6.AutoSprite6.ancho, 4, 4);

	Auto6.HitboxX = currentStartX + 8;
	Auto6.HitboxY = startY;
	Auto6.HitboxAlto = spriteHeight;

	//DibujaHitbox(ptrBufferPixelsWindow, 0xFFFF00FF, ANCHO_VENTANA, ALTO_VENTANA,
	//	Auto6.HitboxX, Auto6.HitboxY, Auto6.HitboxAncho, Auto6.HitboxAlto,
	//	Auto6.HitboxScaleX, Auto6.HitboxScaleY);

	startY += 19;
	if (startY > endY) {
		if (spriteHeight > 5) {
			spriteHeight -= 5;
		}
		else {
			startY = 0;
			spriteHeight = 50;
			lastResetTime = clock();
			if (puntos < 999) {
				puntos = puntos + 4;
				if (puntos > 999) {
					puntos = 999;
				}
			}
			resetPosition = true;
		}
	}
}
void DibujarAuto7() {
	static int startX[] = { 262, 262, 132, 395, 132, 395, 395 }; // Posiciones X posibles
	static int numStartX = sizeof(startX) / sizeof(startX[0]); // Número de posiciones X posibles
	static int currentStartX = startX[rand() % numStartX]; // Posición X actual
	static int startY = 0; // Coordenada Y inicial
	static int endY = 600; // Coordenada Y final
	static int spriteHeight = 44; // Altura del sprite
	static bool resetPosition = false; // Bandera para reiniciar la posición
	static bool initialDelay = true; // Bandera para retraso inicial
	static clock_t lastResetTime = 0; // Variable para almacenar el tiempo del último reinicio

	if (initialDelay && (clock() - lastResetTime) < 30 * CLOCKS_PER_SEC || resetPosition && (clock() - lastResetTime) < 13 * CLOCKS_PER_SEC) {
		return; // Salir de la función si todavía no ha transcurrido el tiempo necesario
	}

	// Reiniciar posición y establecer el tiempo del reinicio
	if (resetPosition) {
		currentStartX = startX[rand() % numStartX];
		resetPosition = false;
	}

	// Establecer el retraso inicial en el primer ciclo
	if (initialDelay) {
		lastResetTime = clock();
		initialDelay = false;
	}

	TranScaleblt(ptrBufferPixelsWindow, (Auto7.AutoSprite7.pixeles),
		currentStartX, startY,
		Auto7.FrameSpriteArray[0][FrameActualA3].x, Auto7.FrameSpriteArray[0][FrameActualA3].y,
		Auto7.FrameSpriteArray[0][FrameActualA3].ancho, spriteHeight,
		870, Auto7.AutoSprite7.ancho, 4, 4);

	Auto7.HitboxX = currentStartX + 12;
	Auto7.HitboxY = startY;
	Auto7.HitboxAlto = spriteHeight;

	//DibujaHitbox(ptrBufferPixelsWindow, 0xFFFF00FF, ANCHO_VENTANA, ALTO_VENTANA,
	//	Auto7.HitboxX, Auto7.HitboxY, Auto7.HitboxAncho, Auto7.HitboxAlto,
	//	Auto7.HitboxScaleX, Auto7.HitboxScaleY);

	startY += 40;
	if (startY > endY) {
		if (spriteHeight > 8) {
			spriteHeight -= 12;

		}
		else {
			startY = 0;
			spriteHeight = 50;
			lastResetTime = clock();
			if (puntos < 999) {
				puntos = puntos + 7;
				if (puntos > 999) {
					puntos = 999;
				}
			}
			resetPosition = true;
		}
	}
}
void DibujarAuto8() {
	static int startX[] = { 262, 262, 395, 132, 395 }; // Posiciones X posibles
	static int numStartX = sizeof(startX) / sizeof(startX[0]); // Número de posiciones X posibles
	static int currentStartX = startX[rand() % numStartX]; // Posición X actual
	static int startY = 0; // Coordenada Y inicial
	static int endY = 600; // Coordenada Y final
	static int spriteHeight = 42; // Altura del sprite
	static bool resetPosition = false; // Bandera para reiniciar la posición
	static bool initialDelay = true; // Bandera para retraso inicial
	static clock_t lastResetTime = 0; // Variable para almacenar el tiempo del último reinicio

	if (initialDelay && (clock() - lastResetTime) < 10 * CLOCKS_PER_SEC || resetPosition && (clock() - lastResetTime) < 26 * CLOCKS_PER_SEC) {
		return; // Salir de la función si todavía no ha transcurrido el tiempo necesario
	}

	// Reiniciar posición y establecer el tiempo del reinicio
	if (resetPosition) {
		currentStartX = startX[rand() % numStartX];
		resetPosition = false;
	}

	// Establecer el retraso inicial en el primer ciclo
	if (initialDelay) {
		lastResetTime = clock();
		initialDelay = false;
	}

	TranScaleblt(ptrBufferPixelsWindow, (Auto8.AutoSprite8.pixeles),
		currentStartX, startY,
		Auto8.FrameSpriteArray[0][FrameActualA3].x, Auto8.FrameSpriteArray[0][FrameActualA3].y,
		Auto8.FrameSpriteArray[0][FrameActualA3].ancho, spriteHeight,
		870, Auto8.AutoSprite8.ancho, 4, 4);

	Auto8.HitboxX = currentStartX + 12;
	Auto8.HitboxY = startY;
	Auto8.HitboxAlto = spriteHeight;

	//DibujaHitbox(ptrBufferPixelsWindow, 0xFFFF00FF, ANCHO_VENTANA, ALTO_VENTANA,
	//	Auto8.HitboxX, Auto8.HitboxY, Auto8.HitboxAncho, Auto8.HitboxAlto,
	//	Auto8.HitboxScaleX, Auto8.HitboxScaleY);

	startY += 20;
	if (startY > endY) {
		if (spriteHeight > 5) {
			spriteHeight -= 5;
		}
		else {
			startY = 0;
			spriteHeight = 50;
			lastResetTime = clock();
			if (puntos < 999) {
				puntos = puntos + 6;
				if (puntos > 999) {
					puntos = 999;
				}
			}

			resetPosition = true;
		}
	}
}

//Dibujar Gas
void DibujarGas() {
	static int startX[] = { 160, 290, 420, 160, 420 }; // Posiciones X posibles
	static int numStartX = sizeof(startX) / sizeof(startX[0]); // Número de posiciones X posibles
	static int currentStartX = startX[rand() % numStartX]; // Posición X actual
	static int startY = 0; // Coordenada Y inicial
	static int endY = 660; // Coordenada Y final
	static int spriteHeight = 33; // Altura del sprite
	static bool resetPosition = false; // Bandera para reiniciar la posición
	static bool initialDelay = true; // Bandera para retraso inicial
	static clock_t lastResetTime = 0; // Variable para almacenar el tiempo del último reinicio

	if (initialDelay && (clock() - lastResetTime) < 45 * CLOCKS_PER_SEC || resetPosition && (clock() - lastResetTime) < 60 * CLOCKS_PER_SEC) {
		return; // Salir de la función si todavía no ha transcurrido el tiempo necesario
	}

	// Reiniciar posición y establecer el tiempo del reinicio
	if (resetPosition) {
		currentStartX = startX[rand() % numStartX];
		resetPosition = false;
	}

	// Establecer el retraso inicial en el primer ciclo
	if (initialDelay) {
		lastResetTime = clock();
		initialDelay = false;
	}

	TranScaleblt(ptrBufferPixelsWindow, (Gas.GasSprite.pixeles),
		currentStartX, startY,
		Gas.FrameSpriteArray[0][FrameActualGas].x, Gas.FrameSpriteArray[0][FrameActualGas].y,
		Gas.FrameSpriteArray[0][FrameActualGas].ancho, spriteHeight,
		870, Gas.GasSprite.ancho, 2, 2);

	Gas.HitboxX = currentStartX;
	Gas.HitboxY = startY;
	Gas.HitboxAlto = spriteHeight;
	//DibujaHitbox(ptrBufferPixelsWindow, 0xFFFF00FF, ANCHO_VENTANA, ALTO_VENTANA,
	//	currentStartX, startY, Gas.HitboxAncho, spriteHeight,
	//	Gas.HitboxScaleX, Gas.HitboxScaleY);

	startY += 27;
	if (startY > endY) {
		if (spriteHeight > 5) {
			spriteHeight -= 6;
		}
		else {
			startY = 0;
			spriteHeight = 33;
			lastResetTime = clock();
			resetPosition = true;
		}
	}

	if (DetectarColision(miPersonaje.HitboxX, miPersonaje.HitboxY, miPersonaje.HitboxAncho * 4, miPersonaje.HitboxAlto * 4, Gas.HitboxX, Gas.HitboxY, Gas.HitboxAncho * 2, Gas.HitboxAlto * 2)) {
		vida++;
		startY = 0;
		spriteHeight = 33;
		lastResetTime = clock();
		resetPosition = true;
		ReproductorEfecto("Recursos/Gas.mp3");
	}

}

//Guardar Puntajes
void guardararchivo(int newRecord)
{
	ofstream archivo;
	archivo.open("../Plantilla LENS 2023 EX/Puntajes1.txt", ios::out | ios::trunc);
	int indexMin = 0; // índice del puntaje más bajo
	for (int i = 0; i < cont; i++)
	{
		if (archivo.fail())
		{
			exit(1);
		}

		if (punts[i].puntajesArr < punts[indexMin].puntajesArr) {
			indexMin = i;
		}
	}
	if (newRecord >= punts[indexMin].puntajesArr) {
		// reemplazar puntaje más bajo con nuevo puntaje y nombre
		punts[indexMin].puntajesArr = newRecord;
		punts[indexMin].nombresArr[0] = Auxnombre[0];
		punts[indexMin].nombresArr[1] = Auxnombre[1];
		punts[indexMin].nombresArr[2] = Auxnombre[2];
	}
	bool swapped = false;
	do {
		swapped = false;
		for (int i = 0; i < cont - 1; i++) {
			if (punts[i].puntajesArr < punts[i + 1].puntajesArr) {
				swap(punts[i], punts[i + 1]);
				swapped = true;
			}
		}
	} while (swapped);
	for (int i = 0; i < 3; i++)
	{
		archivo << punts[i].nombresArr << endl;
		archivo << punts[i].puntajesArr << endl;
	}
}
void Leerarchivo()
{
	{
		int contleer = 0;
		char c;
		char texto[4];
		ifstream archivo("../Plantilla LENS 2023 EX/Puntajes1.txt", ios::in);
		if (archivo.is_open())
		{

			while (!archivo.eof())
			{

				archivo >> texto;//guardamos los datos del archivo en la variable texto
				if (strcmp(texto, "") != 0)
				{


					if (contleer == 0)
					{
						punts[cont].nombresArr[0] = texto[0];
						punts[cont].nombresArr[1] = texto[1];
						punts[cont].nombresArr[2] = texto[2];
						contleer++;
					}
					else if (contleer == 1)
					{
						punts[cont].puntajesArr = atoi(texto);
						contleer = 0;
						cont++;
					}
				}

			}
			archivo.close();

		}
	}
}

bool compararPuntos(int newRecord) {
	bool superaAlMenosUno = false;
	for (int i = 0; i < 3; i++) {
		if (newRecord >= punts[i].puntajesArr) {
			superaAlMenosUno = true;
			break;
		}
	}
	return superaAlMenosUno;
}

//Reset segun vea
void resetMemory()
{
	pantallaGameOver = false;
	pantallaInicial = true;
	vida = 3;
	RegName = false;
	Pletra = true;
	Sletra = false;
	Tletra = false;
	Letraint = A;
	__asm {
		xor eax, eax
		xor ebx, ebx
		xor ecx, ecx
		xor edx, edx
		xor esi, esi
		xor edi, edi
	}
	ReproductorCambiarCancionYReproduce(1);

	//Init();
}
