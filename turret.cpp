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

bool TurretController::moveLeft(short amt)
{
	if ( ! maestroGetPosition(port, 0, &tempPosition) ){ return false; }
	if(tempPosition-amt > TURRET_START_POSITION_X)
	{
		if ( ! maestroSetTarget(port, 0, tempPosition-amt) ){ return false; }
	}
	else 
		if ( ! maestroSetTarget(port, 0, TURRET_START_POSITION_X) ){ return false; }
	return true;
}

bool TurretController::moveRight(short amt)
{
	if ( ! maestroGetPosition(port, 0, &tempPosition) ){ return false; }
	if(tempPosition+amt < TURRET_END_POSITION_X)
	{
		if ( ! maestroSetTarget(port, 0, tempPosition+amt) ){ return false; }
	}
	else 
		if ( ! maestroSetTarget(port, 0, TURRET_END_POSITION_X) ){ return false; }
	return true;
}

bool TurretController::moveUp(short amt)
{
	if ( ! maestroGetPosition(port, 1, &tempPosition) ){ return false; }
	if(tempPosition+amt < TURRET_END_POSITION_Y)
	{
		if ( ! maestroSetTarget(port, 0, tempPosition+amt) ){ return false; }
	}
	else 
		if ( ! maestroSetTarget(port, 0, TURRET_END_POSITION_Y) ){ return false; }
	return true;
}

bool TurretController::moveDown(short amt)
{
	if ( ! maestroGetPosition(port, 1, &tempPosition) ){ return false; }
	if(tempPosition-amt > TURRET_END_POSITION_Y)
	{
		if ( ! maestroSetTarget(port, 0, tempPosition-amt) ){ return false; }
	}
	else 
		if ( ! maestroSetTarget(port, 0, TURRET_START_POSITION_Y) ){ return false; }
	return true;
}

bool TurretController::updatePosition()
{
	//get maestro current position:
	if ( ! maestroGetPosition(port, 0, &prevPositionX) ){ return false; }
	if ( ! maestroGetPosition(port, 1, &prevPositionY) ){ return false; }

	if(prevPositionX == TURRET_END_POSITION_X)
	{
		positionX = TURRET_START_POSITION_X;
		positionY = prevPositionY + 250;
		
		if(positionY > TURRET_END_POSITION_Y)
		{
			std::cout<<"END OF SWEEP";
			system("pause");
			//BEGIN NEW SWEEP ------ user input to determine course of action ------ bound by time
			positionY = TURRET_START_POSITION_Y;//temporary?
		}
	}
	else
	{
		positionX = prevPositionX += 250;
		positionY = prevPositionY;
	}

	//set new turret position to continue sweep:
	if ( ! maestroSetTarget(port, 0, positionX) ){ return false; }
	if ( ! maestroSetTarget(port, 1, positionY) ){ return false; }

	position[0] = positionX;
	position[1] = positionY;


	return true;
}

bool TurretController::initPosition()
{
	if ( ! maestroSetTarget(port, 0, TURRET_START_POSITION_X) ){ return false; }
	if ( ! maestroSetTarget(port, 1, TURRET_START_POSITION_Y) ){ return false; }
	return true;
}



/** Implements the Maestro's Get Position serial command.
 * channel: Channel number from 0 to 23
 * position: A pointer to the returned position value (for a servo channel, the units are quarter-milliseconds)
 * Returns 1 on success, 0 on failure.
 * For more information on this command, see the "Serial Servo Commands"
 * section of the Maestro User's Guide: http://www.pololu.com/docs/0J40 */
	BOOL TurretController::maestroGetPosition(HANDLE port, unsigned char channel, unsigned short * position)
{
	unsigned char command[2];
	unsigned char response[2];
	BOOL success;
	DWORD bytesTransferred;

	// Compose the command.
	command[0] = 0x90;
	command[1] = channel;

	// Send the command to the device.
	success = WriteFile(port, command, sizeof(command), &bytesTransferred, NULL);
	if (!success)
	{
		fprintf(stderr, "Error: Unable to write Get Position command to serial port.  Error code 0x%x.", GetLastError());
		return 0;
	}
	if (sizeof(command) != bytesTransferred)
	{
		fprintf(stderr, "Error: Expected to write %d bytes but only wrote %d.", sizeof(command), bytesTransferred);
		return 0;
	}

	// Read the response from the device.
	success = ReadFile(port, response, sizeof(response), &bytesTransferred, NULL);
	if (!success)
	{
		fprintf(stderr, "Error: Unable to read Get Position response from serial port.  Error code 0x%x.", GetLastError());
		return 0;
	}
	if (sizeof(response) != bytesTransferred)
	{
		fprintf(stderr, "Error: Expected to read %d bytes but only read %d (timeout). "
			"Make sure the Maestro's serial mode is USB Dual Port or USB Chained.", sizeof(command), bytesTransferred);
		return 0;
	}

	// Convert the bytes received in to a position.
	*position = response[0] + 256*response[1];

	return 1;
}

/** Implements the Maestro's Set Target serial command.
 * channel: Channel number from 0 to 23
 * target: The target value (for a servo channel, the units are quarter-milliseconds)
 * Returns 1 on success, 0 on failure.
 * Fore more information on this command, see the "Serial Servo Commands"
 * section of the Maestro User's Guide: http://www.pololu.com/docs/0J40 */
	BOOL TurretController::maestroSetTarget(HANDLE port, unsigned char channel, unsigned short target)
{
	unsigned char command[4];
	DWORD bytesTransferred;
	BOOL success;

	// Compose the command.
	command[0] = 0x84;
	command[1] = channel;
	command[2] = target & 0x7F;
	command[3] = (target >> 7) & 0x7F;

	// Send the command to the device.
	success = WriteFile(port, command, sizeof(command), &bytesTransferred, NULL);
	if (!success)
	{
		fprintf(stderr, "Error: Unable to write Set Target command to serial port.  Error code 0x%x.", GetLastError());
		return 0;
	}
	if (sizeof(command) != bytesTransferred)
	{
		fprintf(stderr, "Error: Expected to write %d bytes but only wrote %d.", sizeof(command), bytesTransferred);
		return 0;
	}

	return 1;
}

/** Opens a handle to a serial port in Windows using CreateFile.
 * portName: The name of the port.
 * baudRate: The baud rate in bits per second.
 * Returns INVALID_HANDLE_VALUE if it fails.  Otherwise returns a handle to the port.
 *   Examples: "COM4", "\\\\.\\USBSER000", "USB#VID_1FFB&PID_0089&MI_04#6&3ad40bf600004# */
	HANDLE TurretController::openPort(const char * portName, unsigned int baudRate)
{
	HANDLE port;
	DCB commState;
	BOOL success;
	COMMTIMEOUTS timeouts;

	/* Open the serial port. */
	port = CreateFileA(portName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (port == INVALID_HANDLE_VALUE)
	{
		switch(GetLastError())
		{
		case ERROR_ACCESS_DENIED:	
			fprintf(stderr, "Error: Access denied.  Try closing all other programs that are using the device.\n");
			break;
		case ERROR_FILE_NOT_FOUND:
			fprintf(stderr, "Error: Serial port not found.  "
				"Make sure that \"%s\" is the right port name.  "
				"Try closing all programs using the device and unplugging the "
				"device, or try rebooting.\n", portName);
			break;
		default:
			fprintf(stderr, "Error: Unable to open serial port.  Error code 0x%x.\n", GetLastError());
			break;
		}
		return INVALID_HANDLE_VALUE;
	}

	/* Set the timeouts. */
	success = GetCommTimeouts(port, &timeouts);
	if (!success)
	{
		fprintf(stderr, "Error: Unable to get comm timeouts.  Error code 0x%x.\n", GetLastError());
		CloseHandle(port);
		return INVALID_HANDLE_VALUE;
	}
	timeouts.ReadIntervalTimeout = 1000;
	timeouts.ReadTotalTimeoutConstant = 1000;
	timeouts.ReadTotalTimeoutMultiplier = 0;
	timeouts.WriteTotalTimeoutConstant = 1000;
	timeouts.WriteTotalTimeoutMultiplier = 0;
	success = SetCommTimeouts(port, &timeouts);
	if (!success)
	{
		fprintf(stderr, "Error: Unable to set comm timeouts.  Error code 0x%x.\n", GetLastError());
		CloseHandle(port);
		return INVALID_HANDLE_VALUE;
	}

	/* Set the baud rate. */
	success = GetCommState(port, &commState);
	if (!success)
	{
		fprintf(stderr, "Error: Unable to get comm state.  Error code 0x%x.\n", GetLastError());
		CloseHandle(port);
		return INVALID_HANDLE_VALUE;
	}
	commState.BaudRate = baudRate;
	success = SetCommState(port, &commState);
	if (!success)
	{
		fprintf(stderr, "Error: Unable to set comm state.  Error code 0x%x.\n", GetLastError());
		CloseHandle(port);
		return INVALID_HANDLE_VALUE;
	}

	/* Flush out any bytes received from the device earlier. */
	success = FlushFileBuffers(port);
	if (!success)
	{
		fprintf(stderr, "Error: Unable to flush port buffers.  Error code 0x%x.\n", GetLastError());
		CloseHandle(port);
		return INVALID_HANDLE_VALUE;
	}

	return port;
}


#endif