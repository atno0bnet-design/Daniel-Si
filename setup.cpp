#include "header.h"
int uart0_filestream = -2;



void init(){
	uart0_filestream = open(MODEMDEVICE, O_RDWR| O_NOCTTY|O_NDELAY);
	if(uart0_filestream == -1){
		printf("Unable to open UART\n");
		exit(-1);
	}
	struct termios options;
	tcgetattr(uart0_filestream, &options);
	options.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
	options.c_iflag = IGNPAR;
	options.c_oflag = 0;
	options.c_lflag = 0;
	tcflush(uart0_filestream, TCIFLUSH);
	tcsetattr(uart0_filestream, TCSANOW, &options);
}

void waitForcytron(){
	int rx_length = 0;
	char rx_buff[255];
	while(rx_length <= 0){
		rx_length = read(uart0_filestream,(void*)rx_buff,255);
	}
}


void sendSpeed(int condition, int LS, int RS){
	char buff[255];
	sprintf(buff, "[%d,%d,%d]", condition, LS, RS);
	
	
	//cout<<buff<<endl;
	int count = write(uart0_filestream,&buff[0],strlen(buff));
	
	if(count < 0){
		printf("Error sending the message");		
	}

	
}


void setupImg(Mat& frame, Mat& framewhite, Mat& green){
	
	blur(frame, frame, Size(3, 3));
	 cvtColor(frame, frame, COLOR_BGR2GRAY);
	 cvtColor(framewhite, framewhite, COLOR_BGR2GRAY);
    cvtColor(green, green, COLOR_BGR2HSV);

    // home threshold for green

     inRange(green, Scalar(92, 101, 49), Scalar(113, 255, 220), green);
    // other threshold for green
     //inRange(green, Scalar(29, 107, 106), Scalar(86, 255,185), green);
	
    threshold(framewhite, framewhite, 110, 255, THRESH_BINARY);
    Mat kernel = getStructuringElement(MORPH_RECT, Size(40, 40));
   // morphologyEx(framewhite, framewhite, MORPH_CLOSE, kernel);

    threshold(frame, frame, 60, 255, THRESH_BINARY_INV);
    morphologyEx(frame, frame, MORPH_CLOSE, kernel);
    //morphologyEx(frontframe, frontframe, MORPH_CLOSE, kernel);
    morphologyEx(green, green, MORPH_CLOSE, kernel);
}



bool contour_compare(const vector<Point> &a,const vector<Point> &b){
	return contourArea(a)<contourArea(b);
}


void speed_calc(int& LS, int& RS, Point exit){

	double ka = 3;
	int default_speed = 45;
	if(ramp_condition == upr||ramp_condition == leftr||ramp_condition==rightr){
		cout<<"ramp"<<endl;
		ka = 0.8;
		default_speed = 45;
	}
	if(ramp_condition==downr){
		default_speed = 20;
		ka = 0.8;
	}
	if(ramp_condition == upr){
		default_speed = 50;
	}
	
	
	 int ang = getAngle(exit, Point((1640 / 4)/2, (1232 / 4)/2));

      int adjust = ang * ka;

      LS = default_speed + adjust;
      RS = default_speed - adjust;
     
	if(ramp_condition == leftr){
		LS+=5;
	}
	if(ramp_condition == rightr){


		RS+=5;
	}

      // Slow BOTH motors based on angle
      double speedScale = 1.0 - abs(ang) / 90.0;

      speedScale = max(speedScale, 0.3);

      LS *= speedScale;
      RS *= speedScale;
      LS = clamp(LS, -100, 100);
      RS = clamp(RS, -100, 100);
}


optional<State> check_message(){
	char rx_buff[255];
	int rx_length = read(uart0_filestream,(void*)rx_buff,255);
	ramp_condition = flat;
	if(rx_length!=0){
		cout<<"message detected"<<endl;
		switch(rx_buff[0]){
			case 'o':{
				return obstacle;
				break;
			}
			case 'l':{
				return left_obstacle;
				break;
			}
			case 'r':{
				return right_obstacle;
				break;
			}
			case 'u':{
				ramp_condition = upr;

				break;
			}
			case 'q':{
				ramp_condition = rightr;
				break;
			}
			case 'w':{
					ramp_condition = leftr;
					break;
			}
			case 'd':{
				ramp_condition = downr;
				break;
			}
			default:{
				return nullopt;
			}
		}
	}
	else{
		return nullopt;
	}
	
	
	return nullopt;
}



string stateName(State s) {
    switch (s) {
        case line_following: return "line_following";
        case double_green:   return "double_green";
        case left_green:     return "left_green";
        case right_green:    return "right_green";
        case gap:             return "gap";
        case alignment:       return "alignment";
        case front_trace:     return "front_trace";
        case obstacle:        return "obstacle";
        case left_obstacle:   return "left_obstacle";
        case right_obstacle:  return "right_obstacle";
        case move_forward:    return "move_forward";
        case down_ramp:       return "down_ramp";
        default:              return "unknown";
    }
}
