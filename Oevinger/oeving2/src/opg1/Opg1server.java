package opg1;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.SocketException;
import java.nio.charset.StandardCharsets;
import java.util.Arrays;

public class Opg1server extends Thread{
    private DatagramSocket socket;
    private boolean running;
    private byte[] buf = new byte[256];

    public static void main(String[] args) throws SocketException {
        new Opg1server().start();
    }
    public Opg1server() throws SocketException {
        socket = new DatagramSocket(4445);
    }

    public void run() {
        running = true;

        while (running) {
            buf = new byte[256];
            DatagramPacket packet = new DatagramPacket(buf, buf.length);
            try {
                socket.receive(packet);
            } catch (IOException e) {
                e.printStackTrace();
            }

            String s = new String(buf, StandardCharsets.UTF_8);
            String[] nums = s.split(",");

            int num1 = Integer.parseInt(nums[0]);
            int num2 = Integer.parseInt(nums[2].trim());
            String response;
            if (nums[1].equals("+")){
                int sum = num1 + num2;
                response = "" + sum;
            } else if (nums[1].equals("-")){
                int sum = num1 - num2;
                response = "" + sum;
            }
            else {
                response = "error";
            }
            buf = response.getBytes();

            InetAddress address = packet.getAddress();
            int port = packet.getPort();
            packet = new DatagramPacket(buf, buf.length, address, port);
            String received = new String(packet.getData(), 0, packet.getLength());

            if (received.equals("end")) {
                running = false;
                continue;
            }
            try {
                socket.send(packet);
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
        socket.close();
    }
}
