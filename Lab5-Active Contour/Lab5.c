#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#define SQR(x) ((x)*(x))
/* Submitted by Rahil Modi C14109603 */

/*------------------------------------------------------------------------------------------------------*/
void initial_image(unsigned char *image, int *contour_row, int *contour_col, int ROWS, int COLS)
{
	int i, r, c;
	
	for (i = 0; i < 42; i++)
	{
		for (r = -3; r <= 3; r++)
		{
			image[(contour_row[i]+r)*COLS+contour_col[i]] = 0;
		}
		for (c = -3; c <= 3; c++)
		{
			image[contour_row[i]*COLS+(contour_col[i]+c)] = 0;
		}
	}
	return;
}

/*-------------------------------------------------------------------------------------------------------*/

#define SQR(x) ((x)*(x))
void sobel_filter(unsigned char *image, float *sobel_image, int ROWS, int COLS)
{
	int   		r, c, r2, c2;
	float 		sum_gx, sum_gy;
	int 		gx[9] = {-1, 0, 1, -2, 0, 2, -1, 0, 1};
    int 		gy[9] = {-1, -2, -1, 0, 0, 0, 1, 2, 1};
	
	for (r = 3; r <ROWS-3; r++)
	{
		for (c = 3; c <COLS-3; c++)
		{
			sum_gx = 0;
			sum_gy = 0;
			for (r2 = -1; r2 <= 1; r2++)
			{
				for (c2 = -1; c2 <= 1; c2++)
				{
					sum_gx += image[(r+r2)*COLS+(c+c2)] * gx[(r2+1)*3+(c2+1)];
					sum_gy += image[(r+r2)*COLS+(c+c2)] * gy[(r2+1)*3+(c2+1)];
				}
			}
			
			sobel_image[r*COLS+c] = sqrt(SQR(sum_gx) + SQR(sum_gy));
		}
	}
	
	return;
}

/*-----------------------------------------------------------------------------------------------------*/

void normalize(float *dummy_image, int ROWS, int COLS, int range)
{
  int i,j;
  float max, min;
 
  max  = dummy_image[0];
  min  = dummy_image[0];

  for (i = 0; i < ROWS * COLS; i++)
  {
	if (dummy_image[i] > max)
    {
      max = dummy_image[i];
    }
    if (dummy_image[i] < min)
    {
      min = dummy_image[i];
    }
  }

  for (i = 0; i < ROWS*COLS; i++)
  {
    dummy_image[i] = (dummy_image[i] - min)*range/(max-min);
  }
  return;
}

/*------------------------------------------------------------------------------------------------------*/

void invert_sobel(float *sobel_image, int ROWS, int COLS)
{
	int i;
	 for (i = 0; i < ROWS*COLS; i++)
	 {
		 sobel_image[i] = 1 - sobel_image[i];
	 }
	 
	 return;
}

/*------------------------------------------------------------------------------------------------------*/

void minimum(int *min_row, int *min_col, float *dummy_image)
{
	int i;
	float min = dummy_image[0];
	
	for (i = 0; i < 7*7; i++)
	{
		if (dummy_image[i] < min)
		{
			min = dummy_image[i];
			*min_row = (i/7) - 3;
			*min_col = (i%7) - 3;
		}
	}
	
}

/*-------------------------------------------------------------------------------------------------------*/

int distance(int x1, int x2, int y1, int y2)
{
	int euclidean_distance;
	
	euclidean_distance = sqrt(SQR(x2-x1)+SQR(y2-y1));
	
	return(euclidean_distance);
}

/*--------------------------------------------------------------------------------------------------------*/

void internal_energy_1(float *total_int_energy, int *contour_row, int *contour_col, int i)
{
	int r, c;
	
	for (r = -3; r <= 3; r++)
	{
		for (c = -3; c <= 3; c++)
		{
			if (i != 41)
			{
				total_int_energy[(r+3)*7+(c+3)] = SQR(distance(contour_row[i+1], contour_row[i]+r, contour_col[i+1], contour_col[i]+c));
			}
			else
			{
				total_int_energy[(r+3)*7+(c+3)] = SQR(distance(contour_row[0], contour_row[i]+r, contour_col[0], contour_col[i]+c));
			}
		}
	}
	return;
}

/*------------------------------------------------------------------------------------------------------------*/

void internal_energy_2(float *total_int2_energy, int *contour_row, int *contour_col, int avg, int i)
{
	int r, c;
	
	for (r = -3; r <= 3; r++)
	{
		for (c = -3; c <= 3; c++)
		{
			if (i != 41)
			{
				total_int2_energy[(r+3)*7+(c+3)] = SQR(avg - distance(contour_row[i+1], contour_row[i]+r, contour_col[i+1], contour_col[i]+c));
			}
			else
			{
				total_int2_energy[(r+3)*7+(c+3)] = SQR(avg - distance(contour_row[0], contour_row[i]+r, contour_col[0], contour_col[i]+c));
			}
		}
	}
	return;
}

/*----------------------------------------------------------------------------------------------------------------------*/

void external_energy(float *total_ext_energy, float *sobel_image, int *contour_row, int *contour_col, int COLS, int i)
{
	int r, c;
	
	for (r = -3; r <= 3; r++)
	{
		for (c = -3; c <= 3; c++)
		{
			total_ext_energy[(r+3)*7+(c+3)] = SQR(sobel_image[(contour_row[i]+r)*COLS+(contour_col[i]+c)]);
		}
	}
	return;	
}

/*----------------------------------------------------------------------------------------------------------------------*/

void active_contour(float *sobel_image, int *contour_row, int *contour_col, int ROWS, int COLS)
{
	int i, j, k, l;
	
	float 	*internal_1;
	float 	*internal_2;
	float 	*external;
	float 	*total_energy;
	int   	*dummy_row;
	int   	*dummy_col;
	int   	min_row;
	int   	min_col;
	float 	average_distance;
	
	internal_1   = (float *)calloc(7*7, sizeof(float));
	internal_2   = (float *)calloc(7*7, sizeof(float));
	external   	 = (float *)calloc(7*7, sizeof(float));
	total_energy = (float *)calloc(7*7, sizeof(float));
	dummy_row    = (int *)calloc(42, sizeof(int));
	dummy_col    = (int *)calloc(42, sizeof(int));
	
	normalize(sobel_image,  ROWS, COLS, 1);
	invert_sobel(sobel_image, ROWS, COLS);

	for (i = 0; i < 31; i++)
	{
		average_distance = 0;
		for (j = 0; j < 41; j++)
		{
			average_distance += distance(contour_row[j+1], contour_row[j], contour_col[j+1], contour_col[j]);
		}
		average_distance += distance(contour_row[0], contour_row[j], contour_col[0], contour_col[j]);
		average_distance = average_distance/42;
	
		for (j = 0; j < 42; j++)
		{
			internal_energy_1(internal_1, contour_row, contour_col, j);
			internal_energy_2(internal_2, contour_row, contour_col, average_distance, j);
			external_energy(external, sobel_image, contour_row, contour_col, COLS, j);
			normalize(internal_1, 7, 7, 1);
			normalize(internal_2, 7, 7, 1);
			
			for (k = 0; k < 7*7; k++)
			{
				total_energy[k] =  internal_1[k] + internal_2[k] + 2*external[k];
			}
			
			minimum(&min_row, &min_col, total_energy);
			
			dummy_row[j] = contour_row[j]+min_row;
			dummy_col[j] = contour_col[j]+min_col;
		}
		for (j = 0; j < 42; j++)
		{
			contour_row[j] = dummy_row[j];
			contour_col[j] = dummy_col[j];
		}
	}
	return;
}

/*--------------------------------------------------------------------------------------------------------------------*/

int main()
{
	FILE 			*fpt;
	unsigned char	*image;
	unsigned char	*sobel_final;
	unsigned char	*final;
	float			*sobel_image;
	int				*contour_row;
	int				*contour_col;
	char			header[320];
	int				ROWS,COLS,BYTES;
	int 			i;

	/* read image */
	if ((fpt=fopen("D:/Computer_Vision/Lab5/hawk.ppm","rb")) == NULL)
	{
		printf("Unable to open hawk.ppm for reading\n");
		exit(0);
	}
	fscanf(fpt,"%s %d %d %d",header,&COLS,&ROWS,&BYTES);
	if (strcmp(header,"P5") != 0  ||  BYTES != 255)
	{
		printf("Not a greyscale 8-bit PPM image\n");
		exit(0);
	}
	image=(unsigned char *)calloc(ROWS*COLS,sizeof(unsigned char));
	header[0]=fgetc(fpt);	/* read white-space character that separates header */
	fread(image,1,COLS*ROWS,fpt);
	fclose(fpt);
	
	if ((fpt=fopen("D:/Computer_Vision/Lab5/hawk_init.txt", "rb")) == NULL)
	{
		printf("Unable to open intial contours file\n");
		exit(0);
	}
	
	contour_row = (int *)calloc(42, sizeof(int));
	contour_col = (int *)calloc(42, sizeof(int));

	i = 0;
	while(!feof(fpt))
	{
		fscanf(fpt, "%d %d\n", &contour_col[i], &contour_row[i]);
		i++;
	}
	
	final = (unsigned char *)calloc(ROWS*COLS, sizeof(unsigned char));
	for (i = 0; i < ROWS*COLS; i++)
		final[i] = image[i];
	
	initial_image(final, contour_row, contour_col, ROWS, COLS);
	fpt = fopen("D:/Computer_Vision/Lab5/initial.ppm", "wb");
	fprintf(fpt, "P5 %d %d 255\n", COLS, ROWS);
	fwrite(final, COLS*ROWS, 1, fpt);
	fclose(fpt);
	
	sobel_image = (float *)calloc(ROWS*COLS, sizeof(float));
	sobel_final = (unsigned char *)calloc(ROWS*COLS, sizeof(unsigned char));

	sobel_filter(image, sobel_image, ROWS, COLS);
	normalize(sobel_image, ROWS, COLS, 255);
	for (i = 0; i < ROWS*COLS; i++)
  	{
    sobel_final[i] = sobel_image[i];
  	}
	fpt = fopen("D:/Computer_Vision/Lab5/sobel.ppm", "wb");
	fprintf(fpt, "P5 %d %d 255\n", COLS, ROWS);
	fwrite(sobel_final, COLS*ROWS, 1, fpt);
	fclose(fpt);

	active_contour(sobel_image, contour_row, contour_col, ROWS, COLS);
	final = (unsigned char *)calloc(ROWS*COLS, sizeof(unsigned char));
	for (i = 0; i < ROWS*COLS; i++)
		final[i] = image[i];
	
	initial_image(final, contour_row, contour_col, ROWS, COLS);
	
	fpt = fopen("D:/Computer_Vision/Lab5/final.ppm", "wb");
	fprintf(fpt, "P5 %d %d 255\n", COLS, ROWS);
	fwrite(final, COLS*ROWS, 1, fpt);
	fclose(fpt);
	
	for (i = 0; i < 42; i++)
	{
		printf("Contour Point %d: %d %d\n", i, contour_row[i], contour_col[i]);
	}
	return(0);
	
}