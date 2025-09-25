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
-   -
-   Simple obfuscation for quick hacks: XOR, rolling-XOR, trivial byte transform
-   Packers / crypters: UPX (both legitimate use and abused), commercial crypters and custom packers.
-   Standard symmetric crypto for serious samples: AES (often AES-128 or AES-256), frequently in authenticated modes (GCM is common in newer tooling), or ChaCha20-Poly1305 on toolchains that favor it.
-   Custom multi-stage formats: compressed → encrypted → encoded blobs inside .rsrc or custom sections; small loader/stub in .text unpacks/decrypts in memory.
-   Polymorphism/metamorphism:
- c2/network encryption (https and custom tsl wrappers)
-   TLS 1.3
-   Mutual TLS
-   Use HTTP(S) or WebSockets over TLS if you need easy firewall traversal and visibility via HTTP semantics.
-   Ephemeral key exchange (ECDHE) for Perfect Forward Secrecy (PFS).
-   -
-   HTTPS / TLS (very common): attackers prefer HTTPS because it blends with normal web traffic. Many C2 frameworks use TLS 1.2 or TLS 1.3.
-   Mutual TLS (mTLS) occasionally: more advanced operators use mTLS for client auth (makes detection/control harder).
-   WebSockets over TLS for persistent, bidirectional channels that look like normal web apps.
-   DoH / DoT / DNS tunneling when environments restrict normal web egress. DNS tunneling tools (Iodine, DNScat2 variants) or DoH (DNS over HTTPS) are used for stealthy beacons and tiny data exfil.
-   Custom encrypted TCP/UDP channels: AES/ChaCha20 used over raw sockets when stealth beyond TLS is desired.
-   Abuse of legitimate cloud/web services: GitHub Gists, Google Drive, Slack, Discord, cloud functions, or pastebins as C2 or staging; traffic to these looks normal and is hard to block broadly.

# TODO
- rev https stub (use logic from tcp stub)
