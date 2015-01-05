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

    while(1)//Create infinte loop for live streaming
	{
		overlord.stream.read(frame);

		overlord.search(frame,frame);//process the frame looking for targets

		imshow("Camera_Output", frame);

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
			system("pause");////////////////////////////////////////////////////////////////////////////////////

		overlord.targeting.clock.tick();
    }
    cvDestroyWindow("Camera_Output"); //Destroy Window

    return 0;

}