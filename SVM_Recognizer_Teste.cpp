#include <iostream>
#include <fstream>
#include <windows.h>
#include "cv.h"
#include "highgui.h"
#include <ml.h>

#include <cstdio>
#include <vector>


using namespace std;
using namespace cv;


///PrepareData Original - Sendo usado atualmente (27/03/2013)
//void prepareData(char *path, CvMat** data){
//		int IMAGE_PER_CLASSE=3;
//	int CLASSES = 26;
//	int NORM_WIDTH = 15;
//	int NORM_HEIGHT = 30;
//	int FEATURE_COUTING= NORM_WIDTH * NORM_HEIGHT;
//				*data = cvCreateMat( 1, FEATURE_COUTING, CV_32F );
//				CvMat* symbol =  cvLoadImageM(path,CV_LOAD_IMAGE_GRAYSCALE);					
//
//				CvMat* resized = cvCreateMat(NORM_HEIGHT, NORM_WIDTH,symbol->type);
//				cvResize(symbol,resized);
//                //resize(symbol,resized,Size(NORM_WIDTH,NORM_HEIGHT));
//				//resized.reshape(1,NORM_WIDTH*NORM_HEIGHT);
//				CvMat row_header, row1;
//				CvMat *reshaped = cvReshape(resized, &row_header, 0, 1 );
//				
//				//cvGetRow(data[0],&row1,n_sample);
//				CvMat* reshaped_c = cvCreateMat(1, reshaped->cols, data[0]->type);
//				cvConvertScale(reshaped,reshaped_c);
//				
//				//row1 = cvReshape(resized, &row_header, 0, 1 );
//				
//				cvShowImage("Resized",reshaped_c);
//				//cvvWaitKey("0");
//				
//				//cvCopy(reshaped_c,&row1);
//				cvCopy(reshaped_c,data[0]);
//
//				//cvSetData(data[0][n_sample],cvReshape(resized, &row_header, 0, 1 ),row1->step);
//
//				//cvSet2D(responses[0],n_sample,0,cvScalar(i));
//}

CvSVM* svm;

int const MAX_IMAGE_CLASS = 30;
int const TOTAL_CLASSES = 26;
int vetorUsados[TOTAL_CLASSES][MAX_IMAGE_CLASS];
int confusionMatrix[TOTAL_CLASSES][TOTAL_CLASSES];
const int TOTAL_FEATURES = 320;

const int START_ASCII = 65;

IplImage* formatDataTrainingTeste(IplImage* symbol){
		//int IMAGE_PER_CLASSE=8;
	//int CLASSES = 26;
	int NORM_WIDTH = 16;
	int NORM_HEIGHT = 20;
	//int FEATURE_COUTING= NORM_WIDTH * NORM_HEIGHT;
				//*data = cvCreateMat( 1, FEATURE_COUTING, CV_32F );
				//CvMat* symbol =  cvLoadImageM(path,CV_LOAD_IMAGE_GRAYSCALE);					
								
				double fatHeigth = (double)NORM_HEIGHT/symbol->height;
				double fatWidth = (double)NORM_WIDTH/symbol->width;
				double adjustFat = fatHeigth<fatWidth? fatHeigth:fatWidth;
				//CvMat* adjustResized = cvCreateMat(symbol->height*adjustFat, NORM_WIDTH*adjustFat,symbol->type);
				IplImage* adjustResized = cvCreateImage (cvSize(symbol->width*adjustFat,symbol->height*adjustFat), symbol->depth,symbol->nChannels);
				
				cvSaveImage("train/teste_symbol.png",symbol);
				cvResize(symbol,adjustResized);
				cvSaveImage("train/teste_adjustResized.png",adjustResized);

				//CvMat* normResized = cvCreateMat(NORM_HEIGHT, NORM_WIDTH,symbol->type);
				IplImage* normResized = cvCreateImage (cvSize(NORM_WIDTH,NORM_HEIGHT), symbol->depth,symbol->nChannels);
				cvSet(normResized,cvScalar(0));
				//cvResize(adjustResized,normResized);
				cvSetImageROI(normResized,cvRect((NORM_WIDTH-adjustResized->width + 1)/2,(NORM_HEIGHT-adjustResized->height + 1)/2,adjustResized->width,adjustResized->height));
				cvCopy(adjustResized,normResized);				
				cvResetImageROI(normResized);
				cvSaveImage("train/teste_normResized.png",normResized);
				//normResized->widthStep=NORM_WIDTH;
				cvReleaseImage(&adjustResized);
				//int FEATURE_WIDTH=5;
				//int FEATURE_HEIGHT=8;
				//int N_SUM_WIDTH=3;//Number of rows to sum in the feature
				//int N_SUM_HEIGHT=4;//Number of cols to sum in the feature

				//IplImage* finalFeature = cvCreateImage (cvSize(FEATURE_WIDTH*FEATURE_HEIGHT,1), symbol->depth,symbol->nChannels);
				//int contw=0;
				//int conth=0;
				//for(int i = 0;i<NORM_WIDTH;i+=N_SUM_WIDTH){
				//	//conth=0;
				//	for(int j = 0;j<NORM_HEIGHT;j+=N_SUM_HEIGHT){
				//		cvSetImageROI(normResized,cvRect(i,j,N_SUM_WIDTH,N_SUM_HEIGHT));
				//		double sum = cvSum(normResized).val[0]/255;
				//		//double sum = ((cvSum(normResized).val[0]/255)/(N_SUM_WIDTH*N_SUM_HEIGHT)*2)-1 ;//Normalized
				//		cvSet2D(finalFeature,0,conth,cvScalar(sum));
				//		conth++;
				//	}
				//	contw++;
				//}
				//cvReleaseImage(&normResized);
				//return finalFeature;
				return normResized;
				//CvMat* reshaped_c = cvCreateMat(1, reshaped->cols, data[0]->type);
				//cvConvertScale(reshaped,reshaped_c);				
				
				//cvShowImage("Resized",reshaped_c);
				//cvCopy(reshaped_c,data[0]);
}

static int
read_num_class_data( CvMat** data, CvMat** responses, int nSamples )
{
	int IMAGE_PER_CLASSE=nSamples;
	
	int NORM_WIDTH = 16;
	int NORM_HEIGHT = 20;
	int FEATURE_COUTING = NORM_WIDTH * NORM_HEIGHT;
	char caracteres[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	//char caracteres[] = "BCEFGHIJKLMNOPRVX";//17 CLASSES
	//char caracteres[] = "BEGHLMNOV";
	int CLASSES = sizeof(caracteres)/sizeof(char)-1;
	int NUM_IMAGES =nSamples * CLASSES;
	//int FEATURES = NORM_WIDTH*NORM_HEIGHT;
	int var_count = TOTAL_FEATURES;
    *data = cvCreateMat( NUM_IMAGES, var_count, CV_32F );
    *responses = cvCreateMat( NUM_IMAGES, 1, CV_32F );	
	//char caracteres[] = "0123456789";
	//char caracteres[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	
	//printf("New group\n");
	
   
	for(int j =0;j<IMAGE_PER_CLASSE;j++){
		for(int i =0;i<CLASSES;i++){  
				int n_sample = j*CLASSES+i;
				//printf("%d,",n_sample);
				char path[1024];				
				if(i==16 && j==4){
					int debug=1;
				}
				int nImage;
				int posicao;
				do{
					nImage= rand()% (MAX_IMAGE_CLASS);
					posicao=(j*CLASSES*IMAGE_PER_CLASSE)+nImage;
				}while(vetorUsados[i][nImage]!=0);
				//printf("setting=%d,%d or %d = %d\n",j,i,posicao,nImage);
				vetorUsados[i][nImage]=1;
				if(n_sample==17)
					int debug=1;
				sprintf(path, "div/%c/%d.png",caracteres[i], nImage);
				//printf("%c/%d - ",caracteres[i], nImage);
				IplImage* symbol =  cvLoadImage(path,CV_LOAD_IMAGE_GRAYSCALE);
				CvMat row_header, row1;
				IplImage* prepared = formatDataTrainingTeste(symbol);
				char pathTrain[1024];
				sprintf(pathTrain, "train/%c_%d.png",caracteres[i], nImage);
				cvSaveImage(pathTrain,prepared);




				CvMat *reshaped = cvReshape(prepared, &row_header, 0,1);
				//for(int i = 0;i<reshaped->cols;i++){
				//	cvSet2D(reshaped,0,i,cvScalar(cvGet2D(reshaped,0,1).val[0] *2 -1));
				//}

				
				cvGetRow(data[0],&row1,n_sample);
				CvMat* reshaped_c = cvCreateMat(1, reshaped->width, row1.type);
				cvConvertScale(reshaped,reshaped_c);
				//cvShowImage("Resized",reshaped_c);				
				cvCopy(reshaped_c,&row1);
				cvSet2D(responses[0],n_sample,0,cvScalar(i));
				cvReleaseImage(&prepared);
				//cvReleaseMat(&reshaped);
				cvReleaseMat(&reshaped_c);
				cvReleaseImage(&symbol);

				//cvShowImage("Image",data[0]);
				//cvWaitKey(1);
        }
       
	}
    return 1;
}

static boolean isTrueResult(float f1, float f2){
	if (f1==f2) return true;
	//68,81,79
	f1+=65;
	f2+=65;
	if ((f1==68 || f1==81 || f1==79) && (f2==68 || f2==81 || f2==79)) 
		return true;
	return false;

}


static
void build_svm_classifier(CvMat* data, CvMat* responses )
{

    CvMat train_data;
    int nsamples_all = 0, ntrain_samples = 0;
    int var_count;
    	

    ////////// SVM parameters ///////////////////////////////
    CvSVMParams param;	
	//param.kernel_type=CvSVM::LINEAR;
    //param.svm_type=CvSVM::C_SVC;
    //param.C=1;
	//param.gamma=0.001;
	//param.degree=2;
	//param.gamma=0.002250;
	//param.C=1;
	

	///*Melhores resultados linear, 300 features
	param.kernel_type=CvSVM::LINEAR;	
    //param.svm_type=CvSVM::C_SVC;
    //param.C=10;
	//param.gamma=0.001;
	//param.degree=2;//*/	
    ///////////////////////////////////////////////////////////

    //printf( "The database %s is loaded.\n", data_filename );
    nsamples_all = data->rows;
    ntrain_samples = (int)(nsamples_all*1);
    var_count = data->cols;

    // train classifier
    //printf( "Training the classifier (may take a few minutes)...\n");
    cvGetRows( data, &train_data, 0, ntrain_samples );
    CvMat* train_resp = cvCreateMat( ntrain_samples, 1, CV_32FC1);
    for (int i = 0; i < ntrain_samples; i++){
        train_resp->data.fl[i] = responses->data.fl[i];
		//printf("Class for %d = %f\n",i,responses->data.fl[i]);
	}
    //svm->train(&train_data, train_resp, 0, 0, param);
	svm->train_auto(&train_data, train_resp, 0, 0, param);
	cvReleaseMat(&train_resp);

	//svm.save("SVM-TESTE-2.svm");    
	//return svm;
}

float teste_svm_classifier( CvMat* data,CvMat* responses)
{
	// classification
	unsigned int tempo = time(0);
	srand(tempo);
	//int var_count = 300;
	int var_count = TOTAL_FEATURES;
	int nsamples = data->rows;
    std::vector<float> _sample(var_count * (nsamples));
    CvMat sample = cvMat( nsamples, var_count, CV_32FC1, &_sample[0] );
    std::vector<float> true_results(nsamples);
    for (int j = 0; j < nsamples; j++)
    {
        float *s = data->data.fl + j * var_count;
        
        for (int i = 0; i < var_count; i++)
        {   
            sample.data.fl[(j) * var_count + i] = s[i];
        }
        true_results[j] = responses->data.fl[j];
    }
    CvMat *result = cvCreateMat(1, nsamples, CV_32FC1);
    
    //printf("Classification (may take a few minutes)...\n");
    svm->predict(&sample, result);
    int true_resp = 0;
    for (int i = 0; i < nsamples; i++)
    {
		int esperado = (int)(true_results[i]);
		int encontrado= (int)(result->data.fl[i]);
		if (isTrueResult(result->data.fl[i], true_results[i])){			
            true_resp++;
		}else{
			confusionMatrix[esperado][encontrado]++;
			//printf("*");
		}
		//printf("Item=%.2f, found=%.2f\n", true_results[i], result->data.fl[i]);
		//printf("Item=%c, found=%c\n", (int)true_results[i] + 65, (int)result->data.fl[i] + 65);//Letras
		//printf("Item=%c, found=%c\n", (int)true_results[i] + 48, (int)result->data.fl[i] + 48);//Numeros
    }
    
    //printf("true_resp = %f%%\n", (float)true_resp / (nsamples) * 100);
    
    //cvReleaseMat( &train_resp );
    cvReleaseMat( &result );
    //cvReleaseMat( &data );
    //cvReleaseMat( &responses );

    return (float)true_resp / (nsamples);
}




int testSVM()
//int main()
{
	std::ofstream outfile;
	outfile.open("testeSVM.csv");
	
    //char* data_filename = default_data_filename;
    int method = 0;
	int N_SAMPLES = 25;
	int TAMANHO_TEST = 5;
	int QUANT_TEST = 20;
	svm = new CvSVM();
	for(;N_SAMPLES<=(MAX_IMAGE_CLASS-TAMANHO_TEST);N_SAMPLES+=5){
			for(int i =0;i<TOTAL_CLASSES;i++){
				for(int j =0;j<TOTAL_CLASSES;j++){
					confusionMatrix[i][j]=0;
				}
			}
		for(int j=1;j<=QUANT_TEST;j++){
			CvMat* data = 0;
			CvMat* responses = 0;
			//vetorUsados = (int*)malloc(sizeof(int)*30*17);
			for(int i =0;i<MAX_IMAGE_CLASS;i++){
				for(int j =0;j<TOTAL_CLASSES;j++){
					vetorUsados[j][i]=0;
				}
			}
			svm->clear();
			int ok = read_num_class_data( &data, &responses,N_SAMPLES );
			if( !ok )
			{
				printf( "Could not load the images" );
				return -1;
			}
			build_svm_classifier(data, responses);
			cvReleaseMat(&data);
			cvReleaseMat(&responses);
			data = 0;
			responses = 0;
			ok = read_num_class_data( &data, &responses,TAMANHO_TEST );
	
			//printf("Result = %.2f",teste_svm_classifier(data,responses) * 100);
			//printf("%.4f;",teste_svm_classifier(data,responses) * 100);
			
			double finalResult = teste_svm_classifier(data,responses) * 100;

			outfile << (int) finalResult;
			outfile << ",";
			outfile <<  (int)((finalResult - (double)((int)finalResult))*1000);
			outfile << ";";
			//vfprintf(outfile,"%d;",finalResult);
			
			
			printf("Amostras: %d, iteracao: %d,metodo: %d, gama = %f, C = %.2f\n",N_SAMPLES,j, svm->get_params().kernel_type, svm->get_params().gamma,svm->get_params().C);
			printf("Result: %.2f\n", finalResult);
			//free(vetorUsados);
			cvReleaseMat(&data);
			cvReleaseMat(&responses);

		}
		//printf("\n");
		outfile << "\n";	
		printf("  ");
		for(int i=0;i<26;i++) printf("    %c",i+START_ASCII);
		for(int i =0;i<TOTAL_CLASSES;i++){
			printf("\n%c-",i+START_ASCII);
			for(int j =0;j<TOTAL_CLASSES;j++){
				printf ("%5d",confusionMatrix[i][j]);
			}
		}
		printf("\n");
	}
	outfile.close();
	system("pause");
    return 0;
}