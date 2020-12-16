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
a binary image was outputted which showed white dots at places where the letter e was located in the original image.

Original Image:

<img src="https://user-images.githubusercontent.com/56795228/93153443-41279e00-f6cf-11ea-9e79-a1f486c8b47f.jpg" width="300">

MSF Image:

<img src="https://user-images.githubusercontent.com/56795228/93153408-2f45fb00-f6cf-11ea-843b-d30ccfbc313b.jpg" width="300">

Binary Image:

<img src="https://user-images.githubusercontent.com/56795228/93153414-3240eb80-f6cf-11ea-86e9-6c0ddf6903d1.jpg" width="300">

* Lab3 - Letters

In this lab we had to implement thinning, branchpoint and endpoint detection to recognize letters in an image of text. This lab was built upon Lab 2. The original image and the MSF image were the same from the previous lab. After looping through every ground truth point and copying 9x15 area around it, thresholding it to 128 later thinning that copied image and at last checking for branchpoints and endpoints in that image as letter e has 1 branchpoint and 1 endpoint and cross checking it with ground truth and generating True positive rate and False positive rate and generating ROC Curve and finding the most optimal threshold point with good balance between tru positive and false positive.

Copied Image of e:

<img src="https://user-images.githubusercontent.com/56795228/94979493-02784d00-04f1-11eb-9e61-009d9743030f.jpg" width="100">

Thresholded image at 128:

<img src="https://user-images.githubusercontent.com/56795228/94979491-01dfb680-04f1-11eb-862d-5505771c633c.jpg" width="100">

Thinned Image:

<img src="https://user-images.githubusercontent.com/56795228/94979494-02784d00-04f1-11eb-980d-4c43928dcf6b.jpg" width="100">

* Lab4 - Region Interaction with a GUI

In this lab we had to implement region grow and also implement 2 predicates for distance of region grow and pixel intensity difference for the region grow which would decide till what pixel values the region grow should take place.

Example 1 in GUI:

<img src="https://user-images.githubusercontent.com/56795228/96209617-0a4adf00-0f3e-11eb-86c4-bc81df8f03fb.png" width="300">

Example 2 in GUI:

<img src="https://user-images.githubusercontent.com/56795228/96209627-120a8380-0f3e-11eb-9907-0ea0d9c9f4f1.png" width="300">

* Lab 5 - Active Contour

In this lab we had to active contouring algorithm on a given image of eagle sitting on the branch, the initial contour points were given and with the help of 2 internal energy
and one external energy the initial contour had to converge properly to the border of the eagle.

Image with initial contour points:

<img src="https://user-images.githubusercontent.com/56795228/100312854-551a3680-2f81-11eb-99e6-1157f1c46288.jpg" width="300">

Sobel filter output of the initial image:

<img src="https://user-images.githubusercontent.com/56795228/100312861-59465400-2f81-11eb-8f4c-5d2e533130d1.jpg" width="300">

Image with the final contour points:

<img src="https://user-images.githubusercontent.com/56795228/100312865-5ba8ae00-2f81-11eb-9c6a-efbcf555aeb2.jpg" width="300">

* Lab 6 - Camera Calibration and Tracking

In this lab calibration of 6 cameras in a room is required and after the calibration of the cameras tracking of anything in the defined area ahould be possible.

Room where tracking was to be performed:

<img src="https://user-images.githubusercontent.com/56795228/100313414-9a8b3380-2f82-11eb-9202-81dd497f8d2b.png" width="400">

Calibration image of one of the camera:

<img src="https://user-images.githubusercontent.com/56795228/100313450-ad9e0380-2f82-11eb-9d33-8d3d1d300dc0.png" width="400">

Occupancy map with camera tracking the objects in the defined area:

<img src="https://user-images.githubusercontent.com/56795228/100313478-b68ed500-2f82-11eb-8c71-9ba403159650.png" width="400">

* Lab 7 - Motion tracking 

In this lab linear motion of a mobile was required to be detected by the gyroscope and accelerometer data recorded by the phone itself. The details of the lab and results can be seen in the report of the lab which is the Lab 7 folder.







