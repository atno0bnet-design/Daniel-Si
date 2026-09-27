#include "header.h"
Point previousentrance(200,400);
Point previousexit(200,0);
const int default_speed = 45;
double ka = 1.6;
enum State {
  line_following,
  double_green,
  obstacle,
  gap,
  end,
  find_alive_victim,
  find_green_zone,
  find_dead_victim,
  find_dead_zone,
  find_evac_exit
};

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
    cout << "running" << endl;
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
    
    vector<Point> exits;
    
    exits = getExits(frame);

    
    for(Point p : exits){
		circle(display,p,5,Scalar(255,255,255),7,LINE_8,0);
	}
    

    Point exit = chooseExit(exits,framewhite,display,previousentrance,previousexit);
    previousexit = exit;
    circle(display,exit,5,Scalar(0,0,255),7,LINE_8,0);
    
    
    
    int ang = getAngle(exit,frame.cols,frame.rows,Point(frame.cols/2,frame.rows/2));

    int adjust = ang*ka;
	int LS = default_speed + adjust;
	int RS = default_speed- adjust;

	LS = clamp(LS, -100,100);
	RS = clamp(RS, -100,100);
    cout<<"RS:" <<RS<<"  LS"<<LS<<endl;
sendSpeed(4,LS,RS);
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
