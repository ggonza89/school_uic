package hospital.registration;

import java.rmi.*;
import java.io.Serializable;
import java.util.Date;

public class Visit implements Serializable {

    private Registration visitor;
    private Date visitationDate;

    public Visit(Registration v, Date d) {

        visitor = v;
        visitationDate = d;

    }

    public Registration getRegistration() {
        return visitor;
    }

    public Date getDate() {
        return visitationDate;
    }

    public String toString() {
        return "Visitor " + visitor.getID() + ": " + visitor.getLastName() + ", " + visitor.getFirstName() + " came in on " + visitationDate;
    }

}
