#include <iostream>
#include <stdio.h>
#include "cv.h"
#include "highgui.h"
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;

void saveImage(char* prefix, int nro, CvArr * image){
	char pathSave[1024];
	sprintf(pathSave, "%s%d.png",prefix,nro);
	cvSaveImage( pathSave, image );
}