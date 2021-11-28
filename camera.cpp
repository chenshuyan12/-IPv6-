#include <opencv2/highgui/highgui.hpp>
#include <opencv/cv.h>
#include <opencv2/opencv.hpp>

#include <iostream>

using namespace std;

using namespace cv;

int main(int, char**)
{
	VideoCapture cap(0); // open the default camera
	/*
	cap.set(CV_CAP_PROP_FRAME_WIDTH, 800);
	cap.set(CV_CAP_PROP_FRAME_HEIGHT,800);
	*/
	if (!cap.isOpened())  // check if we succeeded
		return -1;

	while (1)
	{
		Mat frame;
		cap >> frame; // get a new frame from camera
		if (frame.empty())
		{
			cout << "the image is NULL" << endl;
			//break;
		}
		else
		{
			imshow("VideoCapture Basic Demo", frame);
		}
		

		if (waitKey(30) >= 0) break;
	}
	// the camera will be deinitialized automatically in VideoCapture destructor
	return 0;
}