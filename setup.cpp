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
	
	
	cout<<buff<<endl;
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
    Mat kernel = getStructuringElement(MORPH_RECT, Size(30, 30));
   // morphologyEx(framewhite, framewhite, MORPH_CLOSE, kernel);

    threshold(frame, frame, 60, 255, THRESH_BINARY_INV);
    morphologyEx(frame, frame, MORPH_CLOSE, kernel);
    morphologyEx(green, green, MORPH_CLOSE, kernel);
}



bool contour_compare(const vector<Point> &a,const vector<Point> &b){
	return contourArea(a)<contourArea(b);
}


void speed_calc(int& LS, int& RS, Point exit){
	double ka = 3;
	
	const int default_speed = 45;
	 int ang = getAngle(exit, Point((1640 / 4)/2, (1232 / 4)/2));

      int adjust = ang * ka;

      LS = default_speed + adjust;
      RS = default_speed - adjust;

      // Slow BOTH motors based on angle
      double speedScale = 1.0 - abs(ang) / 90.0;

      speedScale = max(speedScale, 0.3);

      LS *= speedScale;
      RS *= speedScale;
      LS = clamp(LS, -100, 100);
      RS = clamp(RS, -100, 100);
}






