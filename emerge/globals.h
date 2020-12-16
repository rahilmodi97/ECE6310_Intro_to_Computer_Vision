
#define SQR(x) ((x)*(x))	/* macro for square */
#ifndef M_PI			/* in case M_PI not found in math.h */
#define M_PI 3.1415927
#endif
#ifndef M_E
#define M_E 2.718282
#endif

HWND	MainWnd;

#define			ROWS	  700
#define			COLS	  1000
unsigned char	*DisplayImage;
int				repeat;

		// critter variables
#define			MAX_CRITTERS	  100
#define			MIN_ENERGY		  50	/* every critter can always move a little */
int				TotalFood;
int				FoodRow[MAX_CRITTERS],FoodCol[MAX_CRITTERS];
int				TotalSharks;
int				SharkRow[MAX_CRITTERS],SharkCol[MAX_CRITTERS],SharkEnergy[MAX_CRITTERS];
int				TotalRobots;
int				RobotRow[MAX_CRITTERS],RobotCol[MAX_CRITTERS],RobotEnergy[MAX_CRITTERS];
#define			FoodRadius		  5
#define			SharkRadius		  20
#define			RobotRadius		  10

		// Function prototypes
LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);
void MakeDisplayImage();
void PaintImage();
void InitializeCritters();
void EmergeThread(void *);

		// Robot function prototypes
void RandomRobot(int,int,int,int,int,int,int,int *,int *);
void GreedyRobot(int,int,int,int,int,int,int,int *,int *);
void ScaredRobot(int,int,int,int,int,int,int,int *,int *);
void Ultron(int,int,int,int,int,int,int,int *,int *);
		/* ADD YOUR ROBOT FUNCTION PROTOTYPE HERE (identical to above) */
