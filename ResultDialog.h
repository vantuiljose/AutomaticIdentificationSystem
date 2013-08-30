#include <iostream>
#include <stdio.h>
#include "cv.h"
#include "highgui.h"
#include <opencv2/imgproc/imgproc.hpp>
using namespace std;

class ResultDialog{

	IplImage* dialogImage;
	char* WINDOW_TITLE;
	CvRect rectTracking;
	CvRect rectTracked;
	CvRect rectPlate;
	CvRect rectSegPlate;
	CvRect rectResultPlate;

	CvFont font;

	public:
		void setImages(IplImage* tracking,IplImage* tracked,IplImage* plate, IplImage* segPlate);
		void init();
		void show();
		void setResultPlate(char* numberPlate);
		void printSegmentation(vector<CvRect> caracteres, IplImage* segPlate);
	};