#pragma once


#include "opencv2/opencv.hpp"
#include <lccv.hpp>
#include <vector>
#include <algorithm>
#include <math.h>
#include <string.h>
#include <thread>
#include <chrono>
#include <termios.h>
#include <fcntl.h>
#include <sys/signal.h>
#include <sstream>
#include <opencv2/ximgproc.hpp>
#include <cstdarg>
#include <stdlib.h>


enum State {
  line_following,
  double_green,left_green,right_green,
  obstacle,
  gap,
  end,
  find_alive_victim,
  find_green_zone,
  find_dead_victim,
  find_dead_zone,
  find_evac_exit
};


#define DEBUG
#define BAUDRATE B115200
#define MODEMDEVICE "/dev/ttyAMA0"




using namespace std::chrono_literals;
using namespace std;
using namespace cv;
using namespace ximgproc;

void sendSpeed(int condition,int LS,int RS);

void waitForcytron();

void init();

void setupImg(Mat& frame, Mat& framewhite, Mat& green);

bool contour_compare(const vector<Point> &a,const vector<Point> &b);

vector<Point> getExits(Mat frame);


Point getIntersection(Mat framewhite,Mat& display);

Point chooseExit(vector<Point> exits,Mat framewhite,Mat& display,Point& previousentrance, Point& previousexit);

double getAngle(Point exit,int col,int row,Point mid_point);

State findGreen(vector<vector<Point>> green_cont, Mat& display, Mat frame);

Point chooseLeft(vector<Point> exits,Mat framewhite,Mat& display,Point& previousentrance, Point& previousexit);



