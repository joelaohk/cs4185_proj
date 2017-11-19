/**
* CS4185/CS5185 Multimedia Technologies and Applications
* Course Assignment
* Image Retrieval Task
*
* Program completed by LAO Choi Hin (54045244)
*/

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <algorithm>

using namespace std;
using namespace cv;

#define IMAGE_folder ".\\dataset"     // change to your folder location
#define IMAGE_LIST_FILE "dataset1"         //the dataset1 for retrieval
#define output_LIST_FILE "searchResults"  //the search results will store in this file
#define SEARCH_IMAGE "999.jpg"   //change from 990 to 999 as the search images to get your output

/**
* @function main
*/

//Compute pixel-by-pixel difference
double compareImgs(Mat img1, Mat img2)
{
	int w = img1.cols, h = img1.rows;
	Mat new_img2;
	resize(img2, new_img2, img1.size());
	double sum = 0;
	for (int i = 0; i < w; i++)for (int j = 0; j < h; j++)
	{
		sum += abs(img1.at<uchar>(j, i) - new_img2.at<uchar>(j, i));
	}
	return sum;
}

void errorMsg() {
	printf("The input image is not found, please make sure:\n");
	printf("\t 1.\tYou put the dataset folder next to the this program\n");
	printf("\t 2.\tEvery image file is contained in its specified folder (0-999 in dataset1, 1-10 in dataset2, football.png in dataset)\n");
}

//Mat calcHisto(Mat &hsl_src) {
//	Mat hist;
//	int histSize = 256;
//	int channel[] = { 1 };
//	float range[] = { 0, 255 };
//	const float* histRange = { range };
//
//	calcHist(&hsl_src, 1, channel, Mat(), hist, 1, &histSize, &histRange, true, false);
//	return hist;
//}

int main(int argc, char** argv)
{
	Mat src_input, gray_input, hsl_input;
	Mat db_img, db_gray_img;

	const int filename_len = 900;
	char tempname[filename_len];

	const int db_size = 1000;
	int db_id = 0;

	const int score_size = 10;   //change this to control return top n images
	double minscore[score_size] = { DBL_MAX };
	int minFilename[score_size];

	char minimg_name[filename_len];
	Mat min_img;

	int query;
	cout << "Please enter the number of the query image (990-999): ";
	cin >> query;
	while (query < 990 || query > 999) {
		cout << "Your query number is not correct, please enter a number between 990 and 999, now try again" << endl;
		cout << "Please enter the number of the query image (990-999): ";
		cin >> query;
	}
	
	sprintf_s(tempname, filename_len, "%s\\%s\\%d.jpg", IMAGE_folder, IMAGE_LIST_FILE, query);
	src_input = imread(tempname); // read input image
	if (!src_input.data)
	{
		errorMsg();
		system("pause");
		return -1;
	}
	printf("Your query image is %d, now searching for similar images...\n", query);
	imshow("Input", src_input);
	cvtColor(src_input, gray_input, COLOR_BGR2GRAY); // Convert to grayscale
	GaussianBlur(gray_input, gray_input, Size(9, 9), 2, 2);	//Gaussian blur to remove noise for Hough Circle Transform
	vector<Vec3f> circles;

	/// Apply the Hough Transform to find the circles
	double def = 1.25;
	int centerDist = 900;
	int param1 = 200;
	int param2 = 60;

	HoughCircles(gray_input, circles, CV_HOUGH_GRADIENT, def, centerDist, param1, param2, 0, 0);
	//cout << circles.size() << endl;
	//double football_bw_ratio = 0;
	if (circles.size() > 0) {
		
		int radius = cvRound(circles[0][2]);
		int upLeftX = max(0, cvRound(circles[0][0] - radius));
		int upLeftY = max(0, cvRound(circles[0][1] - radius));
		int downRightX = min(src_input.cols, cvRound(circles[0][0] + radius));
		int downRightY = min(src_input.rows, cvRound(circles[0][1] + radius));
		Rect roi = Rect(upLeftX, upLeftY, downRightX - upLeftX, downRightY - upLeftY);
		hsl_input = src_input(roi).clone();
		cvtColor(hsl_input, hsl_input, CV_BGR2HSV);

		////calculate HSL histogram from src
		//Mat football_hist = calcHisto(hsl_input);
		////get black-to-white ratio
		//double b_sum = 0, w_sum = 0;
		//for (int i = 0; i < 3; i++) {
		//	b_sum += football_hist.at<float>(i);
		//	w_sum += football_hist.at<float>(245 + i);
		//}
		////ignore background white
		//double football_bg = (roi.width * roi.height) - CV_PI * pow(((double)roi.height) / 2, 2);
		////cout << football_bg << endl;
		//football_bw_ratio = b_sum / (w_sum - football_bg);
		//cout << football_bw_ratio << endl;
	}
	else {
		printf("No football was found in this picture");
		return -4;
	}
	

													 //Read Database
	//int countX = 0;
	for (db_id; db_id < db_size; db_id++) {
		sprintf_s(tempname, filename_len, "%s\\%s\\%d.jpg", IMAGE_folder, IMAGE_LIST_FILE, db_id);
		db_img = imread(tempname); // read database image
		if (!db_img.data)
		{
			errorMsg();
			system("pause");
			return -1;
		}
		cvtColor(db_img, db_gray_img, COLOR_BGR2GRAY); // Convert to grayscale
		GaussianBlur(db_gray_img, db_gray_img, Size(9, 9), 2, 2);	//Gaussian blur to remove noise for Hough Circle Transform

		vector<Vec3f> circles;

		// Hough Circle Transform
		double def = 1.25;
		int centerDist = 900;
		int param1 = 210;
		int param2 = 60;
		HoughCircles(db_gray_img, circles, CV_HOUGH_GRADIENT, def, centerDist, param1, param2, 0, 0);

		//Mat min_bw_ratio_circle;
		double bw_ratio_dist_min = DBL_MAX;
		if (circles.size() > 0) {
			//countX++;
			//cout << db_id << "\t" << circles.size() << endl;
			Mat srcCir = db_img.clone();									// for debugging

																			//calculate score for each candidate
			for (size_t i = 0; i < circles.size(); i++)
			{

				//Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
				int radius = cvRound(circles[i][2]);
				// circle center
				//circle(srcCir, center, 3, Scalar(0, 255, 0), -1, 8, 0);		// for debugging
																			// circumference
				//circle(srcCir, center, radius, Scalar(0, 0, 255), 3, 8, 0);	// for debugging

																			//prevent accessing vertices outside the img
				int upLeftX = max(0, cvRound(circles[i][0] - radius));
				int upLeftY = max(0, cvRound(circles[i][1] - radius));
				int downRightX = min(db_img.cols, cvRound(circles[i][0] + radius));
				int downRightY = min(db_img.rows, cvRound(circles[i][1] + radius));
				//printf("%d\t%d\t%d\t%d\t%d\t%d\n", upLeftX, upLeftY, downRightX, downRightY, src.rows, src.cols);
				Rect roi = Rect(upLeftX, upLeftY, downRightX - upLeftX, downRightY - upLeftY);
				//cout << roi.size().width << "\t" << roi.size().height << endl;
				Mat roi_hsl = db_img(roi).clone();

				//roi_hsl.convertTo(roi_hsl, CV_32F);
				cvtColor(roi_hsl, roi_hsl, CV_BGR2HSV);
				//imshow(tempname, roi_hsl);
				/*Mat hist = calcHisto(roi_hsl);

				double b_sum = 0, w_sum = 0;
				for (int i = 0; i < 3; i++) {
					b_sum += hist.at<float>(i);
					w_sum += hist.at<float>(245 + i);
				}
				double bg = (roi_hsl.cols * roi_hsl.rows) - CV_PI * pow(((double)roi_hsl.cols) / 2, 2);

				double bw_ratio = b_sum / (w_sum - bg);
				if (db_id == 990) cout << bw_ratio;*/
				double ratio_dist = compareImgs(hsl_input, roi_hsl);//abs(football_bw_ratio - bw_ratio); //use the distance of BW ratio as score
				//if (db_id >= 990 && db_id < 1000) cout << ratio_dist << endl;
				bw_ratio_dist_min = min(bw_ratio_dist_min, ratio_dist);

			}
		}
		
		double tempScore = bw_ratio_dist_min;
		//store the top k min score ascending
		for (int k = 0; k < score_size; k++) {
			if (tempScore < minscore[k]) {
				for (int k1 = score_size - 1; k1 > k; k1--) {
					minscore[k1] = minscore[k1 - 1];
					minFilename[k1] = minFilename[k1 - 1];
				}
				minscore[k] = tempScore;
				minFilename[k] = db_id;
				break;
			}
		}

		

	}
	//cout << countX << endl;

	//read the top k max score image and write them to the a designated folder
	for (int k = 0; k < score_size; k++) {
		sprintf_s(minimg_name, filename_len, "%s\\%s\\%d.jpg", IMAGE_folder, IMAGE_LIST_FILE, minFilename[k]);
		min_img = imread(minimg_name);
		printf("the most similar image %d is %d.jpg\n", k + 1, minFilename[k], minscore[k]);
		sprintf_s(tempname, filename_len, "%s\\%s\\%d.jpg", IMAGE_folder, output_LIST_FILE, minFilename[k]);
		imwrite(tempname, min_img);
		//imshow(tempname,max_img);
	}

	//output your precesion and recall (the ground truth are from 990 to 999)
	int count = 0;
	for (int k = 0; k<score_size; k++) {
		if (minFilename[k] >= 990 && minFilename[k] <= 999) {
			count++;
		}
	}
	double precision = (double)count / score_size;
	double recall = (double)count / 10;

	printf("the precision and the recall for %d is %.2f and %.2f.\n", query, precision, recall);

	printf("Done \n");

	// Wait for the user to press a key in the GUI window.
	//Press ESC to quit
	int keyValue = 0;
	while (keyValue >= 0)
	{
		keyValue = cvWaitKey(0);

		switch (keyValue)
		{
		case 27:keyValue = -1;
			break;
		}
	}

	return 0;
}
