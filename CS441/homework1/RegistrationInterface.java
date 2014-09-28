package hospital.registration;

import java.rmi.Remote;
import java.rmi.RemoteException;

public interface RegistrationInterface extends Remote {

    public Visit createRegistration(String firstName, String lastName, int id, String host) throws RemoteException;

}
