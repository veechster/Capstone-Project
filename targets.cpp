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

	cv::circle(output,cv::Point(LASER_POSITION_X,LASER_POSITION_Y),5,cv::Scalar(150,0,200),2);//where the laser is aimed

	cv::circle(output,cv::Point(FRAME_WIDTH/2,FRAME_HEIGHT/2),30,cv::Scalar(75,75,75),1);//the kill circle
	
		//identify each target on screen
		//draws targets on each found target.
	for (std::vector<cv::Vec3f>::iterator circleIterator = control.circle.begin(); circleIterator != control.circle.end(); ++circleIterator)
		this->control.crosshair(output,cvRound((*circleIterator)[0]), cvRound((*circleIterator)[1]), cvRound((*circleIterator)[2]),  cv::Vec3i(0,0,255), "target");

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

//find out which target is closest to center
cv::Vec3f TargetingController::getBestTarget()
{
	float temp=1000.0;
	std::vector<cv::Vec3f>::iterator best;

	//find out which target is closest to center
	for (std::vector<cv::Vec3f>::iterator circleIterator = control.circle.begin(); circleIterator != control.circle.end(); ++circleIterator)
	{
		if(abs((*circleIterator)[0]-FRAME_WIDTH)+abs((*circleIterator)[0]-FRAME_HEIGHT) < temp)
		{
			temp = abs((*circleIterator)[0]-FRAME_WIDTH)+abs((*circleIterator)[0]-FRAME_HEIGHT);
			best = circleIterator;
		}
	}

	if(temp==1000)
		return cv::Vec3f(-1.0,-1.0);

	return (*best);
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