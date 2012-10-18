
#include <cv.h>
#include <highgui.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

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

short getFocus(IplImage *image)
{
	IplImage* dst = 0;
	IplImage* dst2 = 0;
	
	dst = cvCreateImage(cvGetSize(image), IPL_DEPTH_16S, image->nChannels);
	dst2 = cvCreateImage(cvGetSize(image), image->depth, image->nChannels);

	assert(image != 0);
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

	cvReleaseImage(&dst);
	cvReleaseImage(&dst2);

	return maxLap;
}

int captureCam(void)
{
	cvNamedWindow("capture", CV_WINDOW_AUTOSIZE);
	CvCapture* capture = cvCreateCameraCapture(CV_CAP_ANY);
	assert(capture);
	double width = cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH);
        double height = cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT);
        printf("[i] %.0f x %.0f\n", width, height);
	IplImage* frame = 0;
	while(true){
		frame = cvQueryFrame(capture);
		cvShowImage("capture", frame);
                getFocus(frame);
		//cvWaitKey(0);
        }
        cvReleaseCapture(&capture);
	cvDestroyWindow("capture");

	return 0;
}

main(int argc, char* argv[])
{
	captureCam();

	return 0;
}
