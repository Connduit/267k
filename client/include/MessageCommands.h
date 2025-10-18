/* MessageCommands.h */

/*
 * This file defines the command structures and constants needed to define (interface?) data
 * */

// enum command type struct
enum CommandType
{
	HANDSHAKE,          // Initial connection
	HEARTBEAT,          // Regular check-in
	SYSTEM_INFO,        // Victim system data
	COMMAND_RESULT,     // Output from executed command
	DATA_EXFIL,         // Stolen data
	FILE_UPLOAD,        // File transfer
	ERROR_REPORT        // Error information

};

// full message?
/*
 * struct FullMessage
 * {
 * 		header
 * 		CommandTypeStruct // ex. HandshakeCommand (not sure how this will work without c++ inheritence)
 *
 * }
 *
 * */

// define struct for each CommandType enum? 
struct HandshakeCommand // TODO: think of better name
{

};
