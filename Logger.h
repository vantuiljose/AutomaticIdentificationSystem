#include <sstream>
#include <iostream> 
#include <cstdio>
#include <fstream>
#include "cv.h"
using namespace std;

class Logger{


	std::ofstream outfile;
	
	public:		
		void init(char* fileName);
		void log(char* plate, int nFrame);
		void completeLog(int nFrame, IplImage* track, IplImage* plate, IplImage* charImage, vector<CvRect> chars, char* numberPlate); 
		void save();
	};