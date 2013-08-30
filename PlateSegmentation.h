#include <iostream>
#include <stdio.h>
#include "cv.h"
#include "highgui.h"
#include <opencv2/imgproc/imgproc.hpp>
#include "SVMRecognizer.h"
using namespace std;

const int NUMBER_OF_CHARS = 7;
const int RED_DIFF = 15;
const int RED_MIN=50;

class PlateSegmentation{

	SVMRecognizer* svmRecog;
	vector<CvRect> caracteres;
	char* plateNumber;
	IplImage* improvedImage;
	
	public:
		boolean processPlate(IplImage* plate);
		void init();
		vector<CvRect> getCaracteres();
		char* getResultPlate();
		IplImage* getImprovedImage();
	};
