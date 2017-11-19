/*
*	SiftMain.cpp 
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
#include <opencv2/legacy/legacy.hpp>
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/nonfree/features2d.hpp>

using namespace std;
using namespace cv;

#define IMAGE_folder "C:\\Users\\Joe\\Desktop\\dataset\\"     // change to your folder location
#define IMAGE_LIST_FILE "dataset2"         //the dataset1 for retrieval
#define output_LIST_FILE "searchResults"  //the search results will store in this file
#define SEARCH_IMAGE "football.png"   //change from 990 to 999 as the search images to get your output

int main() {
	Mat src, db;
	
	const int filename_len = 900;
	char tempname[filename_len];

	const int db_size = 10;
	int db_id = 1;

	//sprintf_s(tempname, filename_len, "%s\\%s\\%s", IMAGE_folder, IMAGE_LIST_FILE, SEARCH_IMAGE);
	src = imread("C:\\Users\\Joe\\Desktop\\dataset\\football.png");

	for (db_id; db_id < db_size; db_id++) {
		sprintf_s(tempname, filename_len, "%s\\%s\\%d.jpg", IMAGE_folder, IMAGE_LIST_FILE, db_id);
		db = imread(tempname);
		if (!src.data || !db.data) {
			printf("img not found");
			return -1;
		}

		vector<KeyPoint> key_points1, key_points2;

		Mat descripters1, descriptors2;
		SIFT sift;

		sift(src, Mat(), key_points1, descripters1);
		sift(db, Mat(), key_points2, descriptors2);

		Mat outputSrc, outputDB;

		FlannBasedMatcher matcher;
		vector<DMatch> matches;
		matcher.match(descripters1, descriptors2, matches);

		double max_dist = 0, min_dist = 100;

		for (int i = 0; i < descripters1.rows; i++) {
			double dist = matches[i].distance;
			max_dist = max(dist, max_dist);
			min_dist = min(dist, min_dist);
		}

		if (min_dist < 90) {
			cout << db_id << "\t" << min_dist << "\t" << max_dist << endl;
		}

		printf("--Max dist : %f \n", max_dist);
		printf("--Min dist : %f \n", min_dist);

		vector<DMatch> good_matches;

		for (int i = 0; i < descripters1.rows; i++) {
			if (matches[i].distance <= 3 * min_dist) good_matches.push_back(matches[i]);
		}

		Mat img_matches;
		drawMatches(src, key_points1, db, key_points2, good_matches, img_matches, Scalar::all(-1), Scalar::all(-1), vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

		imshow(tempname, img_matches);
	}

	

	

	waitKey(0); //press any key to quit

	return 0;
}