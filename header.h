#ifndef HEADER_H
#define HEADER_H

#include <stdio.h>
#include <windows.h>
#include <sstream>
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>

//width and height of playback screen.
#define FRAME_WIDTH 640
#define FRAME_HEIGHT 480

#define LASER_POSITION_X FRAME_WIDTH/2+18
#define LASER_POSITION_Y FRAME_HEIGHT/2+12

#define TURRET_START_POSITION_X 4000
#define TURRET_START_POSITION_Y 4600
#define TURRET_END_POSITION_X 8000
#define TURRET_END_POSITION_Y 6500





/***
ADT: Ptime
Purpose: provides a way of keeping track how "long" a program has been iterating
Notes:
***/
class Ptime
{
	long unsigned count; //program loop counter
	void reset(){count=0;}//private to avoid misuse
public:
	Ptime(){count=0;}
	void tick(){count++;}
	long unsigned getTime(){return count;}
};





/***
ADT: Target
Purpose: provides a way of keeping track of a targets location and previous 10 locations 
Notes: this class will be used to keep track of an individual target and information about it
***/
class Target
{
	friend class TargettingController;

	cv::Vec3i position;//(x,y,r) where r is radius
	cv::Vec2s orientation;//keeps track of the turret position
	long unsigned lastSeen;//last "time" the target was updated
	unsigned timesSeen;//number of times this target has been seen
public:
	Target(){position[0]=position[1]=position[2]=0;orientation[0]=orientation[1]=0;}
	Target(short a, short b, int x, int y, int r,  long unsigned time,std::string c){orientation[0]=a,orientation[1]=b,position[0]=x,position[1]=y,position[2]=r,lastSeen=time;}
	void location(short&,short&,int&,int&,int&);//returns the last reported position of this target
	//~Target(){}
};





/***
ADT: Tools
Purpose: helper for targetingController, most notably, contains all the logic for debugging
Notes: any drawing and so-forth functions for ProgramData will be contained in this class
***/
class Tools
{
	friend class TargetingController;

	int detectionMethod;//detection method (0=HSV image, 1=for IR targeting, anything else is alternate method)
	void searchFrame(cv::Mat);//searchs frame, stores identified targets in "circles"

	void createTrackbars();//for debugging - adjusting filters
	void crosshair(cv::Mat&,int,int,int,cv::Vec3i,std::string);//draws a crosshair at a given location with a given color and name
	void enabledebugging();//for debugging mode
	void disabledebugging();//for debugging mode

	//temporary storage variables:
	cv::Mat imgHSV;//an image in HSVS
	cv::Mat imgBIN;//an image in binary threshold
	cv::vector<cv::Vec3f> circle;//temporary storage space for a circle (x,y,r)

	//debugging/filtering variables:
	bool debuggingmode;
	int H_MIN;
	int H_MAX;
	int S_MIN;
	int S_MAX;
	int V_MIN;
	int V_MAX;
	int dp;// = 1: The inverse ratio of resolution
	int min_dist;// = src_gray.rows/8: Minimum distance between detected centers
	int param_1;//= 200: Upper threshold for the internal Canny edge detector
	int param_2;//= 1/00*: Threshold for center detection.
	int min_radius;//= 0: Minimum radio to be detected. If unknown, put zero as default.
	int max_radius;

public:
	Tools();
	void changeDetectionMethod(int i){detectionMethod=i;}
	int getDetectionMethod(){return detectionMethod;}
};





/***
ADT: targetingController
Purpose: all of the main data and functions for the processing of frames and tracking of targets therein
Notes: 
***/
class TargetingController
{
	friend class Zeus;

	Tools control;//tools to control the tracking, etc

	cv::vector<Target *> targets;//list of all current targets
	bool processFrame(cv::Mat,cv::Mat &);//process a frame for targets and mark them
	const cv::vector<cv::Vec3f> processFrame(cv::Mat);//process a frame for targets and do not mark them.

public:
	Ptime clock;//for keeping track of program time

	cv::Vec3f getBestTarget();

	void enabledebugging(){control.enabledebugging();}
	void disabledebugging(){control.disabledebugging();}
	bool debuggingModeActive(){return control.debuggingmode;}

	//TargetingController();
};





/***
ADT: Turret Controller
Purpose: this function controls the target aquisition and elimination functions, controls turret orientation and function.
Notes: 
***/
class TurretController
{
	friend class Zeus;

	cv::Mat frame;

	unsigned short positionX;
	unsigned short positionY;
	unsigned short prevPositionX;
	unsigned short prevPositionY;

	cv::Vec3f targetPosition;

	BOOL maestroGetPosition(HANDLE port, unsigned char channel, unsigned short * position);

	BOOL maestroSetTarget(HANDLE port, unsigned char channel, unsigned short target);

	HANDLE openPort(const char * portName, unsigned int baudRate);	

	//move left, right, down, up, return to prev position, output current position, etc.
	bool moveLeft(short);
	bool moveRight(short);
	bool moveUp(short);
	bool moveDown(short);

	HANDLE port;
	char * portName;
	int baudRate;

	bool updatePosition();
	bool initPosition();

public:

	TurretController();
	
};


//the system controlling class
class Zeus
{
	cv::Mat frame;
	bool killTarget();

	TargetingController targeting;
	TurretController turret;	
	
	cv::VideoCapture stream;

public:
	char key;//for getting user input

	int begin();
	int run();

	Zeus(){}
};




extern cv::vector<cv::Vec3f> customDetectionMethod(cv::Mat);
extern void customDebuggingMenu();
extern void exitCustomDebuggingMenu();


#endif