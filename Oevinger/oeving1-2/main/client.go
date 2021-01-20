

// socket client for golang
// https://golangr.com
package main

import "net"
import "fmt"
import "bufio"
import "os"

func main() {

	// connect to server
	conn, _ := net.Dial("tcp", "127.0.0.1:8000")
	for {
		// what to send?
		reader := bufio.NewReader(os.Stdin)
		fmt.Print("Text to send: ")
		num1, _ := reader.ReadString('\n')
		fmt.Fprintf(conn, num1 )

		operation, _ := reader.ReadString('\n')
		fmt.Fprintf(conn, operation )

		num2, _ := reader.ReadString('\n')

		// send to server
		fmt.Fprintf(conn, num2)

		// wait for reply
		message, _ := bufio.NewReader(conn).ReadString('\n')
		fmt.Print("Message from server: "+message)
	}
}