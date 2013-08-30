#include <iostream>
#include <stdio.h>
#include "cv.h"
#include "highgui.h"
#include "PlateLocation.h"

using namespace cv;
using namespace std;
//int main(void){	
void testePlateLocation(void){	
int contImage = 0;
		for(contImage=1;contImage<=55;contImage++){
			printf("\nResultado para imagem %d", contImage);
			char path[1024];
			sprintf(path, "carros/car (%d).JPG",contImage);
			IplImage* img;			
			img = cvLoadImage( path, CV_LOAD_IMAGE_GRAYSCALE);
			IplImage* imgColor = cvLoadImage( path, CV_LOAD_IMAGE_COLOR);
	
			Plate p = vlpl(img);////////-------------------------//////------------------------
			if (p.x>0){
				//IplImage* plateImageTemp = cropImage(trackedGray,p);
				//IplImage* plateImage = cvCreateImage( cvGetSize( plateImageTemp ), IPL_DEPTH_8U, 1);
				//cvThreshold(plateImageTemp,plateImage,0,255,CV_THRESH_OTSU);

				IplImage* plateImage = cropImage(imgColor,p);
							
				char filePlateName[50];
				sprintf(filePlateName, "carros/result%d.png",contImage);
				cvSaveImage(filePlateName,plateImage);
				char window[32];
				sprintf(window,"Plate",contImage);
				cvNamedWindow(window, 1); 	
				cvShowImage("Plate", plateImage);
			}
			cvWaitKey(1);
			cvReleaseImage(&img);
		}
		system("pause");
}