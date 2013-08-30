#include <iostream>
#include <stdio.h>
#include "cv.h"
#include "highgui.h"
#include <opencv2/imgproc/imgproc.hpp>
#include "PlateSegmentation.h"
#include "VantzUtils.h"

using namespace cv;
using namespace std;
int countImg = 0;

boolean isRedPlate(IplImage* img){
	//return false;
	//*****HSV
	//IplImage* imgHSV = cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 3);
 //   cvCvtColor(img, imgHSV, CV_BGR2HSV);
	//IplImage* imgThreshed = cvCreateImage(cvGetSize(img), 8, 1);
	//cvInRangeS(imgHSV, cvScalar(20, 100, 100), cvScalar(30, 255, 255), imgThreshed);
	//cvReleaseImage(&imgHSV);
	//*********

	//******RGB
	int height,width,step,channels;
	int stepr, channelsr;
	uchar *data,*datar;
	//IplImage *result=cvCreateImage( cvGetSize(img), 8, 1 );
	height = img->height;
	width = img->width;
	step =img->widthStep;
	channels = img->nChannels;
	data = (uchar *)img->imageData; 
	//stepr=result->widthStep;
	//channelsr=result->nChannels;
    //datar = (uchar *)result->imageData; 
	int i,j;
	int reds=0, noreds=0;
	for(i=0;i < (height);i++) 
	for(j=0;j <(width);j++) 
		if(((data[i*step+j*channels+2]) > (RED_DIFF+data[i*step+j*channels])) && ((data[i*step+j*channels+2]) > (RED_DIFF+data[i*step+j*channels+1]))  && (data[i*step+j*channels+2] >RED_MIN))
			reds++;
		//datar[i*stepr+j*channelsr]=255;
		else
		//datar[i*stepr+j*channelsr]=0;
		noreds++;

	return reds>noreds/15;

	//*********


}

boolean PlateSegmentation::processPlate(IplImage* img_){

			//IplImage* img;			
			//img = cvLoadImage( path, CV_LOAD_IMAGE_GRAYSCALE );
			//IplImage* rsp = cvLoadImage( path, CV_LOAD_IMAGE_COLOR );
			//IplImage* ellipses = cvCloneImage(rsp);			
			countImg++;
			IplImage* img;
			if (img_->nChannels==3){
				boolean isRed = isRedPlate(img_);
				img = cvCreateImage( cvGetSize( img_ ), IPL_DEPTH_8U, 1);
				cvCvtColor(img_,img,CV_BGR2GRAY);
				if (!isRed) cvNot(img,img);//Red plates don't need to be inveted
				else {
					//printf("\n++VERMELHA++\n");					
					//cvSaveImage("PlacaVERMELHA.png",img);
				}
			}else{
				img = img_;
				cvNot(img,img);
			}

			
			
		//Binarização
			//vector<int> vect_sortPixel(img->width*img->height);
			//unsigned char *input = (unsigned char*)(img->imageData);
			//int i,j,p;
			//for(int i = 0;i < img->height;i++){
			//	for(int j = 0;j < img->width;j++){
			//		p = input[img->height * j + i] ;
			//		vect_sortPixel[img->height * j + i]=(p);
			//	}
			//}
			//sort(vect_sortPixel.begin(), vect_sortPixel.end());
			//int th_value = vect_sortPixel.at(vect_sortPixel.size()* 0.6);//threshold value

			//IplImage* bigger = cvCloneImage(img);	
			//IplImage* lower = cvCloneImage(img);

			//cvThreshold(img,bigger,th_value,255,CV_THRESH_TOZERO);
			//cvThreshold(img,lower,th_value+1,255,CV_THRESH_TOZERO_INV);
			//cvScale(lower,lower,0.5);
			//cvAdd(bigger,lower,img);
			////cvNot(img,img);
			


			IplImage* img_dest = cvCreateImage( cvGetSize( img ), IPL_DEPTH_8U, 1);			
			cvThreshold(img, img_dest, 00000, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
			
			IplImage* imgOriginal = cvCloneImage(img_dest);//Cauculate the real Y axes without cover parts
			improvedImage = cvCloneImage(img_dest);
			//imgShow =cvCreateImage(cvGetSize(img_dest), 8, 3); //usually RGB image are not 
			//cvCvtColor(img_dest, imgShow, CV_GRAY2BGR);//Just to show the bounding boxes
			//Covering parts of image
			int coverValue = img_dest->height/6;
			cvRectangle(img_dest,cvPoint(0,0),cvPoint(img->width,coverValue),cvScalar(0),CV_FILLED);
			cvRectangle(img_dest,cvPoint(0,img->height-coverValue),cvPoint(img->width,img->height),cvScalar(0),CV_FILLED);			
			/*Using close-------------
			int pos=3;
			IplConvKernel* element = cvCreateStructuringElementEx( pos, pos+1, pos/2, pos/2, CV_SHAPE_RECT, 0 );
			cvErode(img_dest,img_dest,element);
			cvDilate(img_dest,img_dest,element);
			saveImage("placas/melhoradas/vantzImg", contImage,img_dest);			
			*///--------------------------
			CvMemStorage* storage = cvCreateMemStorage(0);
			CvSeq* contour,*tmp;


			IplImage* img_contour = cvCloneImage(img_dest);
			IplImage* img_point =cvCreateImage(cvGetSize(img_dest), 8, 3); //usually RGB image are not 
			
			cvCvtColor(img_dest, img_point, CV_GRAY2BGR); 
			int num = cvFindContours(img_contour, storage, &contour, sizeof (CvContour),CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE );
			CvSeq* result;

			int cont=0;
			int alturaminina = img_dest->height * 0.4;
			int maxWidth = img_dest->width * 0.5;
			while(contour)
			{
				CvRect rec = cvBoundingRect(contour);								
				int area = rec.height*rec.width;
				cont++;
				if (area<25 || rec.height<alturaminina){
					CvPoint* point = (CvPoint*)cvGetSeqElem(contour, 1);
					//cvFloodFill(img_dest,cvPoint(point->x,point->y),cvScalar(0),cvScalar(100),cvScalar(1024));					
					cvRectangleR(img_dest,rec,cvScalar(0),CV_FILLED);
					cvRectangle(img_point,cvPoint(point->x,point->y),cvPoint(point->x+3,point->y+3),cvScalar(255,0,0));
				}else if(rec.width>maxWidth){
					CvPoint* point = (CvPoint*)cvGetSeqElem(contour, 1);
					cvFloodFill(img_dest,cvPoint(point->x,point->y),cvScalar(0),cvScalar(1),cvScalar(127));
				}else{
					//cvRectangleR(imgShow,rec,cvScalar(255,0,0));
				}
				contour = contour->h_next;
			}		

			
			CvMat * colSum = cvCreateMat(1, img_dest->width, CV_64FC1);
			cvReduce(img_dest, colSum, 0, CV_REDUCE_SUM);
			//cvSmooth(ColSum,ColSum,2,15);		
			

			//vector<CvRect> caracteres;
			caracteres.clear();
			byte lastNull = 1;
			int inix=0;
			int finx = 0;
			int bin = 0;
			int NULL_BIN = 1; 
			double BIN_PERCENT_LOOK_FOR = 0.3;
			int TAMANHO_MININO_CHAR = 2;
			int lmedia=0;			
			for (int i =0;i<colSum->cols;i++){
				bin = cvGet2D(colSum,0,i).val[0]/255;
				//if(bin <= NULL_BIN) inix=i;
				if(bin > NULL_BIN && lastNull==1){//Achei o início de um caractere
					inix=i;
					lastNull=0;
				}else if(bin > NULL_BIN){//Continua fazendo parte do caractere
					finx=i;					
				}else if(lastNull==0 && bin<=NULL_BIN){//Fim do caractere
					if((finx-inix)>=TAMANHO_MININO_CHAR){
						caracteres.push_back(cvRect(inix,0,finx-inix +1,img_dest->height));
						lmedia+=finx-inix;
						lastNull=1;
					}else{
						inix=i;
						lastNull=1;
					}
				}
			}
			if (lastNull ==0 && (finx-inix)>TAMANHO_MININO_CHAR){//Se o último bin não for nulo trata-se de um caracter cujo último bin está na borda
				caracteres.push_back(cvRect(inix,0,finx-inix,img_dest->height));
				lmedia+=finx-inix;
			}
			if (caracteres.size()<=1 || lmedia<=0) //Não foi encontrado nenhum objeto, provavelmente não trata-se de uma placa
				return 0;
			//lmedia/=caracteres.size();
			lmedia/=(caracteres.size()>=7?caracteres.size():7);

			if (lmedia<5) return false;
			if (caracteres.size()>10) return false;


			/* -> Salvar imagem
			IplImage* imgNull = cvCloneImage(imgOriginal);
			for(int i =0;i<caracteres.size();i++){
				CvRect r = caracteres.at(i);
				cvRectangleR(imgNull,r,cvScalar(128,255,0));				
			}
			saveImage("placas/result/imgNull", countImg,imgNull);
			printf("Saving image %d",countImg);
			*/

			if (caracteres.size()<=3) return false;

			//*******************************************************************
			//********************SPLITING BIG BOUNDING BOXES********************
			//*******************************************************************
			//if (caracteres.size() < 7){
				for(int i =0;i<caracteres.size();i++){
					CvRect r = caracteres.at(i);
					double rMedia = ((double)(r.width + (r.width*0.0)))/lmedia;
					/*if (caracteres.size()+rMedia -1 >7){
						rMedia = 7 - caracteres.size()+1;//rMedia - (caracteres.size()+rMedia-7);//+1 cause we will eliminate that Rect
					}*/
					if (rMedia>=1.8){
						int probWidth = r.width/rMedia;
						caracteres.erase(caracteres.begin() +i);
						//Split
						for(int j=0;j<rMedia-1;j++){
							int prob = r.x + (probWidth);
							//Look for best place to split
							int lookIni = prob -  probWidth * BIN_PERCENT_LOOK_FOR;
							int lookFin = prob +  probWidth * BIN_PERCENT_LOOK_FOR;
							int MenorCusto=cvGet2D(colSum,0,lookIni).val[0]/255;
							int xMenorCusto=lookIni;
							while(lookIni<lookFin){
								int custo = cvGet2D(colSum,0,lookIni).val[0]/255;
								if (custo<MenorCusto){
									MenorCusto=custo;
									xMenorCusto=lookIni;
								}
								lookIni++;
							}
							int antWidth = r.width;
							r.width = xMenorCusto-r.x;
							if(r.width>=TAMANHO_MININO_CHAR)//Just allow a new one if fits the size of min width for new object
								caracteres.insert(caracteres.begin()+i,r);
							r = cvRect(xMenorCusto, r.y, antWidth-r.width, img_dest->height);															
							i++;
						}
						if (r.width >TAMANHO_MININO_CHAR )caracteres.insert(caracteres.begin()+i,r);
					}
				}
			//}
			//*******************************************************************


			IplImage* imgSplit = cvCloneImage(imgOriginal);
			for(int i =0;i<caracteres.size();i++){
				CvRect r = caracteres.at(i);
				cvRectangleR(imgSplit,r,cvScalar(128,255,0));				
			}
			//NaoSalvar saveImage("placas/result/split", cont,imgSplit);

			if (caracteres.size()>7){
				///****Removing and merging by distance*****
				int meanDistance=0;
				CvRect r = caracteres.at(0);
				for(int i =1;i<caracteres.size();i++){
					CvRect rNext = caracteres.at(i);
					meanDistance += (rNext.x + rNext.width/2)-(r.x + r.width/2);
					r= rNext;
				}
				r = caracteres.at(0);
				meanDistance = meanDistance/caracteres.size();
				for(int i =1;i<caracteres.size();i++){
					CvRect rNext = caracteres.at(i);
					int distance = (rNext.x + rNext.width/2)-(r.x + r.width/2);
					if (distance< meanDistance/2) {//Merge
						caracteres.erase(caracteres.begin()+i);
						caracteres.at(i).width = rNext.x - r.x + rNext.width;
					}
					r= rNext;
				}

				//Removing first ones by distance
				if(caracteres.size()>NUMBER_OF_CHARS){
					int numberToRemove = caracteres.size() - NUMBER_OF_CHARS;
					r = caracteres.at(0);
					while(numberToRemove>0){
						CvRect rNext = caracteres.at(numberToRemove);
						int distance = (rNext.x + rNext.width/2)-(r.x + r.width/2);
						if (distance > 2*meanDistance){
							caracteres.erase(caracteres.begin()+numberToRemove);
						}
						numberToRemove--;
					}
					//Removing last ones
					numberToRemove = caracteres.size() - NUMBER_OF_CHARS;
					r = caracteres.at(NUMBER_OF_CHARS-1);				
					while(numberToRemove>0){
						CvRect rNext = caracteres.at(caracteres.size() - numberToRemove);
						int distance = (rNext.x + rNext.width/2)-(r.x + r.width/2);
						if (r.width > 2*meanDistance){
							caracteres.erase(caracteres.begin()+caracteres.size()-numberToRemove);
						}
						numberToRemove--;
					}

				}
			}
			//****************************************************************


			if (caracteres.size()>7){
				///****Removing by middle determination*****
				//Determining the middle
				int pStart = colSum->width * 0.4;// * 1/3 ;
				int pEnd = colSum->width * 1/2 ;
				int middle=0;
				while(pStart<pEnd){
					int bin = cvGet2D(colSum,0,pStart).val[0]/255;
					if (bin<=NULL_BIN+3){
						middle=pStart;
						break;
					}
					pStart++;
				}
				if (middle!=0){
					int contFirst=0;//Count the number of char in the first part of the plate
					int contSecond=0;
					for(int i =0;i<caracteres.size();i++){
						CvRect r = caracteres.at(i);
						if (r.x< middle) contFirst++;
						else contSecond++;
					}
					if (contSecond>4){
						caracteres.erase(caracteres.begin()+caracteres.size()-1);//Removing last
					}else if(contFirst>3){
						caracteres.erase(caracteres.begin()+0);//Removing First						
					}
				}
			}
			//****************************************************************


			//***********************Removing false candidates****************
			if (caracteres.size()>7){
				//***Removing vey large objects
				double PERCENT_LARGE_ANORMAL_OBJECT=0.6;
				for(int i =0;i<caracteres.size();i++){
					CvRect r = caracteres.at(i);
					if (r.width>= lmedia + (lmedia*PERCENT_LARGE_ANORMAL_OBJECT)){
						caracteres.erase(caracteres.begin()+i);
					}
				}
			}



			//**********************Finding the HEIGTH and Y axe************************
			int meanHeight=0;
			int meany = 0;
			for(int i =0;i<caracteres.size();i++){
				CvRect r = caracteres.at(i);
				cvSetImageROI(imgOriginal,r);
				CvMemStorage* storage = cvCreateMemStorage(0);
				CvSeq* contour,*tmp;
				int num = cvFindContours(imgOriginal, storage, &contour, sizeof (CvContour),CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE );
				int cont=0;
				int maxArea=0;
				CvRect maxRect;
				//Find Biggest one
				if (num>0){
					while(contour)
					{
						CvRect rec = cvBoundingRect(contour);								
						int area = rec.height*rec.width;
						cont++;
						if (area>maxArea){
							maxRect=rec;
							maxArea=area;
						}
						contour = contour->h_next;
					}
					meanHeight+=maxRect.height;
					meany+=maxRect.y;
					caracteres.at(i).y=maxRect.y;
					caracteres.at(i).height=maxRect.height;
					cvResetImageROI(imgOriginal);
				}
			}
			meanHeight/=caracteres.size();
			meany/=caracteres.size();
			//*****************************************************************************
			//************** Removing stranges characteres*********************************
			double MAX_PERCENT_HEIGTH=0.25;
			double MAX_PERCENT_Y=0.25;
			//if (caracteres.size()>7){
			//	for(int i =0;i<caracteres.size();i++){//Start from the second one
			//		CvRect* r = &caracteres.at(i);
			//		if ((r->height > meanHeight - MAX_PERCENT_HEIGTH*meanHeight) || (r->height < meanHeight - MAX_PERCENT_HEIGTH*meanHeight)){
			//			caracteres.erase(caracteres.begin()+i);
			//		}
			//	}
			//}
			//*******************************************************************************


			//****Adjusting the heigth and Y for outliers************************
			//We just ajust the ones that was not removed as a STRANGE
			for(int i =0;i<caracteres.size();i++){//Start from the second one
				CvRect* r = &caracteres.at(i);
				if ((r->height > meanHeight + MAX_PERCENT_HEIGTH*meanHeight) || (r->height < meanHeight - MAX_PERCENT_HEIGTH*meanHeight)){
					r->height =meanHeight;
					if ((r->y > meany - MAX_PERCENT_HEIGTH*meany) || (r->y < meany - MAX_PERCENT_Y*meany)){//We just adjust the Y if height has changed
						r->y =meany;
					}
				}
			}
			//**************************************************





			//****Removing irrelevant Cols***********************************

			//int col=0,nextcol=0;
			//for(int i =0;i<caracteres.size();i++){//start from the second one
			//	//start (at x)
			//	col = cvget2d(colsum,0,caracteres[i].x).val[0]/255;
			//	nextcol = cvget2d(colsum,0,caracteres[i].x + 1).val[0]/255;
			//	while (nextcol*0.5>col){
			//		caracteres[i].x++;
			//		caracteres[i].width--;
			//		col=nextcol;
			//		nextcol = cvget2d(colsum,0,caracteres[i].x + 1).val[0]/255;
			//	}
			//	//end (at end of width)
			//	col = cvget2d(colsum,0,caracteres[i].x + caracteres[i].width).val[0]/255;
			//	nextcol = cvget2d(colsum,0,caracteres[i].x + caracteres[i].width -1).val[0]/255;
			//	while (nextcol*0.5>col){
			//		caracteres[i].width--;
			//		col=nextcol;
			//		nextcol = cvget2d(colsum,0,caracteres[i].x + caracteres[i].width -1).val[0]/255;
			//	}
			//	
			//}			
			//***************************************


			/*
			//*******************************************************
			//******Adjusting the WIDTH and HEIGHT*******************
			//*******************************************************
			int largest=0;//Size of the largest rectangle
			int higher=0;
			for(int i =0;i<caracteres.size();i++){
				CvRect r = caracteres.at(i);
				if (r.width>largest) largest = r.width;
				if (r.height>higher) higher = r.height;
				//cvRectangleR(imgShow,r,cvScalar(0,255,0));
				//cvRectangleR(imgShow,r,random_color(&rng));
				
			}

			int incSize=0, leftSize=0, rightSize =0;//Values to adjust
			for(int i =0;i<caracteres.size();i++){
				CvRect& r = caracteres.at(i);
				int incSize = largest-r.width;
				//if (incSize%2==0){
				//	leftSize=incSize/2;
				//	rightSize=leftSize;
				//}else{
				//	leftSize=incSize/2;
				//	rightSize=leftSize+1;//Always one in the left (in case ofodd)
				//}
				r.x = r.x - incSize/2;
				r.width = r.width+ incSize;
				//r.x = r.x - incSize/2 + incSize%2; r.width = r.width + incSize/2;//Resume all the if's
				int incHeight = higher - r.height;
				r.y = r.y - incHeight/2 ; 
				r.height = r.height + incHeight;
				if(r.y<0){
					r.y=0;
					r.height+=abs(r.y);
				}
				//cvRectangleR(imgShow,r,random_color(&rng));
				//printf(">>>>>>>>Flooding Image. Area=%d, x=%d, y=%d,w=%d,h=%d\n",r.width*r.height,r.x,r.y,r.width,r.height);
			}
			//***********************************
			*/

			//*********Adjusting overlapped bounding boxes******************************
			//CvRect* rAnt= &caracteres[0];
			//for(int i =1;i<caracteres.size();i++){//Start from the second one
			//	CvRect* r = &caracteres.at(i);
			//	if (r->x <= (rAnt->x + rAnt->width)+1){
			//		int adjust = r->x - rAnt->x - rAnt->width;
			//		rAnt->width-=adjust/2 +1;
			//		r->x+=adjust/2 +1;
			//	}
			//	//cvRectangleR(imgShow,rAnt,random_color(&rng));
			//	//printf(">>>>>>>>Flooding Image. Area=%d, x=%d, y=%d,w=%d,h=%d\n",r.width*r.height,r.x,r.y,r.width,r.height);
			//	rAnt=r;
			//}			
			//if ((rAnt->x+ rAnt->width) >colSum->cols)
			//	caracteres[caracteres.size()-1].width=colSum->cols-rAnt->x;
			//cvRectangleR(imgShow,rAnt,random_color(&rng));
			//***************************************


			
			//if (caracteres.size()>10 || caracteres.size()<5 ){
			//if (true) return true;//Code to test
			if (caracteres.size()>7) caracteres.erase(caracteres.begin());
			if (caracteres.size()>7 || caracteres.size()<7 ){
				return false;
			}else{
				if (plateNumber!=NULL)
					free(plateNumber);
				 plateNumber = (char*)malloc((sizeof(char)*caracteres.size()) + 1);
				 plateNumber[caracteres.size()]='\0';
				//******Recognizing Plate*********************
				for(int i =0;i<caracteres.size();i++){
					CvRect r = caracteres.at(i);
					//cvSetImageROI(img_dest,r);
					cvSetImageROI(improvedImage,r);
					//IplImage* newImg = cvCreateImage(cvSize(r.width,r.height),img_dest->depth,img_dest->nChannels);
					IplImage* newImg = cvCreateImage(cvSize(r.width,r.height),improvedImage->depth,improvedImage->nChannels);
					//CvMat *newImg = cvCreateMat(r.height,r.width,CV_8U);
					
					cvCopy(improvedImage,newImg);
					//cvShowImage("Char",newImg);
					//cvvWaitKey('q');
					//char pathSave[1024];
					//sprintf(pathSave, "%sp%dc%d.png","char/char",contImage,i);
					//cvSaveImage( pathSave, newImg );
					char result;
					if (i>=3) 
					result = svmRecog->recognize(newImg);
					else result = svmRecog->recognizeLetter(newImg);
					plateNumber[i] = result;
					cvResetImageROI(improvedImage);									
				}

				//**********************************************************	
			}							
			return true;
		}

void PlateSegmentation::init(){
	svmRecog = new SVMRecognizer();
	svmRecog->init();
	plateNumber=NULL;
}

vector<CvRect> PlateSegmentation::getCaracteres(){
	return caracteres;
}

char* PlateSegmentation::getResultPlate(){
	return plateNumber;
}

IplImage* PlateSegmentation::getImprovedImage(){
	IplImage* imgShow =cvCreateImage(cvGetSize(improvedImage), 8, 3); //usually RGB image are not 
	cvCvtColor(improvedImage, imgShow, CV_GRAY2BGR);//Just to show the bounding boxes
	return imgShow;
}