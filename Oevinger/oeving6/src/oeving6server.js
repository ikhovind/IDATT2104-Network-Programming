
var crypto = require('crypto')
const net = require('net');

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
        //string defined my rfc6455
        key.concat("258EAFA5-E914-47DA-95CA-C5AB0DC85B11");
        console.log(key);
        let shasum = crypto.createHash('sha1');
        shasum.update(key);
        let hash = shasum.digest('hex');
        console.log(hash + '\n');
        let httpResponse = ('HTTP/1.1 101 Switching Protocols\r\n' +
        'Upgrade: websocket\r\n' +
        'Connection: Upgrade\r\n' +
        'Sec-WebSocket-Accept: ' + hash +'\n');

        console.log('response written to client:\n' + httpResponse);
        connection.write(httpResponse);

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

