#ifndef TARGETS_CPP
#define TARGETS_CPP

#include "header.h"


//functions yet to be written:
TargetList::TargetList(){}
void TargetList::clean(){}
void TargetList::add(Target){}
void TargetList::add(int,int,int,std::string){}
Target TargetList::getTarget(){return Target();}
Target TargetList::getTarget(std::string){return Target();}
bool TargetList::removeTarget(Target){return 0;}
Target TargetList::search(int,int){return Target();}
std::string TargetList::getFoundTargets(){return "0";}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//function looks for the current position relative to the past poitions.
//if it is a position of this target, it is atomatically added
bool Target::isTarget(float a, float b, int x, int y, int r,long unsigned time)
{
	if(cursor==0 && full==false)
		return false;//no previous position stored

	//this algorithm requires refinement -- look for previous positions, patterns, but keep execution time very fast.
	//is set up for stationary targets
	if(x<=position[cursor][0]+10 && x>=position[cursor][0]-10 && y<=position[cursor][1]+10 && y>=position[cursor][1]-10 && r<=position[cursor][2]+5 && r>=position[cursor][2]-5)
	{
		add(a,b,x,y,r,time);	
		return true;	
	}

	return 0;
}


//function processes frame and updates target datatype
bool TargetingController::processFrame(cv::Vec2f turretPos,cv::Mat frame,cv::Mat& output)
{
	while(!control.circle.empty())//clean temp storage
		control.circle.pop_back();

	control.searchFrame(frame);

	control.crosshair(output,FRAME_WIDTH/2, FRAME_HEIGHT/2,0,cv::Vec3i(100,100,100),"center");

	//TEMPORARY CODE FOR DEMO
	//draws targets on each found target.
	for( size_t i = 0; i < this->control.circle.size(); i++ )
		this->control.crosshair(output,cvRound(this->control.circle[i][0]), cvRound(this->control.circle[i][1]), cvRound(this->control.circle[i][2]),  cv::Vec3i(0,0,255), "target");


	for( size_t i = 0; i < this->control.circle.size(); i++ )
		this->control.circle.pop_back();
	//END OF DEMO SNIPPET

	
	//DRAW LASER TARGET ON SCREEN
	cv::circle(output,cv::Point(LASER_POSITION_X,LASER_POSITION_Y),5,cv::Scalar(0,255,0),1);//(where the laser is aimed)

	
	/*CODE FOR ADDING TARGETS TO ADT
	for( size_t i = 0; i < circle.size(); i++ )
    {
		//add to adt
		this->targetList.add(cvRound(circle.back()[0]),cvRound(circle.back()[1]),cvRound(circle.back()[2]),this->targetList.getFoundTargets());
		circle.pop_back();

		std::cout<<"Target "<<this->targetList.getFoundTargets()<<"Aquired.\n";//outputing may require too much time.
	

		//clear old targets here?
	}

	for(int i = this->targetList.numCurrentTargets();i>0;i--)/////////////////////////////////////////////////////////////////////////////////////////////DEMO//////////////////////////////////////////
	{
		//smooth out target "bouncing"

		//draw crosshair on target
		crosshair(imgRGB,cvRound(circle[i][0]), cvRound(circle[i][1]), "target");        
		
		//draw circle of radius around target
        cv::circle(imgRGB, cv::Point(cvRound(circle[i][0]), cvRound(circle[i][1])), cvRound(circle[i][2]), cv::Scalar(0,255,255), 2);

	}
	*/
		

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

const cv::vector<cv::Vec3f> TargetingController::processFrame(cv::Mat frame)
{
	while(!control.circle.empty())//clean temp storage
		control.circle.pop_back();

	control.searchFrame(frame);
	return control.circle;
}


#endif