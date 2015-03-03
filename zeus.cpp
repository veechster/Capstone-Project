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

			key = 0;
			key = cvWaitKey(250);//wait for cancellation command?
			
			std::cout<<"Moving to Eliminate. Press 'i' to ignore the target.\n";

			key = cvWaitKey(250);

			if (char(key) == 'i')//If you hit ESC nothing will happen
				std::cout<<"Target Ignored.\n";
			else
			{
				rVal=killTarget();
				if(rVal==0)
					std::cout<<"Target Eliminated.\n";
				else if(rVal==2)
					std::cout<<"Target Ignored.\n";
				else if(rVal==3)
					std::cout<<"Kill Cancelled by User.\n";
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




int Zeus::killTarget()
{
	turret.targetPosition = targeting.getBestTarget();

	//check target to see if it makes sense
	if(turret.targetPosition[0] + 15 >= FRAME_WIDTH/2 && turret.targetPosition[0] - 15 <= FRAME_WIDTH/2 &&
		turret.targetPosition[1] + 15 >= FRAME_HEIGHT/2 && turret.targetPosition[1] - 15 <= FRAME_HEIGHT/2)
	{
		std::cout<<"Firing.\n Press any key to cancel...\n";

		key = 0;
		key = cvWaitKey(250);

		//turn on laser

		//leave laser on for 60 frames at 60 frames per sec (1000/60) : assuming all other steps take 0 time : Sleep(1000/60)
		for(int i=0;i<60;i++)
		{
			if(key != 0)
				return 3;//user cancelled firing
			stream.read(frame);
			targeting.processFrame(frame,frame);
			imshow("Camera_Output", frame);
			key = 0;
			key = cvWaitKey(1000/60);
		}

		//turn off laser

		system("pause");///////////////////////////////////////debugging line///////////////////////////////////////////////////////////////////////

		turret.updatePosition();

		return 0;
	}

	if(turret.targetPosition[0]==-1)
	{
		counter++;
		if(counter > ZEUS_PATIENCE)//how many frames to reaquire
			return -41;
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

	stream.read(frame);
	targeting.processFrame(frame,frame);
	imshow("Camera_Output", frame);
	key = 0;
	key = cvWaitKey(250);
	if(key != 0)
	{
		if(key=='i')
		{
			//SET TARGET AS IGNORED?//////////////////////////////////////////////////////////////////////////////////////
			return 2;
		}
		if(onKey()==1)
			return 1;
	}

	return killTarget();
}





int Zeus::onKey()
{
	if (char(key) == 'ESC')//27
		return 1;//If you hit "ESC", program will close.
	else if (char(key) == 'd')//enable debugging mode
	{ 
		if(!targeting.debuggingModeActive())
			targeting.enabledebugging();
		else
			targeting.disabledebugging(); 
	}
	else if (char(key) == 'c')//open command prompt
	{
		system("pause");///////////////////////////////////////debugging line///////////////////////////////////////////////////////////////////////


		//use waitkey for all input (y/n commands)

			//show
			//target list
			//detection mode
			//current 

			//change
			//detection method

			//kill a target

			//remove target

			//reset system

			//exit

	}
	else if(char(key) == 'h')//help menu
	{
		std::cout<<"\n  HELP:  \n---------\n";
		std::cout<<"Enter 'ESC' to exit program.\n";
		std::cout<<"Enter 'd' to open real time debugging.\n";
		std::cout<<"Enter 'c' to open the command prompt.\n";
	}

	return 0;
}




#endif