#include "header.h"


int main()
{
	cv::Mat frame;//holds a frame from camera
	TurretController overlord;

	cvStartWindowThread();
	cvNamedWindow("Camera_Output", cv::WINDOW_AUTOSIZE);//Create window
	
	overlord.stream.open(1);//capture using usb camera (not default)
	overlord.stream.set(CV_CAP_PROP_FRAME_WIDTH,FRAME_WIDTH);//set height and width of capture frame
	overlord.stream.set(CV_CAP_PROP_FRAME_HEIGHT,FRAME_HEIGHT);

	//set up com port
	//NEXT LINE: or use COM3
	overlord.portName = "\\\\.\\USBSER000";  // Each double slash in this source code represents one slash in the actual name.;
	overlord.baudRate= 9600;
	overlord.port = overlord.openPort(overlord.portName, overlord.baudRate);
	if (overlord.port == INVALID_HANDLE_VALUE){ return -1; }

	overlord.initPosition();


    while(1)//Create infinte loop for live streaming
	{
		if(overlord.targeting.clock.getTime() % 10 == 0)
			overlord.updatePosition();

		overlord.stream.read(frame);//get a frame

		overlord.search(frame,frame);//process the frame looking for targets

		imshow("Camera_Output", frame);//show the result

        overlord.targeting.key = cvWaitKey(30);//Capture Keyboard stroke
        if (char(overlord.targeting.key) == 27)
            break;//If you hit ESC key loop will break.
		else if (char(overlord.targeting.key) == 100)//enable debugging mode
		{ 
			if(!overlord.targeting.debuggingModeActive())
				overlord.targeting.enabledebugging();
			else
				overlord.targeting.disabledebugging(); 
		}

		else if (char(overlord.targeting.key) == 99)//open command prompt
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
		else if(char(overlord.targeting.key) == 104)//help menu
		{
			std::cout<<"\n  HELP:  \n---------\n";
			std::cout<<"Enter 'ESC' to exit program\n";
			std::cout<<"Enter 'd' to open real time debugging\n";
			std::cout<<"Enter 'c' to open the cmd prompt system\n";
		}

		overlord.targeting.clock.tick();//advance program clock
    }
    cvDestroyWindow("Camera_Output");

    return 0;

}