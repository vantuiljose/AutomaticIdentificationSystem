#include <iostream>
#include <stdio.h>
#include "cv.h"
#include "highgui.h"
#include <opencv2/imgproc/imgproc.hpp>
#include "VantzUtils.h"
#include "PlateSegmentation.h"

using namespace cv;
using namespace std;

//void saveImage(char* prefix, int nro, CvArr * image){
//	char pathSave[1024];
//	sprintf(pathSave, "%s%d.png",prefix,nro);
//	cvSaveImage( pathSave, image );
//}

CvScalar random_color(CvRNG* rng)
{
	int color = cvRandInt(rng);
	return CV_RGB(color&255, (color>>8)&255, (color>>16)&255);
}

IplImage* DrawHistogram(CvMat *hist, int widthimg, float scaleX = 1, float scaleY = 1)
{

    float histMax = widthimg;	
	
	int size = hist->cols;

    IplImage* imgHist = cvCreateImage(cvSize(256*scaleX, 64*scaleY), 8 ,1);
    cvZero(imgHist);
    for(int i=0;i<size-1;i++)
    {
		CvScalar v = cvGet2D(hist,0,i);
		double histValue = v.val[0]/255;
		CvScalar v2 = cvGet2D(hist,0,i+1);
        float nextValue = v2.val[0]/255;
 
        CvPoint pt1 = cvPoint(i*scaleX, 64*scaleY);
        CvPoint pt2 = cvPoint(i*scaleX+scaleX, 64*scaleY);
        CvPoint pt3 = cvPoint(i*scaleX+scaleX, (64-nextValue*64/histMax)*scaleY);
        CvPoint pt4 = cvPoint(i*scaleX, (64-histValue*64/histMax)*scaleY);
 
        int numPts = 5;
        CvPoint pts[] = {pt1, pt2, pt3, pt4, pt1};
 
        cvFillConvexPoly(imgHist, pts, numPts, cvScalar(255));
    }
	int spaceLine=5*scaleY;
	int line = spaceLine;
	int endPoint = 255*scaleX;
	while(line<64*scaleY){
		cvLine(imgHist,cvPoint(0,line),cvPoint(endPoint,line),cvScalar(128));

		line+=spaceLine;

	}

    return imgHist;
}


//int main(void){		
void testeVerticalProjection(void){
		PlateSegmentation* ps = new PlateSegmentation();
		ps->init();
		CvRNG rng;
		int contImage = 0;
		for(contImage=1;contImage<=544;contImage++){
			//printf("\nResultado para imagem %d\n", contImage);
			char path[1024];
			sprintf(path, "placas/lp (%d).png",contImage);
			IplImage* img;
			img = cvLoadImage( path, CV_LOAD_IMAGE_GRAYSCALE );
			IplImage* rsp = cvLoadImage( path, CV_LOAD_IMAGE_COLOR );
			IplImage* ellipses = cvCloneImage(rsp);
			cvCvtColor(img,ellipses,CV_GRAY2BGR);
			if (contImage== 255) //281
				int debug=1;

			ps->processPlate(rsp);
			vector<CvRect> caracteres = ps->getCaracteres();
			printf("\nlp (%d).png\n%s",contImage,ps->getResultPlate());

			//******Saving parts*********************
			//for(int i =0;i<caracteres.size();i++){
			//	CvRect r = caracteres.at(i);				
			//	cvSetImageROI(img_dest,r);
			//	IplImage* newImg = cvCreateImage(cvSize(r.width,r.height),img_dest->depth,img_dest->nChannels);
			//	cvCopy(img_dest,newImg);				
			//	char pathSave[1024];
			//	sprintf(pathSave, "%sp%dc%d.png","char/char",contImage,i);
			//	cvSaveImage( pathSave, newImg );
			//	cvResetImageROI(img_dest);
			//}
			
			//**********************************************************
			for(int i =0;i<caracteres.size();i++){
				CvRect r = caracteres.at(i);
				cvRectangleR(ellipses,r,cvScalar(0,255,0));				
				//cvRectangleR(imgShow,r,cvScalar(0,255,0));
			}


			//cvNamedWindow( "FloodImage");
			saveImage("placas/melhoradas/lp", contImage,ellipses);
			saveImage("placas/melhoradas/binarizada",contImage,ps->getImprovedImage());
			//cvShowImage( "FloodImage", img_dest );
			//cvvWaitKey(0);
			//cvNamedWindow( "Bounding");
			//cvShowImage( "Bounding", imgShow );			
			//saveImage("placas/melhoradas/melhorada", contImage,img_dest);
			//saveImage("placas/melhoradas/bounding", contImage,imgShow);
			//
			//cvNamedWindow("Plate", 1);					
			//cvShowImage("Plate", hist);
			//cvvWaitKey(0);
			cvWaitKey(10);
			//saveImage("placas/melhoradas/projectionSmooth_8_", contImage,hist);

		}
		system("pause");
}