package opg1;

import java.util.Scanner;

public class Server {
    Scanner in = new Scanner(System.in);

    public void main(String[] args){
        String input = in.nextLine();
        String[] numers = input.split("\\+, -");
        if(input.contains("+")){

        }
    }

    public int addition(int a, int b){
        return a + b;
    }

    public int subtraction(int a, int b){
        return a - b;
    }
}
