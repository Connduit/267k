/* C2Profile.h */

#include <stdint.h>

typedef enum
{
	TCP,
	HTTP,
	HTTPS
	
} TransportType;

typedef struct {
    // === NETWORK CONFIG ===
    char server_host[256];      // C2 server IP/domain
    uint16_t server_port;       // C2 server port
    TransportType transport;    // TCP, HTTP, HTTPS, DNS, etc.
    uint32_t beacon_interval;   // Check-in interval (ms)
    uint32_t jitter;           // Random delay to avoid patterns
    
    // === SECURITY CONFIG ===
    uint8_t crypto_key[32];     // AES-256 key for encryption
    uint8_t command_key[32];    // Key for command data encryption
    bool use_obfuscation;       // Enable code obfuscation
    bool use_antidebug;         // Enable anti-debug techniques
    
    // === OPERATIONAL CONFIG ===  
    char user_agent[256];       // For HTTP camouflage
    char campaign_id[64];       // Campaign identifier
    uint32_t max_payload_size;  // Maximum data exfil size
    bool enable_persistence;    // Install persistence
    char persistence_method[32]; // Registry, service, scheduled task
    
    // === TIMING CONFIG ===
    uint32_t timeout_ms;        // Network timeout
    uint32_t retry_count;       // Connection retries
    uint32_t retry_delay;       // Delay between retries
    
    // === STEALTH CONFIG ===
    bool mimic_process;         // Spoof process name
    char process_name[64];      // Process to mimic
    bool use_tls;              // Use TLS encryption
    bool verify_cert;          // Verify server certificate
    
} Config;

//Config* loadConfig()
Config loadConfig()
{
	//Config config = {0};

	/*
	uint8_t DEFAULT_CRYPTO_KEY[32] = {
		0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
		0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
		0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 
		0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F

	};*/
	//config->crypto_key = DEFAULT_CRYPTO_KEY; // this is just for debugging purposes
	Config config = {
		.crypto_key = {
			0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
			0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
			0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 
			0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F
		}

	};

	return config;
}
