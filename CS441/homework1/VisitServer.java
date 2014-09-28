package hospital.registration;

import java.rmi.registry.Registry;
import java.rmi.registry.LocateRegistry;
import java.rmi.RemoteException;
import java.rmi.server.UnicastRemoteObject;
import java.util.Date;

public class VisitServer implements VisitInterface {

    public VisitServer() throws RemoteException {

        // super();

    }

    public Visit createVisitor(Registration registration, Date date) {


        Visit visitor = new Visit(registration, date);
        System.err.println(visitor);
        return visitor;

    }

    public static void main(String args[]) {

        System.err.println("Initializing Visitor server: please wait...");
        // create server object
        try {

            VisitServer obj = new VisitServer();
            VisitInterface stub = (VisitInterface) UnicastRemoteObject.exportObject(
                obj, 0);

            // bind RegistrationServer object to rmiregistry
            Registry registry = LocateRegistry.getRegistry();
            registry.rebind("VisitInterface", stub);

            System.err.println("Visitor Server ready");

        } catch(Exception e) {

            System.err.println("Visit Server exception: " + e.toString());
            e.printStackTrace();

        }
    }

}
