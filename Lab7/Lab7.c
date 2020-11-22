#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>
#define SQR(x) ((x)*(x))
#define VAR_WINDOW 10
#define SAMPLE_TIME 0.05
#define GRAVITY 9.81
#define ACC_THRESH 0.0001
#define PITCH_THRESH 0.005
#define ROLL_THRESH 0.05
#define YAW_THRESH 0.005
/* Submitted by Rahil Modi - C14109603*/
int     data_size;
enum {
	Time,
	acc_x,
	acc_y,
	acc_z,
	Pitch,
	Roll,
	Yaw
};

/*-------------------------------------------------------------------------------------------*/
float variance (float *input, int current_index)
{
	int     i;
	int     temp_var_window;
	float   mean = 0;
	float   var;

	if (current_index + VAR_WINDOW <= data_size)
	{
		temp_var_window = current_index + VAR_WINDOW;
	}
	else
	{
		temp_var_window = data_size;
	}
	for (i = current_index; i < temp_var_window; i++)
	{
		mean += input[i];
	}
	mean = mean / (VAR_WINDOW);
	
	for (i = current_index; i < temp_var_window; i++)
	{
		var += SQR(input[i] - mean);
	}
	var = var / (VAR_WINDOW + 1);
	
	return var;
}

/*-----------------------------------------------------------------------------------------------------------*/

bool movement(float var_arr[7])
{
	bool ret_val = false;
	if (var_arr[acc_x] > ACC_THRESH || var_arr[acc_y] > ACC_THRESH || var_arr[acc_z] > ACC_THRESH)
	{
		ret_val = true;
	}
	if (var_arr[Pitch] > PITCH_THRESH || var_arr[Roll] > ROLL_THRESH || var_arr[Yaw] > YAW_THRESH)
	{
		ret_val = true;
	}
	
	return (ret_val);
}

/*------------------------------------------------------------------------------------------------------------*/
float accln_integration(float *input, int start, int end)
{
	int i;
	float prev_vel = 0.0;
	float vel = 0.0;
	float dist = 0.0;
	
	for (i = start; i <= end; i++)
	{
		prev_vel = vel;
		vel += input[i] * GRAVITY * SAMPLE_TIME;
		dist += ((vel + prev_vel) / 2) * SAMPLE_TIME;
	}
	
	return (dist);
}

/*-----------------------------------------------------------------------------------------------------------*/

float integration(float *input, int start, int end)
{
	float distance = 0.0;
	int   i;
	for (i = start; i <= end; i++)
	{
		distance += input[i] * SAMPLE_TIME;
	}
	
	return (distance);
}

/*-----------------------------------------------------------------------------------------------------------*/

void result(FILE *fp, float distance[7], float start_time, float end_time, int start, int end)
{
	fprintf(fp, "------------------------------------------------------------\n");
	fprintf(fp, "X linear distance: %f (meters)\nY linear distance: %f (meters)\nZ linear distance: %f (meters)\n", distance[acc_x], distance[acc_y], distance[acc_z]);
	fprintf(fp, "Pitch: %f (radians)\nRoll: %f (radians)\nYaw: %f (radians)\n", distance[Pitch], distance[Roll], distance[Yaw]);
	fprintf(fp, "Start Time: %0.2f (secs)\t\tEnd Time: %0.2f (secs)\n", start_time, end_time);
	fprintf(fp, "Start point: %d\t\tEnd point: %d\n", start, end);
	fprintf(fp, "\n\n");
	
	return;
}

/*-----------------------------------------------------------------------------------------------------------*/
int main()
{
    float   **temp_table;
	FILE    *fpt;
    FILE    *fpt2;
    FILE    *fpt3;
	FILE 	*fpt4;
    float   temp_var[7] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    char    c;
    int     i, j;
    float   var;
    float   start_time = 0.0;
	float   end_time = 0.0;
	float   start = 0;
	float   end = 0;
    float   temp_dist[7] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    bool    move;


    if ((fpt=fopen("D:/Computer_Vision/Lab7/acc_gyro.txt", "rb")) == NULL)
	{
		printf("Unable to open data file\n");
		exit(0);
	}

	i = 0;
	
	for (c = getc(fpt); c != EOF; c = getc(fpt))
	{
		if (c == '\n')
			i++;
	}
	
	data_size = i;
	
	rewind(fpt);
	
	temp_table  = (float **)malloc(7*sizeof(float *));
	for (i = 0; i < 7; i++)
	{
		temp_table[i] = (float *)malloc(data_size*sizeof(float));
	}
	
	i = 0;
	
	while(!feof(fpt) && !(i == data_size))
	{
		if (i == 0)
		{
			fscanf(fpt, "time	accX	accY	accZ	pitch	roll	yaw							\n");
		}
		else
		{
			fscanf(fpt, "%f %f %f %f %f %f %f \n", &temp_table[Time][i-1], &temp_table[acc_x][i-1], &temp_table[acc_y][i-1], &temp_table[acc_z][i-1], &temp_table[Pitch][i-1], &temp_table[Roll][i-1], &temp_table[Yaw][i-1]);
		}
		i++;
	}
	
	for (i = 0; i < data_size; i++)
	{
		temp_table[acc_z][i] += 1;
	}
	
	fpt2 = fopen("D:/Computer_Vision/Lab7/variance.csv", "w");
	fprintf(fpt2, "Time,acc_x,acc_y,acc_z,Pitch,Roll,Yaw\n");
	
	for (i = 0; i < data_size; i++)
	{
		fprintf(fpt2, "%f,", temp_table[Time][i]);
		for (j = acc_x; j <= Yaw; j++)
		{
			var = variance(temp_table[j], i);
			fprintf(fpt2, "%f,", var);
		}
		fprintf(fpt2, "\n");
	}
	fclose(fpt2);

    fpt3 = fopen("D:/Computer_Vision/Lab7/Results.txt", "w");
	fpt4 = fopen("D:/Computer_Vision/Lab7/Results.csv", "w");
	fprintf(fpt3, "Results:\n");
	fprintf(fpt4, "Start Time,End Time,Start Point,End Point,X-Linear distance,Y-Linear distance,Z-Linear distance,Pitch,Roll,Yaw\n");
	
	for (i = 0; i < data_size; i++)
	{
		for (j = acc_x; j <= Yaw; j++)
		{
			temp_var[j] = variance(temp_table[j], i);
		}
		move = movement(temp_var);
		if (move && start_time == 0.0)
		{
			start_time = temp_table[Time][i];
			start = i;
		}
		else if (!move && start_time != 0.0 && end_time == 0.0)
		{
			move = false;
			end_time = temp_table[Time][i];
			end = i;
		}
		if (start_time != 0.0 && end_time != 0.0)
		{
			temp_dist[acc_x] = accln_integration(temp_table[acc_x], start, end);
			temp_dist[acc_y] = accln_integration(temp_table[acc_y], start, end);
			temp_dist[acc_z] = accln_integration(temp_table[acc_z], start, end);
			temp_dist[Pitch] = integration(temp_table[Pitch], start, end);
			temp_dist[Roll] = integration(temp_table[Roll], start, end);
			temp_dist[Yaw] = integration(temp_table[Yaw], start, end);
			
			result(fpt3, temp_dist, start_time, end_time, start, end);
			fprintf(fpt4, "%0.2f,%0.2f,%f,%f,%f,%f,%f,%f,%f,%f\n",start_time,end_time,start,end,temp_dist[acc_x],temp_dist[acc_y],temp_dist[acc_z],temp_dist[Pitch],temp_dist[Roll],temp_dist[Yaw]);
			start_time = 0.0;
			end_time = 0.0;
		}
		
	}
	fclose(fpt4);
	fclose(fpt3);
	return(0);
}