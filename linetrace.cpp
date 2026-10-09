#include "header.h"

void shiftContourDown(std::vector<cv::Point> &contour, int dy) {
  for (size_t i = 0; i < contour.size(); i++) {
    contour[i].y += dy;
  }
}

vector<Point> getExits(Mat frame) {

  vector<Point> topexits, leftexits, rightexits, bottomexits, exits;
  vector<vector<Point>> contours;
  vector<vector<Point>> topgroups, leftgroups, rightgroups, bottomgroups;
  vector<Point> black_line;

  findContours(frame, contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);

  if (contours.size() > 0) {

    black_line =
        *max_element(contours.begin(), contours.end(), contour_compare);

    if (contourArea(black_line) > 7000) {
      vector<Point> top, bottom, left, right;

      for (Point p : black_line) {
        if (p.x == 0) {
          left.push_back(p);
        }
        if (p.x == frame.cols - 1) {
          right.push_back(p);
        }
        if (p.y == 0) {
          top.push_back(p);
        }
        if (p.y == frame.rows - 1) {
          bottom.push_back(p);
        }
      }

      sort(left.begin(), left.end(),
           [](Point a, Point b) { return a.y > b.y; });
      sort(right.begin(), right.end(),
           [](Point a, Point b) { return a.y < b.y; });
      sort(top.begin(), top.end(), [](Point a, Point b) { return a.x < b.x; });
      sort(bottom.begin(), bottom.end(),
           [](Point a, Point b) { return a.x > b.x; });

      // top

      for (Point p : top) {

        if (topgroups.empty()) {
          topgroups.push_back({p});
          continue;
        }

        vector<Point> &lastGroup = topgroups.back();

        Point lastPoint = lastGroup.back();

        if (abs(p.x - lastPoint.x) <= 10) {
          lastGroup.push_back(p);
        } else {
          topgroups.push_back({p});
        }
      }

      // left

      for (Point p : left) {

        if (leftgroups.empty()) {
          leftgroups.push_back({p});

          continue;
        }

        vector<Point> &lastGroup = leftgroups.back();

        Point lastPoint = lastGroup.back();

        if (abs(p.y - lastPoint.y) <= 10) {
          lastGroup.push_back(p);
        } else {
          leftgroups.push_back({p});
        }
      }

      // right

      for (Point p : right) {

        if (rightgroups.empty()) {
          rightgroups.push_back({p});

          continue;
        }

        vector<Point> &lastGroup = rightgroups.back();

        Point lastPoint = lastGroup.back();

        if (abs(p.y - lastPoint.y) <= 10) {
          lastGroup.push_back(p);
        } else {
          rightgroups.push_back({p});
        }
      }

      // bottom

      for (Point p : bottom) {

        if (bottomgroups.empty()) {
          bottomgroups.push_back({p});

          continue;
        }

        vector<Point> &lastGroup = bottomgroups.back();

        Point lastPoint = lastGroup.back();

        if (abs(p.x - lastPoint.x) <= 10) {
          lastGroup.push_back(p);
        } else {
          bottomgroups.push_back({p});
        }
      }


      // merging groups

      vector<vector<Point>> allgroups;

      for (vector<Point> grup : topgroups) {
        allgroups.push_back(grup);
      }

      for (vector<Point> grup : rightgroups) {
        allgroups.push_back(grup);
      }
      for (vector<Point> grup : bottomgroups) {
        allgroups.push_back(grup);
      }
      for (vector<Point> grup : leftgroups) {
        allgroups.push_back(grup);
      }

      // delete all tiny exits or big exits


      /*
            for (int i = 0; i < (int)allgroups.size(); i++) {
              if (allgroups.at(i).size() <= 50 || allgroups.at(i).size() >= 150)
         { allgroups.erase(allgroups.begin() + i); i--;
              }
            }
             */

      while (1) {

        bool merged = false;
       
        for (int i = 1; i < (int)allgroups.size(); i++) {
			if(allgroups.at(i).size()>150){
				continue;
			}

          int dist = norm(allgroups[i].front() - allgroups[i - 1].back());

          if (dist < 10) {
            allgroups[i - 1].insert(allgroups[i - 1].end(),
                                    allgroups[i].begin(), allgroups[i].end());
            allgroups.erase(allgroups.begin() + i);
            merged = true;
            break;
            // merge
          }
        }

        if (allgroups.size() > 0) {
          cout << allgroups.back().back() << allgroups.front().front() << endl;
          int dist = norm(allgroups.back().back() - allgroups.front().front());
          if (dist < 10) {

            allgroups.back().insert(allgroups.back().end(),
                                    allgroups.front().begin(),
                                    allgroups.front().end());
            allgroups.erase(allgroups.begin());
            merged = true;
          }

          if (merged == false) {
            break;
          }
        } else {
          break;
        }
      }

      for (vector<Point> grups : allgroups) {
        Point midpoint((grups.back().x + grups.front().x) / 2,
                       (grups.back().y + grups.front().y) / 2);
        exits.push_back(midpoint);
      }
    } else {
      exits.push_back(Point(frame.cols / 2, 0));
    }
  }

  return exits;
}

Point getIntersection(Mat framewhite, Mat &display) {

  // return Point(framewhite.cols / 2, framewhite.rows / 2);
  vector<Mat> expandedregion;
  Mat dilationsize = getStructuringElement(MORPH_RECT, Size(171, 171));
  vector<vector<Point>> white_cont;
  findContours(framewhite, white_cont, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

  for (vector<Point> white : white_cont) {

    if (contourArea(white) <= 5000) {
      continue;
    }

    Mat mask = Mat::zeros(framewhite.size(), CV_8UC1);

    drawContours(mask, vector<vector<Point>>(1, white), -1, Scalar(255),
                 FILLED);
    drawContours(display, vector<vector<Point>>(1, white), -1, Scalar(255), 3);

    Mat dilated;
    dilate(mask, dilated, dilationsize);

    expandedregion.push_back(dilated);
  }

  if (expandedregion.size() > 2) {

    Mat overlap = expandedregion.at(0).clone();
    for (int i = 1; i < (int)expandedregion.size(); i++) {
      bitwise_and(overlap, expandedregion.at(i), overlap);
    }
    imshow("Dilated stuff", overlap);

    vector<vector<Point>> c;
    vector<vector<Point>> intersection_cont;
    findContours(overlap, intersection_cont, RETR_EXTERNAL,
                 CHAIN_APPROX_SIMPLE);
    if (!intersection_cont.empty()) {

      vector<Point> biggest_intersection_cont = *max_element(
          intersection_cont.begin(), intersection_cont.end(), contour_compare);
      drawContours(display, vector<vector<Point>>(1, biggest_intersection_cont),
                   -1, Scalar(255, 0, 0), 3);
      Moments m = moments(biggest_intersection_cont);

      Point intersection(m.m10 / (m.m00 + 1e-5), m.m01 / (m.m00 + 1e-5));
      return intersection;
    }
  }
  return Point(framewhite.cols / 2, framewhite.rows / 2);
}

Point chooseExit(vector<Point> exits, Mat framewhite, Mat &display,
                 Point &previousentrance, Point &previousexit) {
  Point exit;
  Point entrance;
	if(exits.size()==1){
		return exits.at(0);
	}
  if (exits.size() > 2) {
    Point intersection = getIntersection(framewhite, display);
    
    double min_entrance_dist = DBL_MAX;
    for (Point p : exits) {
      double dist = norm(p - previousentrance);

      if (dist < min_entrance_dist) {
        min_entrance_dist = dist;
        entrance = p;
      }
    }
    circle(display, intersection, 5, Scalar(0, 255, 0), 5);

    Point2f direction(intersection.x - entrance.x, intersection.y - entrance.y);

    float length = sqrt(direction.x * direction.x + direction.y * direction.y);

    if (length > 0.001) {

      direction /= length;

      float distance = 1000;
      Point far_point(intersection.x + direction.x * distance,
                      intersection.y + direction.y * distance);
      clipLine(display.size(), intersection, far_point);

      double min_exit_dist = DBL_MAX;
      for (Point p : exits) {

        int dist = norm(far_point - p);
        if (dist < min_exit_dist) {
          min_exit_dist = dist;
          exit = p;
        }
      }
    }
    return exit;
  }

  
  if (exits.size() == 2) {
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
  return Point(0, 0);
}

double getAngle(Point exit, Point mid_point) {
    double dx = exit.x - mid_point.x;
    double dy = exit.y - mid_point.y;

    if (dx == 0 && dy == 0)
        return 0.0;

    return atan2(dx, -dy) * 180.0 / M_PI;
}



