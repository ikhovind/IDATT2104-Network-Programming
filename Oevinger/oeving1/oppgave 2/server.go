package main

import (
	"fmt"
	"log"
	"net/http"
)

func indexHandler(w http.ResponseWriter, r *http.Request) {
	w.Header().Set("Content-Type", "text/html; charset=utf-8")
	w.Header().Set("Status", "HTTP/1.0 200 OK")
	fmt.Fprintln(w, w.Header().Get("Status"))
	fmt.Fprintln(w, "<br>")
	fmt.Fprintln(w, w.Header().Get("Content-Type"))

	fmt.Fprintf(w,
		"<!DOCTYPE html>"+
			"<html lang=\"en\">"+
			"<head>"+
			"    <meta charset=\"UTF-8\">"+
			"    <title>Home</title>"+
			"</head>"+
			"<body>"+
			"<h1>heeder</h1>"+
			"<ul>")
	for name, values := range r.Header {
		for _, value := range values {
			fmt.Fprintf(w, "<li>"+name+": "+value+"</li>")
		}
	}

	fmt.Fprint(w,
		"</ul>"+
			"</body>\n</html>")

	defer r.Body.Close()
}

func main() {
	http.HandleFunc("/", indexHandler)

	fmt.Printf("starting server wowowowowow")
	if err := http.ListenAndServe(":8080", nil); err != nil {
		log.Fatal(err)
	}

}
