package Multicast;

import DTO.Equation;

import java.io.IOException;
import java.io.PrintWriter;
import java.net.*;
import java.nio.charset.StandardCharsets;
import java.util.ArrayList;
import java.util.Scanner;

public class MulticastServer extends Thread{
    private static ServerSocket socket;
    private static ArrayList<Socket> connections = new ArrayList<>();

    public static void main(String[] args) {
        try {
            socket = new ServerSocket(8080);
            Thread connectionAccepter = new MulticastServer();
            connectionAccepter.start();
        } catch (IOException e){
            e.printStackTrace();
        }
        while (true) {
            System.out.println("Enter something you want to send to all connections:");
            Scanner scanner = new Scanner(System.in);
            String out = scanner.nextLine();
            connections.forEach(c -> {
                try {
                    new PrintWriter(c.getOutputStream(), true).println(out);
                } catch (IOException e) {
                    e.printStackTrace();
                }
            });
        }
    }

    public void run(){
        while(true) {
            try {
                Socket newCon = socket.accept();
                connections.add(newCon);
            } catch (IOException e) {
                e.printStackTrace();
            }
            System.out.println("New connection added");
        }
    }
}
