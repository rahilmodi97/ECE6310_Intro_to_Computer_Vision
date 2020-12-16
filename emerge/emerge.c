#include <windows.h>
#include <wingdi.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <process.h>
#include <math.h>
#include <time.h>
#include <sys/timeb.h>
#include "resource.h"
#include "globals.h"

#define FOOD_MAX_MOVE			6	  /* units are pixels */
#define SHARK_MAX_MOVE			10	  /* units are pixels */
#define	SENSE_DIST_NOISE		4	  /* units are pixels */
#define	SENSE_ANG_NOISE			20	  /* units are degrees */

void EmergeThread(HWND UserWindowHandle)
  
{
int		i,j,k,MoveRow,MoveCol;
int		FoodClosestDistance,FoodClosestAngle;
int		SharkClosestDistance,SharkClosestAngle;
int		RobotClosestDistance,RobotClosestAngle;
int		MoveClosestDistance,MoveClosestAngle;
int		RobotMoveAngle,RobotMoveEnergy;

srand((unsigned)time(NULL));

while (repeat)
  {
  if (repeat == 2)
	{
	Sleep(100);
	continue;
	}

		/* move food - random motion */
  for (i=0; i<TotalFood; i++)
	{
	FoodRow[i]+=((rand()%FOOD_MAX_MOVE)-FOOD_MAX_MOVE/4);
	FoodCol[i]+=((rand()%FOOD_MAX_MOVE)-FOOD_MAX_MOVE/4);
	if (FoodRow[i] < 0) FoodRow[i]=0;
	if (FoodRow[i] >= ROWS) FoodRow[i]=ROWS-1;
	if (FoodCol[i] < 0) FoodCol[i]=0;
	if (FoodCol[i] >= COLS) FoodCol[i]=COLS-1;
	if (FoodRow[i] > ROWS-FOOD_MAX_MOVE  ||  FoodCol[i] > COLS-FOOD_MAX_MOVE)
	  {
	  FoodRow[i]=(rand() % ROWS/4);
	  FoodCol[i]=(rand() % COLS/4);
	  }
	}

		/* move sharks - towards food or robot */
  for (i=0; i<TotalSharks; i++)
	{
		/* sense closest food */
	FoodClosestDistance=ROWS*COLS+1;
	FoodClosestAngle=0;
	for (j=0; j<TotalFood; j++)
	  if ((int)sqrt((double)(SQR(FoodRow[j]-SharkRow[i])+SQR(FoodCol[j]-SharkCol[i]))) < FoodClosestDistance)
		{
		FoodClosestDistance=(int)sqrt((double)(SQR(FoodRow[j]-SharkRow[i])+SQR(FoodCol[j]-SharkCol[i])));
		FoodClosestAngle=(int)(360.0/(2.0*M_PI)*atan2(FoodCol[j]-SharkCol[i],FoodRow[j]-SharkRow[i]));
		}
		/* add noise to sensed data */
	FoodClosestDistance+=((rand()%SENSE_DIST_NOISE)-SENSE_DIST_NOISE/2);
    FoodClosestAngle+=((rand()%SENSE_ANG_NOISE)-SENSE_ANG_NOISE/2);
		/* sense closest robot */
	RobotClosestDistance=ROWS*COLS+1;
	RobotClosestAngle=0;
	for (j=0; j<TotalRobots; j++)
	  {
	  if (RobotRow[j] == -1  &&  RobotCol[j] == -1)
		continue;
	  if ((int)sqrt((double)(SQR(RobotRow[j]-SharkRow[i])+SQR(RobotCol[j]-SharkCol[i]))) < RobotClosestDistance)
		{
		RobotClosestDistance=(int)sqrt((double)(SQR(RobotRow[j]-SharkRow[i])+SQR(RobotCol[j]-SharkCol[i])));
		RobotClosestAngle=(int)(360.0/(2.0*M_PI)*atan2(RobotCol[j]-SharkCol[i],RobotRow[j]-SharkRow[i]));
		}
	  }
		/* add noise to sensed data */
	RobotClosestDistance+=((rand()%SENSE_DIST_NOISE)-SENSE_DIST_NOISE/2);
    RobotClosestAngle+=((rand()%SENSE_ANG_NOISE)-SENSE_ANG_NOISE/2);
	if (RobotClosestDistance < FoodClosestDistance)
	  {
	  MoveClosestDistance=RobotClosestDistance;
	  MoveClosestAngle=RobotClosestAngle;
	  }
	else
	  {
	  MoveClosestDistance=FoodClosestDistance;
	  MoveClosestAngle=FoodClosestAngle;
	  }
		/* move towards closest food */
	if (MoveClosestDistance <= SHARK_MAX_MOVE)
	  {
	  MoveRow=(int)((double)MoveClosestDistance*(double)SharkEnergy[i]/255.0*cos((2.0*M_PI)/360.0*MoveClosestAngle));
	  MoveCol=(int)((double)MoveClosestDistance*(double)SharkEnergy[i]/255.0*sin((2.0*M_PI)/360.0*MoveClosestAngle));
	  }
	else
	  {
	  MoveRow=(int)((double)SHARK_MAX_MOVE*(double)SharkEnergy[i]/255.0*cos((2.0*M_PI)/360.0*MoveClosestAngle));
	  MoveCol=(int)((double)SHARK_MAX_MOVE*(double)SharkEnergy[i]/255.0*sin((2.0*M_PI)/360.0*MoveClosestAngle));
	  }
	SharkRow[i]+=MoveRow;
	SharkCol[i]+=MoveCol;
	SharkEnergy[i]-=1;
	if (SharkEnergy[i] < MIN_ENERGY)
	  SharkEnergy[i]=MIN_ENERGY;
		/* check for collision with other sharks */
	for (j=0; j<TotalSharks; j++)
	  if (i != j  &&  (int)sqrt((double)(SQR(SharkRow[i]-SharkRow[j])+SQR(SharkCol[i]-SharkCol[j]))) <= SharkRadius*2)
		break;
	if (j < TotalSharks)
	  {
	  SharkRow[i]-=MoveRow;
	  SharkCol[i]-=MoveCol;
	  SharkEnergy[i]-=5;
	  if (SharkEnergy[i] < MIN_ENERGY)
	    SharkEnergy[i]=MIN_ENERGY;
	  }
	if (SharkRow[i] < 0) SharkRow[i]=0;
	if (SharkRow[i] >= ROWS) SharkRow[i]=ROWS-1;
	if (SharkCol[i] < 0) SharkCol[i]=0;
	if (SharkCol[i] >= COLS) SharkCol[i]=COLS-1;
	}

		/* see if any sharks caught (eat) food, spawning new food elsewhere */
  for (i=0; i<TotalSharks; i++)
	{
    for (j=0; j<TotalFood; j++)
	  {
	  if (abs(SharkRow[i]-FoodRow[j]) < SharkRadius+FoodRadius  &&  abs(SharkCol[i]-FoodCol[j]) < SharkRadius+FoodRadius)
		break;
	  }
	if (j < TotalFood)
	  {	/* shark caught some food */
	  SharkEnergy[i]+=20;
	  if (SharkEnergy[i] > 255)
	    SharkEnergy[i]=255;
	  FoodRow[j]=(rand() % ROWS);
	  FoodCol[j]=(rand() % COLS);
	  }
	}

		/* see if any robots caught (eaten) */
  for (i=0; i<TotalSharks; i++)
	{
    for (j=0; j<TotalRobots; j++)
	  {
	  if (RobotRow[j] == -1  &&  RobotCol[j] == -1)
		continue;
	  if (abs(SharkRow[i]-RobotRow[j]) < SharkRadius+RobotRadius  &&  abs(SharkCol[i]-RobotCol[j]) < SharkRadius+RobotRadius)
		break;
	  }
	if (j < TotalRobots)
	  {	/* shark caught some food */
	  SharkEnergy[i]+=100;
	  if (SharkEnergy[i] > 255)
	    SharkEnergy[i]=255;
	  RobotRow[j]=RobotCol[j]=-1;
	  for (k=0; k<TotalRobots; k++)
		if (RobotRow[k] != -1  &&  RobotCol[k] != -1)
		  break;
	  if (k == TotalRobots)
		repeat=0;	/* all robots dead */
	  }
	}

		/* move robots */
  for (i=0; i<TotalRobots; i++)
	{
	if (RobotRow[i] == -1  &&  RobotCol[i] == -1)
	  continue;
		/* sense closest food */
	FoodClosestDistance=ROWS*COLS+1;
	FoodClosestAngle=0;
	for (j=0; j<TotalFood; j++)
	  if ((int)sqrt((double)(SQR(FoodRow[j]-RobotRow[i])+SQR(FoodCol[j]-RobotCol[i]))) < FoodClosestDistance)
		{
		FoodClosestDistance=(int)sqrt((double)(SQR(FoodRow[j]-RobotRow[i])+SQR(FoodCol[j]-RobotCol[i])));
		FoodClosestAngle=(int)(360.0/(2.0*M_PI)*atan2(FoodCol[j]-RobotCol[i],FoodRow[j]-RobotRow[i]));
		}
		/* add noise to sensed data */
	FoodClosestDistance+=((rand()%SENSE_DIST_NOISE)-SENSE_DIST_NOISE/2);
    FoodClosestAngle+=((rand()%SENSE_ANG_NOISE)-SENSE_ANG_NOISE/2);
		/* sense closest robot */
	RobotClosestDistance=ROWS*COLS+1;
	RobotClosestAngle=0;
	for (j=0; j<TotalRobots; j++)
	  {
	  if (j == i  ||  (RobotRow[j] == -1  &&  RobotCol[j] == -1))
		continue;
	  if ((int)sqrt((double)(SQR(RobotRow[j]-RobotRow[i])+SQR(RobotCol[j]-RobotCol[i]))) < RobotClosestDistance)
		{
		RobotClosestDistance=(int)sqrt((double)(SQR(RobotRow[j]-RobotRow[i])+SQR(RobotCol[j]-RobotCol[i])));
		RobotClosestAngle=(int)(360.0/(2.0*M_PI)*atan2(RobotCol[j]-RobotCol[i],RobotRow[j]-RobotRow[i]));
		}
	  }
		/* add noise to sensed data */
	RobotClosestDistance+=((rand()%SENSE_DIST_NOISE)-SENSE_DIST_NOISE/2);
    RobotClosestAngle+=((rand()%SENSE_ANG_NOISE)-SENSE_ANG_NOISE/2);
		/* sense closest shark */
	SharkClosestDistance=ROWS*COLS+1;
	SharkClosestAngle=0;
	for (j=0; j<TotalSharks; j++)
	  if ((int)sqrt((double)(SQR(SharkRow[j]-RobotRow[i])+SQR(SharkCol[j]-RobotCol[i]))) < SharkClosestDistance)
		{
		SharkClosestDistance=(int)sqrt((double)(SQR(SharkRow[j]-RobotRow[i])+SQR(SharkCol[j]-RobotCol[i])));
		SharkClosestAngle=(int)(360.0/(2.0*M_PI)*atan2(SharkCol[j]-RobotCol[i],SharkRow[j]-RobotRow[i]));
		}
		/* add noise to sensed data */
	SharkClosestDistance+=((rand()%SENSE_DIST_NOISE)-SENSE_DIST_NOISE/2);
    SharkClosestAngle+=((rand()%SENSE_ANG_NOISE)-SENSE_ANG_NOISE/2);
	switch(i)
	  {
	  case 0:	/* this robot moves randomly */
		RandomRobot(FoodClosestDistance,FoodClosestAngle,
					RobotClosestDistance,RobotClosestAngle,
					SharkClosestDistance,SharkClosestAngle,
					RobotEnergy[i],
					&RobotMoveAngle,&RobotMoveEnergy);
		break;
	  case 1:	/* this robot is greedy for food but ignores sharks */
		GreedyRobot(FoodClosestDistance,FoodClosestAngle,
					RobotClosestDistance,RobotClosestAngle,
					SharkClosestDistance,SharkClosestAngle,
					RobotEnergy[i],
					&RobotMoveAngle,&RobotMoveEnergy);
		break;
	  case 2:	/* this robot only avoids sharks (ignores food) */
		ScaredRobot(FoodClosestDistance,FoodClosestAngle,
					RobotClosestDistance,RobotClosestAngle,
					SharkClosestDistance,SharkClosestAngle,
					RobotEnergy[i],
					&RobotMoveAngle,&RobotMoveEnergy);
		break;
	  case 3:	
		Ultron(FoodClosestDistance,FoodClosestAngle,
					RobotClosestDistance,RobotClosestAngle,
					SharkClosestDistance,SharkClosestAngle,
					RobotEnergy[i],
					&RobotMoveAngle,&RobotMoveEnergy);
		break;
		  /***********************************/
		  /* ADD YOUR ROBOT FUNCTION(S) HERE */
		  /***********************************/
	  default:	/* no function? = no move */
		RobotMoveAngle=RobotMoveEnergy=0;
		break;
	  }

		/* move robot according to desired motion */
	if (RobotMoveEnergy > RobotEnergy[i]-50)
	  RobotMoveEnergy=RobotEnergy[i]-50;
	if (RobotMoveEnergy < 1)
	  RobotMoveEnergy=1;
	MoveRow=(int)((double)RobotMoveEnergy*cos(2.0*M_PI/360.0*RobotMoveAngle));
	MoveCol=(int)((double)RobotMoveEnergy*sin(2.0*M_PI/360.0*RobotMoveAngle));
	RobotRow[i]+=MoveRow;
	RobotCol[i]+=MoveCol;
	RobotEnergy[i]-=(RobotMoveEnergy/10+1);
	if (RobotEnergy[i] < MIN_ENERGY)
	  RobotEnergy[i]=MIN_ENERGY;
		/* check for collision with other robots */
	for (j=0; j<TotalRobots; j++)
	  if (i != j  &&  (int)sqrt((double)(SQR(RobotRow[i]-RobotRow[j])+SQR(RobotCol[i]-RobotCol[j]))) <= RobotRadius*2)
		break;
	if (j < TotalRobots)
	  {
	  RobotRow[i]-=MoveRow;
	  RobotCol[i]-=MoveCol;
	  RobotEnergy[i]-=5;
	  if (RobotEnergy[i] < MIN_ENERGY)
	    RobotEnergy[i]=MIN_ENERGY;
	  }
	if (RobotRow[i] < RobotRadius) RobotRow[i]=RobotRadius;
	if (RobotRow[i] >= ROWS-RobotRadius) RobotRow[i]=ROWS-1-RobotRadius;
	if (RobotCol[i] < RobotRadius) RobotCol[i]=RobotRadius;
	if (RobotCol[i] >= COLS-RobotRadius) RobotCol[i]=COLS-1-RobotRadius;
	}

		/* see if any robots caught (eat) food, spawning new food elsewhere */
  for (i=0; i<TotalRobots; i++)
	{
    for (j=0; j<TotalFood; j++)
	  {
	  if (abs(RobotRow[i]-FoodRow[j]) < RobotRadius+FoodRadius  &&  abs(RobotCol[i]-FoodCol[j]) < RobotRadius+FoodRadius)
		break;
	  }
	if (j < TotalFood)
	  {	/* shark caught some food */
	  RobotEnergy[i]+=20;
	  if (RobotEnergy[i] > 255)
	    RobotEnergy[i]=255;
	  FoodRow[j]=(rand() % ROWS);
	  FoodCol[j]=(rand() % COLS);
	  }
	}


  Sleep(10);		/* slow down or speed up by changing this time (units are milliseconds) */
  MakeDisplayImage();
  PaintImage();
  }

_endthread();
}


