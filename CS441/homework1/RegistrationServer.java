package hospital.registration;

import java.rmi.registry.Registry;
import java.rmi.registry.LocateRegistry;
import java.rmi.RemoteException;
import java.rmi.server.UnicastRemoteObject;
import java.util.Date;

public class RegistrationServer implements RegistrationInterface {

    public RegistrationServer() throws RemoteException {

        // super();

    }

    public Visit createRegistration(String firstName, String lastName, int id, String host) {

        Registration reg = new Registration(firstName, lastName, id);
        try {

            Registry registry = LocateRegistry.getRegistry(host);
            VisitInterface vis_stub = (VisitInterface) registry.lookup("VisitInterface");
            Visit visitor1 = vis_stub.createVisitor(reg, new Date());
            System.out.println(visitor1);
            return visitor1;


        } catch(Exception e) {

            System.err.println("createRegistration exception: " + e.toString());
            e.printStackTrace();

            return null;

        }

    }

    public static void main(String args[]) {

        System.err.println("Initializing Registration server: please wait...");
        // create server object
        try {

            RegistrationServer obj = new RegistrationServer();
            RegistrationInterface stub = (RegistrationInterface) UnicastRemoteObject.exportObject(
                obj, 0);

            // bind RegistrationServer object to rmiregistry
            Registry registry = LocateRegistry.getRegistry();
            registry.rebind("RegistrationInterface", stub);

            System.err.println("Registration Server ready");

        } catch(Exception e) {

            System.err.println("Registration Server exception: " + e.toString());
            e.printStackTrace();

        }
    }

}
