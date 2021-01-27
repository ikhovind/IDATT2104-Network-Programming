package opg1;

import java.io.IOException;
import java.net.*;
import java.util.Scanner;

public class Opg1client {
    private DatagramSocket socket;
    private InetAddress address;
    private byte[] buf;

    public static int test = 4;


    public static void main(String[] args) throws IOException {
        Opg1client client = new Opg1client();

        while(true) {
            Scanner in = new Scanner(System.in);
            int num1 = getNum();
            boolean operation = getOperation();
            int num2 = getNum();
            System.out.println(client.sendNums(num1,num2,operation));
        }
    }

    public static boolean getOperation(){
        Scanner in = new Scanner(System.in);
        System.out.println("Skriv inn operasjon (+ eller -)");
        while(true){
            String op = in.nextLine();
            if(op.equals("-")){
                return false;
            }
            if (op.equals("+")){
                return true;
            }
        }

    }
    public static int getNum(){
        while (true) {
            System.out.printf("Skriv inn tall:");
            Scanner in = new Scanner(System.in);
            String numStr = in.nextLine();
            try{
                int tall = Integer.parseInt(numStr);
                return tall;
            } catch (NumberFormatException e){
                System.out.println("Vennligst skriv inn kun tall med eller uten fortegn, ingen mellomrom mellom fortegn og tall");
            }
        }
    }
    public Opg1client() throws UnknownHostException, SocketException {
        socket = new DatagramSocket();
        address = InetAddress.getByName("localhost");
    }

    public String sendNums(int num1, int num2, boolean addition) throws IOException {
        String msg = num1 + "," + (addition ? "+" : "-") + "," + num2;
        buf = msg.getBytes();
        DatagramPacket packet
                = new DatagramPacket(buf, buf.length, address, 4445);
        socket.send(packet);
        packet = new DatagramPacket(buf, buf.length);
        socket.receive(packet);
        String received = new String(
                packet.getData(), 0, packet.getLength());
        return received;
    }

    public void close() {
        socket.close();
    }
}