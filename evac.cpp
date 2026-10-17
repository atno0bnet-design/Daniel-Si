#include "header.h"








Mat get_image(lccv::PiCamera &c, int width, int height){
Mat img;
if(!c.getVideoFrame(img,1000)){
cout<<"cam Timeout error " << endl;          
    }
   
    resize(img, img, Size(width, height));
    return img;
}





