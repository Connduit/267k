/* MessageTypes.h ... rename to MessageType.h? */

//#include <stdint.h>

//Message types
typedef enum {
	MSG_RECON = 1,      // Recon data from victim
	MSG_CMD = 2,        // Command to victim
	MSG_RESULT = 3,     // Command result from victim
	MSG_ERROR = 4       // Error message
} MessageType;

// Command types  
typedef enum {
	CMD_SHELL = 1,      // Execute shell command
	CMD_DOWNLOAD = 2,   // Download file
	CMD_UPLOAD = 3,     // Upload file
	CMD_EXIT = 4        // Stop implant
} CommandType;

// Generic message header
#pragma pack(push, 1)
typedef struct {
	uint32_t message_type;  // MessageType enum
	uint32_t data_size;     // Size of following data
	uint32_t message_id;    // Unique ID for tracking
} MessageHeader;
#pragma pack(pop)

// Command message from C2 to victim
typedef struct {
	uint32_t command_type;  // CommandType enum
	uint32_t data_size;     // Size of command_data
	char command_data[];    // Flexible array (shell command, filename, etc.)
} CommandMessage;

// Result message from victim to C2  
typedef struct {
	uint32_t exit_code;     // Command exit code
	uint32_t data_size;     // Size of output_data
	char output_data[];     // Command output or file data
} ResultMessage;





