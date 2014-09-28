package hospital.registration;

import java.rmi.registry.LocateRegistry;
import java.rmi.registry.Registry;
import java.util.Date;
import java.io.Console;

public class RegistrationClient {

    private RegistrationClient() {}

    public static void main(String[] args) {

        String host = (args.length < 1) ? null : args[0];

        try {

            Registry registry = LocateRegistry.getRegistry(host);
            RegistrationInterface reg_stub = (RegistrationInterface) registry.lookup("RegistrationInterface");

            Console console = System.console();
            if (console == null) {

                System.err.println("No console.");
                System.exit(1);

            }

            String fullname = console.readLine("Enter your first and last name: ");

            String [] name = fullname.split("\\s+");

            Visit visitor = reg_stub.createRegistration(name[0], name[1], 1, host);
            System.out.println(visitor);

        } catch (Exception e) {

            System.err.println("Registration Client exception: " + e.toString());
            e.printStackTrace();

        }

    }

}
