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


	// run loop forever (or until ctrl-c)
	for {
		// accept connection
		conn, err := ln.Accept()

		if err != nil {
			os.Exit(1)
		}

		go calculate(conn)
	}
}

func calculate(conn net.Conn) {
	// get message, output
	for {
		str1, _ := bufio.NewReader(conn).ReadString('\n')
		operation, _ := bufio.NewReader(conn).ReadString('\n')
		str2, _ := bufio.NewReader(conn).ReadString('\n')

		num1, err1 := strconv.ParseInt(strings.TrimSpace(str1), 10, 64)
		num2, err2 := strconv.ParseInt(strings.TrimSpace(str2), 10, 64)

		if err1 == nil && err2 == nil {
			var ans int64
			validoperation := true
			if operation[0] == '+' {
				ans = num1 + num2
			} else if operation[0] == '-' {
				ans = num1 - num2
			} else {
				ans = -1
				validoperation = false
			}
			if validoperation {
				fmt.Println("   ")
				fmt.Println(ans)
				fmt.Fprintf(conn, strconv.Itoa(int(ans))+"\n")

			} else {
				fmt.Fprintf(conn, "skriv et ordentlig tegn jævla idiot"+"\n")

			}
		} else {
			fmt.Fprintf(conn, "skriv to ordentlige tall jævla idiot"+"\n")

		}
	}
}