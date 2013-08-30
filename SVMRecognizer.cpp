#include <iostream>
#include <fstream>
#include <windows.h>
#include "cv.h"
#include "highgui.h"
#include <ml.h>

#include <cstdio>
#include <vector>

#include "SVMRecognizer.h"
#include "RecognitionParameters.h"
#include "VantzUtils.h"


using namespace std;
using namespace cv;



//class SVMRecognizer{
//
//	double hours;
//	double total;
//	CvSVM *pSvm; 
//	public:
//		int recognize(CvMat** symbol);
//		void init();
//	};

	void SVMRecognizer::init(){
		pSvm = new CvSVM();
		pSvm->load("SVM.svm", 0);
		pSvmLetter = new CvSVM();
		pSvmLetter->load("SVM_LETTER.svm", 0);
	}
	

//	static void prepareData(CvMat* symbol, CvMat** data){
//				*data = cvCreateMat( 1, FEATURE_COUTING, CV_32F );
//				//CvMat* symbol =  cvLoadImageM(path,CV_LOAD_IMAGE_GRAYSCALE);					
//				CvMat* resized = cvCreateMat(NORM_HEIGHT, NORM_WIDTH,symbol->type);
//				cvResize(symbol,resized);
//				CvMat row_header, row1;
//				CvMat *reshaped = cvReshape(resized, &row_header, 0, 1 );			
//				CvMat* reshaped_c = cvCreateMat(1, reshaped->cols, data[0]->type);
//				cvConvertScale(reshaped,reshaped_c);								
//				//cvShowImage("Resized",reshaped_c);				
//				cvCopy(reshaped_c,data[0]);
//}
//


	//CVMAT AS INPUT IMAGE
//	IplImage* formatData(CvMat* inpSymbol){
//
//				//*data = cvCreateMat( 1, FEATURE_COUTING, CV_32F );
//				//CvMat* symbol =  cvLoadImageM(path,CV_LOAD_IMAGE_GRAYSCALE);			
//				IplImage* symbol=cvCreateImage(cvSize(inpSymbol->cols,inpSymbol->rows),IPL_DEPTH_8U,1); //size, depth, channels
//				
//
//				cvCopy(inpSymbol,symbol);
//								
//				double fatHeigth = (double)NORM_HEIGHT/symbol->height;
//				double fatWidth = (double)NORM_WIDTH/symbol->width;
//				double adjustFat = fatHeigth<fatWidth? fatHeigth:fatWidth;
//				//CvMat* adjustResized = cvCreateMat(symbol->height*adjustFat, NORM_WIDTH*adjustFat,symbol->type);
//				IplImage* adjustResized = cvCreateImage (cvSize(symbol->width*adjustFat,symbol->height*adjustFat), symbol->depth,symbol->nChannels);
//				
//				cvResize(symbol,adjustResized);
//
//				//CvMat* normResized = cvCreateMat(NORM_HEIGHT, NORM_WIDTH,symbol->type);
//				IplImage* normResized = cvCreateImage (cvSize(NORM_WIDTH,NORM_HEIGHT), symbol->depth,symbol->nChannels);
//				//cvResize(adjustResized,normResized);
//				cvSetImageROI(normResized,cvRect((NORM_WIDTH-adjustResized->width)/2,(NORM_HEIGHT-adjustResized->height)/2,adjustResized->width,adjustResized->height));
//				cvCopy(adjustResized,normResized);
//				cvResetImageROI(normResized);
//				normResized->widthStep=NORM_WIDTH;
//
//				return normResized;
//				//CvMat* reshaped_c = cvCreateMat(1, reshaped->cols, data[0]->type);
//				//cvConvertScale(reshaped,reshaped_c);				
//				
//				//cvShowImage("Resized",reshaped_c);
//				//cvCopy(reshaped_c,data[0]);
//}

		IplImage* formatData(IplImage* symbol){

				//*data = cvCreateMat( 1, FEATURE_COUTING, CV_32F );
				//CvMat* symbol =  cvLoadImageM(path,CV_LOAD_IMAGE_GRAYSCALE);			
				//IplImage* symbol=cvCreateImage(cvSize(inpSymbol->cols,inpSymbol->rows),IPL_DEPTH_8U,1); //size, depth, channels
				

				//cvCopy(inpSymbol,symbol);
								
				double fatHeigth = (double)NORM_HEIGHT/symbol->height;
				double fatWidth = (double)NORM_WIDTH/symbol->width;
				double adjustFat = fatHeigth<fatWidth? fatHeigth:fatWidth;
				//CvMat* adjustResized = cvCreateMat(symbol->height*adjustFat, NORM_WIDTH*adjustFat,symbol->type);
				IplImage* adjustResized = cvCreateImage (cvSize(symbol->width*adjustFat,symbol->height*adjustFat), symbol->depth,symbol->nChannels);
				
				cvResize(symbol,adjustResized);

				//CvMat* normResized = cvCreateMat(NORM_HEIGHT, NORM_WIDTH,symbol->type);
				IplImage* normResized = cvCreateImage (cvSize(NORM_WIDTH,NORM_HEIGHT), symbol->depth,symbol->nChannels);
				cvSet(normResized,cvScalar(0));
				//cvResize(adjustResized,normResized);
				cvSetImageROI(normResized,cvRect((NORM_WIDTH-adjustResized->width + 1)/2,(NORM_HEIGHT-adjustResized->height + 1)/2,adjustResized->width,adjustResized->height));
				cvCopy(adjustResized,normResized);
				cvResetImageROI(normResized);
				//normResized->widthStep=NORM_WIDTH;
				cvReleaseImage(&adjustResized);

				return normResized;
				//CvMat* reshaped_c = cvCreateMat(1, reshaped->cols, data[0]->type);
				//cvConvertScale(reshaped,reshaped_c);				
				
				//cvShowImage("Resized",reshaped_c);
				//cvCopy(reshaped_c,data[0]);
}

		int countChar=0;
	static void prepareData(IplImage* symbol, CvMat** data){
		*data = cvCreateMat( 1, FEATURE_COUTING, CV_32F );
		CvMat row_header, row1;
		countChar++;
		saveImage("teste/charPrepared/charOri",countChar,symbol);
		IplImage* prepared = formatData(symbol);
		
		
		saveImage("teste/charPrepared/charPrep",countChar,prepared);
		//CvMat *reshaped = cvReshape(prepared, &row_header, 1);
				
		CvMat *reshaped = cvReshape(prepared, &row_header, 0,1);								
		cvGetRow(data[0],&row1,0);
		CvMat* reshaped_c = cvCreateMat(1, reshaped->width, row1.type);
		cvConvertScale(reshaped,reshaped_c);								
		cvCopy(reshaped_c,&row1);

	}


	static int testSVM(CvSVM** svm, CvMat* data)
	{
		CvMat* responses = 0;
		int nsamples_all = 0, ntrain_samples = 0;
		int var_count;        
		nsamples_all = data->rows;   
		var_count = data->cols;
		CvMat *result = cvCreateMat(1, nsamples_all, CV_32FC1);       
		(*svm)->predict(data, result);	
		int resposta = result->data.fl[0];    
		cvReleaseMat( &result );
		cvReleaseMat( &data );
		cvReleaseMat( &responses );
		return resposta;
	}

	//char SVMRecognizer::recognize(CvMat* symbol){			
	//	CvMat* data = 0;	       
	//	prepareData(symbol,&data);
	//	int r = testSVM(&pSvm,data) + 1;
	//	char resp = r==10?'0':r+48;
	//	return resp;
	//}

	//char SVMRecognizer::recognizeLetter(CvMat* symbol){			
	//	CvMat* data = 0;	       
	//	prepareData(symbol,&data);
	//	int r = testSVM(&pSvmLetter,data);
	//	char resp = r+65;
	//	return resp;
	//}

	char SVMRecognizer::recognize(IplImage* symbol){			
		CvMat* data = 0;	       
		prepareData(symbol,&data);
		char resp = testSVM(&pSvm,data) + 48;
		//Old trainning
		//int r = testSVM(&pSvm,data) + 1;
		//char resp = r==10?'0':r+48;
		return resp;
	}

	char SVMRecognizer::recognizeLetter(IplImage* symbol){			
		CvMat* data = 0;	       
		prepareData(symbol,&data);
		int r = testSVM(&pSvmLetter,data);
		char resp = r+65;
		return resp;
	}