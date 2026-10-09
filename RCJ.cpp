#include "header.h"

State state = line_following;
bool wait_for_disappear = false;
Point currentExit;
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
    findContours(frame,contours,RETR_EXTERNAL,CHAIN_APPROX_SIMPLE);
    if(wait_for_disappear==true){
		findContours(frame,contours,RETR_EXTERNAL,CHAIN_APPROX_SIMPLE);
		if(contours.size()>0){
		vector<Point> c = *max_element(contours.begin(), contours.end(), contour_compare);
		if(contourArea(c)>1000){
			continue;
		}
		else{
			wait_for_disappear = false;
		}
		}
		else{
			wait_for_disappear = false;
		}
	}
    vector<Point> exits;

    exits = getExits(frame);

    for (Point p : exits) {
      circle(display, p, 5, Scalar(255, 255, 255), 7, LINE_8, 0);
    }

    if (state == line_following || state == double_green ||
        state == left_green || state == right_green) {
      state = findGreen(green_cont, display, frame);
    }

    if (state == line_following||state==gap) {
      state = checkGap(exits);
    }
    
    if(state== move_forward){
		if(exits.size()>0){
			state = line_following;
		}
	}
	
	auto s = check_message();
	if(s){
		cout<<"obstacle"<<endl;
		if(s.value()==obstacle){
			cout<<"OOooObbyyy"<<endl;
		}
		state = obstacle;
	}
	
    switch (state) {
    case line_following: {
      currentExit = chooseExit(exits, framewhite, display, previousentrance,
                               previousexit);
      previousexit = currentExit;
      circle(display, currentExit, 5, Scalar(0, 0, 255), 7, LINE_8, 0);
      int LS, RS;
      speed_calc(LS, RS, currentExit);
      cout << "RS:" << RS << "  LS" << LS << endl;
      sendSpeed(4, LS, RS);
      break;
    }
    case left_green: {
      cout << "left green" << endl;
      currentExit = chooseLeft(exits, framewhite, display, previousentrance,
                               previousexit);
      putText(display, "Left", Point(50, 50), FONT_HERSHEY_SIMPLEX, 1.2,
              Scalar(0, 0, 255), 2, LINE_AA);
      circle(display, currentExit, 5, Scalar(0, 0, 255), 7, LINE_8, 0);
      int LS, RS;
      speed_calc(LS, RS, currentExit);
      cout << "RS:" << RS << "  LS" << LS << endl;
      sendSpeed(4, LS, RS);
      break;
    }
    case right_green: {
      cout << "right green" << endl;
      currentExit = chooseRight(exits, framewhite, display, previousentrance,
                                previousexit);
      putText(display, "Right", Point(50, 50), FONT_HERSHEY_SIMPLEX, 1.2,
              Scalar(0, 0, 255), 2, LINE_AA);
      circle(display, currentExit, 5, Scalar(0, 0, 255), 7, LINE_8, 0);
      int LS, RS;
      speed_calc(LS, RS, currentExit);
      cout << "RS:" << RS << "  LS" << LS << endl;
      sendSpeed(4, LS, RS);

      break;
    }
    case double_green: {
      cout << "double green" << endl;
      putText(display, "Double", Point(50, 50), FONT_HERSHEY_SIMPLEX, 1.2,
              Scalar(0, 0, 255), 2, LINE_AA);

      imshow("display", display);
      int key = waitKey(1);
      if (key == 'q') {
        break;
      }
      sendSpeed(3, 0, 0);
      waitForcytron();
      state = line_following;
      break;
    }
    case gap: {
      cout << "gap" << endl;
      currentExit = getGapexit(frame, exits.at(0));
      circle(display, currentExit, 5, Scalar(0, 0, 255), 7, LINE_8, 0);
      double gap_dist = norm(currentExit-exits.at(0));
      if (gap_dist > 125) {
        int LS, RS;
        speed_calc(LS, RS, currentExit);
        cout << "RS:" << RS << "  LS" << LS << endl;
        sendSpeed(4, LS, RS);
        break;
      }
    }
    case alignment: {
		
      cout << "Alignment" << endl;
      double ang = getAngle(currentExit,exits.at(0));
      cout << "ang:" << ang << endl;

      cout<<"sended"<<endl;

      
      if(ang > 0){
		  if(abs(ang>30))
		  ang -= (ang*0.15);
		  sendSpeed(1, ang, 1);
	  }
	  else{
		  if(abs(ang>30))
		  ang += (ang*0.15);
		  sendSpeed(1,ang,-90);
		}
      waitForcytron();
      state = move_forward;
      break;
    }
    case move_forward:{
		sendSpeed(4,45,45);
		break;
	}
	case obstacle:{
		while(true){
			auto side = check_message();
			if(side){
			if(side.value() == left_obstacle){
				state = left_obstacle;
				wait_for_disappear = true;
				break;
			}
			if(side.value() == right_obstacle){
				
				state = right_obstacle;
				wait_for_disappear = true;
				break;
			}
		}
		}
		
		break;
	}
	case left_obstacle:{
		cout<<"left obby"<<endl;
		putText(display, "Searching right", Point(50, 50), FONT_HERSHEY_SIMPLEX, 1.2,
              Scalar(0, 0, 255), 2, LINE_AA);
              if(check_right(frame,display) == true){
			cout<<"Line found"<<endl;
			sendSpeed(99,0,0);
			while(1);
			
		}
		break;
	}
	case right_obstacle:{
		cout<<"right obby"<<endl;
		putText(display, "Searching left", Point(50, 50), FONT_HERSHEY_SIMPLEX, 1.2,
              Scalar(0, 0, 255), 2, LINE_AA);
		if(check_left(frame,display) == true){
			cout<<"Line found"<<endl;
			sendSpeed(99,0,0);
			while(1);
		}
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
    if(key == ' '){
		while(key==' '){
			sendSpeed(4,0,0);
			this_thread::sleep_for(1000ms);
			key = waitKey(1);
		}
	}
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
