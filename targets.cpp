#ifndef TARGETS_CPP
#define TARGETS_CPP

#include "header.h"

//returns last reported position of this target
void Target::location(short& a, short& b, int& x, int& y, int& r)
{		
	a=orientation[0];
	b=orientation[1];
	x=position[0];
	y=position[1];
	r=position[2];

}

//function processes frame and updates target datatype
bool TargetingController::processFrame(cv::Mat frame,cv::Mat& output)
{
	while(!control.circle.empty())//clean temp storage
		control.circle.pop_back();

	control.searchFrame(frame);

	//DRAW LASER TARGET ON SCREEN
	control.crosshair(output,FRAME_WIDTH/2, FRAME_HEIGHT/2,0,cv::Vec3i(100,100,100),"center");
	cv::circle(output,cv::Point(LASER_POSITION_X,LASER_POSITION_Y),5,cv::Scalar(150,0,200),2);//(where the laser is aimed)
	
		//identify each target on screen
		//draws targets on each found target.
	for(int i = control.circle.size(); i<0; i--)
	{
		this->control.crosshair(output,cvRound(this->control.circle.back()[0]), cvRound(this->control.circle.back()[1]), cvRound(this->control.circle.back()[2]),  cv::Vec3i(0,0,255), "target");
		control.circle.pop_back();
	}

	if(debuggingModeActive())//DEBUGGING MODE
	{
		if(control.getDetectionMethod()==0)
		{
			imshow("HSV_Image", control.imgHSV);
			imshow("Editted_Image", control.imgBIN);
		}
		else if(control.getDetectionMethod()==1)
			imshow("Unprocessed", frame);
		else
			customDebuggingMenu();

	}

	return false;//no targets found
}

cv::Vec3f TargetingController::getBestTarget()
{
	//find out which target is closest to center

	return cv::Vec3f(FRAME_WIDTH, FRAME_HEIGHT);

}


//following function does not update target database, instead returns a vector containing all the targets found in the frame.
const cv::vector<cv::Vec3f> TargetingController::processFrame(cv::Mat frame)
{
	while(!control.circle.empty())//clean temp storage
		control.circle.pop_back();

	control.searchFrame(frame);
	return control.circle;
}


#endif