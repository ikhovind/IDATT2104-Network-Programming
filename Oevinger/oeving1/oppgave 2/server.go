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
			"<ul>\n    <li>kule</li>\n    <li>is</li>\n</ul>"+
			"</body>\n</html>")

	/*ua := r.Header.Values("User-Agent")
	fmt.Fprint(w, ua)*/
}

func main() {
	http.HandleFunc("/", indexHandler)

	fmt.Printf("starting server wowowowowow")
	if err := http.ListenAndServe(":8080", nil); err != nil {
		log.Fatal(err)
	}

}
