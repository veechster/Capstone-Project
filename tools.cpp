#ifndef TOOLS_CPP
#define TOOLS_CPP

#include "header.h"


Tools::Tools()
{
	debuggingmode = false;
	detectionMethod = 0;
	//initalize the filtering values
	H_MIN = 70;//0;
	H_MAX = 90;//256;
	S_MIN = 40;//0;
	S_MAX = 150;//256;
	V_MIN = 95;//0;
	V_MAX = 216;//256;
	//for houghcircles debugging
	dp = 2;
	min_dist = 100;
	param_1 = 100;
	param_2 = 100 ;
	min_radius = 0;
	max_radius = 200;
}

//draws a crosshair cenetered at (x,y)
void Tools::crosshair(cv::Mat &frame, int x, int y, int r, cv::Vec3i colour,std::string targetName)
{
	if(y<0 || y>FRAME_HEIGHT || x<0 || x>FRAME_WIDTH){std::cout<<"error1";return;}

    cv::line(frame,cv::Point(x+15,y),cv::Point(x-15,y),cv::Scalar(colour[0],colour[1],colour[2]),2);
	cv::line(frame,cv::Point(x,y+15),cv::Point(x,y-15),cv::Scalar(colour[0],colour[1],colour[2]),2);

	if(r==0)
		cv::circle(frame, cv::Point(x,y), cvRound(r), cv::Scalar(colour[0],colour[1],colour[2]), 2);

	cv::putText(frame,targetName,cv::Point(x+2,y+12),cv::FONT_HERSHEY_SIMPLEX,0.35,cv::Scalar(200,200,200),1);
}

void Tools::enabledebugging()
{
	debuggingmode = true;
	if(detectionMethod==0)
	{
		cvStartWindowThread();
		cvNamedWindow("HSV_Image", cv::WINDOW_AUTOSIZE);
		cvNamedWindow("Editted_Image", cv::WINDOW_AUTOSIZE);
		cvNamedWindow("Adjustors",0);
		cvCreateTrackbar( "H_MIN", "Adjustors", &H_MIN, 256, 0 );
		cvCreateTrackbar( "H_MAX", "Adjustors", &H_MAX, 256, 0 );
		cvCreateTrackbar( "S_MIN", "Adjustors", &S_MIN, 256, 0 );
		cvCreateTrackbar( "S_MAX", "Adjustors", &S_MAX, 256, 0 );
		cvCreateTrackbar( "V_MIN", "Adjustors", &V_MIN, 256, 0 );
		cvCreateTrackbar( "V_MAX", "Adjustors", &V_MAX, 256, 0 );
		cvCreateTrackbar( "dp", "Adjustors", &dp, 5, 0 );
		cvCreateTrackbar( "min_dist", "Adjustors", &min_dist, 400, 0 );
		cvCreateTrackbar( "param_1", "Adjustors", &param_1, 300, 0 );
		cvCreateTrackbar( "param_2", "Adjustors", &param_2, 300, 0 );
		cvCreateTrackbar( "min_radius", "Adjustors", &min_radius, 100, 0 );
		cvCreateTrackbar( "max_radius", "Adjustors", &max_radius, 400, 0 );
	}
	else if(detectionMethod==1)
	{
		//code for IR detection debugging
	}
	else
		customDebuggingMenu();
}

void Tools::disabledebugging()
{
	debuggingmode = false;
	if(detectionMethod==0)
	{
		cvDestroyWindow("HSV_Image");
		cvDestroyWindow("Editted_Image");
		cvDestroyWindow("Adjustors");
	}
	else if(detectionMethod==1)
	{
		//code for closing IR detection debugging
	}
	else
		exitCustomDebuggingMenu();
}

void Tools::searchFrame(cv::Mat frame)
{
	if(detectionMethod==0)
	{
		cv::cvtColor(frame,imgHSV,cv::COLOR_BGR2HSV);//create HSV image for color filtering
		inRange(imgHSV,cv::Scalar(H_MIN,S_MIN,V_MIN),cv::Scalar(H_MAX,S_MAX,V_MAX),imgBIN);//creates binary threshold image

		cv::GaussianBlur(imgBIN, imgBIN, cv::Size(13, 13), 2, 2 );//blur image so HoughCircles argorithm doesnt shit itself
		if(!debuggingmode)//if HoughCircles is running while tuning, it shits itself
			HoughCircles(imgBIN, circle, CV_HOUGH_GRADIENT, dp, min_dist, param_1, param_2, min_radius, max_radius );
	}
	else if(detectionMethod==1)
	{
		//code for IR detection
	}
	else
		circle = customDetectionMethod(frame);
}




#endif