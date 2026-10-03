#include "header.h"
Point previousentrance(200, 400);
Point previousexit(200, 0);



State state = line_following;

int main() {

  //{ setting up camera
  init();
  lccv::PiCamera frontcam;

  frontcam.options->camera = 0;
  frontcam.options->video_width = 100;
  frontcam.options->video_height = 100;
  frontcam.options->framerate = 30;

  lccv::PiCamera cam;

  cam.options->camera = 1;
  cam.options->video_width = 1640;
  cam.options->video_height = 1232;
  cam.options->framerate = 30;
  cam.options->verbose = false;

  cam.startVideo();
  frontcam.startVideo();
  //}
  Mat frontframe;
  Mat frame;
  Mat kernel = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));
//{ setting up recording
#ifdef DEBUG
  int fourcc = cv::VideoWriter::fourcc('M', 'J', 'P', 'G');
  cv::VideoWriter writer("output_video2.avi", fourcc, 20,
                         cv::Size(1640 / 4, 1232 / 4));

  if (!writer.isOpened()) {
    std::cerr << "Error: Could not open the video writer file for writing."
              << std::endl;
    return -1;
  }
  cv::VideoWriter thresh("thresh_video2.avi", fourcc, 20,
                         cv::Size(1640 / 4, 1232 / 4), false);
  if (!thresh.isOpened()) {
    std::cerr << "Error: Could not open the video writer file for writing."
              << std::endl;
    return -1;
  }

  char filename[] = "/home/pi/RCJ.txt";
  FILE *file = fopen(filename, "w");
  if (!file) {
    cout << "file not opened" << endl;
    while (true)
      ;
  }
#endif
  //}

  waitForcytron();

  while (true) {

    if (!cam.getVideoFrame(frame, 1000)) {
      cout << "CAM ERROR" << endl;
      break;
    }
    if (frame.empty()) {
      cout << "Frame empty" << endl;
      break;
    }

    resize(frame, frame, Size(frame.cols / 4, frame.rows / 4));

    /*
    Mat mask = cv::Mat::zeros(frame.size(), CV_8UC1);
    vector<Point> triangle_points;
    triangle_points.push_back(cv::Point(0,0));
    triangle_points.push_back(cv::Point(0,80));
    triangle_points.push_back(cv::Point(80, 0));
        vector<Point> other_triangle_points;
        other_triangle_points.push_back(cv::Point(frame.cols,0));
    other_triangle_points.push_back(cv::Point(frame.cols,80));
    other_triangle_points.push_back(cv::Point(frame.cols-80, 0));

    vector<vector<Point>> tri = { triangle_points,other_triangle_points };

    fillPoly(frame, tri, Scalar(255, 255, 255));
    */

    Mat display = frame.clone();
    Mat green = frame.clone();
    Mat framewhite = frame.clone();

    setupImg(frame, framewhite, green);

    vector<vector<Point>> contours, green_cont;
    findContours(green, green_cont, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    vector<Point> exits;

    exits = getExits(frame);

    for (Point p : exits) {
      circle(display, p, 5, Scalar(255, 255, 255), 7, LINE_8, 0);
    }

    if (state == line_following||state == double_green||state == left_green||state == right_green) {
      state = findGreen(green_cont, display,frame);
    }


    switch (state) {
    case line_following: {
      Point exit = chooseExit(exits, framewhite, display, previousentrance,
                              previousexit);
      previousexit = exit;
      circle(display, exit, 5, Scalar(0, 0, 255), 7, LINE_8, 0);
		int LS,RS;
     speed_calc(LS,RS,exit);
      cout << "RS:" << RS << "  LS" << LS << endl;
      sendSpeed(4, LS, RS);
      break;
    }
    case left_green: {
      cout << "left green" << endl;
      Point exit = chooseLeft(exits, framewhite, display, previousentrance,previousexit);
       circle(display, exit, 5, Scalar(0, 0, 255), 7, LINE_8, 0);
       int LS,RS;
      speed_calc(LS,RS,exit);
      cout << "RS:" << RS << "  LS" << LS << endl;
       sendSpeed(4, LS, RS);
      break;
    }
    case right_green: {
      cout << "right green" << endl;
      Point exit = chooseRight(exits, framewhite, display, previousentrance,previousexit);
       circle(display, exit, 5, Scalar(0, 0, 255), 7, LINE_8, 0);
       int LS,RS;
      speed_calc(LS,RS,exit);
      cout << "RS:" << RS << "  LS" << LS << endl;
       sendSpeed(4, LS, RS);
      
      break;
    }
    case double_green: {
      cout << "double green" << endl;
      break;
    }

    default: {
    }
    }
    imshow("display", display);
    imshow("frame", frame);
    imshow("green", green);
    imshow("framewhite", framewhite);
    writer.write(display);
    thresh.write(frame);
    int key = waitKey(1);
    if (key == 'q')
      break;
  }

  sendSpeed(0, 0, 0);
#ifdef DEBUG
  writer.release();
  thresh.release();
#endif
  destroyAllWindows();
  cam.stopVideo();

  return 0;
}
