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
- encryption of stub
- hide code in different/unusal sections of the PE

# TODO
- rev https stub
