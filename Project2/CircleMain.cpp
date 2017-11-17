#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>

using namespace cv;
using namespace std;

#define IMAGE_folder "C:\\Users\\Joe\\Desktop\\dataset"     // change to your folder location
#define IMAGE_LIST_FILE "dataset2"         //the dataset1 for retrieval
#define output_LIST_FILE "searchResults"  //the search results will store in this file
#define SEARCH_IMAGE "football.png"   //change from 990 to 999 as the search images to get your output

void drawHistImg(const Mat &src, Mat &dst) {
	int histSize = 256;
	float histMaxValue = 0;
	for (int i = 0; i<histSize; i++) {
		float tempValue = src.at<float>(i);
		if (histMaxValue < tempValue) {
			histMaxValue = tempValue;
		}
	}

	float scale = (0.9 * 256) / histMaxValue;
	for (int i = 0; i<histSize; i++) {
		int intensity = static_cast<int>(src.at<float>(i)*scale);
		line(dst, Point(i, 255), Point(i, 255 - intensity), Scalar(0));
	}
}

Mat calcHisto(Mat &hsl_src) {
	Mat hist;
	int histSize = 256;
	int channel[] = { 1 };
	float range[] = { 0, 255 };
	const float* histRange = { range };

	calcHist(&hsl_src, 1, channel, Mat(), hist, 1, &histSize, &histRange, true, false);
	return hist;
}


/** @function main */
int main(int argc, char** argv)
{

	const int filename_len = 900;
	char tempname[filename_len];

	sprintf_s(tempname, filename_len, "%s\\%s", IMAGE_folder, SEARCH_IMAGE);
	Mat football, footballHSL;
	football = imread(tempname);
	cvtColor(football, footballHSL, CV_BGR2HLS);
	Mat football_hist = calcHisto(footballHSL);

	double b_sum = 0, w_sum = 0;
	for (int i = 0; i < 5; i++) {
		b_sum += football_hist.at<float>(i);
		w_sum += football_hist.at<float>(245 + i);
	}
	double football_bg = (football.cols * football.rows) - CV_PI * pow(((double) football.cols)/2, 2);
	cout << football_bg << endl;
	double football_bw_ratio = b_sum / (w_sum-football_bg);

	int db_id = 1;
	const int db_size = 10;
	for (db_id; db_id <= db_size; db_id++) {
		sprintf_s(tempname, filename_len, "%s\\%s\\%d.jpg", IMAGE_folder, IMAGE_LIST_FILE, db_id);
		Mat src, src_gray;

		/// Read the image
		src = imread(tempname);

		if (!src.data)
		{
			cout << "No img" << endl;
			return -1;
		}

		/// Convert it to gray
		cvtColor(src, src_gray, CV_BGR2GRAY);

		/// Reduce the noise so we avoid false circle detection
		GaussianBlur(src_gray, src_gray, Size(9, 9), 2, 2);

		vector<Vec3f> circles;

		/// Apply the Hough Transform to find the circles
		double def = 1.5;
		int centerDist = 100;
		int param1 = 130;
		int param2 = 50;

		HoughCircles(src_gray, circles, CV_HOUGH_GRADIENT, def, centerDist, param1, param2, 0, 0);

		cout << db_id << "\t" << circles.size() << endl;

		if (circles.size() > 0) {
			//cout << db_id << "\t" << circles.size() << endl;
			Mat srcCir = src.clone();
			double bw_ratio_dist_min = DBL_MAX;
			Mat min_bw_ratio_hist, min_bw_ratio_circle;
			for (size_t i = 0; i < circles.size(); i++)
			{
				
				Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
				int radius = cvRound(circles[i][2]);
				// circle center
				circle(srcCir, center, 3, Scalar(0, 255, 0), -1, 8, 0);
				// circle outline
				circle(srcCir, center, radius, Scalar(0, 0, 255), 3, 8, 0);

				//prevent accessing vertices outside the img
				int upLeftX = max(0, cvRound(circles[i][0] - radius));
				int upLeftY = max(0, cvRound(circles[i][1] - radius));
				int downRightX = min(src.cols, cvRound(circles[i][0] + radius));
				int downRightY = min(src.rows, cvRound(circles[i][1] + radius));
				//printf("%d\t%d\t%d\t%d\t%d\t%d\n", upLeftX, upLeftY, downRightX, downRightY, src.rows, src.cols);
				Rect roi = Rect(upLeftX, upLeftY, downRightX - upLeftX, downRightY - upLeftY);
				//cout << roi.size().width << "\t" << roi.size().height << endl;
				Mat roi_hsl = src(roi).clone();
				
				//roi_hsl.convertTo(roi_hsl, CV_32F);
				cvtColor(roi_hsl, roi_hsl, CV_BGR2HLS);
				//imshow(tempname, roi_hsl);
				Mat hist;
				int histSize = 256;
				int channel[] = { 1 };
				float range[] = { 0, 255 };
				const float* histRange = { range };
				calcHist(&roi_hsl, 1, channel, Mat(), hist, 1, &histSize, &histRange, true, false);

				double b_sum = 0, w_sum = 0;
				for (int i = 0; i < 5; i++) {
					b_sum += hist.at<float>(i);
					w_sum += hist.at<float>(245 + i);
				}
				double bg = (roi_hsl.cols * roi_hsl.rows) - CV_PI * pow(((double)roi_hsl.cols) / 2, 2);

				Mat showHistImg(100, 100, CV_8UC1, Scalar(255));
				drawHistImg(hist, showHistImg);
				
				double bw_ratio = b_sum / w_sum;
				cout << bw_ratio << endl;
				if (abs(bw_ratio - football_bw_ratio) < bw_ratio_dist_min) {
					bw_ratio_dist_min = bw_ratio;
					min_bw_ratio_hist = showHistImg;
					min_bw_ratio_circle = roi_hsl;
				}
				

			}
			//printf("%d", bw_ratio_dist_min);
			//printf("%d\t%d", min_bw_ratio_circle.size().width, min_bw_ratio_circle.size().height);
			//imshow(tempname, min_bw_ratio_circle);
			
			imshow(tempname, srcCir);
		}
		
	}
	cv::waitKey(0);
	
	return 0;
}