#include <iostream>
#include <fstream>
#include <windows.h>
#include "cv.h"
#include "highgui.h"
#include <ml.h>

#include <cstdio>
#include <vector>

#include "ResultDialog.h"

using namespace std;
using namespace cv;


	void ResultDialog::init(){
		WINDOW_TITLE = "Result";
		dialogImage = cvCreateImage(cvSize(1200,700),IPL_DEPTH_8U,3);
		rectTracking = cvRect(0,0,0,0);
		rectTracked =cvRect(10,200,300,300) ;
		rectPlate = cvRect(350,10,250,80);
		rectSegPlate = cvRect(350,200,250,80);
		rectResultPlate = cvRect(900,10,100,80);

		cvRectangleR(dialogImage,rectTracked,cvScalar(0,0,0),2);
		cvRectangleR(dialogImage,rectPlate,cvScalar(0,0,0),2);
		cvRectangleR(dialogImage,rectSegPlate,cvScalar(0,0,0),2);
		cvRectangleR(dialogImage,rectResultPlate,cvScalar(0,0,0),2);


		cvInitFont( &font, CV_FONT_HERSHEY_SIMPLEX, 0.6f, 0.6f, 0.0f,2 );
		

	}

	void ResultDialog::setImages(IplImage* tracking,IplImage* tracked,IplImage* plate, IplImage* segPlate){
		
		if (tracking){
			cvSetImageROI(dialogImage,cvRect(10,10,tracking->width,tracking->height));
			cvCopy(tracking,dialogImage);
		}

		if (tracked){
			cvSetImageROI(dialogImage,rectTracked);
			cvResize(tracked,dialogImage);
		}

		if (plate){
			cvSetImageROI(dialogImage,rectPlate);
			cvResize(plate,dialogImage);
		}

		if (segPlate){
			cvSetImageROI(dialogImage,rectSegPlate);
			cvResize(segPlate,dialogImage);
		}
		cvResetImageROI(dialogImage);

	}

	void ResultDialog::show(){
		cvNamedWindow( WINDOW_TITLE, 1 );
		cvShowImage( WINDOW_TITLE, dialogImage);
		//cvWaitKey(0);
	}

	void ResultDialog::setResultPlate(char* numberPlate){
		if(numberPlate==NULL) return;
		//cvSetImageROI(dialogImage,rectResultPlate);
		cvRectangleR(dialogImage,rectResultPlate,cvScalar(127,127,127),CV_FILLED);
		cvPutText(dialogImage,numberPlate,cvPoint(rectResultPlate.x + 4,rectResultPlate.y + rectResultPlate.height/2),&font,cvScalar(255,255,255));


		//cvResetImageROI(dialogImage);
	}

	void ResultDialog::printSegmentation(vector<CvRect> caracteres, IplImage* segPlate){
		if (!segPlate) return;
		for(int i =0;i<caracteres.size();i++){//Start from the second one
			cvRectangleR(segPlate,caracteres[i],cvScalar(0,255,0));
		//printf(">>>>>>>>Flooding Image. Area=%d, x=%d, y=%d,w=%d,h=%d\n",r.width*r.height,r.x,r.y,r.width,r.height);
		}
	}