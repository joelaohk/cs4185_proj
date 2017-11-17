#include <iostream>
#include <stdio.h>
#include "opencv2/objdetect/objdetect.hpp";
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace std;
using namespace cv;

String cascadeName = "C:\\Users\\Joe\\Desktop\\train\\cascade.xml";
CascadeClassifier ballDetector;

int main() {
	Mat src, srcgray;

	if (!ballDetector.load(cascadeName)) {
		printf("Classifier not found!\n");
		return -1;
	}

	src = imread("C:\\Users\\Joe\\Desktop\\dataset\\dataset1\\998.jpg");
	if (!src.data)
	{
		printf("Cannot find src");
		return -1;
	}

	cvtColor(src, srcgray, CV_BGR2GRAY);

	vector<Rect> balls;
	ballDetector.detectMultiScale(src, balls, 1.1);

	if (balls.size() == 0) printf("No balls\n"); 

	for (size_t i = 0; i < balls.size(); i++) {
		rectangle(src, balls[i], Scalar(255,0,0));
	}

	imshow("found", src);
	waitKey(0);
	return 0;
}