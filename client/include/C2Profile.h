/* C2Profile.h */

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