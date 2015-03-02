#ifndef ZEUS_CPP
#define ZEUS_CPP

#include "header.h"





int Zeus::begin()
{
	
	cvStartWindowThread();
	cvNamedWindow("Camera_Output", cv::WINDOW_AUTOSIZE);//Create window
	
	this->stream.open(1);//capture using usb camera (not default)
	this->stream.set(CV_CAP_PROP_FRAME_WIDTH,FRAME_WIDTH);//set height and width of capture frame
	this->stream.set(CV_CAP_PROP_FRAME_HEIGHT,FRAME_HEIGHT);

	//set up com port
	turret.portName = "COM3";
	turret.baudRate= 9600;
	turret.port = turret.openPort(turret.portName,turret.baudRate);
	if (turret.port == INVALID_HANDLE_VALUE){return -1;}

	turret.initPosition();

	this->run();

	cvDestroyWindow("Camera_Output");
	return 0;
}




int Zeus::run()
{
	cv::Mat frame;

	while(1)//Create infinte loop for live streaming
	{
		//check to move turret
		if(targeting.clock.getTime() % 10 == 0)
			0;//targeting.updatePosition();

		stream.read(frame);//get a frame

		targeting.processFrame(frame,frame);//process the frame looking for targets

		imshow("Camera_Output", frame);//show the result

        key = cvWaitKey(30);//Capture Keyboard stroke
        if (char(key) == 27)
            break;//If you hit ESC key loop will break.
		else if (char(key) == 100)//enable debugging mode
		{ 
			if(!targeting.debuggingModeActive())
				targeting.enabledebugging();
			else
				targeting.disabledebugging(); 
		}

		else if (char(key) == 99)//open command prompt
		{
			system("pause");////////////////////////////////////////////////////////////////////////////////////

			//show
			//target list
			//detection mode
			//current 

			//change
			//detection method
			//mode//search//s&d//kill a target//kill all targets

			//kill a target

			//remove target

			//reset system

			//exit

		}
		else if(char(key) == 104)//help menu
		{
			std::cout<<"\n  HELP:  \n---------\n";
			std::cout<<"Enter 'ESC' to exit program\n";
			std::cout<<"Enter 'd' to open real time debugging\n";
			std::cout<<"Enter 'c' to open the cmd prompt system\n";
		}

		targeting.clock.tick();//advance program clock
    }

	return 0;
}




bool Zeus::killTarget()
{
	/*
	//kill the target
	targetPosition = targeting.getBestTarget();

	//check target to see if it makes sense
	if(targetPosition[0] + 10 >= FRAME_WIDTH/2 && targetPosition[0] - 10 <= FRAME_WIDTH/2 &&
		targetPosition[1] + 10 >= FRAME_HEIGHT/2 && targetPosition[1] - 10 <= FRAME_HEIGHT/2)
	{
		//fire laser
		system("pause");
	}
	
	if(targetPosition[0] > FRAME_WIDTH/2)	
		this->moveRight(100);
		
	if(targetPosition[0] < FRAME_WIDTH/2)
		this->moveLeft(100);

	if(targetPosition[1] > FRAME_HEIGHT/2)
		this->moveDown(100);
		
	if(targetPosition[1] < FRAME_HEIGHT/2)
		this->moveUp(100);


	
	stream.read(frame);
	//targetting.processframe
	*/
	return true;
}



#endif