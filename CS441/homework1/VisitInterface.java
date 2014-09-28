package hospital.registration;

import java.rmi.Remote;
import java.rmi.RemoteException;
import java.util.Date;

public interface VisitInterface extends Remote {

    public Visit createVisitor(Registration registration, Date date) throws RemoteException;

}
