
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <sys/timeb.h>
#include <windows.h>
#include <wingdi.h>
#include <winuser.h>
#include <process.h>
#include "resource.h"
#include "globals.h"


#if defined (WIN32)
	#define IS_WIN32	TRUE
#else
	#define IS_WIN32	FALSE
#endif

#define IS_NT	IS_WIN32 && (BOOL)(GetVersion() < 0x80000000)
#define IS_WIN32S	IS_WIN32 && (BOOL)(!(IS_NT) && \
					(LOBYTE(LOWORD(GetVersion()))<4))
#define IS_WIN95	(BOOL)(!(IS_NT) && !(IS_WIN32S)) && IS_WIN32

HINSTANCE				hInst;						// current instance

LPCTSTR lpszTitle="Emerge";

BOOL RegisterWin95(CONST WNDCLASS* lpwc);


int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
				LPTSTR lpCmdLine, int nCmdShow)

{
MSG			msg;
HWND		hWnd;
WNDCLASS	wc;

// In Windows 95 or WIndows NT the hPrevInstance will always be NULL.

wc.style=CS_HREDRAW | CS_VREDRAW;
wc.lpfnWndProc=(WNDPROC)WndProc;
wc.cbClsExtra=0;
wc.cbWndExtra=0;
wc.hInstance=hInstance;
wc.hIcon=LoadIcon(hInstance,"ID_EMERGE_ICON");
wc.hCursor=LoadCursor(NULL,IDC_ARROW);
wc.hbrBackground=(HBRUSH)(COLOR_WINDOW+1);
wc.lpszMenuName="ID_MAIN_MENU";
wc.lpszClassName="EMERGE";

if (IS_WIN95)
  {
  if (!RegisterWin95(&wc))
    return(FALSE);
  }
else if (!RegisterClass(&wc))
  return(FALSE);

hInst=hInstance;

hWnd=CreateWindow("EMERGE",lpszTitle,
		WS_OVERLAPPEDWINDOW | WS_HSCROLL | WS_VSCROLL,
		CW_USEDEFAULT,0,COLS+7,ROWS+25,NULL,NULL,hInstance,NULL);
if (!hWnd)
  return(FALSE);

ShowScrollBar(hWnd,SB_BOTH,FALSE);
ShowWindow(hWnd,nCmdShow);
UpdateWindow(hWnd);
MainWnd=hWnd;

DisplayImage=(unsigned char *)calloc(ROWS*COLS*3,1);	/* RGB image */

		// initialize critters
TotalFood=TotalRobots=TotalSharks=0;
//InitializeCritters();

MakeDisplayImage();
InvalidateRect(hWnd,NULL,TRUE);
UpdateWindow(hWnd);

repeat=0;

while (GetMessage(&msg,NULL,0,0))
  {
  TranslateMessage(&msg);
  DispatchMessage(&msg);
  }
return(msg.wParam);
}



BOOL RegisterWin95(CONST WNDCLASS* lpwc)

{
WNDCLASSEX wcex;

wcex.style=lpwc->style;
wcex.lpfnWndProc=lpwc->lpfnWndProc;
wcex.cbClsExtra=lpwc->cbClsExtra;
wcex.cbWndExtra=lpwc->cbWndExtra;
wcex.hInstance=lpwc->hInstance;
wcex.hIcon=lpwc->hIcon;
wcex.hCursor=lpwc->hCursor;
wcex.hbrBackground=lpwc->hbrBackground;
wcex.lpszMenuName=lpwc->lpszMenuName;
wcex.lpszClassName=lpwc->lpszClassName;
		// Added elements for Windows 95:
wcex.cbSize=sizeof(WNDCLASSEX);
wcex.hIconSm=LoadIcon(wcex.hInstance,"ID_EMERGE_ICON");
return (RegisterClassEx(&wcex));
}



LRESULT CALLBACK WndProc (HWND hWnd, UINT uMsg,
		WPARAM wParam, LPARAM lParam)

{

switch (uMsg)
  {
  case WM_COMMAND:
    switch (LOWORD(wParam))
      {
	  case ID_GO:
		if (repeat != 0)
		  break;
		InitializeCritters();
		MakeDisplayImage();
		InvalidateRect(hWnd,NULL,TRUE);
		UpdateWindow(hWnd);
		repeat=1;
		_beginthread(EmergeThread,0,hWnd);
		break;
	  case ID_PAUSE:
		if (repeat == 2)
		  repeat=1;
		else if (repeat == 1)
		  repeat=2;
		break;
	  case ID_STOP:
		repeat=0;
		break;
	  case ID_EXIT:
		repeat=0;
		Sleep(100);
		free(DisplayImage);
        DestroyWindow(hWnd);
        break;
      }
    break;
  case WM_PAINT:
	if (hWnd != MainWnd  ||  GetUpdateRect(hWnd,NULL,FALSE) == 0)
	  {
      return(DefWindowProc(hWnd,uMsg,wParam,lParam));
	  break;
	  }
	PaintImage();
    return(DefWindowProc(hWnd,uMsg,wParam,lParam));
	break;
  case WM_DESTROY:
    PostQuitMessage(0);
    break;
  default:
    return(DefWindowProc(hWnd,uMsg,wParam,lParam));
    break;
  }


return(0L);
}




void InitializeCritters()

{
int	  i,j,Overlap,Runaway;

srand((unsigned)time(NULL));

TotalFood=5;
TotalSharks=3;
TotalRobots=4;

for (i=0; i<TotalFood; i++)
  {
  FoodRow[i]=(rand() % (ROWS-FoodRadius*2))+FoodRadius;
  FoodCol[i]=(rand() % (COLS-FoodRadius*2))+FoodRadius;
  }
Runaway=0;
for (i=0; i<TotalSharks; i++)
  {
  SharkRow[i]=(rand() % (ROWS-SharkRadius*2))+SharkRadius;
  SharkCol[i]=(rand() % (COLS-SharkRadius*2))+SharkRadius;
  SharkEnergy[i]=255;
  Overlap=0;
  for (j=0; j<i  &&  Overlap == 0; j++)
	{
	if (abs(SharkRow[i]-SharkRow[j]) < SharkRadius*5  &&  abs(SharkCol[i]-SharkCol[j]) < SharkRadius*5)
	  Overlap=1;
	}
  if (Overlap == 1  &&  Runaway < 10000)
	i--;
  Runaway++;
  }
Runaway=0;
for (i=0; i<TotalRobots; i++)
  {
  RobotRow[i]=(rand() % (ROWS-RobotRadius*2))+SharkRadius;
  RobotCol[i]=(rand() % (COLS-RobotRadius*2))+SharkRadius;
  RobotEnergy[i]=255;
  Overlap=0;
  for (j=0; j<i  &&  Overlap == 0; j++)
	{
	if (abs(RobotRow[i]-RobotRow[j]) < RobotRadius*5  &&  abs(RobotCol[i]-RobotCol[j]) < RobotRadius*5)
	  Overlap=1;
	}
  for (j=0; j<TotalSharks  &&  Overlap == 0; j++)
	{
	if (abs(RobotRow[i]-SharkRow[j]) < SharkRadius*8  &&  abs(RobotCol[i]-SharkCol[j]) < SharkRadius*8)
	  Overlap=1;
	}
  if (Overlap == 1  &&  Runaway < 10000)
	i--;
  Runaway++;
  }
}





void PaintImage()

{
PAINTSTRUCT			Painter;
HDC					hDC;
BITMAPINFOHEADER	bm_info_header;
BITMAPINFO			bm_info;
int					i;
SIZE				textsize;
char				text[300];

BeginPaint(MainWnd,&Painter);
hDC=GetDC(MainWnd);
bm_info_header.biSize=sizeof(BITMAPINFOHEADER); 
bm_info_header.biWidth=COLS;
bm_info_header.biHeight=-ROWS; 
bm_info_header.biPlanes=1;
bm_info_header.biBitCount=24; 
bm_info_header.biCompression=BI_RGB; 
bm_info_header.biSizeImage=0; 
bm_info_header.biXPelsPerMeter=0; 
bm_info_header.biYPelsPerMeter=0;
bm_info_header.biClrUsed=0; 
bm_info_header.biClrImportant=0;
// bm_info.bmiColors=NULL;
bm_info.bmiHeader=bm_info_header;
SetDIBitsToDevice(hDC,0,0,COLS,ROWS,0,0,
			  0, /* first scan line */
			  ROWS, /* number of scan lines */
			  DisplayImage,&bm_info,DIB_RGB_COLORS);
for (i=0; i<TotalRobots; i++)
  {
  if (RobotRow[i] == -1  &&  RobotCol[i] == -1)
	continue;
  sprintf(text,"%d",i);
  GetTextExtentPoint32(hDC,text,strlen(text),&textsize);
  TextOut(hDC,RobotCol[i]-textsize.cx/2,RobotRow[i]-textsize.cy/2,text,strlen(text));
  }
ReleaseDC(MainWnd,hDC);
EndPaint(MainWnd,&Painter);
}


void MakeDisplayImage()

{
int	  i,r,c;

for (i=0; i<ROWS*COLS*3; i++)
  DisplayImage[i]=0;

for (i=0; i<TotalFood; i++)
  {
  for (r=FoodRow[i]-FoodRadius; r<=FoodRow[i]+FoodRadius; r++)
	{
	if (r < 0  ||  r >= ROWS)
	  continue;
	for (c=FoodCol[i]-FoodRadius; c<=FoodCol[i]+FoodRadius; c++)
	  {
	  if (c < 0  ||  c >= COLS)
		continue;
	  if (sqrt((double)(SQR(FoodRow[i]-r)+SQR(FoodCol[i]-c))) > (double)FoodRadius)
		continue;
	  DisplayImage[(r*COLS+c)*3+0]=0;
	  DisplayImage[(r*COLS+c)*3+1]=255;
	  DisplayImage[(r*COLS+c)*3+2]=0;
	  }
	}
  }

for (i=0; i<TotalSharks; i++)
  {
  for (r=SharkRow[i]-SharkRadius; r<=SharkRow[i]+SharkRadius; r++)
	{
	if (r < 0  ||  r >= ROWS)
	  continue;
	for (c=SharkCol[i]-SharkRadius; c<=SharkCol[i]+SharkRadius; c++)
	  {
	  if (c < 0  ||  c >= COLS)
		continue;
	  if (sqrt((double)(SQR(SharkRow[i]-r)+SQR(SharkCol[i]-c))) > (double)SharkRadius)
		continue;
	  DisplayImage[(r*COLS+c)*3+0]=0;
	  DisplayImage[(r*COLS+c)*3+1]=0;
	  DisplayImage[(r*COLS+c)*3+2]=SharkEnergy[i];
	  }
	}
  }

for (i=0; i<TotalRobots; i++)
  {
  if (RobotRow[i] == -1  &&  RobotCol[i] == -1)
	continue;
  for (r=RobotRow[i]-RobotRadius; r<=RobotRow[i]+RobotRadius; r++)
	{
	if (r < 0  ||  r >= ROWS)
	  continue;
	for (c=RobotCol[i]-RobotRadius; c<=RobotCol[i]+RobotRadius; c++)
	  {
	  if (c < 0  ||  c >= COLS)
		continue;
	  if (sqrt((double)(SQR(RobotRow[i]-r)+SQR(RobotCol[i]-c))) > (double)RobotRadius)
		continue;
	  DisplayImage[(r*COLS+c)*3+0]=RobotEnergy[i];
	  DisplayImage[(r*COLS+c)*3+1]=100;
	  DisplayImage[(r*COLS+c)*3+2]=100;
	  }
	}
  }


}

