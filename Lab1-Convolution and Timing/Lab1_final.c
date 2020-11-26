
	/*
    ** Lab1 ECE 6310 
	** This program reads bridge.ppm, a 512 x 512 PPM image.
	** It smooths it using a standard 7x7 mean filter.
    ** It smooths it with separable filters.
    ** It also smooths it with separable filters and sliding windows.
	** The program also demonstrates how to time a piece of code.
	**
	** To compile, must link using -lrt  (man clock_gettime() function).
	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


void normal_filter()
{
    FILE		    *fpt;
    unsigned char	*image;
    unsigned char	*smoothed;
    char		     header[320];
    int		         ROWS,COLS,BYTES;
    int		         r,c,r2,c2,sum;
    struct timespec	 tp1,tp2;

        /* read image */
    if ((fpt=fopen("bridge.ppm","rb")) == NULL)
    {
    printf("Unable to open bridge.ppm for reading\n");
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

        /* allocate memory for smoothed version of image */
    smoothed=(unsigned char *)calloc(ROWS*COLS,sizeof(unsigned char));

        /* query timer */
    clock_gettime(CLOCK_REALTIME,&tp1);
    printf("%ld %ld\n",(long int)tp1.tv_sec,tp1.tv_nsec);

        /* smooth image, skipping the border points */
    for (r=0; r<ROWS; r++)
    for (c=0; c<COLS; c++)
        {
        sum=0;
        if (r<3 || r>=ROWS-3)
        {
            sum=0; 
        }
        else if (c<3 || c>=COLS-3)
        {
            sum=0;
        }
        else
        {
        for (r2=-3; r2<=3; r2++)
        for (c2=-3; c2<=3; c2++)
            sum += image[(r+r2)*COLS+(c+c2)];
        smoothed[r*COLS+c]=sum/49; 
        }
        }

        /* query timer */
    clock_gettime(CLOCK_REALTIME,&tp2);
    printf("%ld %ld\n",(long int)tp2.tv_sec,tp2.tv_nsec);

        /* report how long it took to smooth */
    printf("%ld\n",tp2.tv_nsec-tp1.tv_nsec);

        /* write out smoothed image to see result */
    fpt=fopen("smoothed.ppm","w");
    fprintf(fpt,"P5 %d %d 255\n",COLS,ROWS);
    fwrite(smoothed,COLS*ROWS,1,fpt);
    fclose(fpt);
    free(image);
    free(smoothed);
    return;
}

void separable_filter()
{
    FILE		    *fpt;
    unsigned char	*image;
    unsigned char	*smoothed_2;
    float           *smoothed;
    char		     header[320];
    int		         ROWS,COLS,BYTES;
    int		         r,c,r2,c2,sum;
    struct timespec	 tp1,tp2;

        /* read image */
    if ((fpt=fopen("bridge.ppm","rb")) == NULL)
    {
    printf("Unable to open bridge.ppm for reading\n");
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

        /* allocate memory for smoothed version of image */
    smoothed=(float *)calloc(ROWS*COLS,sizeof(float));
    smoothed_2=(unsigned char *)calloc(ROWS*COLS,sizeof(unsigned char));


        /* query timer */
    clock_gettime(CLOCK_REALTIME,&tp1);
    printf("%ld %ld\n",(long int)tp1.tv_sec,tp1.tv_nsec);

        /* smooth image, skipping the border points */
        /* Horizontal filter (1x7)*/
    for (r=0; r<ROWS; r++)
    for (c=0; c<COLS; c++)
    {
        sum=0;
        if (c<3 || c>=COLS-3)
        {
            sum=0; 
        }
        else
        {
        for (c2=-3; c2<=3; c2++)
        sum+=image[(r)*COLS+(c+c2)];
        smoothed[r*COLS+c]=sum;
        }
    }
        /* Vertical filter (7x1) */
    for (r=0; r<ROWS; r++)
    for (c=0; c<COLS; c++)
    {
        sum=0;
        if (r<3 || r>=ROWS-3)
        {
            sum=0; 
        }
        else
        {
        for (r2=-3; r2<=3; r2++)
        sum+=smoothed[(r+r2)*COLS+(c)];
        smoothed_2[r*COLS+c]=sum/49;
        }
    }

        /* query timer */
    clock_gettime(CLOCK_REALTIME,&tp2);
    printf("%ld %ld\n",(long int)tp2.tv_sec,tp2.tv_nsec);

        /* report how long it took to smooth */
    printf("%ld\n",tp2.tv_nsec-tp1.tv_nsec);

        /* write out smoothed image to see result */
    fpt=fopen("smoothed_sp.ppm","w");
    fprintf(fpt,"P5 %d %d 255\n",COLS,ROWS);
    fwrite(smoothed_2,COLS*ROWS,1,fpt);
    fclose(fpt);
    free(image);
    free(smoothed);
    free(smoothed_2);
    return;
}

void separable_sliding_filter()
{
    FILE		    *fpt;
    unsigned char	*image;
    unsigned char	*smoothed_2;
    float           *smoothed;
    char		     header[320];
    int		         ROWS,COLS,BYTES;
    int		         r,c,r2,c2;
    float            sum;
    struct timespec	 tp1,tp2;

        /* read image */
    if ((fpt=fopen("bridge.ppm","rb")) == NULL)
    {
    printf("Unable to open bridge.ppm for reading\n");
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

        /* allocate memory for smoothed version of image */
    smoothed=(float *)calloc(ROWS*COLS,sizeof(float));
    smoothed_2=(unsigned char *)calloc(ROWS*COLS,sizeof(unsigned char));


        /* query timer */
    clock_gettime(CLOCK_REALTIME,&tp1);
    printf("%ld %ld\n",(long int)tp1.tv_sec,tp1.tv_nsec);

        /* smooth image, skipping the border points */
    for (r=0; r<ROWS; r++)
    {
        sum = 0;
        for (c=0; c<COLS; c++)
        {
            if (c<3 || c>=COLS-3)
        {
            sum=0; 
        }
        else
        {
            for (c2=-3; c2<=3; c2++)
            {
                if (c == 3)
                {
                    sum+=image[(c+c2)+r*COLS];
                }
                else
                {
                    sum = sum - image[(c2+c-1)+r*COLS] + image[(c+c2+6)+r*COLS];
                    break;
                }
            }
            smoothed[(r)*COLS+c]=sum;
        }
        }    
    }
    for (c=0; c<COLS; c++)
    {
        sum = 0;
        for (r=0; r<ROWS; r++) 
        {
            if (r<3 || r>=ROWS-3)
        {
            sum=0; 
        }
        else
        {
            for (r2=-3; r2<=3; r2++)
            {
                if(r==3)
                {
                    sum+=smoothed[(r+r2)*COLS+c];
                }
                else
                {
                    sum = sum - smoothed[(r+r2-1)*COLS+c] + smoothed[(r+r2+6)*COLS+c];
                    break;
                }
            }
            smoothed_2[(r)*COLS+c]=(sum/49);
        }
        }   
    }
        /* query timer */
    clock_gettime(CLOCK_REALTIME,&tp2);
    printf("%ld %ld\n",(long int)tp2.tv_sec,tp2.tv_nsec);

        /* report how long it took to smooth */
    printf("%ld\n",tp2.tv_nsec-tp1.tv_nsec);

        /* write out smoothed image to see result */
    fpt=fopen("smoothed_spsw.ppm","w");
    fprintf(fpt,"P5 %d %d 255\n",COLS,ROWS);
    fwrite(smoothed_2,COLS*ROWS,1,fpt);
    fclose(fpt);
    return;
}

int main()
{
printf("7 x 7 filter starting.\n");
printf("\n");
normal_filter();
printf("\n");
printf("Normal 7x7 filter output generated as smoothed.ppm\n");
printf("\n");
printf("Separable filters code starting.\n");
printf("\n");
separable_filter();
printf("\n");
printf("Separable filters output generated as smoothed_sp.ppm\n");
printf("\n");
printf("Separable filters with sliding window starting.\n");
printf("\n");
separable_sliding_filter();
printf("\n");
printf("Separable filters with sliding windows output generated as smoothed_spsw.ppm\n");
return 0;
}

