package DTO;

import java.io.Serializable;

/**
 * represents addition or subtraction
 */
public class Equation implements Serializable {
    private int num1;
    private int num2;
    private boolean addition;

    public Equation(int num1, boolean addition , int num2) {
        this.num1 = num1;
        this.num2 = num2;
        this.addition = addition;
    }

    public int getNum1() {
        return num1;
    }

    public int getNum2() {
        return num2;
    }

    public boolean isAddition() {
        return addition;
    }
}
