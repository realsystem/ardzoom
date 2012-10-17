//
// ������ ������ ��������� ������� - cvLaplace()
//
// robocraft.ru
//

#include <cv.h>
#include <highgui.h>
#include <stdlib.h>
#include <stdio.h>

IplImage* image = 0;
IplImage* dst = 0;
IplImage* dst2 = 0;

int main(int argc, char* argv[])
{
	// ��� �������� ������� ������ ����������
	char* filename = "image0.jpg";

	if (argv[1]) filename = argv[1];
	// �������� ��������
	image = cvLoadImage(filename, 1);
	// ������ ��������
	dst = cvCreateImage( cvGetSize(image), IPL_DEPTH_16S, image->nChannels);
	dst2 = cvCreateImage( cvGetSize(image), image->depth, image->nChannels);

	printf("[i] image: %s\n", filename);
	assert( image != 0 );

	// ���� ��� ����������� ��������
	cvNamedWindow("original", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("cvLaplace", CV_WINDOW_AUTOSIZE);

	int aperture = argc == 3 ? atoi(argv[2]) : 3;

	// ��������� �������� �������
	cvLaplace(image, dst, aperture);
	short maxLap = -32767;
	short* imgData = (short*)dst->imageData;
	for(int i =0;i<(dst->imageSize/2);i++)
	{
		if(imgData[i] > maxLap) maxLap = imgData[i];
	}
	printf("maxLap=%d\n", maxLap);

	// ����������� ����������� � 8-�������
	cvConvertScale(dst, dst2);

	// ���������� ��������
	cvShowImage("original", image);
	cvShowImage("cvLaplace", dst2);

	cvWaitKey(0);

	// ����������� �������
	cvReleaseImage(&image);
	cvReleaseImage(&dst);
	cvReleaseImage(&dst2);
	// ������� ����
	cvDestroyAllWindows();
	return 0;
}
