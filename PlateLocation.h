struct TPlate{
	int x;
	int y;
	int height;
	int width;
};
 typedef struct TPlate Plate;

 Plate getPlate(IplImage* myimage);
 Plate vlpl(IplImage* myimage);
 void showPlate(IplImage* myImage, int xWindow, int yWindow, int height, int width);
 IplImage* cropImage(IplImage* myImage, int xWindow, int yWindow, int height, int width);
 IplImage* cropImage(IplImage* myImage, Plate p);
 IplImage* cropImage(IplImage* myImage, int xWindow, int yWindow, int height, int width, double cutLeft, double cutRight, double cutUp, double cutDown);