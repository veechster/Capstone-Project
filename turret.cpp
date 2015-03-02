#ifndef TURRET_CPP
#define TURRET_CPP

#include "header.h"

int TurretController::moveLeft(short amt)
{
	if ( ! maestroGetPosition(port, 0, &tempPosition) ){ return -11; }
	if(tempPosition-amt > TURRET_START_POSITION_X)
	{
		if ( ! maestroSetTarget(port, 0, tempPosition-amt) ){ return -11; }
		return 0;
	}
	else 
	{
		if ( ! maestroSetTarget(port, 0, TURRET_START_POSITION_X) ){ return -11; }
		return -23;
	}
}

int TurretController::moveRight(short amt)
{
	if ( ! maestroGetPosition(port, 0, &tempPosition) ){ return -11; }
	if(tempPosition+amt < TURRET_END_POSITION_X)
	{
		if ( ! maestroSetTarget(port, 0, tempPosition+amt) ){ return -11; }
		return 0;
	}
	else 
	{
		if ( ! maestroSetTarget(port, 0, TURRET_END_POSITION_X) ){ return false; }
		return -23;
	}
}

int TurretController::moveUp(short amt)
{
	if ( ! maestroGetPosition(port, 1, &tempPosition) ){ return -11; }
	if(tempPosition+amt < TURRET_END_POSITION_Y)
	{
		if ( ! maestroSetTarget(port, 1, tempPosition+amt) ){ return -11; }
		return 0;
	}
	else 
	{
		if ( ! maestroSetTarget(port, 1, TURRET_END_POSITION_Y) ){ return -11; }
		return -23;
	}
}

int TurretController::moveDown(short amt)
{
	if ( ! maestroGetPosition(port, 1, &tempPosition) ){ return -11; }
	if(tempPosition-amt > TURRET_START_POSITION_Y)
	{
		if ( ! maestroSetTarget(port, 1, tempPosition-amt) ){ return -11; }
		return 0;
	}
	else 
	{
		if ( ! maestroSetTarget(port, 1, TURRET_START_POSITION_Y) ){ return -11; }
		return -23;
	}
}

int TurretController::updatePosition()
{
	if(prevPositionX == TURRET_END_POSITION_X)
	{
		positionX = TURRET_START_POSITION_X;
		positionY = prevPositionY + TURRET_MOVE_AMT_Y;
		
		if(positionY > TURRET_END_POSITION_Y)
		{
			std::cout<<"End of sweep.\n";
			Sleep(1000);
			std::cout<<"Beginning new sweep.\n";
			positionY = TURRET_START_POSITION_Y;
		}
	}
	else
	{
		positionX = prevPositionX += TURRET_MOVE_AMT_X;
		positionY = prevPositionY;
	}

	//set new turret position to continue sweep
	if ( ! maestroSetTarget(port, 0, positionX) ){ return -11; }
	if ( ! maestroSetTarget(port, 1, positionY) ){ return -11; }

	prevPositionX = positionX;
	prevPositionY = positionY;

	return 0;
}

int TurretController::initPosition()
{
	if ( ! maestroSetTarget(port, 0, TURRET_START_POSITION_X) ){ return -11; }
	if ( ! maestroSetTarget(port, 1, TURRET_START_POSITION_Y) ){ return -11; }

	prevPositionX = TURRET_START_POSITION_X;
	prevPositionY = TURRET_START_POSITION_Y;

	return 0;
}


//following functions were written by polulu:

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