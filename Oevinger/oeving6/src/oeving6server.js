
var crypto = require('crypto')
const net = require('net');
const { createHash } = require('crypto');


// Simple HTTP server responds with a simple WebSocket client test
const httpServer = net.createServer((connection) => {
    connection.on('data', () => {
        let content = `<!DOCTYPE html>
<html>
  <head>
    <meta charset="UTF-8" />
  </head>
  <body>
    WebSocket test page
    <script>
      let ws = new WebSocket('ws://localhost:3001');
      ws.onmessage = event => alert('Message from server: ' + event.data);
      ws.onopen = () => ws.send('hello');
      
    </script>
  </body>
</html>
`;
        connection.write('HTTP/1.1 200 OK\r\nContent-Length: ' + content.length + '\r\n\r\n' + content);
    });
});
httpServer.listen(3000, () => {
    console.log('HTTP server listening on port 3000');
});

// Incomplete WebSocket server
const wsServer = net.createServer((connection) => {
    console.log('Client connected');

    connection.on('data', (data) => {
        console.log('Data received from client: ', data.toString());
        //todo nothing is more permanent than a temporary solution, this is very scuffed
        let key = (data.toString().substr((data.toString()).indexOf("Sec-WebSocket-Key") + "Sec-WebSocket-Key".length+2, "VhkJDv+ei89JaGJpX6i4AQ==".length));

        const hash = createHash('sha1')
            .update(key + '258EAFA5-E914-47DA-95CA-C5AB0DC85B11')
            .digest('base64');

        const responseHeaders = [ 'HTTP/1.1 101 Web Socket Protocol Handshake', 'Upgrade: WebSocket', 'Connection: Upgrade', `Sec-WebSocket-Accept: ${hash}` , 'Sec-WebSocket-Protocol: json'];
        let response = responseHeaders.join('\r\n') + '\r\n\r\n';
        console.log(response);
        connection.write(response);
        connection.pipe(connection);

    });


    connection.write("hello");

    connection.on('end', () => {

        console.log('Client disconnected');
    });
});
wsServer.on('error', (error) => {

    console.error('Error: ', error);
});
wsServer.listen(3001, () => {

    console.log('WebSocket server listening on port 3001');
});

function generateAcceptValue (acceptKey) {
    return crypto
        .createHash('sha1')
        .update(acceptKey + '258EAFA5-E914–47DA-95CA-C5AB0DC85B11', 'binary')
        .digest('base64');
}