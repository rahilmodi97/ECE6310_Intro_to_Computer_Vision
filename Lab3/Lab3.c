/* Rahil Modi - C14109603
 * Lab2 Optical Character Recognition
 * In this lab we are asked to develop a code 
 * that will recognition a particular letter from a image.
 * This program will take a image full of text, 
 * a template of specific letter and ground truth of all the letter 
 * and detect the position of the specific letter.
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
struct coordinates_s
{
  int  column;
  int  row;
  char letter;
};

/*-----------------------------------------------------------------------------------------------------------*/
void threshold(unsigned char *image, unsigned char *dummy_image, int ROWS, int COLS, int threshold_value)
{
  int i;
  for (i = 0; i < ROWS*COLS; i++)
  {
    if (image[i] > threshold_value)
    {
    dummy_image[i] = 255;
    }
    else
    {
    dummy_image[i] = 0;
    }
  }
  return; 
}

/*----------------------------------------------------------------------------------------------------------*/
void create_copy(unsigned char *image, unsigned char *dummy, int rows, int cols, int column)
{
  int j, k, l, m;
  m = 0;
  for (j = rows - 7; j <= rows + 7; j++ )
  {
    l = 0;
    for (k = cols - 4; k <= cols + 4; k++)
    {
      dummy[m*9+l] = image[j*column+k];
      l++;
    }
    m++;
  }

}
/*--------------------------------------------------------------------------------------------------------------*/

void thinning(unsigned char *dummy)
{
  int row, col, i, neighbors, edge2nedge;
  bool erase;
  bool pre_pixel;

  /* thresholding*/
  for (i = 0; i < 9*15; i++)
  {
    if(dummy[i] < 128)
    {
      dummy[i] = 255;
    }
    else
    {
      dummy[i] = 0;
    }
  }

  erase = true;
  while (erase)
  {
    erase = false;
    for (row = 0; row < 15; row++)
    {
      for (col = 0; col < 9; col++)
      {
        i = row*9+col;
        /* Checking if pixel is on*/
        if ((dummy[i] > 0))
        {
          edge2nedge = 0;
          neighbors = 0;
          /* North */
          if (dummy[i-1-9] == 255)
          {
            neighbors++;
            pre_pixel = true;
          }
          else
          {
            pre_pixel = false;
          }
          /* North East */
          if (dummy[i-9] == 255)
          {
            neighbors++;
            pre_pixel = true;
          }
          else
          {
            if (pre_pixel)
            {
              edge2nedge++;
            }
            pre_pixel = false;
          }
          /* East */
          if (dummy[i-9+1] == 255)
          {
            neighbors++;
            pre_pixel = true;
          }
          else
          {
            if (pre_pixel)
            {
              edge2nedge++;
            }
            pre_pixel = false;
          }
          /* South East */
          if (dummy[i+1] == 255)
          {
            neighbors++;
            pre_pixel = true;
          }
          else
          {
            if (pre_pixel)
            {
              edge2nedge++;
            }
            pre_pixel = false;
          }
          /* South */
          if (dummy[i+1+9] == 255)
          {
            neighbors++;
            pre_pixel = true;
          }
          else
          {
            if (pre_pixel)
            {
              edge2nedge++;
            }
            pre_pixel = false;
          }
          /* South West */
          if (dummy[i+9] == 255)
          {
            neighbors++;
            pre_pixel = true;
          }
          else
          {
            if (pre_pixel)
            {
              edge2nedge++;
            }
            pre_pixel = false;
          }
          /* West */
          if (dummy[i+9-1] == 255)
          {
            neighbors++;
            pre_pixel = true;
          }
          else
          {
            if (pre_pixel)
            {
              edge2nedge++;
            }
            pre_pixel = false;
          }
          /* North West */
          if (dummy[i-1] == 255)
          {
            neighbors++;
            pre_pixel = true;
          }
          else
          {
            if (pre_pixel)
            {
              edge2nedge++;
            }
            pre_pixel = false;
          }
          /* loop to check edge to non edge for north west to north*/
          if ((dummy[i-1-9] == 0) && pre_pixel)
          {
            edge2nedge++;
          }

          /* loop to erase the pixel */
          if (edge2nedge == 1)
          {
            if (2 <= neighbors && neighbors <= 6)
            {
              if ((dummy[i-9] == 0) || (dummy[i+1] == 0) || ((dummy[i-1] == 0) && (dummy[i+9] == 0)))
              {
                dummy[i] = 0;
                erase = true;
              }
            }
          }
        }
      }
    }
  }
  return;
}
/*------------------------------------------------------------------------------------------------------------------------------*/

bool branchpoint_endpoint_detection(unsigned char *dummy)
{
  int row, col, i, endpoint, branchpoint, edge2nedge;
  bool letter_e;
  bool pre_pixel;

  endpoint = 0;
  branchpoint = 0;

  for (row = 0; row < 15; row++)
   {
     for (col = 0; col < 9; col++)
     {
        i = row*9+col;
        /* Checking if pixel is on*/
        if (dummy[i] > 0)
        {
          edge2nedge = 0;
          /* North */
          if (dummy[i-9] == 255)
          {
            pre_pixel = true;
          }
          else
          {
            pre_pixel = false;
          }
          /* North East */
          if (dummy[i-9+1] == 255)
          {
            pre_pixel = true;
          }
          else
          {
            if (pre_pixel)
            {
              edge2nedge++;
            }
            pre_pixel = false;
          }
          /* East */
          if (dummy[i+1] == 255)
          {
            pre_pixel = true;
          }
          else
          {
            if (pre_pixel)
            {
              edge2nedge++;
            }
            pre_pixel = false;
          }
          /* South East */
          if (dummy[i+1+9] == 255)
          {
            pre_pixel = true;
          }
          else
          {
            if (pre_pixel)
            {
              edge2nedge++;
            }
            pre_pixel = false;
          }
          /* South */
          if (dummy[i+9] == 255)
          {
            pre_pixel = true;
          }
          else
          {
            if (pre_pixel)
            {
              edge2nedge++;
            }
            pre_pixel = false;
          }
          /* South West */
          if (dummy[i+9-1] == 255)
          {
            pre_pixel = true;
          }
          else
          {
            if (pre_pixel)
            {
              edge2nedge++;
            }
            pre_pixel = false;
          }
          /* West */
          if (dummy[i-1] == 255)
          {
            pre_pixel = true;
          }
          else
          {
            if (pre_pixel)
            {
              edge2nedge++;
            }
            pre_pixel = false;
          }
          /* North West */
          if (dummy[i-1-9] == 255)
          {
            pre_pixel = true;
          }
          else
          {
            if (pre_pixel)
            {
              edge2nedge++;
            }
            pre_pixel = false;
          }
          /* loop to check edge to non edge for north west to north*/
          if ((dummy[i-9] == 0) && pre_pixel)
          {
            edge2nedge++;
          }
          if (edge2nedge == 1)
          {
            endpoint++;
          }
          else if (edge2nedge > 2)
          {
            branchpoint++;
          }

        }
     }
   }
   if (endpoint == 1 && branchpoint == 1)
   {
     letter_e = true;
   }
   else
   {
     letter_e = false;
   }

   return letter_e;
}
/*------------------------------------------------------------------------------------------------------------------------------*/

int main()
{
  FILE          *fpt, *fpt1, *fpt3;
  unsigned char *image, *dummy; 
  unsigned char  *dummy2;
  char           header[320];
  int            ROWS, COLS, BYTES;
  unsigned char  *MSF;
  int            threshold_value;
  bool           result; 
  bool           letter_e;
  int            tp, fp, fn, tn,r,c, iter, i;
  float          tpr, fpr, sensitivity, specifity;
  int            j, k, l, m;
  struct coordinates_s *coordinates;
  /*-----------------------------------------------------------------------*/
  /* Read Image */
  if ((fpt=fopen("D:/Computer_Vision/Lab3/parenthood.ppm","rb")) == NULL)
  {
	  printf("Unable to open image for reading.\n");
	  exit(0);
  }

  fscanf(fpt, "%s %d %d %d", header, &COLS, &ROWS, &BYTES);
  if (strcmp(header, "P5") != 0 || BYTES != 255)
  {
	  printf("Not a greyscale 8-bit PPM image\n");
	  exit(0);
  }

  image = (unsigned char *)calloc(ROWS*COLS, sizeof(unsigned char));
  header[0] = fgetc(fpt); /* read white-space character that separates header */
  fread(image, 1, COLS*ROWS, fpt);
  fclose(fpt);

  /*----------------------------------------------------------------------*/
  /* allocate memory for the Matched Spatial Filtering */
  MSF = (unsigned char *)calloc(ROWS*COLS, sizeof(unsigned char));
  /* Read the template*/
  if ((fpt=fopen("D:/Computer_Vision/Lab3/MSF.ppm","rb")) == NULL)
  {
	  printf("Unable to open MSF image for reading.\n");
	  exit(0);
  }

  fscanf(fpt, "%s %d %d %d", header, &COLS, &ROWS, &BYTES);
  header[0]   = fgetc(fpt);
  fread(MSF, 1, COLS*ROWS, fpt);

  fclose(fpt);

  /*----------------------------------------------------------------------*/
  /* opening the ground truth */
  fpt = fopen("D:/Computer_Vision/Lab3/parenthood_gt.txt", "rb");
  if (fpt == NULL)
  {
    printf("Error opening ground truth\n");
    exit(0);
  }

  dummy = (unsigned char *)calloc(ROWS*COLS, sizeof(unsigned char));
  dummy2 = (unsigned char *)calloc(15*9, sizeof(unsigned char));
  iter = 0;
  coordinates = (struct coordinates_s*)calloc(1300,sizeof(struct coordinates_s));
  while (!feof(fpt))
  {
    fscanf(fpt, "%c %d %d\n", &coordinates[iter].letter, &coordinates[iter].column, &coordinates[iter].row);
    iter++;
  }
  /*-----------------------------------------------------------------------*/

  for (threshold_value = 50; threshold_value < 256; threshold_value++)
  {
    threshold(MSF, dummy, ROWS, COLS, threshold_value);

    fpt1 = fopen("D:/Computer_Vision/Lab3/ROC.csv", "w");
    fprintf(fpt1, "Threshold,TP,FP,TN,FN,TPR,FPR,Sensitivity,Specificity\n");
    fpt1 = fopen("D:/Computer_Vision/Lab3/ROC.csv", "a");

    tp = 0;
    fp = 0;
    tn = 0;
    fn = 0;
    tpr=0;
    fpr=0;
    specifity=0;
    sensitivity=0;

	// Loop through the ground truth data
    for (i = 0; i < iter; i++)
    {
      result = false;
      letter_e = false; 
	  // Loop through 9x15 area around the coordinates from the ground truth data
      for (r = coordinates[i].row - 7; r <= coordinates[i].row + 7 && result == false; r++)
      {
        for (c = coordinates[i].column - 4; c <= coordinates[i].column + 4 && result == false; c++)
        {
          if (dummy[r*COLS+c] == 255)
          {
            create_copy(image, dummy2, coordinates[i].row, coordinates[i].column, COLS);
            thinning(dummy2);
            letter_e = branchpoint_endpoint_detection(dummy2);
            result = true;
          }
        }
      }
      if (letter_e == true && coordinates[i].letter == 'e')
      {
        tp++;
      }
      else if (letter_e == true && coordinates[i].letter != 'e')
      {
        fp++;
      }
      else if (letter_e != true && coordinates[i].letter == 'e')
      {
        fn++;
      }
      else if (letter_e != true && coordinates[i].letter != 'e')
      {
        tn++;
      }
    }
    tpr = (float) tp/(tp+fn);
    fpr = (float) fp/(fp+tn);
    sensitivity = tpr;
    specifity = 1 - fpr;
    fprintf(fpt1, "%d,%d,%d,%d,%d,%f,%f,%f,%f\n", threshold_value, tp, fp, tn, fn, tpr, fpr, sensitivity, specifity);
  }
  fclose(fpt1);

  create_copy(image, dummy2, 25, 55, COLS);
  fpt3 = fopen("D:/Computer_Vision/Lab3/copy_image.ppm", "w");
  fprintf(fpt3, "P5 %d %d 255\n", 9, 15);
  fwrite(dummy2, 9*15, 1, fpt3);
  fclose(fpt3);

  thinning(dummy2);
  fpt3 = fopen("D:/Computer_Vision/Lab3/Thinned.ppm", "w");
  fprintf(fpt1, "P5 %d %d 255\n", 9, 15);
  fwrite(dummy2, 9*15, 1, fpt3);
  fclose(fpt3);

  branchpoint_endpoint_detection(dummy2);
  fpt3 = fopen("D:/Computer_Vision/Lab3/branch_end.ppm", "w");
  fprintf(fpt1, "P5 %d %d 255\n", 9, 15);
  fwrite(dummy2, 9*15, 1, fpt3);
  fclose(fpt3);

 return(1);
}


