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

IplImage* formatDataTraining(IplImage* symbol){
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

static int
read_num_class_data( const char* filename, int var_count, CvMat** data, CvMat** responses )
{
		int IMAGE_PER_CLASSE=30;
	//int CLASSES = 26;
	int CLASSES = 10;
	int NORM_WIDTH = 16;
	int NORM_HEIGHT = 20;
	int FEATURE_COUTING= NORM_WIDTH * NORM_HEIGHT;


    const int M = 1024;
    FILE* f = fopen( filename, "rt" );
    CvMemStorage* storage;
    CvSeq* seq;
    char buf[M+2];
    float* el_ptr;
    //CvSeqReader reader;

	int NUM_IMAGES =IMAGE_PER_CLASSE * CLASSES;
	int FEATURES = NORM_WIDTH*NORM_HEIGHT;
	var_count = NORM_WIDTH*NORM_HEIGHT;
    *data = cvCreateMat( NUM_IMAGES, var_count, CV_32F );
	//*data = cvCreateMat( NUM_IMAGES, var_count, CV_8U );
    *responses = cvCreateMat( NUM_IMAGES, 1, CV_32F );
	


	char caracteres[] = "0123456789";
	//char caracteres[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	for(int j =0;j<IMAGE_PER_CLASSE;j++){
	//for(int j =IMAGE_PER_CLASSE-1;j>=0;j--){
	for(int i =0;i<CLASSES;i++){        				
				int n_sample = j*CLASSES+i;
				if (n_sample==124)
					int debug=0;
				//int n_sample = (IMAGE_PER_CLASSE-j-1)*CLASSES+i;
				printf("%d,",n_sample);
				//printf(">>>Caractere = %d, valor = %c\n", j,caracteres[i]);
				//printf(">>>i = %d, j = %d\n", i,j);
				if(j ==10 && i==0){
					int debug=0;
				}
				char path[1024];
				sprintf(path, "div/%c/%d.png",caracteres[i], j);
                //Mat symbol = imread(path);
				//CvMat* symbol =  cvLoadImageM(path,CV_LOAD_IMAGE_GRAYSCALE);					
				IplImage* symbol =  cvLoadImage(path,CV_LOAD_IMAGE_GRAYSCALE);

				CvMat row_header, row1;
				IplImage* prepared = formatDataTraining(symbol);
				
				//CvMat *reshaped = cvReshape(prepared, &row_header, 1);
				
				CvMat *reshaped = cvReshape(prepared, &row_header, 0,1);				
				
				cvGetRow(data[0],&row1,n_sample);
				CvMat* reshaped_c = cvCreateMat(1, reshaped->width, row1.type);
				cvConvertScale(reshaped,reshaped_c);
				
				//row1 = cvReshape(resized, &row_header, 0, 1 );
				
				cvShowImage("Resized",reshaped_c);
				
				cvCopy(reshaped_c,&row1);

				//cvSetData(data[0][n_sample],cvReshape(resized, &row_header, 0, 1 ),row1->step);

				cvSet2D(responses[0],n_sample,0,cvScalar(i));
				//cvSet2D(data[0],i,j,cvScalar(255));
				cvShowImage("Image",data[0]);
				cvWaitKey(1);
        }
       
        }
    return 1;
}

static boolean isTrueResult(float f1, float f2){
	if (f1==f2) return true;
	//68,81,79
	f1+=65;
	f2+=65;
	if ((f1==68 || f1==81 || f1==79)&&(f2==68 || f2==81 || f2==79)) return true;
	return false;

}

static
int build_svm_classifier( char* data_filename )
{
    CvMat* data = 0;
    CvMat* responses = 0;
    CvMat train_data;
    int nsamples_all = 0, ntrain_samples = 0;
    int var_count;
    CvSVM svm;

	//CvSVM *pSvm = new CvSVM();
	//pSvm->load("SVM.svm", 0);



    int ok = read_num_class_data( data_filename, 16, &data, &responses );
    if( !ok )
    {
        printf( "Could not read the database %s\n", data_filename );
        return -1;
    }
    ////////// SVM parameters ///////////////////////////////
    CvSVMParams param;
    param.kernel_type=CvSVM::LINEAR;
    //param.svm_type=CvSVM::C_SVC;
    //param.C=1;
    ///////////////////////////////////////////////////////////

    printf( "The database %s is loaded.\n", data_filename );
    nsamples_all = data->rows;
    ntrain_samples = (int)(nsamples_all*0.9);
    var_count = data->cols;

    // train classifier
    printf( "Training the classifier (may take a few minutes)...\n");
    cvGetRows( data, &train_data, 0, ntrain_samples );
    CvMat* train_resp = cvCreateMat( ntrain_samples, 1, CV_32FC1);
    for (int i = 0; i < ntrain_samples; i++){
        train_resp->data.fl[i] = responses->data.fl[i];
		//printf("Class for %d = %f\n",i,responses->data.fl[i]);
	}
	svm.train_auto (&train_data, train_resp, 0, 0, param);
	//svm.save("SVM-TESTE-30 Letras.svm");
	svm.save("SVM-TESTE-Digitos.svm");
    // classification
    std::vector<float> _sample(var_count * (nsamples_all - ntrain_samples));
    CvMat sample = cvMat( nsamples_all - ntrain_samples, var_count, CV_32FC1, &_sample[0] );
    std::vector<float> true_results(nsamples_all - ntrain_samples);
    for (int j = ntrain_samples; j < nsamples_all; j++)
    {
        float *s = data->data.fl + j * var_count;
        
        for (int i = 0; i < var_count; i++)
        {   
            sample.data.fl[(j - ntrain_samples) * var_count + i] = s[i];
        }
        true_results[j - ntrain_samples] = responses->data.fl[j];
    }
    CvMat *result = cvCreateMat(1, nsamples_all - ntrain_samples, CV_32FC1);
    
    printf("Classification (may take a few minutes)...\n");
    svm.predict(&sample, result);

    int true_resp = 0;
    for (int i = 0; i < nsamples_all - ntrain_samples; i++)
    {        
		if (isTrueResult(result->data.fl[i], true_results[i])){
            true_resp++;
		}else{
			printf("*");
		}
		//printf("Item=%.2f, found=%.2f\n", true_results[i], result->data.fl[i]);
		printf("Item=%c, found=%c\n", (int)true_results[i] + 65, (int)result->data.fl[i] + 65);//Letras
		//printf("Item=%c, found=%c\n", (int)true_results[i] + 48, (int)result->data.fl[i] + 48);//Numeros

    }
    
    printf("true_resp = %f%%\n", (float)true_resp / (nsamples_all - ntrain_samples) * 100);
    
    cvReleaseMat( &train_resp );
    cvReleaseMat( &result );
    cvReleaseMat( &data );
    cvReleaseMat( &responses );

    return 0;
}




int testMLP()
//int main( int argc, char *argv[] )
{
    char* filename_to_save = "FileToSave";
    char* filename_to_load = 0;
    char default_data_filename[] = "./letter-recognition.data";
    char* data_filename = default_data_filename;
    int method = 0;  

	//char txt[100];
	//printf("Digite o nome da imagem: ");
	//scanf("%s", txt);
	//printf("\nNome da imagem eh %s",txt);
	//CvSVM pSvm;// = new CvSVM();
	//pSvm.load("SVM.svm", 0);



    //build_mlp_classifier( data_filename, filename_to_save, filename_to_load );
	build_svm_classifier(data_filename);
	system("pause");
    return 0;
}