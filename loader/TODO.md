# Loader

### Methods
- basic: alloc -> protect -> CreateThread -> execute
- Reflective DLL 
- Process Hollowing
- just add a shellcode execution feature in other process memory and get output via named pipe to your beacon and Transfer that to server

### Strategies
- !sandbox && !vm -> gain persistence -> download main modules/payload from server

        1. Initial Infection
           ↓
        2. Raw Loader Installed (persistence)
           ↓  
        3. First Beacon → C&C Profiling
           ↓
        4. C&C Sends Appropriate Shellcode Modules:
           - Corporate network → Infostealer + Lateral movement
           - Home user → Cryptominer + Password stealer
           - Government → Document collector + Keylogger
           ↓
        5. Modules Execute → Report Back
           ↓
        6. C&C Sends Updated Modules as Needed



### TODO
- powershell script... WMI_Script.ps1?
