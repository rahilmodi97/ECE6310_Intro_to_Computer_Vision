#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>

/* Submitted by Rahil Modi C14109603 */
typedef struct cart_coord_s
{
	double X;
	double Y;
	double Z;
} cart_coord_t;

#define MAX_QUEUE 10000	/* max perimeter size (pixels) of border wavefront */
#define SQR(x) ((x)*(x))
#define ANGLE_THRESH 0.72

/*-------------------------------------------------------------------------------------------------------*/

void color_image(unsigned char *grayscale, int ROWS, int COLS)
{
	FILE            *fpt;
	int             r, c;
	unsigned char   color[3];
	
	fpt = fopen("D:/Computer_Vision/Lab8/colored.ppm", "wb");
	fprintf(fpt, "P6 %d %d 255\n", COLS, ROWS);
	
	for (r = 0; r < ROWS; r++)
	{
		for (c = 0; c < COLS; c++)
		{
			color[0] = (grayscale[r*COLS+c] * 4) % 256;
			color[1] = (grayscale[r*COLS+c] * 2) % 256;
			color[2] = (grayscale[r*COLS+c] * 3) % 256;
			fwrite(color, 1, 3, fpt);
		}
	}
	fclose(fpt);
	
	return;
}

/* -------------------------------------------------------------------------------------------------------*/

void surface_normal(cart_coord_t *coordinates, cart_coord_t *surface_norm, int ROWS, int COLS)
{
	int    r;
	int    c;
	double x1;
	double x2;
	double y1;
	double y2;
	double z1;
	double z2;
	
	for (r = 0; r < ROWS - 3; r++)
	{
		for (c = 0; c < COLS - 3; c++)
		{
			x1 = coordinates[r*COLS+(c+3)].X - coordinates[r*COLS+c].X;
			x2 = coordinates[(r+3)*COLS+c].X - coordinates[r*COLS+c].X;
			y1 = coordinates[r*COLS+(c+3)].Y - coordinates[r*COLS+c].Y;
			y2 = coordinates[(r+3)*COLS+c].Y - coordinates[r*COLS+c].Y;
			z1 = coordinates[r*COLS+(c+3)].Z - coordinates[r*COLS+c].Z;
			z2 = coordinates[(r+3)*COLS+c].Z - coordinates[r*COLS+c].Z;
			
			surface_norm[r*COLS+c].X = (y1 * z2) - (z1 * y2);
			surface_norm[r*COLS+c].Y = ((z1 * x2) - (z2 * x1))*-1;
			surface_norm[r*COLS+c].Z = (x1 * y2) - (y1 * x2);
		}
	}
	
	return;
}

/*------------------------------------------------------------------------------------------------------*/

void threshold(unsigned char *image, unsigned char *output_image, int thresh_val, int ROWS, int COLS)
{
  int i;
  for (i = 0; i < ROWS*COLS; i++)
  {
    if (image[i] > thresh_val)
    {
		output_image[i] = 255;
    }
    else
    {
		output_image[i] = image[i];
    }
  }

  return;
}

/*----------------------------------------------------------------------------------------------------------------*/

void odetics_to_coords(unsigned char *RangeImage, cart_coord_t *coordinates, int ROWS, int COLS)
{
	int	    r,c;
	double	cp[7];
	double	xangle,yangle,dist;
	double	ScanDirectionFlag,SlantCorrection;
	
	ScanDirectionFlag = 1;

	cp[0]=1220.7;		/* horizontal mirror angular velocity in rpm */
	cp[1]=32.0;		/* scan time per single pixel in microseconds */
	cp[2]=(COLS/2)-0.5;		/* middle value of columns */
	cp[3]=1220.7/192.0;	/* vertical mirror angular velocity in rpm */
	cp[4]=6.14;		/* scan time (with retrace) per line in milliseconds */
	cp[5]=(ROWS/2)-0.5;		/* middle value of rows */
	cp[6]=10.0;		/* standoff distance in range units (3.66cm per r.u.) */

	cp[0]=cp[0]*3.1415927/30.0;	/* convert rpm to rad/sec */
	cp[3]=cp[3]*3.1415927/30.0;	/* convert rpm to rad/sec */
	cp[0]=2.0*cp[0];		/* beam ang. vel. is twice mirror ang. vel. */
	cp[3]=2.0*cp[3];		/* beam ang. vel. is twice mirror ang. vel. */
	cp[1]/=1000000.0;		/* units are microseconds : 10^-6 */
	cp[4]/=1000.0;			/* units are milliseconds : 10^-3 */

	/* start with semi-spherical coordinates from laser-range-finder: */
	/*			(r,c,RangeImage[r*COLS+c])		  */
	/* convert those to axis-independant spherical coordinates:	  */
	/*			(xangle,yangle,dist)			  */
	/* then convert the spherical coordinates to cartesian:           */
	/*			(P => X[] Y[] Z[])			  */

	for (r=0; r<ROWS; r++)
	{
		for (c=0; c<COLS; c++)
		{
		  SlantCorrection=cp[3]*cp[1]*((double)c-cp[2]);
		  xangle=cp[0]*cp[1]*((double)c-cp[2]);
		  yangle=(cp[3]*cp[4]*(cp[5]-(double)r))+SlantCorrection*ScanDirectionFlag;	/*  + slant correction */
		  dist=(double)RangeImage[r*COLS+c]+cp[6];
		  coordinates[r*COLS+c].Z=sqrt((dist*dist)/(1.0+(tan(xangle)*tan(xangle))+(tan(yangle)*tan(yangle))));
		  coordinates[r*COLS+c].X=tan(xangle)*coordinates[r*COLS+c].Z;
		  coordinates[r*COLS+c].Y=tan(yangle)*coordinates[r*COLS+c].Z;
		}
	}
	
	return;
}

/*--------------------------------------------------------------------------------------------------------------------------------*/
/*
** Given an image, a starting point, and a label, this routine
** paint-fills (8-connected) the area with the given new label
** according to the given criteria (pixels close to the average
** intensity of the growing region are allowed to join).
*/

void RegionGrow(unsigned char *image,	/* image data */
	            unsigned char *labels,	/* segmentation labels */
	            int r, int c,		/* pixel to paint from */
	            int ROWS, int COLS,
	            int           paint_over_label,	/* image label to paint over */
	            int           new_label,		/* image label for painting */
	            cart_coord_t  *surface_norm, /* Input: Surface Noraml Image */
	            int           *indices,		/* output:  indices of pixels painted */
	            int           *count         /* output:  count of pixels painted */
	)
{
	int	            r2, c2;
	int	            queue[MAX_QUEUE], qh, qt;
	int             current_position;
	cart_coord_t    average, total; 
	double          dot_product;
	double          dummyX;
	double          dummyY;
	double          dummyZ;
	double          magnitude_1;	
	double          magnitude_2;	
	double          angle;
	
	*count = 0;
	
	if (labels[r * COLS + c] != paint_over_label)
		return;
	labels[r * COLS + c] = new_label;
	
	average.X = surface_norm[r*COLS+c].X;
	average.Y = surface_norm[r*COLS+c].Y;
	average.Z = surface_norm[r*COLS+c].Z;
	
	total.X = surface_norm[r*COLS+c].X;
	total.Y = surface_norm[r*COLS+c].Y;
	total.Z = surface_norm[r*COLS+c].Z;
	
	if (indices != NULL)
		indices[0] = r*COLS + c;
	
	queue[0] = r*COLS + c;
	qh = 1;	/* queue head */
	qt = 0;	/* queue tail */
	(*count) = 1;
	
	while (qt != qh)
	{
		for (r2 = -1; r2 <= 1; r2++)
			for (c2 = -1; c2 <= 1; c2++)
			{
				if (r2 == 0 && c2 == 0)
					continue;
				if ((queue[qt] / COLS + r2) < 0 || (queue[qt] / COLS + r2) >= ROWS - 3 ||
					(queue[qt] % COLS + c2) < 0 || (queue[qt] % COLS + c2) >= COLS - 3)
					continue;
				if (labels[(queue[qt] / COLS + r2)*COLS + queue[qt] % COLS + c2] != paint_over_label)
					continue;
				
				current_position = (queue[qt] / COLS + r2) * COLS + (queue[qt] % COLS + c2);
				
				/* test criteria to join region */
				dummyX = (average.X * surface_norm[current_position].X);
				dummyY = (average.Y * surface_norm[current_position].Y);
				dummyZ = (average.Z * surface_norm[current_position].Z);
				
				dot_product = dummyX + dummyY + dummyZ;
				magnitude_1 = sqrt(SQR(average.X) + SQR(average.Y) + SQR(average.Z));
				magnitude_2 = sqrt(SQR(surface_norm[current_position].X) + SQR(surface_norm[current_position].Y) + SQR(surface_norm[current_position].Z));
				
				angle = acos(dot_product / (magnitude_1 * magnitude_2));
				
				if (angle > ANGLE_THRESH)
					continue;

				labels[(queue[qt] / COLS + r2)*COLS + queue[qt] % COLS + c2] = new_label;
				if (indices != NULL)
					indices[*count] = (queue[qt] / COLS + r2)*COLS + queue[qt] % COLS + c2;
				
				total.X += surface_norm[current_position].X;
				total.Y += surface_norm[current_position].Y;
				total.Z += surface_norm[current_position].Z;
				
				(*count)++;
				
				average.X = total.X / (*count);
				average.Y = total.Y / (*count);
				average.Z = total.Z / (*count);
				
				queue[qh] = (queue[qt] / COLS + r2)*COLS + queue[qt] % COLS + c2;
				qh = (qh + 1) % MAX_QUEUE;
				if (qh == qt)
				{
					printf("Max queue size exceeded\n");
					exit(0);
				}
			}
		qt = (qt + 1) % MAX_QUEUE;

	}
	printf("X: %lf, Y: %lf, Z: %lf, Count: %d\n", average.X, average.Y, average.Z, (*count));
}

/*---------------------------------------------------------------------------------------------------------*/

int main ()
{
	FILE          *fpt;
	int           ROWS, COLS, BYTES;
	char          header[320];
	cart_coord_t  *coordinates;
	unsigned char *RangeImage;
	unsigned char *threshold_image;
	cart_coord_t  *surface_norm;
	unsigned char *output_image;
	char          *output_filename;
	int           i, j, k;
	int           r, c;
	int           *indices;
	bool          seed_possible;
	int           RegionSize, TotalRegions;
	
	/* read image */
	if ((fpt=fopen("D:/Computer_Vision/Lab8/chair-range.ppm","rb")) == NULL)
	{
		printf("Unable to open range image for reading\n");
		exit(0);
	}
	fscanf(fpt,"%s %d %d %d",header,&COLS,&ROWS,&BYTES);
	if (strcmp(header,"P5") != 0  ||  BYTES != 255)
	{
		printf("Not a greyscale 8-bit PPM image\n");
		exit(0);
	}
	RangeImage = (unsigned char *)calloc(ROWS*COLS,sizeof(unsigned char));
	header[0] = fgetc(fpt);	/* read white-space character that separates header */
	fread(RangeImage, 1, COLS*ROWS, fpt);
	fclose(fpt);
	
	coordinates = (cart_coord_t *)calloc(ROWS*COLS, sizeof(cart_coord_t));
	surface_norm = (cart_coord_t *)calloc(ROWS*COLS, sizeof(cart_coord_t));

	threshold_image = (unsigned char *)calloc(ROWS*COLS, sizeof(unsigned char));
	output_image = (unsigned char *)calloc(ROWS*COLS, sizeof(unsigned char));
	
	threshold(RangeImage, threshold_image, 137, ROWS, COLS);
    fpt = fopen("D:/Computer_Vision/Lab8/threshold.ppm", "wb");
	fprintf(fpt, "P5 %d %d 255\n", COLS, ROWS);
	fwrite(threshold_image, COLS*ROWS, sizeof(unsigned char), fpt);
	fclose(fpt);
	
	odetics_to_coords(RangeImage, coordinates, ROWS, COLS);
	surface_normal(coordinates, surface_norm, ROWS, COLS);
	
	indices = (int *)calloc(ROWS*COLS, sizeof(int));
	TotalRegions = 30;
	
	for (i = 2; i < ROWS-2; i++)
	{
		for (j = 2; j < COLS-2; j++)
		{
			seed_possible = true;
			for (r = -2; r <= 2; r++)
			{
				for (c = -2; c <= 2; c++)
				{
					if ((threshold_image[(i+r)*COLS+(j+c)] == 255) || output_image[(i+r)*COLS+(j+c)] != 0)
					{
						seed_possible = false;
					}
				}
			}
			if (seed_possible)
			{
				printf("I: %d, J: %d\n", i, j);
				TotalRegions += 30;
				RegionGrow(RangeImage, output_image, i, j, ROWS, COLS, 0, TotalRegions, surface_norm, indices, &RegionSize);
				if (RegionSize < 100)
				{
					for (k = 0; k < RegionSize; k++)
					{
						output_image[indices[k]] = 0;
					}
					TotalRegions -= 30;
				}
				else
				{
					printf("Region Number: %d \t Number of Pixels: %d\n", (TotalRegions/30)-1, RegionSize);
				}
			}
			
		}
	}
	
    fpt = fopen("D:/Computer_Vision/Lab8/output.ppm", "wb");
	fprintf(fpt, "P5 %d %d 255\n", COLS, ROWS);
	fwrite(output_image, COLS*ROWS, sizeof(unsigned char), fpt);
	fclose(fpt);
	color_image(output_image, ROWS, COLS);
	
	return (0);
}