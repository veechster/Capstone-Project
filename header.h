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

#define LASER_POSITION_X FRAME_WIDTH/2-25
#define LASER_POSITION_Y FRAME_HEIGHT/2-25

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
//MAY HAVE TO CHANGE STORAGE TYPE TO FLOAT
//MAY HAVE TO CHANGE CLASS INTERFACES
class Target
{
	friend class TargetList;

	cv::Vec3i position[10];//array of last 10 positions (x,y,r) where r is radius
	cv::Vec2f orientation[10];//keeps track of the turret position
	int cursor;//hold the position of the NEXT place in the array of positions to be used.
	bool full;//hold whether or not position array is fully used.
	std::string name;//holds the name of the target
	long unsigned lastSeen;//last "time" the target was updated
	unsigned timesSeen;//number of times this target has been seen
public:
	Target(){position[0][0]=position[0][1]=position[0][2]=0;orientation[0][0]=orientation[0][1]=0.0;cursor=0;full=false;}
	Target(float a, float b, int x, int y, int r,  long unsigned time,std::string c){addSighting(a,b,x,y,r,time);cursor=1;name=c;lastSeen=time;full=false;}
	void addSighting(float a, float b, int x, int y, int r, long unsigned time){orientation[cursor][0]=a;orientation[cursor][1]=b;position[cursor][0]=x;position[cursor][1]=y;position[cursor][2]=r;cursor=(cursor+1)%10;lastSeen=time;if(cursor==0)full=true;}//adds a position to the array of past positions
	const bool isTarget(float,float,int,int,int,long unsigned);//alogirthm for identifying weather this set of points belong to this target
	bool isTarget_add(float,float,int,int,int,long unsigned);//alogirthm for identifying weather this set of points belong to this target, adds if it is
	void location(float&,float&,int&,int&,int&);//returns the last reported position of this target
	//~Target(){}
};



/***
ADT: TargetList
Purpose: keep track of several targets (allows differentiation between targets)
Notes:
***/
class TargetList
{
	friend class TargetingController;

	std::vector<Target*> list;//stores all current targets
	int foundTargets;//total targets found 
	Target* temp;
public:
	TargetList(){foundTargets=0;}

	void clean();//deletes everything
	void add(Target);//adds a target
	void add(float,float,int,int,int,std::string,long unsigned);//adds a target
	Target getLastTarget();//gets the most recently inserted target
	Target getTarget(std::string);//gets a specific target
	bool removeTarget(Target);//removes a specifc target
	Target search(float,float,int,int,int,long unsigned);//looks for a target (current time)
	int numCurrentTargets(){return list.size();}//returns size of list (num)
	std::string getFoundTargets(){return "target " + std::to_string(foundTargets);}//returns total number of found targets (x) in string form: "target x"

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
	friend class TurretController;

	TargetList targets;//list of all current targets
	bool processFrame(cv::Vec2s,cv::Mat,cv::Mat &);//process a frame for targets and add them
	const cv::vector<cv::Vec3f> processFrame(cv::Mat);//process a frame for targets and return them.

public:
	char key;//for getting user input

	Ptime clock;//for keeping track of program time
	Tools control;//tools to control the tracking, etc


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
	cv::Mat frame;
	Target temp;

	unsigned short positionX;
	unsigned short positionY;
	long unsigned lastMove;
	unsigned short prevPositionX;
	unsigned short prevPositionY;
	unsigned short tempPosition;

	cv::Vec2s position;

	int mode;//0=search, 1=search and destroy, 3=killall; 2=kill (requires more input or just kills the most recent target.)
	
	void killAll();
	bool killTarget(Target);
	bool killTarget(std::string);

	//move left, right, down, up, return to prev position, output current position, etc.
	bool moveLeft(short);
	bool moveRight(short);
	bool moveUp(short);
	bool moveDown(short);

	BOOL maestroGetPosition(HANDLE port, unsigned char channel, unsigned short * position);

	BOOL maestroSetTarget(HANDLE port, unsigned char channel, unsigned short target);

public:
	TargetingController targeting;
	cv::VideoCapture stream;
	cv::VideoCapture stream1;
	int primaryStream;

	HANDLE port;
	char * portName;
	int baudRate;

	void search(cv::Mat,cv::Mat &);//searchs a frame for targets using TargetingController
	void search();//uses primaryStream stream
	void reset(bool);//resets the system. argument is a command (0 = clear targets, 1 = kill all)
	bool changeMode(int i){if (i<4 && i>0){mode=i;} else {return false;} return true;}
	//0=search, 1=search and destroy, 3=killall; 2=kill (requires more input or just kills the most recent target.)

	bool updatePosition();
	bool initPosition();

	TurretController();

	HANDLE openPort(const char * portName, unsigned int baudRate);
};


extern cv::vector<cv::Vec3f> customDetectionMethod(cv::Mat);
extern void customDebuggingMenu();
extern void exitCustomDebuggingMenu();


#endif