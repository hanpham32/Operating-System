/*
 * Han Pham
 * CSS 430 B Au 23 Operating System
 * Professor Robert Dimpsey
 * Multi-Threaded Java Pool DateServer Class
 */

import java.net.*;
import java.util.concurrent.*;
import java.io.IOException;

public class DateServerMTP {
    public static void main(String[] args) {

        // check for user command line arguments
        if (args.length != 1) {
            System.out.println("Please provide a port");
            System.exit(1);
        }

        // convert port argument to int
        int port = Integer.parseInt(args[0]);

        // create a thread pool executor service to handle request concurrently
        ExecutorService executor = Executors.newFixedThreadPool(20); // thread pool of 20

        try {
            // create a server socket to listen for client connections with the specified
            // port
            ServerSocket sock = new ServerSocket(port);

            // run a loop to listen for connections
            while (true) {
                Socket client = sock.accept();

                // execute client requests
                executor.execute(new Worker(client));
            }
        } catch (IOException ie) {
            System.err.println(ie);
        } finally {
            executor.shutdown();
        }
    }
}