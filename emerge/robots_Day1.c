
#include <stdio.h>
#include <math.h>
#include <windows.h>
#include <sys/timeb.h>
#include <time.h>


		/*
		** Robot functions
		*/



		/* This robot moves randomly */

void RandomRobot(int FoodClosestDistance,	/* input - closest food in pixels */
			int FoodClosestAngle,		/* input - angle in degrees towards closest food */
			int RobotClosestDistance,	/* input - closest other robot, in pixels */
			int RobotClosestAngle,		/* input - angle in degrees towards closest robot */
			int SharkClosestDistance,	/* input - closest shark in pixels */
			int SharkClosestAngle,		/* input - angle in degrees towards closest shark */
			int CurrentRobotEnergy,		/* input - this robot's current energy (50 - 255) */
			int *RobotMoveAngle,		/* output - angle in degrees to move */
			int *RobotExpendEnergy)		/* output - energy to expend in motion (cannot exceed Current-50) */

{
(*RobotMoveAngle)=(rand() % 360);
if (CurrentRobotEnergy > 50)
  (*RobotExpendEnergy)=(rand() % (CurrentRobotEnergy-50));
else
  (*RobotExpendEnergy)=1;
if ((*RobotExpendEnergy) > 10)
  (*RobotExpendEnergy)=10;
}



		/* This robot moves towards food, ignoring sharks */

void GreedyRobot(int FoodClosestDistance,	/* input - closest food in pixels */
			int FoodClosestAngle,		/* input - angle in degrees towards closest food */
			int RobotClosestDistance,	/* input - closest other robot, in pixels */
			int RobotClosestAngle,		/* input - angle in degrees towards closest robot */
			int SharkClosestDistance,	/* input - closest shark in pixels */
			int SharkClosestAngle,		/* input - angle in degrees towards closest shark */
			int CurrentRobotEnergy,		/* input - this robot's current energy (50 - 255) */
			int *RobotMoveAngle,		/* output - angle in degrees to move */
			int *RobotExpendEnergy)		/* output - energy to expend in motion (cannot exceed Current-50) */

{
(*RobotMoveAngle)=FoodClosestAngle;
(*RobotExpendEnergy)=30;
}



		/* This robot moves away from sharks, ignoring food */

void ScaredRobot(int FoodClosestDistance,	/* input - closest food in pixels */
			int FoodClosestAngle,		/* input - angle in degrees towards closest food */
			int RobotClosestDistance,	/* input - closest other robot, in pixels */
			int RobotClosestAngle,		/* input - angle in degrees towards closest robot */
			int SharkClosestDistance,	/* input - closest shark in pixels */
			int SharkClosestAngle,		/* input - angle in degrees towards closest shark */
			int CurrentRobotEnergy,		/* input - this robot's current energy (50 - 255) */
			int *RobotMoveAngle,		/* output - angle in degrees to move */
			int *RobotExpendEnergy)		/* output - energy to expend in motion (cannot exceed Current-50) */

{
(*RobotMoveAngle)=(180+SharkClosestAngle)%360;
if (SharkClosestDistance < 30)
  (*RobotExpendEnergy)=30;
else
  (*RobotExpendEnergy)=30-SharkClosestDistance/10;
if ((*RobotExpendEnergy) < 3)
  (*RobotExpendEnergy)=3;
}

void Ultron(int FoodClosestDistance,	/* input - closest food in pixels */
	int FoodClosestAngle,		/* input - angle in degrees towards closest food */
	int RobotClosestDistance,	/* input - closest other robot, in pixels */
	int RobotClosestAngle,		/* input - angle in degrees towards closest robot */
	int SharkClosestDistance,	/* input - closest shark in pixels */
	int SharkClosestAngle,		/* input - angle in degrees towards closest shark */
	int CurrentRobotEnergy,		/* input - this robot's current energy (50 - 255) */
	int *RobotMoveAngle,		/* output - angle in degrees to move */
	int *RobotExpendEnergy)		/* output - energy to expend in motion (cannot exceed Current-50) */

{
	/* Define the Trigger Points*/
	int EnemySpotted_FallBack = 125;
	int NeedFood = 48; 
	int iSeeFood_Attack = 90;

	/*Priority
	1. I am strong and have energy. I will tackle the shark first.
	2. Now I will eat, as I am always hungry.
	3. Won't mess with my competitior robots.
	4. Browsing around for food slowly.
	*/

	if (CurrentRobotEnergy > NeedFood)
	{
		if (SharkClosestDistance < EnemySpotted_FallBack)
		{
			/* Shark ahead fallback and save your life*/
			(*RobotExpendEnergy) = 20;
			(*RobotMoveAngle) = (130 + SharkClosestAngle) % 360;
		}
		else if (FoodClosestDistance < iSeeFood_Attack)
		{
			/* I am on a seafood diet, I see food and eat it */
			(*RobotExpendEnergy) = 25;
			(*RobotMoveAngle) = FoodClosestAngle;
		}
		else
		{
			/* Area is clear will just browse and check for food slowly*/
			(*RobotExpendEnergy) = 10;
			(*RobotMoveAngle) = FoodClosestAngle; 
		}
	}
	/* Energy Low I need food.
	1. I am weak, survival mode on, I will find food first.
	2. I am recharged I will again run from shark.
	3. Won't mess with my competitior robots.
	4. Browsing around for food slowly.*/
	else
	{
		if (FoodClosestDistance < iSeeFood_Attack)
		{
			(*RobotExpendEnergy) = 10 + FoodClosestDistance/10;
			(*RobotMoveAngle) = FoodClosestAngle;
		}
		else if (SharkClosestDistance < EnemySpotted_FallBack)
		{
			(*RobotExpendEnergy) = 18;
			(*RobotMoveAngle) = (130 + SharkClosestAngle) % 360;
		}
		else
		{
			(*RobotMoveAngle) = 0;
		}
	}
}


