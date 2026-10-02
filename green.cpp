#include "header.h"



State findGreen(vector<vector<Point>> green_cont, Mat& display,Mat frame){
		
		int leftc = 0;
		int rightc = 0;

		if(green_cont.size()>0){
			for(int i = 0;i<(int)green_cont.size();i++){
				double cy;

				if(contourArea(green_cont[i])>=2000){
					Moments m = moments(green_cont[i]);
					cy = m.m01 / m.m00;

					cout<<"cy: "<<cy<<endl;
				}
				
	
				if(contourArea(green_cont[i])>3000&&cy>150){
					/*#ifdef DEBUG
					drawContours(display,vector<vector<Point>>(1,green_cont[i]),0,Scalar(0,255,0),3);
					#endif
					* */
					Point2f points[4];
					RotatedRect square = minAreaRect(green_cont[i]);
					square.points(points);
					
					
					
					
					vector<Point2f> pointsVec(points, points + 4);

	
					sort(pointsVec.begin(), pointsVec.end(), [](const cv::Point2f& a, const cv::Point2f& b) {
						return a.y < b.y;
					});

    
					Point2f top1 = pointsVec[0];
					Point2f top2 = pointsVec[1];
					
					sort(pointsVec.begin(), pointsVec.end(), [](const cv::Point2f& a, const cv::Point2f& b) {
						return a.x < b.x;
					});
					
					Point2f left1 = pointsVec[0];
					Point2f left2 = pointsVec[1];
					
					
					sort(pointsVec.begin(), pointsVec.end(), [](const cv::Point2f& a, const cv::Point2f& b) {
						return a.x > b.x;
					});
					
					Point2f right1 = pointsVec[0];
					Point2f right2 = pointsVec[1];
					
					
					Point2f midpoint = (top1 + top2) * 0.5f;
					midpoint.y-=15;
					
					Point2f midL = (left1 + left2) * 0.5f;
					midL.x-=15;
	
					Point2f midR = (right1 + right2) * 0.5f;
					midR.x+=15;
			
					
					int color = frame.at<unsigned char>(midpoint);
					cout<<color<<endl;
				
					if(color==255){
						
	
						color = frame.at<unsigned char>(midL);
						if(color==255){
							#ifdef DEBUG
							cout<<"right"<<endl;
							#endif
							rightc++;
						}
						else{
							#ifdef DEBUG
							cout<<"left"<<endl;
							#endif
							leftc++;
						}
					}
					else{
						#ifdef DEBUG
						cout<<"false"<<endl;
						#endif
					}
					
					
					
				}
			}
		}
		
		
		if(green_cont.size()>0){
			for(int i = 0;i<(int)green_cont.size();i++){

	
				if(contourArea(green_cont[i])>2000){
					drawContours(display,vector<vector<Point>>(1,green_cont[i]),0,Scalar(0,255,0),3);
					/*#ifdef DEBUG
					drawContours(display,vector<vector<Point>>(1,green_cont[i]),0,Scalar(0,255,0),3);
					#endif
					* */
					Point2f points[4];
					RotatedRect square = minAreaRect(green_cont[i]);
					square.points(points);
					
					
					
					
					vector<Point2f> pointsVec(points, points + 4);

	
					sort(pointsVec.begin(), pointsVec.end(), [](const cv::Point2f& a, const cv::Point2f& b) {
						return a.y < b.y;
					});

    
					Point2f top1 = pointsVec[0];
					Point2f top2 = pointsVec[1];
					
					sort(pointsVec.begin(), pointsVec.end(), [](const cv::Point2f& a, const cv::Point2f& b) {
						return a.x < b.x;
					});
					
					Point2f left1 = pointsVec[0];
					Point2f left2 = pointsVec[1];
					
					
					sort(pointsVec.begin(), pointsVec.end(), [](const cv::Point2f& a, const cv::Point2f& b) {
						return a.x > b.x;
					});
					
					Point2f right1 = pointsVec[0];
					Point2f right2 = pointsVec[1];
					
					
					Point2f midpoint = (top1 + top2) * 0.5f;
					midpoint.y-=15;
				
					Point2f midL = (left1 + left2) * 0.5f;
					midL.x-=15;

					Point2f midR = (right1 + right2) * 0.5f;
					midR.x+=15;
	
					
				
					
					int color = frame.at<unsigned char>(midpoint);
					cout<<color<<endl;
				
					if(color==255){
						
	
						color = frame.at<unsigned char>(midL);
						if(color==255){
							#ifdef DEBUG
							
							#endif
							rightc++;
						}
						else{
							#ifdef DEBUG
							
							#endif
							leftc++;
						}
					}
					else{
						#ifdef DEBUG
						cout<<"false"<<endl;
						#endif
					}
					
					
					
				}
			}
		}
        
		if(leftc==2&&rightc==2){
			cout<<"double"<<endl;
			return double_green;
		}
		else if(leftc==2){
			cout<<"left"<<endl;
			return left_green;
		}
		else if(rightc==2){
			cout<<"right"<<endl;
			return right_green;
	
		}
		else{
			return line_following;
		}
		
		
}



Point chooseLeft(vector<Point> exits,Mat framewhite,Mat& display,Point& previousentrance, Point& previousexit){
	cout<<"choosing left exit"<<endl;
	Point entrance, exit;
	if(exits.size()==2){
		double entrancedist1 = norm(exits.at(0) - previousentrance);
    double entrancedist2 = norm(exits.at(1) - previousentrance);
    if (entrancedist1 < entrancedist2) {
      entrance = exits.at(0);
      exit = exits.at(1);
    } else {
      entrance = exits.at(1);
      exit = exits.at(0);
    }
    return exit;
	}
	if(exits.size()>2){
		Point intersection = getIntersection(framewhite, display);
		return intersection;
	}
	
		
	
	return Point(0,framewhite.rows/2);
}
