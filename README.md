# 267k
tcp stub

### Commands
- g++ stub.cpp -o stub
- x86_64-w64-mingw32-g++ stub_win.cpp -o stub_win.exe -lws2_32
- server: printf "fart" | ncat -l 4444
- client: run executable

### Concepts
- listener with auto-deploy?

### Workflow
stub -> payload (establish persistence) -> full install -> profit? 

##### stub
- avoid installation / when stub is being transfer to the victim
- avoid detection while idle but installed on victim machine
- avoid detection while stub is executing / alive
- avoid detection after stub is done... clean up or find somewhere to hide?

# Counter Measures
- Ivanti Device & Application Control
- Symantec Endpoint Protection
- RTNotify?
- /////
- Zscaler
- Secret Double Octopus
- BeyondTrust Privilege Management / Avecto Privilege Guard Client
- Forcepoint DLP Endpoint
- Boldon James File Classifier
- Cisco Secure Client

# Counter-Counter Measures
- dll side-loading
- reflective dll injection
- dll as a payload container
- com / activeX dll abuse
- hide code in different/unusal sections of the PE
- payload/stub encryption (xor)
-   AES-GCM (AES in Galois/Counter Mode)
-   ChaCha20-Poly1305 — faster on devices without AES hardware acceleration
- c2/network encryption (https and custom tsl wrappers)
-   TLS 1.3
-   Mutual TLS
-   Use HTTP(S) or WebSockets over TLS if you need easy firewall traversal and visibility via HTTP semantics.
-   Ephemeral key exchange (ECDHE) for Perfect Forward Secrecy (PFS).

# TODO
- rev https stub (use logic from tcp stub)
