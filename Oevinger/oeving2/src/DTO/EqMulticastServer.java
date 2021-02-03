package DTO;

import Multicast.MulticastServer;

import java.io.IOException;
import java.io.ObjectOutputStream;
import java.net.*;
import java.util.ArrayList;
import java.util.Scanner;

public class EqMulticastServer extends Thread{
    private static ServerSocket socket;
    private static ArrayList<Socket> connections = new ArrayList<>();

    public static void main(String[] args) {
        try {
            socket = new ServerSocket(8081);
            Thread connectionAccepter = new EqMulticastServer();
            connectionAccepter.start();
        } catch (IOException e){
            e.printStackTrace();
        }
        while (true) {
            ArrayList<Socket> brokenConnections = new ArrayList<>();
            Equation eq = getEquation();
            connections.forEach(c -> {
                try {
                    new ObjectOutputStream(c.getOutputStream()).writeObject(eq);
                } catch (IOException e) {
                    brokenConnections.add(c);
                }
            });
            brokenConnections.forEach(c-> {
                try {
                    c.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            });

            brokenConnections.forEach(c-> connections.remove(c));
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

    public static Equation getEquation(){
        Scanner in = new Scanner(System.in);
        String num1 = in.nextLine();
        String operation = in.nextLine();
        String num2 = in.nextLine();
        return new Equation(Integer.parseInt(num1), operation.equals("+"), Integer.parseInt(num2));
    }

}
