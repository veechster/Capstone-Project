#ifndef TURRET_CPP
#define TURRET_CPP

#include "header.h"

TurretController::TurretController()
{
	lastMove = targeting.clock.getTime();

	mode = 0;

	primaryStream = 0;

}
	
void TurretController::killAll()
{
	while (targeting.targets.numCurrentTargets()>0)
	{
		temp = targeting.targets.getLastTarget();
		if(!killTarget(temp))//if there was a problem, remove it anyway.
			targeting.targets.removeTarget(temp);
	}
}

bool TurretController::killTarget(Target enemy)
{
	//check target to see if it makes sense

	//align turret of course
	
	stream.read(frame);
	
	//is enemy on frame?

	//use temp and frame to find/identify this target.
	
	//further align turret
	
	//fire until destroyed (or key press?)
	
	targeting.targets.removeTarget(enemy);

	return true;
}

bool TurretController::killTarget(std::string name)
{
	return killTarget(targeting.targets.getTarget(name));
}

void TurretController::search(cv::Mat frame,cv::Mat & output)
{
	//pre process frame and if mode 1 and targets found, kill them and return.
	if(targeting.processFrame(position,frame,output) && mode == 1)
	{
		killTarget(targeting.targets.getLastTarget());//as of right now, will only kill 1 target, even if multiple are found
		return;
	}
	
	if(targeting.clock.getTime()%5==0)//if on a fifth clock, move turret
	{
		//if prevpos == end pos prompt user?

		//check end pos for boundries. if too far right, go up and work the way right, set curr pos
	}



	//prev turret pos = turret pos;
	return;
}

void TurretController::search()
{
	if(primaryStream==0)
		stream.read(frame);
	else
		stream1.read(frame);

	search(frame,frame);
	return;
}

void TurretController::reset(bool command)
{
	if(command==0)
		targeting.targets.clean();
	else
		killAll();

	//move turret to start position, set curr pos.

	return;
}





#endif