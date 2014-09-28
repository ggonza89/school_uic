Giancarlo Gonzalez
net-id: ggonza20
September 28, 2014

I have built two servers.  One is a registration Server and client that asks my second server, a Visitor server, for a Visit object then returns the visitor to my registration server and then returns it back to the Registration client.  To show referential integrity I have printed out the visitors content that contains the full name you entered in the Registration client and the date of visitation, which is just the date you created the object.

To compile run the following command:
javac -d . Registration.java RegistrationInterface.java RegistrationClient.java Visit.java VisitInterface.java VisitServer.java RegistrationServer.java

Then run the following commands in separate terminal windows/tabs:

rmiregistry &

java -classpath . -Djava.rmi.server.codebase=file:. hospital.registration.RegistrationServer

java -classpath . -Djava.rmi.server.codebase=file:. hospital.registration.VisitServer &

java  -classpath . hospital.registration.RegistrationClient

The final command/window will prompt you for a full name then print out the Visitor information and the date you visited (which is the date the object was created).

This concludes the first homework and shows referential integrity held within two servers and one standalone client.
