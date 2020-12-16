
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <sys/timeb.h>
#include <windows.h>
#include <wingdi.h>
#include <winuser.h>
#include <process.h>	/* needed for multithreading */
#include "resource.h"
#include "globals.h"

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
				LPTSTR lpCmdLine, int nCmdShow)

{
MSG			msg;
HWND		hWnd;
WNDCLASS	wc;

wc.style=CS_HREDRAW | CS_VREDRAW;
wc.lpfnWndProc=(WNDPROC)WndProc;
wc.cbClsExtra=0;
wc.cbWndExtra=0;
wc.hInstance=hInstance;
wc.hIcon=LoadIcon(hInstance,"ID_PLUS_ICON");
wc.hCursor=LoadCursor(NULL,IDC_ARROW);
wc.hbrBackground=(HBRUSH)(COLOR_WINDOW+1);
wc.lpszMenuName="ID_MAIN_MENU";
wc.lpszClassName="PLUS";

if (!RegisterClass(&wc))
  return(FALSE);

hWnd=CreateWindow("PLUS","plus program",
		WS_OVERLAPPEDWINDOW | WS_HSCROLL | WS_VSCROLL,
		CW_USEDEFAULT,0,400,400,NULL,NULL,hInstance,NULL);
if (!hWnd)
  return(FALSE);

ShowScrollBar(hWnd,SB_BOTH,FALSE);
ShowWindow(hWnd,nCmdShow);
UpdateWindow(hWnd);
MainWnd=hWnd;

ShowPixelCoords=0;
Showactivecontour = 0;
left = 0;
right = 0;
flag_l = 0;
flag_r = 0;
flag_s = 0;
ShowRegionGrowing_P = 0;
ShowRegionGrowing_S = 0;

redflag = 0;
blueflag = 0;
pshift = 0;

strcpy(filename,"");
OriginalImage=NULL;


ROWS=COLS=0;

InvalidateRect(hWnd,NULL,TRUE);
UpdateWindow(hWnd);

while (GetMessage(&msg,NULL,0,0))
  {
  TranslateMessage(&msg);
  DispatchMessage(&msg);
  }
return(msg.wParam);
}

BOOL CALLBACK AboutDlgProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	switch (Message)
	{
	case WM_INITDIALOG:

		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			EndDialog(hWnd, IDOK);
			BOOL bSuccess;
			difference = GetDlgItemInt(hWnd, IDC_EDIT1, &bSuccess, FALSE);
			distance = GetDlgItemInt(hWnd, IDC_EDIT2, &bSuccess, FALSE);
			break;
		case IDCANCEL:
			EndDialog(hWnd, IDCANCEL);
			break;
		}
		break;
	default:
		return FALSE;
	}
	return TRUE;
}


LRESULT CALLBACK WndProc (HWND hWnd, UINT uMsg,
		WPARAM wParam, LPARAM lParam)

{
HMENU				hMenu;
OPENFILENAME		ofn;
FILE				*fpt;
HDC					hDC;
char				header[320],text[320];
int					BYTES, xPos, yPos, ax, ay;
//int					xxPos,yyPos;
int					r, c, r2, c2,x,y;
int					i,j,ii,seq,jj;
//int					RegionSize, TotalRegions;
double				avg, var;
//void				RegionGrow();		
RECT				rc;
static int			cxClient, cyClient, cxBitmap, cyBitmap;
static HBITMAP		bitmap;
static int			iHScrollBarPos, iVScrollBarPos;

switch (uMsg)
  {
  case WM_COMMAND:
    switch (LOWORD(wParam))
      {
	  case ID_SHOWACTIVECONTOUR_START:
		  if (flag_l == 1)
		    _beginthread(ActiveContourExternal, 0, MainWnd);
		  if (flag_r==1)
			_beginthread(ActiveContourBalloon, 0, MainWnd);
		  if (flag_s == 1)
		  {
			  _beginthread(ActiveContourShiftBalloon, 0, MainWnd);
			  _beginthread(ActiveContourShiftInt, 0, MainWnd);
		  }
		  flag_l = 0;
		  flag_r = 0;
		  flag_s = 0;
		  break;

	  case	ID_SHOWACTIVECONTOUR_INIALIZE:
		  Showactivecontour = (Showactivecontour + 1) % 2;
		  PaintImage();
		  break;
	  case ID_SELECTCOLORS_RED:
		  redflag = (redflag + 1) % 2;
		  blueflag = 0;
		  rrr = 255;
		  g = 0;
		  b = 0;
		  break;
	  case ID_SELECTCOLORS_BLUE:
		  blueflag = (blueflag + 1) % 2;
		  redflag = 0;
		  rrr = 0;
		  g = 0;
		  b = 255;
		  break;
	  case ID_DISPLAY_CLEAR:
		  ShowRegionGrowing_P = 0;
		  ShowRegionGrowing_S = 0;
		  ShowRegionGrowing = 0;
		  clear=1;
		  flag_l = 0;
		  flag_r = 0;
		  for (i = 0;i < 5000;i++)
		  {
			  fc[i] = NULL;
			  fr[i] = NULL;
			  ac[i] = NULL;
			  ar[i] = NULL;
			  fc2[i] = NULL;
			  fr2[i] = NULL;
			  ac2[i] = NULL;
			  ar2[i] = NULL;
		  }
		  labels = (unsigned char *)calloc(ROWS*COLS, sizeof(unsigned char));
		  indices = (int *)calloc(ROWS*COLS, sizeof(int));
		  PaintImage();
		  
		  break;
	  case ID_SHOWPIXELCOORDS:
		ShowPixelCoords=(ShowPixelCoords+1)%2;
		PaintImage();
		break;
	  case ID_SHOWREGIONGROWING_PLAYMODE:
		  ShowRegionGrowing_S = 0;
		  ShowRegionGrowing_P = (ShowRegionGrowing_P + 1) % 2;
		  if (ShowRegionGrowing_P==0)
		  {
			  ShowRegionGrowing = 0;
			  step = 1;
		  }
		  if (ShowRegionGrowing_P == 1)
		  {
			  ShowRegionGrowing = 1;
			  step = 0;
		  }
		  break;
	  case ID_SHOWREGIONGROWING_STEPMODE:
		  ShowRegionGrowing_P = 0;
		  ShowRegionGrowing_S = (ShowRegionGrowing_S + 1) % 2;
		  if (ShowRegionGrowing_S == 0)
		  {
			  ShowRegionGrowing = 0;
			  step = 1;
		  }
		  if (ShowRegionGrowing_S == 1)
		  {
			  ShowRegionGrowing = 1;
			  step = 1;
		  }
		  break;
	  case ID_FILE_LOAD:
		if (OriginalImage != NULL)
		  {
		  free(OriginalImage);
		  OriginalImage=NULL;
		  }

		if (DisplayImage != NULL)
		{
			free(DisplayImage);
			DisplayImage = NULL;
		}

		type = 0;
		memset(&(ofn),0,sizeof(ofn));
		ofn.lStructSize=sizeof(ofn);
		ofn.lpstrFile=filename;
		filename[0]=0;
		ofn.nMaxFile=MAX_FILENAME_CHARS;
		ofn.Flags=OFN_EXPLORER | OFN_HIDEREADONLY;
		ofn.lpstrFilter = "PPM files\0*.ppm\0All files\0*.*\0\0";
		if (!( GetOpenFileName(&ofn))  ||  filename[0] == '\0')
		  break;		/* user cancelled load */
		if ((fpt=fopen(filename,"rb")) == NULL)
		  {
		  MessageBox(NULL,"Unable to open file",filename,MB_OK | MB_APPLMODAL);
		  break;
		  }
		fscanf(fpt,"%s %d %d %d",header,&COLS,&ROWS,&BYTES);
		
		if ((strcmp(header,"P5") && strcmp(header, "P6"))!= 0  ||  BYTES != 255)
		  {
		  MessageBox(NULL,"Not a PPM (P5 greyscale or P6 RGB) image",filename,MB_OK | MB_APPLMODAL);
		  fclose(fpt);
		  break;
		  }
		 
		if ( strcmp(header, "P6") == 0)
		{
			type = 1;
			OriginalImage = (unsigned char *)calloc(ROWS*COLS*3, sizeof(unsigned char));
			header[0] = fgetc(fpt);	/* whitespace character after header */
			fread(OriginalImage, 1, ROWS*COLS*3, fpt);
			fclose(fpt);
			SetWindowText(hWnd, filename);
			PaintImage();
			break;
		}

		OriginalImage=(unsigned char *)calloc(ROWS*COLS, sizeof(unsigned char));
		labels = (unsigned char *)calloc(ROWS*COLS, sizeof(unsigned char));
		indices = (int *)calloc(ROWS*COLS, sizeof(int));
		header[0]=fgetc(fpt);	/* whitespace character after header */
		fread(OriginalImage,1,ROWS*COLS,fpt);
		fclose(fpt);
		SetWindowText(hWnd,filename);
		PaintImage();
		GetObject(OriginalImage, sizeof(BITMAP), &bitmap);

		break;
	
      case ID_FILE_QUIT:
        DestroyWindow(hWnd);
        break;

	  case ID_SHOWREGIONGROWING_SELECTPREDICATEVALUES:
		  {
			  int ret = DialogBox(GetModuleHandle(NULL),
				  MAKEINTRESOURCE(IDD_DIALOG1), hWnd, AboutDlgProc);
			  if (ret == IDOK) {
				  MessageBox(hWnd, "Predicate selected", "Notice",
					  MB_OK | MB_ICONINFORMATION);
			  }
			  else if (ret == IDCANCEL) {
				  MessageBox(hWnd, "Predicate not selected", "Notice",
					  MB_OK | MB_ICONINFORMATION);
			  }
			  else if (ret == -1) {
				  MessageBox(hWnd, "Dialog failed!", "Error",
					  MB_OK | MB_ICONINFORMATION);
			  }
			  break;
		  }
      }
    break;
  case WM_SIZE:		  /* could be used to detect when window size changes */
	GetClientRect(hWnd,&rc);
	width = rc.right - rc.left;
	height = rc.bottom - rc.top;
	PaintImage();
	cxClient = LOWORD(lParam);
	cyClient = HIWORD(lParam);
	SetScrollRange(hWnd, SB_HORZ, 0, COLS - cxClient, FALSE);
	SetScrollRange(hWnd, SB_VERT, 0, ROWS - cyClient, FALSE);

	iHScrollBarPos = min(cxBitmap - cxClient, max(0, iHScrollBarPos));
	iVScrollBarPos = min(cyBitmap - cyClient, max(0, iHScrollBarPos));

	if (iHScrollBarPos != GetScrollPos(hWnd, SB_HORZ))
	{
		SetScrollPos(hWnd, SB_HORZ, iHScrollBarPos, TRUE);
		InvalidateRect(hWnd, NULL, FALSE);
	}
	if (iVScrollBarPos != GetScrollPos(hWnd, SB_VERT))
	{
		SetScrollPos(hWnd, SB_VERT, iVScrollBarPos, SB_VERT);
		InvalidateRect(hWnd, NULL, FALSE);
	}
    return(DefWindowProc(hWnd,uMsg,wParam,lParam));
	break;
  case WM_PAINT:
	PaintImage();
	HDC hdc, hdcSrc;
	PAINTSTRUCT ps;
	hdc = BeginPaint(hWnd, &ps);
	hdcSrc = CreateCompatibleDC(hdc);
	SelectObject(hdcSrc, bitmap);
	BitBlt(hdc, -iHScrollBarPos, -iVScrollBarPos, COLS, ROWS, hdcSrc, 0, 0, SRCCOPY);
	EndPaint(hWnd, &ps);
	DeleteDC(hdcSrc);
    return(DefWindowProc(hWnd,uMsg,wParam,lParam));
	break;

  case WM_RBUTTONDOWN:

	  if (Showactivecontour == 1)
	  {
		  ccenter = LOWORD(lParam);
		  rcenter = HIWORD(lParam);
		  right = 1;
	  }
	  return(DefWindowProc(hWnd, uMsg, wParam, lParam));
	  break;

  case WM_RBUTTONUP:
	  if (pshift == 0 && Showactivecontour == 1 && right==1)
	  {
		  hDC = GetDC(MainWnd);
		  /*
		  for (i = -1;i < 1;i++)
			  for (ii = -1;ii < 1;ii++)
				  SetPixel(hDC, ccenter + i, rcenter + ii, RGB(255, 0, 0));
		  
		  Sleep(100);
		  PaintImage();
		  */
		  seq = 0;
		  
		  for (i=-10;i<=10;i++)
			  {		
				  ac2[seq] = ccenter + i;
				  ar2[seq] = rcenter - sqrt(100 - pow(i, 2));
				  seq = seq + 1;
			  }

		  for (i = 10;i >= -9;i--)
			  {
				  ac2[seq] = ccenter + i;
				  ar2[seq] = rcenter + sqrt(100 - pow(i, 2));
				  seq = seq + 1;
			  }


		  for (j = 0;j < 5000;j = j + 3)
		  {
			  if (ac2[j] != NULL)
			  {
				  fc2[j / 3] = ac2[j];
				  fr2[j / 3] = ar2[j];
				  for (i = -1;i < 1;i++)
					  for (ii = -1;ii < 1;ii++)
						  SetPixel(hDC, fc2[j/3] + i, fr2[j/3] + ii, RGB(255, 0, 0));	/* color the cursor position red */
			  }
			  else {
				  NN = j / 3;
				  flag_r = 1;
				  break;
			  }

		  }

		  ReleaseDC(MainWnd, hDC);
	  }
	  right = 0;
	  return(DefWindowProc(hWnd, uMsg, wParam, lParam));
	  break;

  case WM_LBUTTONDOWN:

	if (Showactivecontour == 1)
			left = 1;

	if (ShowRegionGrowing==1)
		{
		yyPos = LOWORD(lParam);
		xxPos = HIWORD(lParam);

		new_label =255;
		//RegionGrow(OriginalImage, labels, ROWS, COLS, yyPos, xxPos, 0, TotalRegions, indices, &RegionSize);
		_beginthread(RegionGrow, 0, MainWnd);
		if (count < 100)
		{	
			for (i = 0; i < count; i++)
				labels[indices[i]] = 0;
			new_label--;
		}
		else
			printf("Region labeled %d is %d in size\n", new_label, count);	
		}
	
    return(DefWindowProc(hWnd,uMsg,wParam,lParam));
	break;

  case WM_LBUTTONUP:

	  
	  if (pshift == 0 && Showactivecontour == 1 && left==1)
	  {
		  PaintImage();
		  for (j = 0;j < 5000;j = j + 5)
		  {
			  if (ac[j] != NULL)
			  {
				  fc[j / 5] = ac[j];
				  fr[j / 5] = ar[j];
				  hDC = GetDC(MainWnd);
				  for (i = -1;i < 1;i++)
					  for (ii = -1;ii < 1;ii++)
						SetPixel(hDC, fc[j / 5]+i, fr[j / 5]+ii, RGB(255, 0, 0));	/* color the cursor position red */
				  ReleaseDC(MainWnd, hDC);
			  }
			  else {
				  N = j / 5;
				  flag_l = 1;
				  break;
			  }
			  
		  }

	  }
	  left = 0;
	  return(DefWindowProc(hWnd, uMsg, wParam, lParam));
	  break;

  case WM_MOUSEMOVE:
	if (ShowPixelCoords == 1)
	  {
	  xPos=LOWORD(lParam);
	  yPos=HIWORD(lParam);
	  if (xPos >= 0  &&  xPos < COLS  &&  yPos >= 0  &&  yPos < ROWS)
		{
		sprintf(text,"%d,%d=>%d     ",xPos,yPos,OriginalImage[yPos*COLS+xPos]);
		hDC=GetDC(MainWnd);
		TextOut(hDC,0,0,text,strlen(text));		/* draw text on the window */
		SetPixel(hDC,xPos,yPos,RGB(255,0,0));	/* color the cursor position red */
		ReleaseDC(MainWnd,hDC);
		}
	  }

	if (pshift == 0 && Showactivecontour == 1 && left==1)
	{
		ax = LOWORD(lParam);
		ay = HIWORD(lParam);
		if (ax >= 0 && ax < COLS  &&  ay >= 0 && ay < ROWS)
		{
			for (j = 0;j < 5000;j++)
			{
				if (ac[j] == NULL)
				{
					ac[j] = ax;
					ar[j] = ay;
					break;
				}
			}
			sprintf(text, "%d,%d=>%d     ", ac[j], ar[j], DisplayImage[ay*DISPLAY_COLS + ax]);
			hDC = GetDC(MainWnd);
			TextOut(hDC, 0, 0, text, strlen(text));		/* draw text on the window */
			for (i = -1;i < 1;i++)
				for (j = -1;j < 1;j++)
					SetPixel(hDC, ax+i, ay+j, RGB(255, 0, 0));	/* color the cursor position red */
			ReleaseDC(MainWnd, hDC);
		}
	}

	if ((pshift ==1) && (right==1 || left==1) && (Showactivecontour == 1))
	{
		x = LOWORD(lParam);
		y = HIWORD(lParam);
		for (i = -2;i <= 2;i++)		
			for (j = -2;j <= 2;j++)
			{
				for (ii = 0;ii < N;ii++)
				{
					if (x + i == fc[ii] && y + j == fr[ii])
					{
						fc[ii] = x;
						fr[ii] = y;
						lfix = ii;
					}
				}

				for (jj = 0;jj < NN;jj++)
				{
					if (x + i == fc2[jj] && y + j == fr2[jj])
					{
						fc2[jj] = x;
						fr2[jj] = y;
						rfix = jj;
					}
				}
		
			}

		PaintImage();
		flag_s = 1;
	}

    return(DefWindowProc(hWnd,uMsg,wParam,lParam));
	break;

  case WM_KEYUP:
	  pshift = 0;
	  return(DefWindowProc(hWnd, uMsg, wParam, lParam));
	  break;

  case WM_KEYDOWN:
	if (wParam == 's'  ||  wParam == 'S')
	  PostMessage(MainWnd,WM_COMMAND,ID_SHOWPIXELCOORDS,0);	  /* send message to self */
	if (wParam == 'j' || wParam == 'J')
		step = (step + 1) % 2;
	if ((TCHAR)wParam == '1')
	  {
	  TimerRow=TimerCol=0;
	  SetTimer(MainWnd,TIMER_SECOND,10,NULL);	/* start up 10 ms timer */
	  }
	if ((TCHAR)wParam == '2')
	  {
	  KillTimer(MainWnd,TIMER_SECOND);			/* halt timer, stopping generation of WM_TIME events */
	  PaintImage();								/* redraw original image, erasing animation */
	  }
	if ((TCHAR)wParam == '3')
	  {
	  ThreadRunning=1;
	  _beginthread(AnimationThread,0,MainWnd);	/* start up a child thread to do other work while this thread continues GUI */
	  }
 	if ((TCHAR)wParam == '4')
	  {
	  ThreadRunning=0;							/* this is used to stop the child thread (see its code below) */
	  }

	if (wParam == SHIFT_PRESSED)
	{
		pshift = 1;
	}

	return(DefWindowProc(hWnd,uMsg,wParam,lParam));
	break;

  case WM_TIMER:	  /* this event gets triggered every time the timer goes off */
	hDC=GetDC(MainWnd);
	SetPixel(hDC,TimerCol,TimerRow,RGB(0,0,255));	/* color the animation pixel blue */
	ReleaseDC(MainWnd,hDC);
	TimerRow++;
	TimerCol+=2;
	break;
  case WM_HSCROLL:	  /* this event could be used to change what part of the image to draw */
	PaintImage();	  /* direct PaintImage calls eliminate flicker; the alternative is InvalidateRect(hWnd,NULL,TRUE); UpdateWindow(hWnd); */
    return(DefWindowProc(hWnd,uMsg,wParam,lParam));
	break;
  case WM_VSCROLL:	  /* this event could be used to change what part of the image to draw */
	  switch (LOWORD(wParam))
	  {
	  case SB_LINEUP:

		  iVScrollBarPos -= 10;
		  break;
	  case SB_LINEDOWN:
		  iVScrollBarPos += 10;
		  break;
	  case SB_PAGEUP:

		  iVScrollBarPos -= cyClient;
		  break;
	  case SB_PAGEDOWN:
		  iVScrollBarPos += cyClient;
		  break;
	  case SB_THUMBTRACK:
		  iVScrollBarPos = HIWORD(wParam);
		  break;
	  default:
		  break;
	  }
	  iVScrollBarPos = min(cyBitmap - cyClient, max(0, iVScrollBarPos));
	  if (iVScrollBarPos != GetScrollPos(hWnd, SB_VERT))
	  {
		  SetScrollPos(hWnd, SB_VERT, iVScrollBarPos, TRUE);
		  InvalidateRect(hWnd, NULL, FALSE);
	  }
	return(DefWindowProc(hWnd,uMsg,wParam,lParam));
	break;
  case WM_DESTROY:
    PostQuitMessage(0);
    break;
  default:
    return(DefWindowProc(hWnd,uMsg,wParam,lParam));
    break;
  }

hMenu=GetMenu(MainWnd);
if (ShowPixelCoords == 1)
  CheckMenuItem(hMenu,ID_SHOWPIXELCOORDS,MF_CHECKED);	/* you can also call EnableMenuItem() to grey(disable) an option */
else
  CheckMenuItem(hMenu,ID_SHOWPIXELCOORDS,MF_UNCHECKED);

if (Showactivecontour == 1)
CheckMenuItem(hMenu, ID_SHOWACTIVECONTOUR_INIALIZE, MF_CHECKED);	/* you can also call EnableMenuItem() to grey(disable) an option */
else
CheckMenuItem(hMenu, ID_SHOWACTIVECONTOUR_INIALIZE, MF_UNCHECKED);



if (ShowRegionGrowing_P == 1)
{
	CheckMenuItem(hMenu, ID_SHOWREGIONGROWING_PLAYMODE, MF_CHECKED);	/* you can also call EnableMenuItem() to grey(disable) an option */
	CheckMenuItem(hMenu, ID_SHOWREGIONGROWING_STEPMODE, MF_UNCHECKED);	/* you can also call EnableMenuItem() to grey(disable) an option */
}
else
CheckMenuItem(hMenu, ID_SHOWREGIONGROWING_PLAYMODE, MF_UNCHECKED);

if (ShowRegionGrowing_S == 1)
{
	CheckMenuItem(hMenu, ID_SHOWREGIONGROWING_STEPMODE, MF_CHECKED);	/* you can also call EnableMenuItem() to grey(disable) an option */
	CheckMenuItem(hMenu, ID_SHOWREGIONGROWING_PLAYMODE, MF_UNCHECKED);	/* you can also call EnableMenuItem() to grey(disable) an option */
}
else
CheckMenuItem(hMenu, ID_SHOWREGIONGROWING_STEPMODE, MF_UNCHECKED);

if (redflag == 1)
{
	CheckMenuItem(hMenu, ID_SELECTCOLORS_RED, MF_CHECKED);	/* you can also call EnableMenuItem() to grey(disable) an option */
	CheckMenuItem(hMenu, ID_SELECTCOLORS_BLUE, MF_UNCHECKED);	/* you can also call EnableMenuItem() to grey(disable) an option */
}
else
CheckMenuItem(hMenu, ID_SELECTCOLORS_RED, MF_UNCHECKED);

if (blueflag == 1)
{
	CheckMenuItem(hMenu, ID_SELECTCOLORS_BLUE, MF_CHECKED);	/* you can also call EnableMenuItem() to grey(disable) an option */
	CheckMenuItem(hMenu, ID_SELECTCOLORS_RED, MF_UNCHECKED);	/* you can also call EnableMenuItem() to grey(disable) an option */
}
else
CheckMenuItem(hMenu, ID_SELECTCOLORS_BLUE, MF_UNCHECKED);

DrawMenuBar(hWnd);

return(0L);
}




void PaintImage()

{
PAINTSTRUCT			Painter;
HDC					hDC;
BITMAPINFOHEADER	bm_info_header;
BITMAPINFO			*bm_info;
int					i,r,c,j,ii;
int					average,h,w;

if (OriginalImage == NULL)
  return;		/* no image to draw */


		/* Windows pads to 4-byte boundaries.  We have to round the size up to 4 in each dimension, filling with black. */

DISPLAY_ROWS = height;
DISPLAY_COLS = width;

h = ROWS / max(height,1);
w = COLS / max(width,1);

if (h == 0)
{
	h = 1;
	DISPLAY_ROWS = ROWS;
	height = ROWS;
}

if (w == 0)
{
	w = 1;
	DISPLAY_COLS = COLS;
	width = COLS;
}

if (DISPLAY_ROWS % 4 != 0)
  DISPLAY_ROWS=(DISPLAY_ROWS/4+1)*4;
if (DISPLAY_COLS % 4 != 0)
  DISPLAY_COLS=(DISPLAY_COLS/4+1)*4;
DisplayImage=(unsigned char *)calloc(DISPLAY_ROWS*DISPLAY_COLS, sizeof(unsigned char));


if (type==1)
{
	for (r = 0; r < height; r++)
		for (c = 0; c < width; c++)
		{
			average = 0;
			for (int k = 0;k < 3;k++)
				average = average + (int)OriginalImage[r*h*COLS*3 + c *w* 3 + k];
			DisplayImage[r*DISPLAY_COLS + c] = average / 3;
		}
}
else {
	for (r = 0; r < height; r++)
		for (c = 0; c < width; c++)
		{
			DisplayImage[r*DISPLAY_COLS + c]= OriginalImage[r*h*COLS  + c*w];
		}
}

BeginPaint(MainWnd,&Painter);
hDC=GetDC(MainWnd);
bm_info_header.biSize=sizeof(BITMAPINFOHEADER); 
bm_info_header.biWidth=DISPLAY_COLS;
bm_info_header.biHeight=-DISPLAY_ROWS; 
bm_info_header.biPlanes=1;
bm_info_header.biBitCount=8; 
bm_info_header.biCompression=BI_RGB; 
bm_info_header.biSizeImage=0; 
bm_info_header.biXPelsPerMeter=0; 
bm_info_header.biYPelsPerMeter=0;
bm_info_header.biClrUsed=256;
bm_info_header.biClrImportant=256;
bm_info=(BITMAPINFO *)calloc(1,sizeof(BITMAPINFO) + 256*sizeof(RGBQUAD));
bm_info->bmiHeader=bm_info_header;
for (i=0; i<256; i++)
  {
  bm_info->bmiColors[i].rgbBlue=bm_info->bmiColors[i].rgbGreen=bm_info->bmiColors[i].rgbRed=i;
  bm_info->bmiColors[i].rgbReserved=0;
  } 

SetDIBitsToDevice(hDC,0,0,DISPLAY_COLS,DISPLAY_ROWS,0,0,
			  0, /* first scan line */
			  DISPLAY_ROWS, /* number of scan lines */
			  DisplayImage,bm_info,DIB_RGB_COLORS);




ReleaseDC(MainWnd,hDC);
if (indices != NULL)
{
	for (int j = 1;j <= count;j++)
	{
		hDC = GetDC(MainWnd);
		SetPixel(hDC, indices[j] % COLS, indices[j] / COLS, RGB(rrr, g, b));
		ReleaseDC(MainWnd, hDC);
	}
}
EndPaint(MainWnd,&Painter);



if (Showactivecontour == 1)
{
	hDC = GetDC(MainWnd);
	for (i = 0;i < N;i++)
	{
		for (j = -1;j < 1;j++)
			for (ii = -1;ii < 1;ii++)
			{
				SetPixel(hDC, fc[i] + ii, fr[i] + j, RGB(0, 0, 225));	/* color the cursor position blue */
			}
				
	}
	ReleaseDC(MainWnd, hDC);

	hDC = GetDC(MainWnd);
	for (i = 0;i < NN;i++)
	{

		for (j = -2;j < 2;j++)
			for (ii = -2;ii < 2;ii++)
			{
				SetPixel(hDC, fc2[i] + ii, fr2[i] + j, RGB(0, 255, 0));	/* color the cursor position blue */
			}
				
	}
	ReleaseDC(MainWnd, hDC);

}
	

//free(DisplayImage);
free(bm_info);
}



void AnimationThread(HWND AnimationWindowHandle)

{
HDC		hDC;
char	text[300];

ThreadRow=ThreadCol=0;
while (ThreadRunning == 1)
  {
  hDC=GetDC(MainWnd);
  SetPixel(hDC,ThreadCol,ThreadRow,RGB(0,255,0));	/* color the animation pixel green */
  sprintf(text,"%d,%d     ",ThreadRow,ThreadCol);
  TextOut(hDC,300,0,text,strlen(text));		/* draw text on the window */
  ReleaseDC(MainWnd,hDC);
  ThreadRow+=3;
  ThreadCol++;
  Sleep(100);		/* pause 100 ms */
  }
}


#define MAX_QUEUE 10000	/* max perimeter size (pixels) of border wavefront */

void RegionGrow()		/* output:  count of pixels painted */
{
	
		int	r2, c2;
		int	queue[MAX_QUEUE], qh, qt;
		int	average, total;	/* average and total intensity in growing region */
		int rr, cc, new_x, new_y;
		HDC		hDC;

		count = 0;
		if (labels[xxPos*COLS + yyPos] != paint_over_label)
			return;
		labels[xxPos*COLS + yyPos] = new_label;
		average = total = (int)OriginalImage[xxPos*COLS + yyPos];
		if (indices != NULL)
			indices[0] = xxPos * COLS + yyPos;
		rr = new_x = xxPos;
		cc = new_y = yyPos;

		queue[0] = xxPos * COLS + yyPos;
		qh = 1;	/* queue head */
		qt = 0;	/* queue tail */
		(count) = 1;
		while (qt != qh)
		{

			if ((step == 0 && ShowRegionGrowing==1 )|| (ShowRegionGrowing_P==1&& ShowRegionGrowing==1))
			{
				if ((count) % 1 == 0)	/* recalculate average after each 50 pixels join */
				{
					average = total / (count);
					// printf("new avg=%d\n",average);

				}
				if ((count) % 1 == 0)	/* recalculate average after each 200 pixels join */
				{

					rr = new_x / (count);
					cc = new_y / (count);
				}


				for (r2 = -1; r2 <= 1; r2++)
				{
					for (c2 = -1; c2 <= 1; c2++)
					{

						if (sqrt(SQR(rr - (queue[qt] / COLS + r2)) + SQR(cc - (queue[qt] % COLS + c2))) > distance)
							continue;
						if (r2 == 0 && c2 == 0)
							continue;
						if ((queue[qt] / COLS + r2) < 0 || (queue[qt] / COLS + r2) >= ROWS ||
							(queue[qt] % COLS + c2) < 0 || (queue[qt] % COLS + c2) >= COLS)
							continue;
						if (labels[(queue[qt] / COLS + r2)*COLS + queue[qt] % COLS + c2] != paint_over_label)
							continue;
						/* test criteria to join region */
						if (abs((int)(OriginalImage[(queue[qt] / COLS + r2)*COLS + queue[qt] % COLS + c2])
							- average) > difference)
							continue;



						labels[(queue[qt] / COLS + r2)*COLS + queue[qt] % COLS + c2] = new_label;
						if (indices != NULL)
							indices[count] = (queue[qt] / COLS + r2)*COLS + queue[qt] % COLS + c2;
						total += OriginalImage[(queue[qt] / COLS + r2)*COLS + queue[qt] % COLS + c2];


						(count)++;
						new_x = new_x + queue[qt] / COLS + r2;
						new_y = new_y + queue[qt] % COLS + c2;
						Sleep(1);
						hDC = GetDC(MainWnd);

						SetPixel(hDC, queue[qt] % COLS + c2, queue[qt] / COLS + r2, RGB(rrr,g,b));	/* color the cursor position red */
						ReleaseDC(MainWnd, hDC);
						queue[qh] = (queue[qt] / COLS + r2)*COLS + queue[qt] % COLS + c2;
						qh = (qh + 1) % MAX_QUEUE;
						/*
						if (qh == qt)
						{
							printf("Max queue size exceeded\n");
							exit(0);
						}
						*/
					}
					qt = (qt + 1) % MAX_QUEUE;
				}
				if (ShowRegionGrowing_S == 1)
				{
					step = 1;
				}
			}
			if (ShowRegionGrowing_S == 0 && ShowRegionGrowing_P == 0 && clear == 1)
			{
				clear = 0;
				break;
			}
		}
	
}


void ActiveContourExternal(HWND AnimationWindowHandle)
{	
	FILE	*fpt;
	unsigned char	*sobel;
	double		*test;
	unsigned char	*fcontour;
	signed char 	f1[] = { -1,0,1,-2,0,2,-1,0,1 };
	signed char 	f2[] = { -1,-2,-1,0,0,0,1,2,1 };
	int		c1, r1, c2, r2, k;
	int		i, j, ii, jj, countt,windowsize;
	double		sum1, sum2, grad, av_dis, sum_dis, av_color, sum_color, av_grad, sum_grad, min_ex1, min_ex2, min_ex3, min_in1, min_in2, max_ex, max_ex2, max_ex3, max_in1, max_in2, min;
	double		*ExternalEnergy1, *ExternalEnergy2, *ExternalEnergy3,*InternalEnergy1, *InternalEnergy2, *en;
	double		min_t, max_t;
	int			min_fc, max_fc, min_fr, max_fr;
	double		wcolor;
	char	text[320];
	HDC		hDC;
	

	test = (double *)calloc(DISPLAY_COLS*DISPLAY_ROWS, sizeof(double));
	sobel = (unsigned char *)calloc(DISPLAY_COLS*DISPLAY_ROWS, sizeof(unsigned char));
	fcontour = (unsigned char *)calloc(DISPLAY_COLS*DISPLAY_ROWS, sizeof(unsigned char));
	ExternalEnergy1 = (double *)malloc(1000 * sizeof(double));
	ExternalEnergy2 = (double *)malloc(1000 * sizeof(double));
	ExternalEnergy3 = (double *)malloc(1000 * sizeof(double));
	InternalEnergy1 = (double *)malloc(1000 * sizeof(double));
	InternalEnergy2 = (double *)malloc(1000 * sizeof(double));
	en = (double *)malloc(1000 * sizeof(double));

	windowsize = 19;

	for (r1 = 1; r1 < DISPLAY_ROWS - 1; r1++)
		for (c1 = 1; c1 < DISPLAY_COLS - 1; c1++)
		{
			sum1 = 0;
			sum2 = 0;
			for (r2 = -1; r2 <= 1; r2++)
				for (c2 = -1; c2 <= 1; c2++)
				{
					sum1 += DisplayImage[(r1 + r2)*DISPLAY_COLS + (c1 + c2)] * f1[(r2 + 1) * 3 + 1 + c2];
					sum2 += DisplayImage[(r1 + r2)*DISPLAY_COLS + (c1 + c2)] * f2[(r2 + 1) * 3 + 1 + c2];
				}
			grad = sqrt(pow(sum1, 2) + pow(sum2, 2));
			printf("%f\n", grad);
			test[r1*DISPLAY_COLS + c1] = grad;
		}
	

	min_t = sobel[0];
	max_t = sobel[0];

	for (i = 0; i < DISPLAY_ROWS * DISPLAY_COLS; i++)
	{
		if (test[i] > max_t)
		{
			max_t = test[i];
		}
		if (test[i] < min_t)
		{
			min_t = test[i];
		}
	}

	for (i = 0; i < DISPLAY_ROWS*DISPLAY_COLS; i++)
	{
		sobel[i] = (test[i] - min_t)*255/(max_t-min_t);
	}

	min_fc = fc[0];
	min_fr = fr[0];
	max_fc = fc[0];
	max_fr = fr[0];
	for (i = 0;i < N;i++)
	{
		if (fc[i] < min_fc)
			min_fc = fc[i];
		if (fr[i] < min_fr)
			min_fr = fr[i];
		if (fc[i] > max_fc)
			max_fc = fc[i];
		if (fr[i] > max_fr)
			max_fr = fr[i];
	}
	sum_color = 0;
	for (i=-20;i<=20;i++)
		for (j = -20;j <= 20;j++)
		{
			sum_color = sum_color + DisplayImage[((min_fr+max_fr)/2+i)*DISPLAY_COLS+(min_fc+max_fc)/2+j];
		}
	av_color = sum_color / pow(41, 2);

	for (jj = 0;jj < 41;jj++)
	{
		fc[N] = fc[0];
		fr[N] = fr[0];
		sum_dis = 0;
		sum_grad = 0;
		for (i = 0;i < N;i++)
		{
			sum_dis = sum_dis + sqrt(pow(fr[i] - fr[i + 1], 2) + pow(fc[i] - fc[i + 1], 2));
			sum_grad = sum_grad + sobel[fr[i]*DISPLAY_COLS + fc[i]];
		}
		av_dis = sum_dis / N;
		av_grad = sum_grad / N;

		for (i = 0;i < N;i++)
		{
			k = 0;
			for (r2 = -windowsize/2; r2 <= windowsize/2; r2++)
				for (c2 = -windowsize/2; c2 <= windowsize/2; c2++)
				{
					if ((fr[i] + r2) < 0 || (fr[i] + r2) >= DISPLAY_ROWS || (fc[i] + c2) < 0 || (fc[i] + c2) >= DISPLAY_COLS)
					{
						ExternalEnergy1[k] = 900;
						ExternalEnergy2[k] = 900;
						ExternalEnergy3[k] = 900;
						InternalEnergy1[k] = 900;
						InternalEnergy2[k] = 900;
						k++;
						continue;
					}
					ExternalEnergy1[k] = pow(sobel[(fr[i] + r2)*DISPLAY_COLS + fc[i] + c2], 2);
					ExternalEnergy2[k] = pow(DisplayImage[(fr[i] + r2)*DISPLAY_COLS + fc[i] + c2]-av_color,2);
					ExternalEnergy3[k] = pow(sobel[(fr[i] + r2)*DISPLAY_COLS + fc[i] + c2]-av_grad,2);
					InternalEnergy1[k] = pow(fr[i] + r2 - fr[i + 1], 2) + pow(fc[i] + c2 - fc[i + 1], 2);
					InternalEnergy2[k] = pow((pow(fr[i] + r2 - fr[i + 1], 2) + pow(fc[i] + c2 - fc[i + 1], 2)) - av_dis, 2);
					k++;
				}

			min_ex1 = ExternalEnergy1[0];
			min_ex2 = ExternalEnergy2[0];
			min_ex3 = ExternalEnergy3[0];
			min_in1 = InternalEnergy1[0];
			min_in2 = InternalEnergy2[0];
			for (ii = 0; ii < pow(windowsize,2);ii++)
			{
				if (ExternalEnergy3[ii] < min_ex3)
					min_ex3 = ExternalEnergy3[ii];
				if (ExternalEnergy2[ii] < min_ex2)
					min_ex2 = ExternalEnergy2[ii];
				if (ExternalEnergy1[ii] < min_ex1)
					min_ex1 = ExternalEnergy1[ii];
				if (InternalEnergy1[ii] < min_in1)
					min_in1 = InternalEnergy1[ii];
				if (InternalEnergy2[ii] < min_in2)
					min_in2 = InternalEnergy2[ii];
			}

			for (ii = 0; ii < pow(windowsize, 2);ii++)
			{
				ExternalEnergy1[ii] = ExternalEnergy1[ii] - min_ex1;
				ExternalEnergy2[ii] = ExternalEnergy2[ii] - min_ex2;
				ExternalEnergy3[ii] = ExternalEnergy3[ii] - min_ex3;
				InternalEnergy1[ii] = InternalEnergy1[ii] - min_in1;
				InternalEnergy2[ii] = InternalEnergy2[ii] - min_in2;
			}

			max_ex = ExternalEnergy1[0];
			max_ex2 = ExternalEnergy2[0];
			max_ex3 = ExternalEnergy3[0];
			max_in1 = InternalEnergy1[0];
			max_in2 = InternalEnergy2[0];
			for (ii = 0; ii < pow(windowsize, 2);ii++)
			{
				if (ExternalEnergy3[ii] > max_ex3)
					max_ex3 = ExternalEnergy3[ii];
				if (ExternalEnergy2[ii] > max_ex2)
					max_ex2 = ExternalEnergy2[ii];
				if (ExternalEnergy1[ii] > max_ex)
					max_ex = ExternalEnergy1[ii];
				if (InternalEnergy1[ii] > max_in1)
					max_in1 = InternalEnergy1[ii];
				if (InternalEnergy2[ii] > max_in2)
					max_in2 = InternalEnergy2[ii];
			}
			for (ii = 0; ii < pow(windowsize, 2);ii++)
			{
				if (max_ex3 != 0)
					ExternalEnergy3[ii] = ExternalEnergy3[ii] / max_ex3;
				if (max_ex2 != 0)
					ExternalEnergy2[ii] = ExternalEnergy2[ii] / max_ex2;
				if (max_ex != 0)
					ExternalEnergy1[ii] = ExternalEnergy1[ii] / max_ex;
				if (max_in1 != 0)
					InternalEnergy1[ii] = InternalEnergy1[ii] / max_in1;
				if (max_in2 != 0)
					InternalEnergy2[ii] = InternalEnergy2[ii] / max_in2;

				// en[ii] = -1*ExternalEnergy1[ii] + 2*InternalEnergy1[ii] + 4*InternalEnergy2[ii] + 1*ExternalEnergy2[ii] + 1.4*ExternalEnergy3[ii];
				en[ii] = -2*ExternalEnergy1[ii] + 4*InternalEnergy1[ii] + 7*InternalEnergy2[ii];
			}
			min = en[0];
			countt = 0;
			for (ii = 0; ii < pow(windowsize, 2);ii++)
			{
				if (en[ii] < min)
				{
					min = en[ii];
					countt = ii;
				}
			}
			fc[i] = countt % windowsize - windowsize/2 + fc[i];
			fr[i] = countt / windowsize - windowsize/2 + fr[i];
			
			
		}
		
		PaintImage();
		Sleep(30);

	}
	


}



void ActiveContourBalloon(HWND AnimationWindowHandle)
{
	FILE	*fpt;
	unsigned char	*sobel;
	double		*test;
	unsigned char	*fcontour;
	signed char 	f1[] = { -1,0,1,-2,0,2,-1,0,1 };
	signed char 	f2[] = { -1,-2,-1,0,0,0,1,2,1 };
	int		c1, r1, c2, r2, k;
	int		i, j, ii, jj, countt, windowsize;
	double		sum1, sum2, grad, av_dis, sum_dis, av_color, sum_color, av_grad, sum_grad, min_ex1, min_ex2, min_ex3, min_in1, min_in2, max_ex, max_ex2, max_ex3, max_in1, max_in2, min;
	double		*ExternalEnergy1, *ExternalEnergy2, *ExternalEnergy3, *InternalEnergy1, *InternalEnergy2, *en;
	double		min_t, max_t;
	int			min_fc, max_fc, min_fr, max_fr;
	char	text[320];
	HDC		hDC;
	int		wedge;


	test = (double *)calloc(DISPLAY_COLS*DISPLAY_ROWS, sizeof(double));
	sobel = (unsigned char *)calloc(DISPLAY_COLS*DISPLAY_ROWS, sizeof(unsigned char));
	fcontour = (unsigned char *)calloc(DISPLAY_COLS*DISPLAY_ROWS, sizeof(unsigned char));
	ExternalEnergy1 = (double *)malloc(1000 * sizeof(double));
	ExternalEnergy2 = (double *)malloc(1000 * sizeof(double));
	ExternalEnergy3 = (double *)malloc(1000 * sizeof(double));
	InternalEnergy1 = (double *)malloc(1000 * sizeof(double));
	InternalEnergy2 = (double *)malloc(1000 * sizeof(double));
	en = (double *)malloc(1000 * sizeof(double));
	windowsize = 19;
	

	for (r1 = 1; r1 < DISPLAY_ROWS - 1; r1++)
		for (c1 = 1; c1 < DISPLAY_COLS - 1; c1++)
		{
			sum1 = 0;
			sum2 = 0;
			for (r2 = -1; r2 <= 1; r2++)
				for (c2 = -1; c2 <= 1; c2++)
				{
					sum1 += DisplayImage[(r1 + r2)*DISPLAY_COLS + (c1 + c2)] * f1[(r2 + 1) * 3 + 1 + c2];
					sum2 += DisplayImage[(r1 + r2)*DISPLAY_COLS + (c1 + c2)] * f2[(r2 + 1) * 3 + 1 + c2];
				}
			grad = sqrt(pow(sum1, 2) + pow(sum2, 2));
			printf("%f\n", grad);
			test[r1*DISPLAY_COLS + c1] = grad;
		}


	min_t = sobel[0];
	max_t = sobel[0];
	for (i = 0; i < DISPLAY_ROWS * DISPLAY_COLS; i++)
	{
		if (test[i] > max_t)
		{
			max_t = test[i];
		}
		if (test[i] < min_t)
		{
			min_t = test[i];
		}
	}

	for (i = 0; i < DISPLAY_ROWS*DISPLAY_COLS; i++)
	{
		sobel[i] = (test[i] - min_t)*255/(max_t-min_t);
	}

	min_fc = fc2[0];
	min_fr = fr2[0];
	max_fc = fc2[0];
	max_fr = fr2[0];
	for (i = 0;i < NN;i++)
	{
		if (fc2[i] < min_fc)
			min_fc = fc2[i];
		if (fr2[i] < min_fr)
			min_fr = fr2[i];
		if (fc2[i] > max_fc)
			max_fc = fc2[i];
		if (fr2[i] > max_fr)
			max_fr = fr2[i];
	}
	sum_color = 0;
	for (i = -20;i <= 20;i++)
		for (j = -20;j <= 20;j++)
		{
			sum_color = sum_color + DisplayImage[((min_fr + max_fr) / 2 + i)*DISPLAY_COLS + (min_fc + max_fc) / 2 + j];
		}
	av_color = sum_color / pow(41, 2);

	for (jj = 0;jj < 40;jj++)
	{
		fc2[NN] = fc2[0];
		fr2[NN] = fr2[0];
		sum_dis = 0;
		sum_grad = 0;
		for (i = 0;i < NN;i++)
		{
			sum_dis = sum_dis + sqrt(pow(fr2[i] - fr2[i + 1], 2) + pow(fc2[i] - fc2[i + 1], 2));
			sum_grad = sum_grad + sobel[fr2[i] * DISPLAY_COLS + fc2[i]];
		}
		av_dis = sum_dis / NN;
		av_grad = sum_grad / NN;

		for (i = 0;i < NN;i++)
		{
			k = 0;
			for (r2 = -windowsize/2; r2 <= windowsize/2; r2++)
				for (c2 = -windowsize/2; c2 <= windowsize/2; c2++)
				{
					if ((fr2[i] + r2) < 0 || (fr2[i] + r2) >= DISPLAY_ROWS || (fc2[i] + c2) < 0 || (fc2[i] + c2) >= DISPLAY_COLS)
					{
						ExternalEnergy1[k] = 900;
						ExternalEnergy2[k] = 900;
						ExternalEnergy3[k] = 900;
						InternalEnergy1[k] = 900;
						InternalEnergy2[k] = 900;
						k++;
						continue;
					}
					ExternalEnergy1[k] = pow(sobel[(fr2[i] + r2)*DISPLAY_COLS + fc2[i] + c2], 2);
					ExternalEnergy2[k] = pow(DisplayImage[(fr2[i] + r2)*DISPLAY_COLS + fc2[i] + c2] - av_color, 2);
					ExternalEnergy3[k] = pow(sobel[(fr2[i] + r2)*DISPLAY_COLS + fc2[i] + c2] - av_grad, 2);
					InternalEnergy1[k] = pow(fr2[i] + r2 - fr2[i + 1], 2) + pow(fc2[i] + c2 - fc2[i + 1], 2);
					InternalEnergy2[k] = pow(sqrt(pow(fr2[i] + r2 - fr2[i + 1], 2) + pow(fc2[i] + c2 - fc2[i + 1], 2)) - av_dis, 2);
					k++;
				}

			min_ex1 = ExternalEnergy1[0];
			min_ex2 = ExternalEnergy2[0];
			min_ex3 = ExternalEnergy3[0];
			min_in1 = InternalEnergy1[0];
			min_in2 = InternalEnergy2[0];
			for (ii = 0; ii < pow(windowsize,2);ii++)
			{
				if (ExternalEnergy3[ii] < min_ex3)
					min_ex3 = ExternalEnergy3[ii];
				if (ExternalEnergy2[ii] < min_ex2)
					min_ex2 = ExternalEnergy2[ii];
				if (ExternalEnergy1[ii] < min_ex1)
					min_ex1 = ExternalEnergy1[ii];
				if (InternalEnergy1[ii] < min_in1)
					min_in1 = InternalEnergy1[ii];
				if (InternalEnergy2[ii] < min_in2)
					min_in2 = InternalEnergy2[ii];
			}

			for (ii = 0; ii < pow(windowsize, 2);ii++)
			{
				ExternalEnergy1[ii] = ExternalEnergy1[ii] - min_ex1;
				ExternalEnergy2[ii] = ExternalEnergy2[ii] - min_ex2;
				ExternalEnergy3[ii] = ExternalEnergy3[ii] - min_ex3;
				InternalEnergy1[ii] = InternalEnergy1[ii] - min_in1;
				InternalEnergy2[ii] = InternalEnergy2[ii] - min_in2;
			}

			max_ex = ExternalEnergy1[0];
			max_ex2 = ExternalEnergy2[0];
			max_ex3 = ExternalEnergy3[0];
			max_in1 = InternalEnergy1[0];
			max_in2 = InternalEnergy2[0];
			for (ii = 0; ii < pow(windowsize, 2);ii++)
			{
				if (ExternalEnergy3[ii] > max_ex3)
					max_ex3 = ExternalEnergy3[ii];
				if (ExternalEnergy2[ii] > max_ex2)
					max_ex2 = ExternalEnergy2[ii];
				if (ExternalEnergy1[ii] > max_ex)
					max_ex = ExternalEnergy1[ii];
				if (InternalEnergy1[ii] > max_in1)
					max_in1 = InternalEnergy1[ii];
				if (InternalEnergy2[ii] > max_in2)
					max_in2 = InternalEnergy2[ii];
			}
			for (ii = 0; ii < pow(windowsize, 2);ii++)
			{
				if (max_ex3 != 0)
					ExternalEnergy3[ii] = ExternalEnergy3[ii] / max_ex3;
				if (max_ex2 != 0)
					ExternalEnergy2[ii] = ExternalEnergy2[ii] / max_ex2;
				if (max_ex != 0)
					ExternalEnergy1[ii] = ExternalEnergy1[ii] / max_ex;
				if (max_in1 != 0)
					InternalEnergy1[ii] = InternalEnergy1[ii] / max_in1;
				if (max_in2 != 0)
					InternalEnergy2[ii] = InternalEnergy2[ii] / max_in2;
				if (jj < 40)
					wedge = -1.1;
				else
					wedge = -1.3;
				en[ii] = -1*ExternalEnergy1[ii] - 2*InternalEnergy1[ii] + 1.5*InternalEnergy2[ii] + 1.2*ExternalEnergy2[ii] + 1*ExternalEnergy3[ii];
			}
			min = en[0];
			countt = 0;
			for (ii = 0; ii < pow(windowsize, 2);ii++)
			{
				if (en[ii] < min)
				{
					min = en[ii];
					countt = ii;
				}
			}
			fc2[i] = countt % windowsize - windowsize/2 + fc2[i];
			fr2[i] = countt / windowsize - windowsize/2 + fr2[i];


		}

		PaintImage();
		Sleep(30);

	}



}



void ActiveContourShiftInt(HWND AnimationWindowHandle)
{
	FILE	*fpt;
	unsigned char	*sobel;
	double		*test;
	unsigned char	*fcontour;
	signed char 	f1[] = { -1,0,1,-2,0,2,-1,0,1 };
	signed char 	f2[] = { -1,-2,-1,0,0,0,1,2,1 };
	int		c1, r1, c2, r2, k;
	int		i, j, ii, jj, countt;
	double		sum1, sum2, grad, av_dis, sum_dis, min_ex1, min_in1, min_in2, max_ex, max_in1, max_in2, min;
	double		*ExternalEnergy1, *InternalEnergy1, *InternalEnergy2, *en;
	double		min_t, max_t;
	char	text[320];
	HDC		hDC;
	int		eweight;


	test = (double *)calloc(DISPLAY_COLS*DISPLAY_ROWS, sizeof(double));
	sobel = (unsigned char *)calloc(DISPLAY_COLS*DISPLAY_ROWS, sizeof(unsigned char));
	fcontour = (unsigned char *)calloc(DISPLAY_COLS*DISPLAY_ROWS, sizeof(unsigned char));
	ExternalEnergy1 = (double *)malloc(500 * sizeof(double));
	InternalEnergy1 = (double *)malloc(500 * sizeof(double));
	InternalEnergy2 = (double *)malloc(500 * sizeof(double));
	en = (double *)malloc(500 * sizeof(double));



	for (r1 = 1; r1 < DISPLAY_ROWS - 1; r1++)
		for (c1 = 1; c1 < DISPLAY_COLS - 1; c1++)
		{
			sum1 = 0;
			sum2 = 0;
			for (r2 = -1; r2 <= 1; r2++)
				for (c2 = -1; c2 <= 1; c2++)
				{
					sum1 += DisplayImage[(r1 + r2)*DISPLAY_COLS + (c1 + c2)] * f1[(r2 + 1) * 3 + 1 + c2];
					sum2 += DisplayImage[(r1 + r2)*DISPLAY_COLS + (c1 + c2)] * f2[(r2 + 1) * 3 + 1 + c2];
				}
			grad = sqrt(pow(sum1, 2) + pow(sum2, 2));
			printf("%f\n", grad);
			test[r1*DISPLAY_COLS + c1] = grad;
		}


	min_t = sobel[0];
	for (i = 0; i < DISPLAY_ROWS; i++)
		for (j = 0; j < DISPLAY_COLS; j++)
		{
			if (test[i*DISPLAY_COLS + j] < min_t)
				min_t = test[i*DISPLAY_COLS + j];
		}
	printf("Minimum of MSF:%f\n", min_t);


	for (i = 0; i < DISPLAY_ROWS; i++)
		for (j = 0; j < DISPLAY_COLS; j++)
		{
			test[i*DISPLAY_COLS + j] = test[i*DISPLAY_COLS + j] - min_t;
		}

	max_t = sobel[0];
	for (i = 0; i < DISPLAY_ROWS; i++)
		for (j = 0; j < DISPLAY_COLS; j++)
		{
			if (test[i*DISPLAY_COLS + j] > max_t)
				max_t = test[i*DISPLAY_COLS + j];
		}
	printf("Maximum of MSF:%f\n", max_t);



	for (i = 0; i < DISPLAY_ROWS; i++)
		for (j = 0; j < DISPLAY_COLS; j++)
		{
			sobel[i*DISPLAY_COLS + j] = round(test[i*DISPLAY_COLS + j] / max_t * 255);
		}


	for (jj = 0;jj < 80;jj++)
	{
		fc[N] = fc[0];
		fr[N] = fr[0];
		sum_dis = 0;
		for (i = 0;i < N;i++)
		{
			sum_dis = sum_dis + sqrt(pow(fr[i] - fr[i + 1], 2) + pow(fc[i] - fc[i + 1], 2));
		}
		av_dis = sum_dis / N;


		for (i = 0;i < N;i++)
		{
			if (i == lfix)
				continue;

			k = 0;
			for (r2 = -3; r2 <= 3; r2++)
				for (c2 = -3; c2 <= 3; c2++)
				{
					ExternalEnergy1[k] = pow(sobel[(fr[i] + r2)*DISPLAY_COLS + fc[i] + c2], 2);
					InternalEnergy1[k] = pow(fr[i] + r2 - fr[i + 1], 2) + pow(fc[i] + c2 - fc[i + 1], 2);
					/*
					if (jj>10)
						if ((i+1) == lfix)
						{
							InternalEnergy2[k] = 0;
							k++;
							continue;
						}	
					*/
					InternalEnergy2[k] = pow(sqrt(pow(fr[i] + r2 - fr[i + 1], 2) + pow(fc[i] + c2 - fc[i + 1], 2)) - av_dis, 2);
					k++;
				}

			min_ex1 = ExternalEnergy1[0];
			min_in1 = InternalEnergy1[0];
			min_in2 = InternalEnergy2[0];
			for (ii = 0; ii < 49;ii++)
			{
				if (ExternalEnergy1[ii] < min_ex1)
					min_ex1 = ExternalEnergy1[ii];
				if (InternalEnergy1[ii] < min_in1)
					min_in1 = InternalEnergy1[ii];
				if (InternalEnergy2[ii] < min_in2)
					min_in2 = InternalEnergy2[ii];
			}

			for (ii = 0; ii < 49;ii++)
			{
				ExternalEnergy1[ii] = ExternalEnergy1[ii] - min_ex1;
				InternalEnergy1[ii] = InternalEnergy1[ii] - min_in1;
				InternalEnergy2[ii] = InternalEnergy2[ii] - min_in2;
			}

			max_ex = ExternalEnergy1[0];
			max_in1 = InternalEnergy1[0];
			max_in2 = InternalEnergy2[0];
			for (ii = 0; ii < 49;ii++)
			{
				if (ExternalEnergy1[ii] > max_ex)
					max_ex = ExternalEnergy1[ii];
				if (InternalEnergy1[ii] > max_in1)
					max_in1 = InternalEnergy1[ii];
				if (InternalEnergy2[ii] > max_in2)
					max_in2 = InternalEnergy2[ii];
			}
			for (ii = 0; ii < 49;ii++)
			{
				if (max_ex != 0)
					ExternalEnergy1[ii] = ExternalEnergy1[ii] / max_ex;
				if (max_in1 != 0)
					InternalEnergy1[ii] = InternalEnergy1[ii] / max_in1;
				if (max_in2 != 0)
					InternalEnergy2[ii] = InternalEnergy2[ii] / max_in2;
				en[ii] = -1*ExternalEnergy1[ii] + 0*InternalEnergy1[ii] + 2*InternalEnergy2[ii];
			}
			min = en[0];
			countt = 0;
			for (ii = 0; ii < 49;ii++)
			{
				if (en[ii] < min)
				{
					min = en[ii];
					countt = ii;
				}
			}
			fc[i] = countt % 7 - 3 + fc[i];
			fr[i] = countt / 7 - 3 + fr[i];


		}

		PaintImage();
		Sleep(30);

	}



}



void ActiveContourShiftBalloon(HWND AnimationWindowHandle)
{
	FILE	*fpt;
	unsigned char	*sobel;
	double		*test;
	unsigned char	*fcontour;
	signed char 	f1[] = { -1,0,1,-2,0,2,-1,0,1 };
	signed char 	f2[] = { -1,-2,-1,0,0,0,1,2,1 };
	int		c1, r1, c2, r2, k;
	int		i, j, ii, jj, countt;
	double		sum1, sum2, grad, av_dis, sum_dis, min_ex1, min_in1, min_in2, max_ex, max_in1, max_in2, min;
	double		*ExternalEnergy1, *InternalEnergy1, *InternalEnergy2, *en;
	double		min_t, max_t;
	char	text[320];
	HDC		hDC;


	test = (double *)calloc(DISPLAY_COLS*DISPLAY_ROWS, sizeof(double));
	sobel = (unsigned char *)calloc(DISPLAY_COLS*DISPLAY_ROWS, sizeof(unsigned char));
	fcontour = (unsigned char *)calloc(DISPLAY_COLS*DISPLAY_ROWS, sizeof(unsigned char));
	ExternalEnergy1 = (double *)malloc(500 * sizeof(double));
	InternalEnergy1 = (double *)malloc(500 * sizeof(double));
	InternalEnergy2 = (double *)malloc(500 * sizeof(double));
	en = (double *)malloc(500 * sizeof(double));



	for (r1 = 1; r1 < DISPLAY_ROWS - 1; r1++)
		for (c1 = 1; c1 < DISPLAY_COLS - 1; c1++)
		{
			sum1 = 0;
			sum2 = 0;
			for (r2 = -1; r2 <= 1; r2++)
				for (c2 = -1; c2 <= 1; c2++)
				{
					sum1 += DisplayImage[(r1 + r2)*DISPLAY_COLS + (c1 + c2)] * f1[(r2 + 1) * 3 + 1 + c2];
					sum2 += DisplayImage[(r1 + r2)*DISPLAY_COLS + (c1 + c2)] * f2[(r2 + 1) * 3 + 1 + c2];
				}
			grad = sqrt(pow(sum1, 2) + pow(sum2, 2));
			printf("%f\n", grad);
			test[r1*DISPLAY_COLS + c1] = grad;
		}


	min_t = sobel[0];
	for (i = 0; i < DISPLAY_ROWS; i++)
		for (j = 0; j < DISPLAY_COLS; j++)
		{
			if (test[i*DISPLAY_COLS + j] < min_t)
				min_t = test[i*DISPLAY_COLS + j];
		}
	printf("Minimum of MSF:%f\n", min_t);


	for (i = 0; i < DISPLAY_ROWS; i++)
		for (j = 0; j < DISPLAY_COLS; j++)
		{
			test[i*DISPLAY_COLS + j] = test[i*DISPLAY_COLS + j] - min_t;
		}

	max_t = sobel[0];
	for (i = 0; i < DISPLAY_ROWS; i++)
		for (j = 0; j < DISPLAY_COLS; j++)
		{
			if (test[i*DISPLAY_COLS + j] > max_t)
				max_t = test[i*DISPLAY_COLS + j];
		}
	printf("Maximum of MSF:%f\n", max_t);



	for (i = 0; i < DISPLAY_ROWS; i++)
		for (j = 0; j < DISPLAY_COLS; j++)
		{
			sobel[i*DISPLAY_COLS + j] = round(test[i*DISPLAY_COLS + j] / max_t * 255);
		}


	for (jj = 0;jj < 30;jj++)
	{
		fc2[NN] = fc2[0];
		fr2[NN] = fr2[0];
		sum_dis = 0;
		for (i = 0;i < NN;i++)
		{
			sum_dis = sum_dis + sqrt(pow(fr2[i] - fr2[i + 1], 2) + pow(fc2[i] - fc2[i + 1], 2));
		}
		av_dis = sum_dis / NN;


		for (i = 0;i < NN;i++)
		{
			if (i == rfix)
				continue;

			k = 0;
			for (r2 = -3; r2 <= 3; r2++)
				for (c2 = -3; c2 <= 3; c2++)
				{
					ExternalEnergy1[k] = pow(sobel[(fr2[i] + r2)*DISPLAY_COLS + fc2[i] + c2], 2);
					InternalEnergy1[k] = pow(fr2[i] + r2 - fr2[i + 1], 2) + pow(fc2[i] + c2 - fc2[i + 1], 2);
					InternalEnergy2[k] = pow(sqrt(pow(fr2[i] + r2 - fr2[i + 1], 2) + pow(fc2[i] + c2 - fc2[i + 1], 2)) - av_dis, 2);
					k++;
				}

			min_ex1 = ExternalEnergy1[0];
			min_in1 = InternalEnergy1[0];
			min_in2 = InternalEnergy2[0];
			for (ii = 0; ii < 49;ii++)
			{
				if (ExternalEnergy1[ii] < min_ex1)
					min_ex1 = ExternalEnergy1[ii];
				if (InternalEnergy1[ii] < min_in1)
					min_in1 = InternalEnergy1[ii];
				if (InternalEnergy2[ii] < min_in2)
					min_in2 = InternalEnergy2[ii];
			}

			for (ii = 0; ii < 49;ii++)
			{
				ExternalEnergy1[ii] = ExternalEnergy1[ii] - min_ex1;
				InternalEnergy1[ii] = InternalEnergy1[ii] - min_in1;
				InternalEnergy2[ii] = InternalEnergy2[ii] - min_in2;
			}

			max_ex = ExternalEnergy1[0];
			max_in1 = InternalEnergy1[0];
			max_in2 = InternalEnergy2[0];
			for (ii = 0; ii < 49;ii++)
			{
				if (ExternalEnergy1[ii] > max_ex)
					max_ex = ExternalEnergy1[ii];
				if (InternalEnergy1[ii] > max_in1)
					max_in1 = InternalEnergy1[ii];
				if (InternalEnergy2[ii] > max_in2)
					max_in2 = InternalEnergy2[ii];
			}
			for (ii = 0; ii < 49;ii++)
			{
				if (max_ex != 0)
					ExternalEnergy1[ii] = ExternalEnergy1[ii] / max_ex;
				if (max_in1 != 0)
					InternalEnergy1[ii] = InternalEnergy1[ii] / max_in1;
				if (max_in2 != 0)
					InternalEnergy2[ii] = InternalEnergy2[ii] / max_in2;
				en[ii] = -1*ExternalEnergy1[ii] + 0*InternalEnergy1[ii] + 2*InternalEnergy2[ii];
			}
			min = en[0];
			countt = 0;
			for (ii = 0; ii < 49;ii++)
			{
				if (en[ii] < min)
				{
					min = en[ii];
					countt = ii;
				}
			}
			fc2[i] = countt % 7 - 3 + fc2[i];
			fr2[i] = countt / 7 - 3 + fr2[i];


		}

		PaintImage();
		Sleep(30);

	}

}