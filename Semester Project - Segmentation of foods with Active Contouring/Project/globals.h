
#define SQR(x) ((x)*(x))	/* macro for square */
#define SQRT(x) (x^(0.5))
#ifndef M_PI			/* in case M_PI not found in math.h */
#define M_PI 3.1415927
#endif
#ifndef M_E
#define M_E 2.718282
#endif

#define MAX_FILENAME_CHARS	320

char	filename[MAX_FILENAME_CHARS];

HWND	MainWnd;

		// Display flags
int		ShowPixelCoords, Showactivecontour, left,right;
int		ac[5000], ar[5000], fc[5000], fr[5000],ac2[5000], ar2[5000], fc2[5000], fr2[5000];
int		rcenter, ccenter, rfix,lfix;
int		ShowRegionGrowing_P, ShowRegionGrowing_S, ShowRegionGrowing;
int		distance, difference;
int		step;

		// Image data
unsigned char	*OriginalImage;
unsigned char	*DisplayImage;
unsigned char	*labels;
int				*indices;
int				ROWS,COLS,DISPLAY_ROWS,DISPLAY_COLS;
int				yyPos, xxPos, new_label;
int				paint_over_label=0;
int				count,clear;
int				rrr,g,b,redflag,blueflag;
char			type;
int				width, height;
int				N,NN, flag_l, flag_r, pshift, flag_s;


#define TIMER_SECOND	1			/* ID of timer used for animation */

		// Drawing flags
int		TimerRow,TimerCol;
int		ThreadRow,ThreadCol;
int		ThreadRunning;

		// Function prototypes
LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);
void PaintImage();
void AnimationThread(void *);		/* passes address of window */
void RegionGrow(void *);
void ActiveContourExternal(HWND);
void ActiveContourBalloon(HWND);
void ActiveContourShiftInt(HWND);
void ActiveContourShiftBalloon(HWND);