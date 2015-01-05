#ifndef HEADER_H
#define HEADER_H

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
public:
	Target(){position[0][0]=position[0][1]=position[0][2]=0;orientation[0][0]=orientation[0][1]=0.0;cursor=0;full=false;}
	Target(float a, float b, int x, int y, int r,  long unsigned time,std::string c){add(a,b,x,y,r,time);cursor=1;name=c;lastSeen=time;full=false;}
	void add(float a, float b, int x, int y, int r, long unsigned time){orientation[cursor][0]=a;orientation[cursor][1]=b;position[cursor][0]=x;position[cursor][1]=y;position[cursor][2]=r;cursor=(cursor+1)%10;lastSeen=time;if(cursor==0)full=true;}//adds a position to the array of past positions
	bool isTarget(float,float,int,int,int,long unsigned);//alogirthm for identifying weather this set of points belong to this target
	void find(float& a, float& b, int& x, int& y, int& r){if(cursor==0)a=orientation[cursor][0];b=orientation[cursor][1];x=position[cursor][0];y=position[cursor][1];r=position[cursor][2];}
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

	Target* list;//stores all current targets
	int num;//size of list
	int foundTargets;//total targets found 
	
public:
	TargetList();

	void clean();
	void add(Target);
	void add(int,int,int,std::string);
	Target getTarget();
	Target getTarget(std::string);
	bool removeTarget(Target);
	Target search(int,int);
	int numCurrentTargets(){return num;}//returns size of list (num)
	std::string getFoundTargets();//returns total number of found targets (x) in string form: "target x"

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
	bool processFrame(cv::Vec2f,cv::Mat,cv::Mat &);//process a frame for targets and add them
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

	cv::Vec2f startPosition;//this needs to be decided upon and calibrated once the turret is connected and communication is possible///////////////////////////////
	cv::Vec2f endPostion;

	cv::Vec2f position;
	long unsigned lastMove;
	cv::Vec2f prevPosition;

	int mode;//0=search, 1=search and destroy, 3=killall; 2=kill (requires more input or just kills the most recent target.)
	
	void killAll();
	bool killTarget(Target);
	bool killTarget(std::string);

public:
	TargetingController targeting;
	cv::VideoCapture stream;
	cv::VideoCapture stream1;
	int primaryStream;


	void search(cv::Mat,cv::Mat &);//searchs a frame for targets using TargetingController
	void search();//uses primaryStream stream
	void reset(bool);//only works if reasched the end of a sweep where the argument is a command (0 = clear targets, 1 = kill all)
	bool changeMode(int i){if (i<4 && i>0){mode=i;} else {return false;} return true;}
	//0=search, 1=search and destroy, 3=killall; 2=kill (requires more input or just kills the most recent target.)
	
	//move left, right, down, up, return to prev position, output current position, etc.

	//update position function that uses position?

	TurretController();

};


extern cv::vector<cv::Vec3f> customDetectionMethod(cv::Mat);
extern void customDebuggingMenu();


#endif