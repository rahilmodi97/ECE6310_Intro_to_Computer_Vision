# ECE6310_Intro_to_Computer_Vision
This repository will contain all the labs which I have done throughout the course.

* Lab1 - Convolution and timing.
In this lab we used different convolution methods to smoothen a given image with the help of 7x7 kernel. It involved smoothing by a standard 7x7 kernal, separable filters 
which involved horizontal (1x7) and vertical (7x1) filters and the last part involved separable filters with sliding window. We also performed time comparison between all the 3
methods to determine which was the fastest method. Below on the left is the original image and the image on the right is the convoluted image, the output for all the three methods
is same.

<img align="left" src="https://user-images.githubusercontent.com/56795228/93153132-8bf4e600-f6ce-11ea-9c81-09b32d522b07.jpg" width="300">    <img align="right" src="https://user-images.githubusercontent.com/56795228/93153144-931bf400-f6ce-11ea-844f-77327014a794.jpg" width="300"><br /><br /><br /><br /><br /><br /><br /><br /><br /><br /><br /><br /><br />

* Lab2 - Optical Character Recognition.
In this lab we had to implement a matched filter (normalized crosscorrelation) to recognize letters in an image of text. After applying Matched Spatial Filter and normalizing it
a binary image was outputted which showed white dots at places where the letter e was located in the original image. Original Image is on the left, center image is the MSF convoluted image and on the right is the binary image.

<img align="left" src="https://user-images.githubusercontent.com/56795228/93153443-41279e00-f6cf-11ea-9e79-a1f486c8b47f.jpg" width="300">
<p align="center><img src="https://user-images.githubusercontent.com/56795228/93153408-2f45fb00-f6cf-11ea-843b-d30ccfbc313b.jpg" width="300"></p>
<img align="right" src="https://user-images.githubusercontent.com/56795228/93153414-3240eb80-f6cf-11ea-86e9-6c0ddf6903d1.jpg" width="300">
