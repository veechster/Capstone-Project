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

	Sleep(500);

	this->run();

	cvDestroyWindow("Camera_Output");
	return 0;
}




int Zeus::run()
{
	while(1)//Create infinte loop for live streaming
	{
		//check to move turret
		if(targeting.clock.getTime() % TURRET_MOVE_FREQ == 0)
			if(turret.updatePosition() != 0)
				return -1;//error

		stream.read(frame);//get a frame
		
		if(targeting.processFrame(frame,frame))//process the frame looking for targets
		{
			imshow("Camera_Output", frame);//show the result

			std::cout<<"Target Detected.\n";

			key = cvWaitKey(250);//wait for cancellation command?
			
			std::cout<<"Moving to Eliminate Target.\n";

			key = cvWaitKey(250);

			if (char(key) == 100)//If you hit ESC nothing will happen
				std::cout<<"Kill cancelled.\n";
			else
			{
				if(killTarget())
					std::cout<<"Target Eliminated.\n";
				else
					std::cout<<"Target Lost.\n"; 
				
				std::cout<<"Returning to Sweep.\n";
			}
		}

		imshow("Camera_Output", frame);//show the result

		key = 0;
        key = cvWaitKey(30);//Capture Keyboard stroke
		if(key != 0)
			if(onKey()==1)
				return 1;//exit program

		targeting.clock.tick();//advance program clock
    }

	return 0;
}




bool Zeus::killTarget()
{
	turret.targetPosition = targeting.getBestTarget();

	//check target to see if it makes sense
	if(turret.targetPosition[0] + 15 >= FRAME_WIDTH/2 && turret.targetPosition[0] - 15 <= FRAME_WIDTH/2 &&
		turret.targetPosition[1] + 15 >= FRAME_HEIGHT/2 && turret.targetPosition[1] - 15 <= FRAME_HEIGHT/2)
	{
		std::cout<<"Firing.";

		//turn on laser (

		for(int i=0;i<120;i++)
		{
			stream.read(frame);
			targeting.processFrame(frame,frame);
			imshow("Camera_Output", frame);
			cvWaitKey(1);
			Sleep(2000/60);//30fps (assume other steps take about half the time)
		}

		//turn off laser

		system("pause");

		turret.updatePosition();

		return true;
	}

	if(turret.targetPosition[0]==-1)
	{
		counter++;
		if(counter > ZEUS_PATIENCE)//how many frames to reaquire
			return false;
	}
	else
	{
		counter=0;

		if(turret.targetPosition[0] > FRAME_WIDTH/2)	
			turret.moveRight(50);
			
		if(turret.targetPosition[0] < FRAME_WIDTH/2)
			turret.moveLeft(50);
	
		if(turret.targetPosition[1] > FRAME_HEIGHT/2)
			turret.moveDown(50);
			
		if(turret.targetPosition[1] < FRAME_HEIGHT/2)
			turret.moveUp(50);
	}
	Sleep(250);
	
	stream.read(frame);
	targeting.processFrame(frame,frame);
	imshow("Camera_Output", frame);
	key = 0;
	key = cvWaitKey(30);
	if(key != 0)
		onKey();

	return killTarget();
}





int Zeus::onKey()
{
	if (char(key) == 27)
		return 1;//If you hit ESC key loop will break.
	else if (char(key) == 100)//enable debugging mode
	{ 
		if(!targeting.debuggingModeActive())
			targeting.enabledebugging();
		else
			targeting.disabledebugging(); 
	}
	else if (char(key) == 99)//open command prompt
	{
		system("pause");

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

	return 0;
}




#endif