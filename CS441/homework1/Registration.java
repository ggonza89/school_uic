package hospital.registration;

import java.rmi.*;
import java.io.Serializable;

public class Registration implements Serializable {

    private String firstName;
    private String lastName;
    private int id;

    public Registration(String f, String l, int i) {

        firstName = f;
        lastName = l;
        id = i;

    }

    public String getFirstName() {
        return firstName;
    }

    public String getLastName() {
        return lastName;
    }

    public int getID() {
        return id;
    }

    public String toString() {

        return "First Name: " + getFirstName() + " Last Name: " +
                getLastName() + " ID: " + getID();

    }

}
