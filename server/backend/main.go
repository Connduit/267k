package main

import (
	"encoding/json"
	"log"
	"net/http"
)

type Message struct {
	Text string `json:"text"`
}

func main() {
	http.HandleFunc("/api/hello", func(w http.ResponseWriter, r *http.Request) {
		// Important: Allow requests from your HTML file
		w.Header().Set("Access-Control-Allow-Origin", "*")
		w.Header().Set("Content-Type", "application/json")

		message := Message{Text: "Hello from Go Backend!"}
		json.NewEncoder(w).Encode(message)
	})

	log.Println("Go server running on :8080")
	http.ListenAndServe(":8080", nil)
}
