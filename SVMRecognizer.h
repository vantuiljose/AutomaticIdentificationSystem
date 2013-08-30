#include <windows.h>
#include "cv.h"
#include "highgui.h"
#include <ml.h>


class SVMRecognizer{

	CvSVM *pSvm; 
	CvSVM *pSvmLetter;
	public:
		//char recognizeletter(cvmat* symbol);
		//char recognize(cvmat* symbol);
		char recognizeLetter(IplImage* symbol);
		char recognize(IplImage* symbol);
		void init();
	};
