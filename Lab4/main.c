/* Submitted by Rahil Modi - C14109603 */
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <sys/timeb.h>
#include <windows.h>
#include <wingdi.h>
#include <winuser.h>
#include <process.h>	/* needed for multithreading */
#include "resource.h"
#include "globals.h"

#define SQR(x) ((x)*(x))
#define MAX_QUEUE 10000	/* max perimeter size (pixels) of border wavefront */

/* ncmdshow is handle to the window, hinstance is a akind of handle required to load icon, etc, hprevinstance is equal to zero of no use*/
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
				LPTSTR lpCmdLine, int nCmdShow)                      /* APIENTRY is a entry point in DLLs*/

{
MSG			msg;
HWND		hWnd;                                                    /* window handle for a Windows window*/
WNDCLASS	wc;														 /* It contains the window class attributes*/

wc.style=CS_HREDRAW | CS_VREDRAW;
wc.lpfnWndProc=(WNDPROC)WndProc;
wc.cbClsExtra=0;
wc.cbWndExtra=0;
wc.hInstance=hInstance;                                              /* These all atributes define how a window will look*/
wc.hIcon=LoadIcon(hInstance,"ID_PLUS_ICON");
wc.hCursor=LoadCursor(NULL,IDC_ARROW);
wc.hbrBackground=(HBRUSH)(COLOR_WINDOW+1);
wc.lpszMenuName="ID_MAIN_MENU";
wc.lpszClassName="PLUS";

if (!RegisterClass(&wc))
  return(FALSE);
/* hwnd is creating a window of mentioned size */
hWnd=CreateWindow("PLUS","plus program",						
		WS_OVERLAPPEDWINDOW | WS_HSCROLL | WS_VSCROLL,
		CW_USEDEFAULT,0,400,400,NULL,NULL,hInstance,NULL);
if (!hWnd)
  return(FALSE);

ShowScrollBar(hWnd,SB_BOTH,FALSE);	  /* This function shows the scrollbar*/
ShowWindow(hWnd,nCmdShow);            /* This function shows the window created in hwnd*/
UpdateWindow(hWnd);					  /* This function is used to update the window */
MainWnd=hWnd;                         /* assigning hwnd a variable*/

ShowPixelCoords=0;
color=1;
xCor=0;
yCor=0;
reset_image = 0;
key_press = 0;
press_j = 0;
u_difference = 10;
u_distance = 600;
region_grow_play = 0;
step_region_grow = 0;

strcpy(filename,"");
OriginalImage=NULL;
ROWS=COLS=0;

InvalidateRect(hWnd,NULL,TRUE);      /* draws a rectangular area which needs to be updated */
UpdateWindow(hWnd);

while (GetMessage(&msg,NULL,0,0))
  {
  TranslateMessage(&msg);
  DispatchMessage(&msg);
  }
return(msg.wParam);
}

LRESULT CALLBACK WndProc2 (HWND hWnd, UINT uMsg,         /* This function processes messages sent to the window we need this to process the number entered in dialog box */
		WPARAM wParam, LPARAM lParam)
{
	BOOL *flag;
	INT_PTR nResult = NULL; // MSN type for end dialog function
	switch (uMsg)
	{
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			u_difference = GetDlgItemInt(hWnd, IDC_EDIT1, &flag, FALSE);     /* With this line we are extracting the text or number entered in the dialog box */
			EndDialog(hWnd, wParam);										 /* This function to close the processing of the dialog box*/
			break;

		case IDCANCEL:
			EndDialog(hWnd, wParam);
		}
	}
	return(0L);
}

LRESULT CALLBACK WndProc3(HWND hWnd, UINT uMsg,        /* This function processes messages sent to the window we need this to process the number entered in dialog box */
	WPARAM wParam, LPARAM lParam)
{
	BOOL *flag;
	INT_PTR nResult = NULL; // MSN type for end dialog function
	switch (uMsg)
	{
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			u_distance = GetDlgItemInt(hWnd, IDC_EDIT2, &flag, FALSE);      /* With this line we are extracting the text or number entered in the dialog box */
			EndDialog(hWnd, wParam);                                        /* This function to close the processing of the dialog box*/
			break;

		case IDCANCEL:
			EndDialog(hWnd, wParam);
		}
	}
	return(0L);
}

LRESULT CALLBACK WndProc (HWND hWnd, UINT uMsg,								/* In this function all the rest functionality which is required is included */
		WPARAM wParam, LPARAM lParam)

{
HMENU				hMenu;
OPENFILENAME		ofn;
FILE				*fpt;
HDC					hDC;
char				header[320],text[320];
int					BYTES,xPos,yPos;

switch (uMsg)
  {
  case WM_COMMAND:
    	switch (LOWORD(wParam))
    	{
		case ID_SHOWPIXELCOORDS:
			ShowPixelCoords=(ShowPixelCoords+1)%2;
			PaintImage();
			break;
		case ID_COLOR_RED:
			color = 1;
			break;
		case ID_COLOR_GREEN:
			color = 2;
			break;
		case ID_COLOR_BLUE:
			color = 3;
			break;
		case ID_DISPLAY_RESETIMAGE:
			PaintImage();
			reset_image = 1;
			break;
		case ID_DISPLAY_REGIONGROWPLAY:
			region_grow_play = (region_grow_play + 1) % 2;
			if (region_grow_play == 1)
			{
				step_region_grow = 0;
			}
			key_press = 0;
			break;

		case ID_DISPLAY_STEPREGIONGROW:
			step_region_grow = (step_region_grow + 1) % 2;
			if (step_region_grow == 1)
			{
				region_grow_play = 0;
			}
			key_press = 1;
			break;
		case ID_DISPLAY_CHANGEPIXELDIFFERENCE:
			DialogBox(NULL, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, WndProc2);
			break;

		case ID_DISPLAY_PIXELDISTANCE:
			DialogBox(NULL, MAKEINTRESOURCE(IDD_DIALOG2), hWnd, WndProc3);
			break;
		case ID_FILE_LOAD:
			if (OriginalImage != NULL)
			{
				free(OriginalImage);
				OriginalImage=NULL;
			}
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
			if (strcmp(header,"P5") != 0  ||  BYTES != 255)
			{
			MessageBox(NULL,"Not a PPM (P5 greyscale) image",filename,MB_OK | MB_APPLMODAL);
			fclose(fpt);
			break;
			}
			OriginalImage=(unsigned char *)calloc(ROWS*COLS,1);
			header[0]=fgetc(fpt);	/* whitespace character after header */
			fread(OriginalImage,1,ROWS*COLS,fpt);
			fclose(fpt);
			SetWindowText(hWnd,filename);
			PaintImage();
			break;

		case ID_FILE_QUIT:
			DestroyWindow(hWnd);
			break;
	}
    break;
  case WM_SIZE:		  /* could be used to detect when window size changes */
	PaintImage();
	return(DefWindowProc(hWnd,uMsg,wParam,lParam));
	break;
  case WM_PAINT:
	PaintImage();
    return(DefWindowProc(hWnd,uMsg,wParam,lParam));
	break;
  case WM_LBUTTONDOWN:case WM_RBUTTONDOWN:
	xCor = LOWORD(lParam);
	yCor = HIWORD(lParam);
	if (step_region_grow == 1 || region_grow_play == 1)
	{
		_beginthread(RegionGrow_1, 0, MainWnd);
		ThreadRunning_regiongrow = 1;
	}
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
		if (color == 1)
		  {
			  SetPixel(hDC, xPos, yPos, RGB(255, 0, 0));	/* color the animation pixel red */
		  }
		  else if (color == 2)
		  {
			  SetPixel(hDC, xPos, yPos, RGB(0, 255, 0));	/* color the animation pixel green */
		  }
		  else if (color == 3)
		  {
			  SetPixel(hDC, xPos, yPos, RGB(0, 0, 255));	/* color the animation pixel blue*/
		  }
		ReleaseDC(MainWnd,hDC);
		}
	  }
    return(DefWindowProc(hWnd,uMsg,wParam,lParam));
	break;
  case WM_KEYDOWN:
	if (wParam == 'j' || 'J')
	{
		press_j = 1;
	}
	if (wParam == 's'  ||  wParam == 'S')
	  PostMessage(MainWnd,WM_COMMAND,ID_SHOWPIXELCOORDS,0);	  /* send message to self */
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

hMenu=GetMenu(MainWnd);
if (ShowPixelCoords == 1)
  CheckMenuItem(hMenu,ID_SHOWPIXELCOORDS,MF_CHECKED);	/* you can also call EnableMenuItem() to grey(disable) an option */
else
  CheckMenuItem(hMenu,ID_SHOWPIXELCOORDS,MF_UNCHECKED);

if (color == 1)
{
	CheckMenuItem(hMenu, ID_COLOR_RED,   MF_CHECKED);
	CheckMenuItem(hMenu, ID_COLOR_GREEN, MF_UNCHECKED);
	CheckMenuItem(hMenu, ID_COLOR_BLUE,  MF_UNCHECKED);
}
else if (color == 2)
{
	CheckMenuItem(hMenu, ID_COLOR_RED,   MF_UNCHECKED);
	CheckMenuItem(hMenu, ID_COLOR_GREEN, MF_CHECKED);
	CheckMenuItem(hMenu, ID_COLOR_BLUE,  MF_UNCHECKED);
}
else if (color == 3)
{
	CheckMenuItem(hMenu, ID_COLOR_RED,   MF_UNCHECKED);
	CheckMenuItem(hMenu, ID_COLOR_GREEN, MF_UNCHECKED);
	CheckMenuItem(hMenu, ID_COLOR_BLUE,  MF_CHECKED);
}
else
{
	CheckMenuItem(hMenu, ID_COLOR_RED,   MF_UNCHECKED);
	CheckMenuItem(hMenu, ID_COLOR_GREEN, MF_UNCHECKED);
	CheckMenuItem(hMenu, ID_COLOR_BLUE,  MF_UNCHECKED);
}

if (region_grow_play == 1)
{
  CheckMenuItem(hMenu, ID_DISPLAY_REGIONGROWPLAY, MF_CHECKED);
}
else
{
  CheckMenuItem(hMenu, ID_DISPLAY_REGIONGROWPLAY, MF_UNCHECKED);
}
if (step_region_grow == 1)
{
  CheckMenuItem(hMenu, ID_DISPLAY_STEPREGIONGROW, MF_CHECKED);
}
else
{
  CheckMenuItem(hMenu, ID_DISPLAY_STEPREGIONGROW, MF_UNCHECKED);
}
DrawMenuBar(hWnd);

return(0L);
}




void PaintImage()

{
PAINTSTRUCT			Painter;
HDC					hDC;
BITMAPINFOHEADER	bm_info_header;
BITMAPINFO			*bm_info;
int					i,r,c,DISPLAY_ROWS,DISPLAY_COLS;
unsigned char		*DisplayImage;

if (OriginalImage == NULL)
  return;		/* no image to draw */

		/* Windows pads to 4-byte boundaries.  We have to round the size up to 4 in each dimension, filling with black. */
DISPLAY_ROWS=ROWS;
DISPLAY_COLS=COLS;
if (DISPLAY_ROWS % 4 != 0)
  DISPLAY_ROWS=(DISPLAY_ROWS/4+1)*4;
if (DISPLAY_COLS % 4 != 0)
  DISPLAY_COLS=(DISPLAY_COLS/4+1)*4;
DisplayImage=(unsigned char *)calloc(DISPLAY_ROWS*DISPLAY_COLS,1);
for (r=0; r<ROWS; r++)
  for (c=0; c<COLS; c++)
	DisplayImage[r*DISPLAY_COLS+c]=OriginalImage[r*COLS+c];

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
EndPaint(MainWnd,&Painter);

free(DisplayImage);
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
  if (color == 1)
  {
	  SetPixel(hDC, ThreadCol, ThreadRow, RGB(255, 0, 0));	/* color the animation pixel red */
  }
  else if (color == 2)
  {
	  SetPixel(hDC, ThreadCol, ThreadRow, RGB(0, 255, 0));	/* color the animation pixel green */
  }
  else if (color == 3)
  {
	  SetPixel(hDC, ThreadCol, ThreadRow, RGB(0, 0, 255));	/* color the animation pixel blue*/
  }
  sprintf(text,"%d,%d     ",ThreadRow,ThreadCol);
  TextOut(hDC,300,0,text,strlen(text));		/* draw text on the window */
  ReleaseDC(MainWnd,hDC);
  ThreadRow+=3;
  ThreadCol++;
  Sleep(100);		/* pause 100 ms */
  }
}
#define MAX_QUEUE 10000	/* max perimeter size (pixels) of border wavefront */

void RegionGrow(unsigned char *image,	/* image data */
				unsigned char *labels,	/* segmentation labels */
				int ROWS,int COLS,	/* size of image */
				int r,int c,		/* pixel to paint from */
				int paint_over_label,	/* image label to paint over */
				int new_label,		/* image label for painting */
				int *indices,		/* output:  indices of pixels painted */
				int *count)		/* output:  count of pixels painted */
{
	HDC hDC;
	int	r2,c2;
	int	queue[MAX_QUEUE],qh,qt;
	int	average,total;	/* average and total intensity in growing region */
	int i;
	int center_x=0;
	int center_y=0;
	int sum_r=0, sum_c=0;
	float distance = 0.0;

	hDC = GetDC(MainWnd);
	*count=0;
	if (labels[r*COLS+c] != paint_over_label)
		return;
	labels[r*COLS+c]=new_label;
	average=total=(int)image[r*COLS+c];
	if (indices != NULL)
		indices[0]=r*COLS+c;
	queue[0]=r*COLS+c;
	qh=1;	/* queue head */
	qt=0;	/* queue tail */
	(*count)=1;
	center_x = r;
	center_y = c;
	while (qt != qh)
	{
		if ((*count)%50 == 0)	/* recalculate average after each 50 pixels join */
			{
			average=total/(*count);
			for (i = 0; i < *count; i++)
			{
				sum_r += indices[i]/COLS;
				sum_c += indices[i]%COLS;
				printf("r = %d\n",sum_r);
				printf("c = %d\n",sum_c);
			}
			center_x = sum_r / (*count);
			center_y = sum_c / (*count);
			sum_r = 0;
			sum_c = 0;
			}
		for (r2=-1; r2<=1; r2++)
			for (c2=-1; c2<=1; c2++)
			{
				if (r2 == 0  &&  c2 == 0)
					continue;
				if ((queue[qt]/COLS+r2) < 0  ||  (queue[qt]/COLS+r2) >= ROWS  ||
					(queue[qt]%COLS+c2) < 0  ||  (queue[qt]%COLS+c2) >= COLS)
					continue;
				if (labels[(queue[qt]/COLS+r2)*COLS+queue[qt]%COLS+c2]!=paint_over_label)
					continue;
				/* test criteria to join region */
				distance = sqrt(SQR((queue[qt] / COLS + r2) - center_x) + SQR(((queue[qt] % COLS + c2) - center_y)));
				if ((abs((int)(image[(queue[qt]/COLS+r2)*COLS+queue[qt]%COLS+c2])
				-average) > u_difference) || (distance > u_distance))
					continue;
				if (color == 1)
				{
					SetPixel(hDC, queue[qt]%COLS+c2, queue[qt]/COLS+r2, RGB(255, 0, 0));	/* color the animation pixel red */
				}
				else if (color == 2)
				{
					SetPixel(hDC, queue[qt] % COLS + c2, queue[qt] / COLS + r2, RGB(0, 255, 0));	/* color the animation pixel green */
				}
				else if (color == 3)
				{
					SetPixel(hDC, queue[qt] % COLS + c2, queue[qt] / COLS + r2, RGB(0, 0, 255));	/* color the animation pixel blue*/
				}
				labels[(queue[qt]/COLS+r2)*COLS+queue[qt]%COLS+c2]=new_label;
				if (indices != NULL)
					indices[*count]=(queue[qt]/COLS+r2)*COLS+queue[qt]%COLS+c2;
				total+=image[(queue[qt]/COLS+r2)*COLS+queue[qt]%COLS+c2];
				(*count)++;
				queue[qh]=(queue[qt]/COLS+r2)*COLS+queue[qt]%COLS+c2;
				qh=(qh+1)%MAX_QUEUE;
				if (qh == qt)
				{
					printf("Max queue size exceeded\n");
					exit(0);
				}
			}
		qt=(qt+1)%MAX_QUEUE;
		if (key_press == 0)
		{
			Sleep(1);                 /* sleep function */
		}
		if (key_press == 1)
		{
			while (press_j == 0)
			{
				Sleep(10);
			}
			press_j = 0;
		}
		/* This will reset the image and break the loop */
		if (reset_image == 1)
		{
			reset_image = 0;
			break;
		}
	}
}
#define SQR(x) ((x)*(x))
void RegionGrow_1()
{
	unsigned char	*image,*labels;
	FILE            *fpt;
	char			header[80];
	int				r,c,r2,c2;
	int				*indices,i;
	int				RegionSize,*RegionPixels,TotalRegions;
	double			avg,var;
	void			RegionGrow();

	image=(unsigned char *)calloc(ROWS*COLS,sizeof(unsigned char));
	image = OriginalImage;
		/* segmentation image = labels; calloc initializes all labels to 0 */
	labels=(unsigned char *)calloc(ROWS*COLS,sizeof(unsigned char));
		/* used to quickly erase small grown regions */
	indices=(int *)calloc(ROWS*COLS,sizeof(int));
	
	r = yCor;
	c = xCor;
	TotalRegions=0;
	avg=var=0.0;	/* compute average and variance in 7x7 window */
	for (r2=-3; r2<=3; r2++)
		for (c2=-3; c2<=3; c2++)
			avg+=(double)(image[(r+r2)*COLS+(c+c2)]);
	avg/=49.0;
	for (r2=-3; r2<=3; r2++)
		for (c2=-3; c2<=3; c2++)
			var+=SQR(avg-(double)image[(r+r2)*COLS+(c+c2)]);
	var=sqrt(var)/49.0;
	if (var < 1.0)	/* condition for seeding a new region is low var */
	{
		TotalRegions++;
		RegionGrow(image,labels,ROWS,COLS,r,c,0,TotalRegions,
			indices, &RegionSize);
		if (RegionSize < 100)
		{	/* erase region (relabel pixels back to 0) */
			for (i=0; i<RegionSize; i++)
				labels[indices[i]]=0;
			TotalRegions--;
		}
	}
}

