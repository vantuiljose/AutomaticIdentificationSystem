// OpenCV_Teste.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "List.h"
#include "PlateLocation.h"
#include "baseapi.h"
#include "genericvector.h"
#include "strngs.h"
#include "PlateSegmentation.h"
#include "ResultDialog.h"
#include "Logger.h"
#include "VantzUtils.h"

 
//
const int TRAJ_MAX = 5;
const int MAX_TTL = 5;
const int MAX_DISTANCE_TRAJECT = 10;
const int RESIZE_FACTOR=8;
int contId=0;
const int MAX_TOQUE_BORDA = 3;

 int nTrajs;
 TrajetoriasAtivas* tAtivas;

using namespace std;
using namespace cvb;

double tempoGasto=0;



int _tmain(int argc, _TCHAR* argv[])
//int commentPrincipal()
{
	IplImage *img = cvLoadImage("D:/Mestrado/placas/Placa1.png");
	tesseract::TessBaseAPI api; 
	PlateSegmentation *recog = new PlateSegmentation();
	ResultDialog *resultShow = new ResultDialog;
	Logger* logger = new Logger();
	resultShow->init();
	recog->init();
	logger->init("log/C00031.log");

	CvCapture *capture;//D:\Mestrado\VideoMestrado\19-02-2013
	capture = cvCaptureFromAVI("D:/Mestrado/VideoMestrado/base/A/05.avi");
	//capture = cvCaptureFromAVI("G:/Videos Mestrado/Divididos/B/00030_xvid.avi");
	//capture = cvCaptureFromAVI("D:/Mestrado/Converted/13Marco2013/.avi");
	//capture = cvCaptureFromAVI( "D:/Mestrado/VideoMestrado/00015.avi" );
	//capture = cvCaptureFromCAM( 0);
	assert( capture );
	
	//api.Init("C:\Program Files (x86)\Tesseract-OCR","eng");
	
	//GenericVector<STRING> pars_vec ;
	//
    //pars_vec.push_back("load_system_dawg");
 //   pars_vec.push_back("load_freq_dawg");
 //   pars_vec.push_back("load_punc_dawg");
 //   pars_vec.push_back("load_number_dawg");
 //   pars_vec.push_back("load_unambig_dawg");
 //   pars_vec.push_back("load_bigram_dawg");
 //   pars_vec.push_back("load_fixed_length_dawgs");


	//GenericVector<STRING> pars_values;
 //   pars_values.push_back("F");
 //   pars_values.push_back("F");
 //   pars_values.push_back("F");
 //   pars_values.push_back("F");
 //   pars_values.push_back("F");
 //   pars_values.push_back("F");
 //   pars_values.push_back("F");
	
	char *conf;
	api.Init("C:\tesseract\tesseract-ocr\tessdata", "eng");
	tesseract::OcrEngineMode mode = tesseract::OEM_TESSERACT_CUBE_COMBINED;
	//api.Init("C:\tesseract\tesseract-ocr\tessdata", "eng", mode , &conf, 0, &pars_vec, &pars_values, false);

	api.SetPageSegMode(tesseract::PSM_SINGLE_LINE);
	api.SetVariable("load_system_dawg","0");
	api.SetVariable("tessedit_char_whitelist", "ABCDEFGHIJKLMNOPQRSTUVWXYZ-0123456789 ");
	api.SetVariable("textord_min_xheight","25");
	



	CvFont font;
	cvInitFont(&font, CV_FONT_HERSHEY_PLAIN, 1.0, 1.0, 0, 2, CV_AA);

	// image data structures
	IplImage *img1;
	IplImage *img2;
	IplImage *img3;
	IplImage *imgrgb1;
	IplImage *imgrgb2;
	IplImage *imgrgb3;
	IplImage *imgrgbResize;

	IplImage *img1Resize;
	IplImage *img2Resize;
	IplImage *img3Resize;

	IplImage *difAnt;
	IplImage *difPos;
	IplImage *bw_difference;
	IplImage *bwAnt;
	IplImage *bwPos;
	IplImage *moveImage;
	// get the camera image size
	IplImage *imgsize;
	imgsize = cvQueryFrame( capture );
	if( !imgsize ) return -1;
	img1 = cvCreateImage( cvGetSize( imgsize ), IPL_DEPTH_8U, 1);
	img2 = cvCreateImage( cvGetSize( imgsize ), IPL_DEPTH_8U, 1);
	img3 = cvCreateImage( cvGetSize( imgsize ), IPL_DEPTH_8U, 1);

	imgsize->height= imgsize->height/RESIZE_FACTOR;
	imgsize->width= imgsize->width/RESIZE_FACTOR;
	int MAX_HEIGHT = imgsize->height;
	int MAX_WIDTH = imgsize->width;

	imgrgbResize = cvCreateImage( cvGetSize( imgsize ), IPL_DEPTH_8U, 3);
	// grayscale buffers	
	img1Resize = cvCreateImage( cvGetSize( imgsize ), IPL_DEPTH_8U, 1);
	img2Resize = cvCreateImage( cvGetSize( imgsize ), IPL_DEPTH_8U, 1);
	img3Resize = cvCreateImage( cvGetSize( imgsize ), IPL_DEPTH_8U, 1);
	difAnt = cvCreateImage( cvGetSize( imgsize ), IPL_DEPTH_8U, 1);
	difPos = cvCreateImage( cvGetSize( imgsize ), IPL_DEPTH_8U, 1);
	bwAnt = cvCreateImage( cvGetSize( imgsize ), IPL_DEPTH_8U, 1);
	bwPos = cvCreateImage( cvGetSize( imgsize ), IPL_DEPTH_8U, 1);
	bw_difference = cvCreateImage( cvGetSize( imgsize ), IPL_DEPTH_8U, 1);
	moveImage = cvCreateImage( cvGetSize( imgsize ), IPL_DEPTH_8U, 1);


	const int element_shape = CV_SHAPE_RECT;
	int pos = (8/RESIZE_FACTOR)+1;
	IplConvKernel* element = cvCreateStructuringElementEx( pos, pos+1, pos/2, pos/2, element_shape, 0 );

	const int dilateElementShape = CV_SHAPE_RECT;
	pos = (72/RESIZE_FACTOR) + 1;
	IplConvKernel* dilateElement = cvCreateStructuringElementEx( pos, pos, pos/2, pos/2, dilateElementShape, 0 );
	

	const int erode2ElementShape = CV_SHAPE_RECT;
	pos = (64/RESIZE_FACTOR) + 1;
	IplConvKernel* erode2Element = cvCreateStructuringElementEx( pos, pos, pos/2, pos/2, erode2ElementShape, 0 );

	imgrgb1 = cvQueryFrame( capture );
	imgrgb2 = cvQueryFrame( capture );
	cvCvtColor( imgrgb1, img1, CV_RGB2GRAY );
	cvCvtColor( imgrgb2, img2, CV_RGB2GRAY );
	cvResize(img1,img1Resize,CV_INTER_NN);//Interpolation by neighborhood
	cvResize(img2,img2Resize,CV_INTER_NN);//Interpolation by neighborhood

	cvResize(imgrgb2,imgrgbResize, CV_INTER_LINEAR);
	int key = 0;
	int count=0;
	createListTraj(&tAtivas);
	//createListTraj(tAtivas);
	//createListTraj();
	//Video Initialization
	double tempoPlaca=0;
	int numeroPlacas=0;
	CvVideoWriter* writer = cvCreateVideoWriter("out3.avi", 0, 10, cvGetSize( imgrgbResize ), 1);
	double tempoVideoInteiro = omp_get_wtime();
	double timeDisco=0;
	double timeTratamento=0;
	double timeDeteccao=0;
	double timeRastreamento=0;
	double timeLocalizacao=0;
	double timeRecog=0;
	while ( key != 'q' ) {

		count++;
		// load image one
		double timingTotal = omp_get_wtime();
		//double timing = omp_get_wtime();
		double timingLeitura = omp_get_wtime();
		double time = omp_get_wtime();
		//imgrgb3 = cvQueryFrame (capture);
		cvGrabFrame(capture);
		//imgrgb3 = cvQueryFrame (capture);
		cvGrabFrame(capture);
		imgrgb3 = cvQueryFrame (capture);

		if (count==3257){
			int debug = 1;
		}

		timingLeitura = omp_get_wtime() - timingLeitura;
		timeDisco +=(omp_get_wtime()-time);
		//NÃOMOSTRAR printf("Frame: %d\n",count);
		//printf("\ntempo Im3:%f\n; ",timingLeitura);
		if ( !imgrgb3 ){
				logger->save();
				cvReleaseCapture( &capture );
				cvReleaseVideoWriter(&writer);
				cvDestroyWindow( "video" );
				//printf("%f", tempoGasto/count);
				printf("\n\ndisco;trat;loc;rast;loc;recog\n%f;%f;%f;%f;%f;%f;\n\n",timeDisco,timeTratamento,timeLocalizacao,timeRastreamento,timeDeteccao,timeRecog);
				printf("Tempo gasto:%f Nro de Placas:%d Tempo Medio:%f",tempoPlaca, numeroPlacas,tempoPlaca/numeroPlacas);
				system("pause");
				tempoVideoInteiro = omp_get_wtime() - tempoVideoInteiro;
				printf("\nTEMPO TOTAL:%f",tempoVideoInteiro/60);
				return (tempoGasto/count*1000);
		}
		//cvGrabFrame(capture);
		//cvGrabFrame(capture);
		//cvGrabFrame(capture);
		//imgrgb3 = cvRetrieveFrame(capture);
		//timing = omp_get_wtime() - timing;
		//printf("%f; ",timing);

		//timing = omp_get_wtime();
		time = omp_get_wtime();

		// convert rgb to grayscale
		cvResize(imgrgb3,imgrgbResize,CV_INTER_NN);//Interpolation by neighborhood
		//NÃOSALVARsaveImage("teste/original",count,imgrgbResize);
		cvCvtColor( imgrgbResize, img3Resize, CV_RGB2GRAY );

		timeTratamento +=(omp_get_wtime()-time);
		time = omp_get_wtime();
		
		// compute difference
		cvSub(img2Resize,img1Resize, difAnt);
		cvSub(img2Resize,img3Resize, difPos);
		//NÃOSALVAR saveImage("teste/difAnt",count,difAnt);
		//Threshold
		cvThreshold(difAnt, bwAnt, 15, 255, CV_THRESH_BINARY);
		cvThreshold(difPos, bwPos, 15, 255, CV_THRESH_BINARY);

		//And -> Merging the two differences
		cvOr(bwAnt, bwPos, bw_difference);

		//NaoSalvar saveImage("teste/dif",count,bw_difference);

		//Erosão
		cvErode(bw_difference, moveImage, element, 1);
		//NaoSalvar saveImage("teste/erosion1-",count,moveImage);
		cvDilate(moveImage, bw_difference, element, 1);
		//NaoSalvar saveImage("teste/dilatation1-",count,bw_difference);
		cvErode(bw_difference, moveImage, element, 1);
		//NaoSalvar saveImage("teste/erosionLast-",count,moveImage);
		//cvDilate(bw_difference,bw_difference,element,20);
		//Dilatação
		cvDilate(moveImage,bw_difference, dilateElement,2);
		//NaoSalvar saveImage("teste/dilatation2-",count,bw_difference);
		//Erosão
		cvErode(bw_difference, moveImage, erode2Element, 1);
		//NaoSalvar saveImage("teste/erosion2-",count,moveImage);


		//timing = omp_get_wtime() - timing;
		//printf("%f;",timing);
		//timing = omp_get_wtime();
		IplImage *labelImg = cvCreateImage(cvGetSize(moveImage), IPL_DEPTH_LABEL, 1);
		CvBlobs blobs;
		unsigned int result=cvLabel(moveImage, labelImg, blobs);
		cvFilterByArea(blobs,1000,20000);
		//NaoSalvar cvRenderBlobs(labelImg,blobs,imgrgbResize,imgrgbResize);


		timeDeteccao +=(omp_get_wtime()-time);
		time = omp_get_wtime();	
		//printf("Frame = %d\n",count);
		double timing = omp_get_wtime();


		IplImage* plateImage = NULL;						
		IplImage* tracked=NULL;
		IplImage* trackedGray ;
		for (CvBlobs::const_iterator it=blobs.begin(); it!=blobs.end(); ++it){
			CvBlob* blob = it->second;			
			Trajetoria* c = tAtivas->ini;
			while (c->prox != NULL){
				c = c->prox;
				Trajetoria* traj = c;
				//if(abs(traj->x - blob->centroid.x) < MAX_DISTANCE_TRAJECT && abs(traj->y - blob->centroid.y) < MAX_DISTANCE_TRAJECT && traj->toqueBordas<MAX_TOQUE_BORDA){//ForB
				if(abs(traj->x - blob->centroid.x) < MAX_DISTANCE_TRAJECT && abs(traj->y - blob->centroid.y) < MAX_DISTANCE_TRAJECT){//ForA
				//if(abs(traj->x - blob->centroid.x) < MAX_DISTANCE_TRAJECT && abs(traj->y - blob->centroid.y) < MAX_DISTANCE_TRAJECT){//ForC
					//printf("B:{X=%f, Y=%f, area=%d}  a T:{ID =%d,  X=%d, Y=%d, area=%d, ttl = %d}\n ",blob->centroid.x,blob->centroid.y,blob->area, traj->id, traj->x, traj->y,traj->area, traj->ttl );//ForD
					//printf("Diferença: X=%f - Y=%f \n", traj->x - blob->centroid.x,traj->y - blob->centroid.y);					
					traj->area = blob->area;
					traj->dx = traj->x - blob->centroid.x;
					traj->x = blob->centroid.x;
					traj->y = blob->centroid.y;
					traj->ttl = traj->x - blob->centroid.x;
					traj->ttl = MAX_TTL;
					traj->toqueBordas+=(traj->x <10? 1 : (traj->toqueBordas>0 ? -1: 0));
					traj->isTouchingBorder = (blob->minx < 3) || blob->maxx > (MAX_WIDTH -3);				
					blob->usada = 1;
					char idPrint[50];
					double ratio = abs(((double)(blob->maxy -blob->miny)/(blob->maxx -blob->minx)) - 1.0);
					//printf("frame=%d-ratio=%f", count, ratio);
					int space = 20;
					sprintf(idPrint, "%d", traj->id);
					cvPutText(imgrgbResize, idPrint, cvPoint(traj->x, traj->y - space), &font, cvScalar(255, 255, 255, 0));
					sprintf(idPrint, "ratio=%f",ratio);
					cvPutText(imgrgbResize, idPrint, cvPoint(traj->x, traj->y), &font, cvScalar(255, 255, 255, 0));
					sprintf(idPrint, "dx=%d",traj->dx);
					cvPutText(imgrgbResize, idPrint, cvPoint(traj->x, traj->y +space), &font, cvScalar(255, 255, 255, 0));
					
					//if (traj->area>7000 && traj->dx < 3 && traj->recognized==0 && ratio<0.6){
					//if (traj->area>5000 && traj->dx < 10  && ratio<0.6 && traj->recognized==0 !traj->isTouchingBorder){//Base ForB - Final
					if (traj->area>4200 && traj->dx < 10  && ratio<0.6 && traj->recognized==0 && (count - traj->inicio)>10){//ForA
					//if (traj->area>3500 && traj->dx < 10  && ratio<0.6 && traj->recognized==0 && !traj->isTouchingBorder){//ForD
					//if (traj->area>4000 && traj->dx < 10  && ratio<0.6 && traj->recognized==0 && !traj->isTouchingBorder){//6000 for all videos
						numeroPlacas++;
						timeRastreamento += (omp_get_wtime()-time);
						time = omp_get_wtime();	
						//RECONHECER PLACA
						//IplImage* tracked = cropImage(imgrgb3, blob->minx * RESIZE_FACTOR, blob->miny * RESIZE_FACTOR, (blob->maxx -blob->minx) * RESIZE_FACTOR, (blob->maxy -blob->miny)* RESIZE_FACTOR);
						//IplImage* tracked = cropImage(imgrgb3,  blob->miny * RESIZE_FACTOR, blob->minx * RESIZE_FACTOR, (blob->maxy -blob->miny)* RESIZE_FACTOR,  (blob->maxx -blob->minx) * RESIZE_FACTOR, 0.1, 0.1, 0.4,0.2);//double cutLeft, double cutRight, double cutUp, double cutDown)
						//tracked = cropImage(imgrgb3,  blob->miny * RESIZE_FACTOR, blob->minx * RESIZE_FACTOR, (blob->maxy -blob->miny)* RESIZE_FACTOR,  (blob->maxx -blob->minx) * RESIZE_FACTOR, 0.3, 0.1, 0.3,0.05);//double cutLeft, double cutRight, double cutUp, double cutDown)//CAR ENTERING
						//tracked = cropImage(imgrgb3,  blob->miny * RESIZE_FACTOR, blob->minx * RESIZE_FACTOR, (blob->maxy -blob->miny)* RESIZE_FACTOR,  (blob->maxx -blob->minx) * RESIZE_FACTOR, 0.1, 0.35, 0.6,0.01);//CAR for B
						tracked = cropImage(imgrgb3,  blob->miny * RESIZE_FACTOR, blob->minx * RESIZE_FACTOR, (blob->maxy -blob->miny)* RESIZE_FACTOR,  (blob->maxx -blob->minx) * RESIZE_FACTOR, 0.2, 0.15, 0.6,0.01);//CAR for A
						//tracked = cropImage(imgrgb3,  blob->miny * RESIZE_FACTOR, blob->minx * RESIZE_FACTOR, (blob->maxy -blob->miny)* RESIZE_FACTOR,  (blob->maxx -blob->minx) * RESIZE_FACTOR, 0.2, 0.3, 0.4,0.2);//CAR for D
						//tracked = cropImage(imgrgb3,  blob->miny * RESIZE_FACTOR, blob->minx * RESIZE_FACTOR, (blob->maxy -blob->miny)* RESIZE_FACTOR,  (blob->maxx -blob->minx) * RESIZE_FACTOR, 0.4, 0.2, 0.4,0.1);//Cars at C
						trackedGray = cvCreateImage( cvGetSize( tracked ), IPL_DEPTH_8U, 1);
						cvCvtColor( tracked, trackedGray, CV_RGB2GRAY);
						//cvNamedWindow("Tracked", 1);
						//cvShowImage("Tracked", trackedGray);
						printf("Frame = %d T:{ID =%d,  X=%d, Y=%d, area=%d, ttl = %d}\n ",count, traj->id, traj->x, traj->y,traj->area, traj->ttl );
						
						/*NaoSalvar
						char filetrajname[50];
						sprintf(filetrajname, "img/f%d-T%d.png",count, traj->id);
						cvSaveImage(filetrajname,trackedGray);	
						*/
						//Plate p = getPlate(trackedGray);
						double inicioLocPlaca = omp_get_wtime();
						Plate p = vlpl(trackedGray);////////-------------------------//////------------------------
									timeLocalizacao += (omp_get_wtime()-time);
							
						if (p.x>0){
							//IplImage* plateImageTemp = cropImage(trackedGray,p);
							//IplImage* plateImage = cvCreateImage( cvGetSize( plateImageTemp ), IPL_DEPTH_8U, 1);
							//cvThreshold(plateImageTemp,plateImage,0,255,CV_THRESH_OTSU);
							time = omp_get_wtime();	
							plateImage = cropImage(tracked,p);
							
							/* NaoSalvar
							char filePlateName[50];
							sprintf(filePlateName, "img/pf%d-T%d.png",count, traj->id);
							cvSaveImage(filePlateName,plateImage);
							*/

							
							 
							if (recog->processPlate(plateImage)){
								char* resultPlate = recog->getResultPlate();
								printf( "Placa=%s\n",resultPlate);
								resultShow->setResultPlate(resultPlate);
								traj->recognized = 1;
								logger->log(resultPlate,count);
								//NaoSalvar logger->completeLog(count, tracked,plateImage, recog->getImprovedImage(),recog->getCaracteres(),resultPlate);
							}
							timeRecog += (omp_get_wtime()-time);
							//cvWaitKey(0);
							/* Tesseract
							api.SetRectangle(0,0,plateImage->width, plateImage->height);
							api.SetImage( (unsigned char*) plateImage->imageData,  plateImage->width, plateImage->height, plateImage->nChannels, plateImage->widthStep);
							api.Recognize(NULL);
							char* texto= (api.GetUTF8Text());
							printf("Reconhedida=%s",texto);
							traj->recognized=1;
							cvReleaseImage(&plateImage);
							*/
						}else{//Não há placa ou frame há ser localizado
							timeRastreamento += (omp_get_wtime()-time);
						}

						cvReleaseImage(&trackedGray);

					}
					
					break;
				}
			}
		}
		time = (omp_get_wtime());
		for (CvBlobs::const_iterator it=blobs.begin(); it!=blobs.end(); ++it){						
			CvBlob* blob = it->second;
			if(blob->usada==0){//Blob was not used, so is a new Trajectory				
				Trajetoria* t = (Trajetoria*) malloc(sizeof(Trajetoria));
				t->area =blob->area;
				t->x = blob->centroid.x;
				t->y = blob->centroid.y;
				t->id = contId++;
				t->ttl = MAX_TTL;
				t->recognized = 0;
				t->isTouchingBorder = false;
				addTraj( tAtivas,t);
				//printf("\n------------------\nNova Trajetoria. frame=%d, centroide = (%d,%d), area = %d\n",count, t->x,t->y,t->area);				
			}
		}

	Trajetoria* c = tAtivas->ini;
	Trajetoria* cAnt = tAtivas->ini;
	while (c != NULL && c->prox != NULL){		
		c = c->prox;
		Trajetoria* traj = c;
		traj->ttl = traj->ttl-1;
		//printf("--->>>T:{ID =%d,  X=%d, Y=%d, area=%d, ttl = %d}\n ", traj->id, traj->x, traj->y,traj->area, traj->ttl );
		if((traj->ttl)<=0){
			cAnt->prox = c->prox;
			free(c);
			tAtivas->size--;
			c = cAnt->prox;
		}else{
			cAnt = c;			
		}
	}
	timing = omp_get_wtime() - timing;
	//printf("\ntempo:%f\n; ",timing);

	tAtivas->fin = cAnt;
	timeRastreamento += (omp_get_wtime()-time);

	char idPrint[5] = "0000";
	sprintf(idPrint, "%d",count);
	cvPutText(imgrgbResize, idPrint, cvPoint(20, 15), &font, cvScalar(255, 255, 255, 0));
	//cvNamedWindow( "video", 1 );
	//cvShowImage( "video", imgrgbResize);
	IplImage* plateImageMark =NULL;
	if (plateImage!=NULL){
		plateImageMark = cvCloneImage(recog->getImprovedImage());	
		resultShow->printSegmentation(recog->getCaracteres(), plateImageMark);
	}
	resultShow->setImages(imgrgbResize,tracked,plateImage,plateImageMark);
	resultShow->show();

	/* NaoSalvar
	//Save on Video
	cvWriteFrame(writer, imgrgbResize);
	char filename[50];
	sprintf(filename, "img/frame%d.png",count);
	//cvSaveImage(filename,imgrgbResize);
	// quit if user press 'q' and wait a bit between images
	*/			

	//cvReleaseImage(&imgrgb1);
	cvResize(imgrgb3,imgrgbResize, CV_INTER_LINEAR);
	cvCopy(img2Resize, img1Resize,NULL);
	cvCopy(img3Resize, img2Resize,NULL);
	timingTotal = omp_get_wtime() - timingTotal;
	tempoGasto+=timingTotal;
	//printf("%d\n; ",count);
	cvReleaseImage(&tracked);
	key = cvWaitKey( 1 );
	}
	// release camera and clean up resources when "q" is pressed

	return 0;
}

