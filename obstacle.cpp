#include "header.h"


bool check_left(Mat frame,Mat& display){
	Point topleft(0,frame.rows-70);
	Point bottomright((frame.cols/2)-1,frame.rows-1);
	Rect search_area(topleft,bottomright);
	
	rectangle(display, search_area, Scalar(255, 255, 255), 1);
	Mat slice = frame(search_area);
	vector<vector<Point>> contours;
	findContours(slice,contours,RETR_EXTERNAL,CHAIN_APPROX_SIMPLE);
	if(contours.size()>0){
		vector<Point> c = *max_element(contours.begin(), contours.end(), contour_compare);
		if(contourArea(c)>10000){
			return true;
		}
	}
	
	return false;
}


bool check_right(Mat frame, Mat& display){
	Point topleft(frame.cols/2,frame.rows-70);
	Point bottomright(frame.cols-1,frame.rows-1);
	Rect search_area(topleft,bottomright);
	
	
	rectangle(display, search_area, Scalar(255, 255, 255), 1);
	Mat slice = frame(search_area);
	vector<vector<Point>> contours;
	findContours(slice,contours,RETR_EXTERNAL,CHAIN_APPROX_SIMPLE);
	if(contours.size()>0){
		vector<Point> c = *max_element(contours.begin(), contours.end(), contour_compare);
		if(contourArea(c)>10000){
			return true;
		}
	}
	
	return false;
	
	
	
	return false;
}
