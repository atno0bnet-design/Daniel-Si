#include "header.h"


State checkGap(vector<Point> exits){
	if(exits.size()>=2){
		return line_following;
	}
	if(exits.size()==1){
		double entrance_dist = norm(previousentrance-exits.at(0));
		double exit_dist = norm(previousexit-exits.at(0));
		
		if(entrance_dist<exit_dist){
			return gap;
		}
	}
	return line_following;
}


Point getGapexit(Mat frame, Point entrance){
	Point exit;
	
	struct Pointdist{
		Point p;
		double dist;
	};
	vector<vector<Point>> contours;
	findContours(frame,contours,RETR_EXTERNAL,CHAIN_APPROX_NONE);
			vector<Point> black_line;

	if(contours.size()>0){
		

		black_line = *max_element(contours.begin(), contours.end(), contour_compare);
		if(contourArea(black_line)>2000){
			
			
			
			vector<Pointdist> all_points;
			for(Point p : black_line){
				all_points.push_back({p,norm(p-entrance)});
			}
			
			sort(all_points.begin(),all_points.end(),[](const Pointdist& a, const Pointdist& b){return a.dist>b.dist;});
			Point2f sum(0.0f,0.0f);
			int five = static_cast<int>(all_points.size()*(0.22));
			for(int i = 0;i<five;i++){
				sum.x +=all_points.at(i).p.x;
				sum.y +=all_points.at(i).p.y;
				
			}
			
			sum.x/=five;
			sum.y/=five;
			cout<<"sum: "<<sum<<endl;
			
			
			exit = sum;
		}
	}
	
	
	
	return exit;
	
	return exit;
}
