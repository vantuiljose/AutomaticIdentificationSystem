#include "stdafx.h"
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
#include <omp.h>
#include <iostream>
#include <conio.h>
#include "cvblob.h"
#include "PlateLocation.h"



using namespace cv;
using namespace std;

#define MIN_HEIGHT_CHAR 7
#define MIN_HEIGHT_VLP 17

#define MAX_HEIGHT_CHAR 43
#define MIN_WIDTH_VLP 60
#define MAX_DISTANCE_CHARS 20
#define MAX_DISTANCE_PARTS 5
#define HEIGHT_MEAN_FILTER 25
#define WIDTH_MEAN_FILTER 80
#define SIZE_DILATE 5
#define SIZE_DILATE_FINAL 7
#define DECISION_AMONG_CANDIDATES true


//Vantuil
#define MIN_RATIO_PLATE 3.5
#define MAX_RATIO_PLATE 6.6

bool VERBOSE_IMSHOW = false;
#define imgshow(a, b) if(VERBOSE_IMSHOW) imshow(a, b); waitKey(0);
#define min(a, b) a < b ? a : b
#define max(a, b) a > b ? a : b
//


using namespace std;
using namespace cvb;

const double sizeStep = 0.4;
const double minHeight = 40;
const double maxHeight = 60;
const double aspectRatio = 4.5;
const double percentTranslationStep = 0.1;



int calculateUniformity(CvMat* integralImage, int nVertWindows, int nHorizWindows, int xWindow, int yWindow, int height, int width){
	//Uniformity Condition
	//double w1 =  (double)(cvGetReal2D(integralImage,yWindow + height/2, xWindow + width/2 ) +  cvGetReal2D(integralImage,yWindow, xWindow) - cvGetReal2D(integralImage,yWindow, xWindow + width/2) - cvGetReal2D(integralImage,yWindow + width/2, xWindow)) / (double)(width*height)/4;
	//double w2 =  (double)(cvGetReal2D(integralImage,yWindow + width/2, xWindow) +  cvGetReal2D(integralImage,yWindow + height, xWindow + width/2) - cvGetReal2D(integralImage,yWindow + height/2, xWindow + width/2 ) - cvGetReal2D(integralImage,yWindow + height, xWindow)) / (double)(width*height)/4;
	//double w3 =  (double)(cvGetReal2D(integralImage,yWindow, xWindow + width/2 ) +  cvGetReal2D(integralImage,yWindow + height/2, xWindow + width) - cvGetReal2D(integralImage,yWindow, xWindow + width/2) - cvGetReal2D(integralImage,yWindow + width, xWindow)) / (double)(width*height)/4;
	//double w4 =  (double)(cvGetReal2D(integralImage,yWindow + height/2, xWindow + width/2 ) +  cvGetReal2D(integralImage,yWindow, xWindow) - cvGetReal2D(integralImage,yWindow+height/2, xWindow) - cvGetReal2D(integralImage,yWindow + width, xWindow)) / (double)(width*height)/4;

	int uniformWindow = 0;
	double widthStep = (double)width/nHorizWindows;
	double heightStep = height/nVertWindows;
	double areaSubWindow = widthStep * heightStep;
	double density;
	for (int i = 0;i<nVertWindows;i++){
		for (int j = 0;j<nHorizWindows;j++){
			density = ((double)(cvGetReal2D(integralImage,yWindow +heightStep*i, xWindow + widthStep*j ) +  cvGetReal2D(integralImage,yWindow + heightStep*(i+1), xWindow + widthStep*(j+1)) - cvGetReal2D(integralImage,yWindow + heightStep*(i+1), xWindow + widthStep*j) - cvGetReal2D(integralImage,yWindow + heightStep*i, xWindow + widthStep*(j+1)))) / areaSubWindow;						
			if ( density > 0.11){
				//printf("**");
				uniformWindow++;
			}
			//printf(">>>>>>>>>>>>SubWindow x=%f, y=%f, height = %f, width = %f, density=%f\n",  xWindow + widthStep*j, yWindow +heightStep*i, heightStep, widthStep, density);
		}
	}
	return uniformWindow;
}

void showPlate(IplImage* myImage, int xWindow, int yWindow, int height, int width){
	IplImage* crop = cvCreateImage(cvSize(width,height),myImage->depth,myImage->nChannels);
	cvSetImageROI(myImage, cvRect(xWindow,yWindow,width, height));
	cvCopy(myImage,crop);
	cvNamedWindow("Plate", 1); 	
	cvShowImage("Plate", crop);
	cvResetImageROI(myImage);
}

IplImage* cropImage(IplImage* myImage, int xWindow, int yWindow, int height, int width){
	CvRect rect = cvRect(yWindow,xWindow,width,height);
	if (!(rect.width >= 0 && rect.height >= 0 && rect.x < myImage->width && rect.y < myImage->height && rect.x + rect.width >= (int)(rect.width > 0) && rect.y + rect.height >= (int)(rect.height > 0))){
		int a = 10;
	}
	cvSetImageROI(myImage, rect);	
	IplImage* crop = cvCreateImage(cvSize(myImage->roi->width,myImage->roi->height),myImage->depth,myImage->nChannels);
	cvCopy(myImage,crop);
	cvResetImageROI(myImage);
	return crop;
}

IplImage* cropImage(IplImage* myImage, int xWindow, int yWindow, int height, int width, double cutLeft, double cutRight, double cutUp, double cutDown){	
	return cropImage(myImage, cutUp*height + xWindow, cutLeft*width + yWindow,height - (cutDown*height) - (cutUp*height) ,width - (cutRight*width) - (cutLeft*width));
}


IplImage* cropImage(IplImage* myImage, Plate p){
	return cropImage(myImage,p.y,p.x, p.width, p.height);
}


Plate getPlate(IplImage* myimage)
{
	// declare a new IplImage pointer 
//IplImage* myimage;
IplImage *edgeImage = cvCreateImage( cvGetSize( myimage ), IPL_DEPTH_8U, 1);
IplImage *binaryEdgeMap = cvCreateImage( cvGetSize( myimage ), IPL_DEPTH_8U, 1);
CvMat* integralImage = cvCreateMat(myimage->height +1, myimage->width+1, CV_64FC1);

CvSize tamanhoImagem;
CvMat *filtroSobel;
int lado=3;
double kernel[ ] = { -1, 0, 1,
                     -2, 0, 2,
                     -1, 0, 1};

filtroSobel = cvCreateMatHeader( lado, lado, CV_64FC1 );
cvSetData( filtroSobel, kernel, lado*8 );
//Call OpenCV convolution:
cvFilter2D( myimage, edgeImage, filtroSobel, cvPoint(-1,-1) );

cvThreshold(edgeImage,binaryEdgeMap,0,1,CV_THRESH_OTSU);
IplImage *labelImg = cvCreateImage(cvGetSize(binaryEdgeMap), IPL_DEPTH_LABEL, 1);

cvIntegral(binaryEdgeMap, integralImage);

int startX = 0;
int startY=0;
int endX =myimage->width;
int endY = myimage->height;
double hTranslationStep = percentTranslationStep* minHeight;
double vTranslationStep = percentTranslationStep* minHeight*aspectRatio;

int height = (int) minHeight;
int xWindow = startX;
int yWindow = startY;
int width = (int)height *aspectRatio;

Plate plate;
plate.x=-1;
double densityPlate=0;

while(height < maxHeight){
	while(xWindow+width < endX){		
		while(yWindow+height < endY){
			double teste = cvGetReal2D(integralImage,yWindow + height, xWindow + width );
			int maxX = (int) teste;
			int maxY = cvGetReal2D(integralImage,yWindow+height, xWindow);
			int minX = cvGetReal2D(integralImage,yWindow, xWindow + width);
			int minY = cvGetReal2D(integralImage,yWindow, xWindow);
			double density =  (double)(maxX + minY - maxY - minX) / (double)(width*height);
			
			//cvRectangle(edgeImage,cvPoint(xWindow,yWindow),cvPoint(xWindow + width,yWindow + height),cvScalar(255),1,8);
			//printf(">>>>>>Window x=%d, y=%d, height = %d, width = %d, desity=%f\n", xWindow, yWindow, height, width, density);
			if (density > 0.15){			
				//--------------------UNIFORMITY CONDITION--------------------------------------------------------------------------------
				int uniformWindow = calculateUniformity(integralImage, 2, 2, xWindow, yWindow,height,width);
				if (uniformWindow==4){
					//cvRectangle(edgeImage,cvPoint(xWindow,yWindow),cvPoint(xWindow + width,yWindow + height),cvScalar(130),2,8);
					uniformWindow =  calculateUniformity(integralImage, 1, 20, xWindow, yWindow,height,width);
					uniformWindow = uniformWindow + calculateUniformity(integralImage, 10, 1, xWindow, yWindow,height,width);
					if (uniformWindow>21){
						if(density>densityPlate){
							densityPlate=density;
							plate.x = xWindow;
							plate.y = yWindow;
							plate.height = width;
							plate.width = height;
						}
						//cvRectangle(edgeImage,cvPoint(xWindow,yWindow),cvPoint(xWindow + width,yWindow + height),cvScalar(200),2,8);
						//showPlate(myimage,xWindow, yWindow,height,width);rint
					}
				}
				//-----------------------------------------------------------------------------------------------------------------------------			
			}				
			yWindow+=vTranslationStep;
		}
		yWindow=startY;
		xWindow+=hTranslationStep;
	}
	xWindow=startX;
	height+=(sizeStep/2 * height);
	width = (int)height *aspectRatio;
	hTranslationStep = percentTranslationStep* height;
	vTranslationStep = percentTranslationStep* width;
	//printf(">>>>Size Window height = %d, width = %d\n", height, width);
}
//wait for key to close the window 
//cvReleaseImage( &myimage ); 
return plate;
}




// Paper: Towards an automatic vehicle access control system:
// License plate location (2011)
// 
// Authors: Pedro R. Mendes Junior; Jose M. Ribeiro Neves; Andrea
// I. Tavares; David Menotti
// 
// Digital Object Identifier: 10.1109/ICSMC.2011.6084108
// <http://dx.doi.org/10.1109/ICSMC.2011.6084108>



inline void opening(const Mat &img_src, Mat &img_dst, const Mat &se) {
  Mat img_aux;
  erode(img_src, img_aux, se);
  dilate(img_aux, img_dst, se);
}

inline void closing(const Mat &img_src, Mat &img_dst, const Mat &se) {
  Mat img_aux;
  dilate(img_src, img_aux, se);
  erode(img_aux, img_dst, se);
}

inline void tophat(const Mat &img_src, Mat &img_dst, const Mat &se) {
  Mat img_aux;
  opening(img_src, img_aux, se);
  img_dst = img_src - img_aux;
}

inline Rect boundingRect(const vector<Point> &points) {
  // Points initialization
  Point min = points[0];
  Point max = points[0];
 
  // Getting bounding box coordinates
  for (size_t i = 1; i < points.size() ; i++) {
    if(points[i].x > max.x) max.x = points[i].x;
    if(points[i].x < min.x) min.x = points[i].x;
 
    if(points[i].y > max.y) max.y = points[i].y;
    if(points[i].y < min.y) min.y = points[i].y;
  }
 
  return Rect(min.x, min.y, max.x-min.x+1, max.y-min.y+1);
} 

inline Rect boundingRects(const vector<vector<Point> > &listof_points) {
  Rect rect = boundingRect(listof_points[0]);
  for(int i = 1; i < listof_points.size(); ++i) {
    Rect rect_tmp = boundingRect(listof_points[i]);
    int minx = min(rect.tl().x, rect_tmp.tl().x),
      miny = min(rect.tl().y, rect_tmp.tl().y),
      width = max(rect.br().x, rect_tmp.br().x) - minx + 1,
      height = max(rect.br().y, rect_tmp.br().y) - miny + 1;
    rect = Rect(minx, miny, width, height);
  }

  return rect;
} 

inline void fillRectangle(Mat &img, const Rect &rect, const Scalar scalar = Scalar(255, 255, 255)) {
  Point p1 = rect.tl(),
    p3 = rect.br(),
    p2 = Point(p3.x, p1.y),
    p4 = Point(p1.x, p3.y);
  Point ps[] = {p1, p2, p3, p4};
  fillConvexPoly(img, ps, 4, scalar);
}

inline void markImage(Mat &img, const Mat &mask) {
  int value = 70;
  for(int y = 0; y < img.rows; ++y) {
    for(int x = 0; x < img.cols; ++x) {
      if(! mask.at<uchar>(y, x)) {
        img.at<uchar>(y, x) = img.at<uchar>(y, x) < value/2 ? 0 : img.at<uchar>(y, x) - value/2;
      }
      else {
        img.at<uchar>(y, x) = img.at<uchar>(y, x) + value/2 > 255 ? 255 : img.at<uchar>(y, x) + value/2;
      }
    }
  }
}


int contImage;
Plate vlpl(IplImage* myimg) {
	          contImage++;
    Mat img = cvarrToMat(myimg);;// = cvCreateMat(myimg->height,myimg->width,CV_32FC3 );
	

  //img_dst = img.clone();
  Mat img_sobel;
  Sobel(img, img_sobel, 0, 1, 0);
  
  

  Mat img_blur;
  //blur(img_sobel, img_blur, Size(WIDTH_MEAN_FILTER, HEIGHT_MEAN_FILTER));
  blur(img_sobel, img_blur, Size(WIDTH_MEAN_FILTER, HEIGHT_MEAN_FILTER),cvPoint(WIDTH_MEAN_FILTER*0.4, HEIGHT_MEAN_FILTER * 0.25));

  // Darkening small salient regions
  Mat img_opening;
  //Mat se = getStructuringElement(MORPH_RECT, Size(2, MIN_HEIGHT_CHAR), cvPoint(1,1)); // 2 because there is problem in some versions of OpenCV.
  Mat se = getStructuringElement(MORPH_RECT, Size(2, MIN_HEIGHT_CHAR), cvPoint(0,MIN_HEIGHT_CHAR/2)); // 2 because there is problem in some versions of OpenCV. //Vantuil
  opening(img_blur, img_opening, se);  

  // Merging vertical saliences
  Mat img_closing;
  //se = getStructuringElement(MORPH_RECT, Size(MAX_DISTANCE_CHARS, 2));
  se = getStructuringElement(MORPH_RECT, Size(MAX_DISTANCE_CHARS, 2),cvPoint(MAX_DISTANCE_CHARS/2, 0));//Vantuil
  closing(img_opening, img_closing, se);

  // Darkening big saliente regions
  Mat img_tophat;
  //se = getStructuringElement(MORPH_RECT, Size(2, MAX_HEIGHT_CHAR));
  se = getStructuringElement(MORPH_RECT, Size(2, MAX_HEIGHT_CHAR), cvPoint(0, MAX_HEIGHT_CHAR/2));//Vantuil
  tophat(img_closing, img_tophat, se);

  // Joining parts of the VLP
  //se = getStructuringElement(MORPH_RECT, Size(MAX_DISTANCE_PARTS, 2));
  se = getStructuringElement(MORPH_RECT, Size(MAX_DISTANCE_PARTS, 2),cvPoint(MAX_DISTANCE_PARTS/2, 0));//Vantuil
  closing(img_tophat, img_closing, se);

  // Erosion and dilation
  Mat img_erode, img_dilate;
  //se = getStructuringElement(MORPH_RECT, Size(2*WIDTH_MEAN_FILTER, 2));
  se = getStructuringElement(MORPH_RECT, Size(2*WIDTH_MEAN_FILTER, 2), cvPoint(WIDTH_MEAN_FILTER, 0));//Vantuil
  erode(img_closing, img_erode, se);
  //se = getStructuringElement(MORPH_RECT, Size(1.5*WIDTH_MEAN_FILTER, 2));
  se = getStructuringElement(MORPH_RECT, Size(1.5*WIDTH_MEAN_FILTER, 2), cvPoint(0.75*WIDTH_MEAN_FILTER, 0));//Vantuil
  dilate(img_erode, img_dilate, se);
  //Vantuil
  				char filePlateName[50];
				sprintf(filePlateName, "carros/dilate%d.png",contImage);
				//cvSaveImage(filePlateName,img_dilate);
				imwrite(filePlateName,img_dilate);
	//
  
  // Binarization
  Mat img_binary;
  threshold(img_dilate, img_binary, 0, 255, THRESH_OTSU);
  
  

  // Removing non-VLPL shape objects
  vector<vector<Point> > contours;
  findContours(img_binary, contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);
  Mat img_regions = Mat::zeros(img_binary.size(), img_binary.type());
  // vector<Rect> rects_nonVLPL;

    //Vantuil
  				//char filePlateName[50];
				sprintf(filePlateName, "carros/binary%d.png",contImage);
				//cvSaveImage(filePlateName,img_dilate);
				imwrite(filePlateName,img_binary);
	//
  for(int i = 0; i < contours.size(); ++i) {
    Rect rect = boundingRect(contours[i]);	
    if(! (rect.tl().x <= 1 || rect.tl().y <= 1 || // On the boundary
          rect.br().x >= img_binary.cols - 2 || // On the boundary
          rect.br().y >= img_binary.rows - 2 || // On the boundary
          rect.height >= rect.width)) { // Disproportionate
      fillRectangle(img_regions, rect);
      // rects_nonVLPL.push_back(rect);
    }
  }
  contours.clear();

      //Vantuil
  				//char filePlateName[50];
				sprintf(filePlateName, "carros/cands(1)%d.png",contImage);
				//cvSaveImage(filePlateName,img_dilate);
				imwrite(filePlateName,img_regions);

  // Removing candidates in intersection
  vector<Rect> rects;
  img_binary = img_regions.clone();
  findContours(img_binary, contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);
  for(int i = 0; i < contours.size(); ++i) {
    Rect rect = boundingRect(contours[i]);
    Mat img_reg = img_regions(Range(rect.tl().y, rect.br().y),
                              Range(rect.tl().x, rect.br().x));
    if(img_reg.at<uchar>(0, 0) == 0 ||
       img_reg.at<uchar>(0, img_reg.cols-1) == 0 ||
       img_reg.at<uchar>(img_reg.rows-1, 0) == 0 ||
       img_reg.at<uchar>(img_reg.rows-1, img_reg.cols-1) == 0) { // Two regions in one.
      img_reg = Mat::zeros(img_reg.size(), img_reg.type());
    }
    else {
      rects.push_back(rect);
    }
  }
  contours.clear();

  
  // 
  double max_value = 0;
  int max_value_img = 0;
  se = getStructuringElement(MORPH_RECT, Size(SIZE_DILATE, SIZE_DILATE));
  for(int k = 0; k < rects.size(); ++k) {
	  if (k==27){
		  int debug=1;
	  }
	  Mat img_proc = Mat::zeros(img_regions.size(), img_regions.type());
	  fillRectangle(img_proc, rects[k]);
	  dilate(img_proc, img_proc, se);
	  findContours(img_proc, contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);
	  Rect rect = boundingRect(contours[0]); // Only 1 in contours

	  Mat img_dilate_reg = img_dilate(Range(rect.tl().y, rect.br().y),
		  Range(rect.tl().x, rect.br().x));
	  Mat img_proc_reg = img_proc(Range(rect.tl().y, rect.br().y),
		  Range(rect.tl().x, rect.br().x));
	  for(int y = 0; y < img_proc_reg.rows; ++y) {
		  for(int x = 0; x < img_proc_reg.cols; ++x) {
			  img_proc_reg.at<uchar>(y, x) = img_dilate_reg.at<uchar>(y, x);
		  }
	  }
	  threshold(img_proc_reg, img_proc_reg, 0, 255, THRESH_OTSU);

	  se = getStructuringElement(MORPH_RECT, Size(0.25*WIDTH_MEAN_FILTER, 2));
	  erode(img_proc_reg, img_proc_reg, se);
	  se = getStructuringElement(MORPH_RECT, Size(0.5*WIDTH_MEAN_FILTER, 0.5*HEIGHT_MEAN_FILTER));
	  dilate(img_proc_reg, img_proc_reg, se);

	  Mat img_tmp = img_proc.clone();

	  findContours(img_proc, contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);//Erro aqui
	  if (contours.size()<1) continue;//Vantuil- Caso não haja contorno, não haverá bounding box, evita erro!
	  rect = boundingRects(contours); // boundingRectS
	  if(contImage==38 && k==3){
		  int debug=1;
	  }
	  
	  char nroCand[5];
	  sprintf(nroCand, "%d",k);
	  putText(img_regions, nroCand, cvPoint(rect.x - 4, rect.y - 4), FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(128), 1, CV_AA);
	  
	  double ratio = (double)rect.width/rect.height;
	  //double ratio = 5.5;
	  //printf("\n>>>>%d-Ratio=%f",k,ratio);//Show the ratio for all candidates
	  if(rect.height < MIN_HEIGHT_VLP ||
		  rect.width < MIN_WIDTH_VLP ||
		  rect.height >= rect.width //Vantuil
		  || ratio<MIN_RATIO_PLATE || ratio>MAX_RATIO_PLATE
		  //End Vantuil	   
		  ) {
			  continue;
	  }

	  Scalar mu, sd;
	  meanStdDev(img_dilate, mu, sd, img_tmp);
	  double cv = sd[0]/mu[0],
		  value = mu[0]/cv;
	  printf(" value=%f",value);

	  if(value > max_value) {
		  max_value = value;
		  max_value_img = k;
	  }
  }
printf("\nEscolhido = %d\n",max_value);
sprintf(filePlateName, "carros/candsLabel%d.png",contImage);
imwrite(filePlateName,img_regions);



  Plate p;
  p.x=0;
  if (rects.size() >0){//Vantuil's Code
	  Rect rect = rects[max_value_img];
	  int toadd = (SIZE_DILATE_FINAL - 1)/2;
	  p.height = rect.width + toadd;
	  p.width = rect.height+toadd;
	  p.x = rect.x - toadd;
	  p.y = rect.y - toadd;
	  //
	  Mat img_mask = Mat::zeros(img_regions.size(), img_regions.type());
	  fillRectangle(img_mask, rect);
	  se = getStructuringElement(MORPH_RECT, Size(SIZE_DILATE_FINAL, SIZE_DILATE_FINAL));
	  dilate(img_mask, img_mask, se);
	  //img_dst = img_mask;
	  //Vantuil
	  char filePlateName[50];
	  sprintf(filePlateName, "carros/final%d.png",contImage);
	  //cvSaveImage(filePlateName,img_dilate);
	  imwrite(filePlateName,img_mask);
	  //
  }
  return p;
}



//
//int main(int argc, char** argv) {
//	/*
//  Mat img_vlpl;
//  IplImage* myimage = cvLoadImage("D:/Mestrado/placas/placa5.png" ,CV_LOAD_IMAGE_GRAYSCALE);
//	cv::Mat img(myimage);
//	cvReleaseImage(&myimage);*/
//
//	  Mat img, img_vlpl;
//  img = imread("D:/Mestrado/placas/placa3.png", 0);
//
// /* if(argc != 2 || !img.data) {
//    cout << "No image data" << endl;
//    return -1;
//  }*/
//  cvNamedWindow("Placa");
//
//  imshow("Placa",img);
//  cvWaitKey(1000);
//
//  //vlpl(img);
//  imshow("img_mask", img_vlpl); waitKey(0);
//}
