#pragma once

#include "opencv2/opencv.hpp"
#include <algorithm>
#include <chrono>
#include <cstdarg>
#include <fcntl.h>
#include <lccv.hpp>
#include <math.h>
#include <opencv2/ximgproc.hpp>
#include <sstream>
#include <stdlib.h>
#include <string.h>
#include <sys/signal.h>
#include <termios.h>
#include <thread>
#include <vector>
using namespace std::chrono_literals;
using namespace std;
using namespace cv;
using namespace ximgproc;

enum State {
  line_following,
  double_green,
  left_green,
  right_green,
  obstacle,
  gap,
  end,
  find_alive_victim,
  find_green_zone,
  find_dead_victim,
  find_dead_zone,
  find_evac_exit,
  alignment,move_forward
};

#define DEBUG
#define BAUDRATE B115200
#define MODEMDEVICE "/dev/ttyAMA0"

inline Point previousentrance((1640 / 4) / 2, 1232 / 4);
inline Point previousexit((1640 / 4) / 2, 0);

void sendSpeed(int condition, int LS, int RS);

void waitForcytron();

void init();

void setupImg(Mat &frame, Mat &framewhite, Mat &green);

bool contour_compare(const vector<Point> &a, const vector<Point> &b);

vector<Point> getExits(Mat frame);

Point getIntersection(Mat framewhite, Mat &display);

Point chooseExit(vector<Point> exits, Mat framewhite, Mat &display,
                 Point &previousentrance, Point &previousexit);

double getAngle(Point exit, Point mid_point);

State findGreen(vector<vector<Point>> green_cont, Mat &display, Mat frame);

Point chooseLeft(vector<Point> exits, Mat framewhite, Mat &display,
                 Point &previousentrance, Point &previousexit);

Point chooseRight(vector<Point> exits, Mat framewhite, Mat &display,
                  Point &previousentrance, Point &previousexit);

void speed_calc(int &LS, int &RS, Point exit);

State checkGap(vector<Point> exits);

Point getGapexit(Mat frame, Point entrance);
