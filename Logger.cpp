#include <iostream>
#include <stdio.h>
#include "cv.h"
#include "highgui.h"
#include "Logger.h"
#include "VantzUtils.h"

using namespace cv;
using namespace std;


void Logger::init(char* fileName){
	outfile.open(fileName);
}

void Logger::save(){
	outfile.close();
}

void Logger::log(char* plate, int nFrame){
	outfile << nFrame;
	outfile << ":";
	outfile << plate;
	outfile << "\n";
}


void Logger::completeLog(int nFrame, IplImage* track, IplImage* plate, IplImage* charImage, vector<CvRect> chars, char* numberPlate){
	
	for(int i =0;i<chars.size();i++){
		CvRect r = chars.at(i);
		cvRectangleR(charImage,r,cvScalar(0,255,0));
	}
	CvFont font;
	cvInitFont( &font, CV_FONT_HERSHEY_SIMPLEX, 0.5f, 0.5f, 0.0f,2 );
	
	cvPutText(charImage,numberPlate,cvPoint(charImage->width/4,charImage->height), &font, cvScalar(255,0,0));
	saveImage("log/track/img",nFrame,track);
	saveImage("log/plate/img",nFrame,plate);
	saveImage("log/chars/img",nFrame,charImage);

}

