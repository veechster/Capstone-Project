#ifndef TARGETS_CPP
#define TARGETS_CPP

#include "header.h"

//returns last reported position of this target
void Target::location(short& a, short& b, int& x, int& y, int& r)
{
	if(cursor==0)
	{
		if(full==false)
		{
			a=b=0;
			x=y=r=0;

			return;//no previous position stored
		}

		cursor=10;//for getting the last position
	}
		
	a=orientation[cursor-1][0];
	b=orientation[cursor-1][1];
	x=position[cursor-1][0];
	y=position[cursor-1][1];
	r=position[cursor-1][2];

	if(cursor==10)
		cursor=0;
}


//function checks the current position relative to the past poitions.
//if it is a position of this target, it is atomatically added
bool const Target::isTarget(short a, short b, int x, int y, int r,long unsigned time)
{
	if(cursor==0 && full==false)
			return false;//no previous position stored
		

	//this algorithm requires refinement -- look for previous positions, patterns, but keep execution time very fast.
	//is set up for stationary targets
	if(x<=position[(cursor%10)-1][0]+10 && x>=position[(cursor%10)-1][0]-10 && y<=position[(cursor%10)-1][1]+10 && y>=position[(cursor%10)-1][1]-10 && r<=position[(cursor%10)-1][2]+5 && r>=position[(cursor%10)-1][2]-5)
		return true;		

	return false;
}

bool Target::isTarget_add(short a, short b, int x, int y, int r,long unsigned time)
{
	if(this->isTarget(a,b,x,y,r,time))
	{
		addSighting(a,b,x,y,r,time);	
		return true;	
	}

	return false;
}

void TargetList::clean()
{
	while(list.size()>0)
	{
		temp = list.back();
		list.pop_back();
		delete temp;
	}
}

void TargetList::add(short a,short b,int x,int y,int r,std::string n,long unsigned t)
{
	for (std::vector<Target*>::iterator it = list.begin(); it != list.end(); it++)
	{	
		if((*it)->isTarget_add(a,b,x,y,r,t))
		{
			return;
		}
	}

	temp = new Target;
	temp->orientation[0][0]=a;
	temp->orientation[0][1]=b;
	temp->position[0][0]=x;
	temp->position[0][1]=y;
	temp->position[0][2]=r;
	temp->lastSeen=t;
	temp->timesSeen=1;
	temp->name=n;
	temp->full=0;
	temp->cursor=1;
	list.push_back(temp);

	this->foundTargets++;

}

//returns the actual data. (could be used improperly)
Target TargetList::getLastTarget()
{
	temp = list.back();
	return *temp;
}

Target TargetList::getTarget(std::string n)////////////////////////////double check to make sure this works.
{
	for(unsigned i=0;i<list.size();i++)
	{
		if(list[i]->name==n)
			return *list[i];
	}

	return Target();//an empty target
}

bool TargetList::removeTarget(Target t)/////////////////////////////////double check to make sure this works.
{
	unsigned i=0;
	for(;i<list.size();i++)
	{
		if(list[i]->name==t.name)
		{
			list.erase(list.begin()+i);

			return true;
		}
	}

	return false;
}

Target TargetList::search(short a,short b,int x,int y,int r,long unsigned t)/////////////////////////////////double check to make sure this works.
{
	//search each target using Target
	for(unsigned i=0;i<list.size();i++)
	{
		if(list[i]->isTarget(a,b,x,y,r,t))
			return *list[i];
	}

	return Target();
}

//function processes frame and updates target datatype
bool TargetingController::processFrame(cv::Vec2s turretPos,cv::Mat frame,cv::Mat& output)
{
	while(!control.circle.empty())//clean temp storage
		control.circle.pop_back();

	control.searchFrame(frame);

	//DRAW LASER TARGET ON SCREEN
	control.crosshair(output,FRAME_WIDTH/2, FRAME_HEIGHT/2,0,cv::Vec3i(100,100,100),"center");
	cv::circle(output,cv::Point(LASER_POSITION_X,LASER_POSITION_Y),5,cv::Scalar(0,255,0),1);//(where the laser is aimed)
	
	
	//CODE FOR ADDING TARGETS
	for( size_t i = control.circle.size(); i > 0; i-- )
    {
		//check if each target exists and add them
		
		this->targets.add(turretPos[0],turretPos[1],cvRound(control.circle.back()[0]),cvRound(control.circle.back()[1]),cvRound(control.circle.back()[2]),this->targets.getFoundTargets(),clock.getTime());
		
		//identify each target on screen
		//draws targets on each found target.
		this->control.crosshair(output,cvRound(this->control.circle.back()[0]), cvRound(this->control.circle.back()[1]), cvRound(this->control.circle.back()[2]),  cv::Vec3i(0,0,255), this->targets.getFoundTargets());
		
		control.circle.pop_back();

		std::cout<<this->targets.getFoundTargets()<<"Aquired.\n";//outputing may require too much time.
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

//following function does not update target database, instead returns a vector containing all the targets found in the frame.
const cv::vector<cv::Vec3f> TargetingController::processFrame(cv::Mat frame)
{
	while(!control.circle.empty())//clean temp storage
		control.circle.pop_back();

	control.searchFrame(frame);
	return control.circle;
}


#endif