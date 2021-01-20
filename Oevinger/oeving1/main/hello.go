package main

import (
	"bufio"
	"fmt"
	"net"
	"os"
	"strconv"
	"strings"
)





func main() {

	fmt.Println("Start server...")

	// listen on port 8000
	ln, _ := net.Listen("tcp", ":8000")

	var allClients = make(map[net.Conn]int)

	connection, err := ln.Accept()

	// run loop forever (or until ctrl-c)
	for {
		print("hei122313123")
		// accept connection

		if err != nil {
			os.Exit(1)
		}

		allClients[connection] = 1

		for conn, _ := range allClients{
			// get message, output
			str1, _ := bufio.NewReader(conn).ReadString('\n')
			operation, _ := bufio.NewReader(conn).ReadString('\n')
			str2, _ := bufio.NewReader(conn).ReadString('\n')

			num1, err1 := strconv.ParseInt(strings.TrimSpace(str1), 10, 64)
			num2, err2 := strconv.ParseInt(strings.TrimSpace(str2), 10, 64)

			if err1 == nil && err2 == nil {
				var ans int64
				bool := true
				if operation[0] == '+' {
					ans = num1 + num2
				} else if operation[0] == '-' {
					ans = num1 - num2
				} else {
					ans = -1
					bool = false
				}
				if bool {
					fmt.Println("   ")
					fmt.Println(ans)
					fmt.Fprintf(conn, strconv.Itoa(int(ans))+"\n")

				} else {
					fmt.Fprintf(conn, "skriv et ordentlig tegn jævla idiot"+"\n")

				}
			} else{
				fmt.Fprintf(conn, "skriv to ordentlige tall jævla idiot"+"\n")

			}

		}

	}
}