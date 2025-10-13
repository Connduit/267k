# Server

### TODO
- backend: written in go or python?
- gui/frontend (website): react
- gui/frontend (exe): written in c++ qt gui? (do this after react implementation?)

### Diagram

          ┌───────────────────────────────┐
          │       Frontend (Mythic UI)    │
          │  - Web interface for operator │
          └──────────────┬────────────────┘
                         │  REST/WebSocket
                         ▼
          ┌───────────────────────────────┐
          │   Mythic Core (Backend)       │
          │  - Handles tasking, DB, auth  │
          │  - Loads C2 profiles + agents │
          └──────────────┬────────────────┘
                         │
          ┌───────────────────────────────┐
          │     C2 Profile (Python)       │
          │  - Defines network layer      │
          │  - Controls how agents talk   │
          │    to Mythic (format, routes) │
          └──────────────┬────────────────┘
                         │
                  Network / Internet
                         │
          ┌───────────────────────────────┐
          │      Agent (Payload)          │
          │  - Written in C/C++/Go/etc.   │
          │  - Communicates via C2 Profile│
          └───────────────────────────────┘

