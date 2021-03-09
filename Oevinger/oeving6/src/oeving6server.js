
const readline = require('readline');
const { createHash } = require('crypto');
const http = require('http');
const rl = readline.createInterface({
    input: process.stdin,
    output: process.stdout
});

// Simple HTTP server responds with a simple WebSocket client test
const requestListener = function (req, res) {

    res.setHeader("Content-Type", "text/html");
    let content = `<!DOCTYPE html>
<html>
  <head>
    <meta charset="UTF-8" />    
  </head>
  <body>
    WebSocket test page
    <script>
      let ws = new WebSocket('ws://localhost:8080');
      ws.onmessage = event => alert('Message from server: ' + event.data);
      ws.onopen = () => ws.send("hello from client");
      ws.onmessage = (event) => console.log(event.data);
      
      
    </script>
  </body>
</html>
`;

        res.writeHead(200);
        res.end(content);
};
const server = http.createServer(requestListener);
server.listen(8080);

server.on('upgrade', (req, socket) => {
    // Read the websocket key provided by the client:
    const acceptKey = req.headers['sec-websocket-key'];
    // Generate the response value to use in the response:
    const hash = createHash('sha1')
        .update(acceptKey + '258EAFA5-E914-47DA-95CA-C5AB0DC85B11')
        .digest('base64');
    // Write the HTTP response into an array of response lines:
    const responseHeaders = [ 'HTTP/1.1 101 Web Socket Protocol Handshake', 'Upgrade: WebSocket', 'Connection: Upgrade', `Sec-WebSocket-Accept: ${hash}` ];


    // Read the subprotocol from the client request headers:
    const protocol = req.headers['sec-websocket-protocol'];
// If provided, they'll be formatted as a comma-delimited string of protocol
// names that the client supports; we'll need to parse the header value, if
// provided, and see what options the client is offering:
    const protocols = !protocol ? [] : protocol.split(',').map(s => s.trim());
// To keep it simple, we'll just see if JSON was an option, and if so, include
// it in the HTTP response:
    if (protocols.includes('json')) {
        // Tell the client that we agree to communicate with JSON data
        responseHeaders.push(`Sec-WebSocket-Protocol: json`);
    }
    // Write the response back to the client socket, being sure to append two
    // additional newlines so that the browser recognises the end of the response
    // header and doesn't continue to wait for more header data:
    console.log(responseHeaders.join('\r\n') + '\r\n\r\n');
    socket.write(responseHeaders.join('\r\n') + '\r\n\r\n');

    //is run when client calls ws.send
    socket.on("data", (req, socket) => {
        let response = [];
        if(req[0] == parseInt("81", 16)){
            //masked if first bit is 1
            let masked = (req[1] & 128) === 128
            //length in bytes given by next seven bits
            let length = (req[1] & 127);
            let dataStart = 6;
            for(let i = dataStart; i < dataStart + length; i++){
                let byte = req[i] ^ req[2 + ((i - dataStart) % 4)];
                response[i - dataStart] = (String.fromCharCode(byte));
            }
            console.log(response.join(""));
        }
    });

    let resp = "hello from server"
    socket.write(getReplyBuffer(resp));

    function test(){
        console.log("success");
    }
});


function getReplyBuffer(data){
    let bytes = [0x81];
    bytes.push(data.length);
    for(let i = 0; i < data.length; i+=1){
        bytes.push(data.charCodeAt(i));
    }

    return Buffer.from(bytes);
}

