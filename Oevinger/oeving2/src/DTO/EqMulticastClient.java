package DTO;

import java.net.*;
import java.io.*;

public class EqMulticastClient {

    public static void main(String[] args) {

        try (Socket socket = new Socket("localhost", 8081)) {

            InputStream input = socket.getInputStream();

            ObjectInputStream objInput = new ObjectInputStream(input);
            Equation eq = (Equation) objInput.readObject();

            System.out.println(eq.getNum1() + (eq.isAddition() ? eq.getNum2() : eq.getNum2()*-1));

        } catch (UnknownHostException ex) {

            System.out.println("Server not found: " + ex.getMessage());

        } catch (IOException ex) {

            System.out.println("I/O error: " + ex.getMessage());
        } catch (ClassNotFoundException e) {
            e.printStackTrace();
        }
    }
}