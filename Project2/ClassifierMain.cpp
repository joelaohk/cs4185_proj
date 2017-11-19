/*
*	ClassifierMain.cpp 
*	Experimental file for CS4185 Project
*	Created by LAO Choi Hin (54045244)
*
*/

/************************************************************************************************************
*	To Markers:																								*
*	1.	This file is created just for experiment,															*
*		it should not be considered as part of the project submission.										*
*		But you may use it to see how I came up with the approaches used in the official submission.		*
*																											*
*	2.	This file may contains code from other sources and eventually applied in the official submission.	*
*		Refereces to those sources have been made in the report.											*
*																											*
*	Thank you for your kind attention and understanding.													*
************************************************************************************************************/

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