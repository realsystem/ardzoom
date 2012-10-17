
#include <cv.h>
#include <highgui.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

IplImage* image = 0;
IplImage* dst = 0;
IplImage* dst2 = 0;

char message[] = "Hello there!\n";
char buf[sizeof(message)];

int send_cmd(char *cmd, int port)
{
    int sock;
    struct sockaddr_in addr;

    if (! cmd) return -1;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0)
    {
        perror("socket");
        exit(1);
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    if(connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("connect");
        exit(2);
    }
    printf("sending %s\n", cmd);
    send(sock, cmd, sizeof(cmd), 0);
    recv(sock, buf, sizeof(cmd), 0);

    printf("received %s\n", buf);
    close(sock);

    return 0;
}

short getFocus(char *filename)
{
	if (! filename) return -1;
	image = cvLoadImage(filename, 1);
	dst = cvCreateImage( cvGetSize(image), IPL_DEPTH_16S, image->nChannels);
	dst2 = cvCreateImage( cvGetSize(image), image->depth, image->nChannels);

	printf("[i] image: %s\n", filename);
	assert( image != 0 );

	cvNamedWindow("original", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("cvLaplace", CV_WINDOW_AUTOSIZE);

	int aperture = 3;

	cvLaplace(image, dst, aperture);
	short maxLap = -32767;
	short* imgData = (short*)dst->imageData;
	for(int i =0;i<(dst->imageSize/2);i++)
	{
		if(imgData[i] > maxLap) maxLap = imgData[i];
	}
	printf("maxLap=%d\n", maxLap);

	//cvConvertScale(dst, dst2);

	//cvShowImage("original", image);
	//cvShowImage("cvLaplace", dst2);

	//cvWaitKey(0);

	cvReleaseImage(&image);
	cvReleaseImage(&dst);
	cvReleaseImage(&dst2);
	cvDestroyAllWindows();
	return maxLap;
}

int main(int argc, char* argv[])
{
	short tmpLap, curLap;
	int i;

	if (argc <= 0) return -1;
	for (i=1; i<argc; i++)
	{
		tmpLap = getFocus(argv[i]);
		if (tmpLap < 900) send_cmd("focusIn", 3425);
		if (tmpLap > curLap) curLap = tmpLap;
	}
	printf("actually maximum Lap=%d\n", curLap);

	return 0;
}
