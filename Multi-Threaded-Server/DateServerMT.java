/*
 * Han Pham
 * CSS 430 B Au 23 Operating System
 * Professor Robert Dimpsey
 * Multi-Threaded DataServer Class
 */

import java.net.*;
import java.io.*;

public class DateServerMT {

    // check for user command line arguments
    public static void main(String[] args) {
        if (args.length != 1) {
            System.out.println("Please provide a port");
            System.exit(1);
        }

        // convert port argument to int
        int port = Integer.parseInt(args[0]);

        try {
            // create a server socket to listen for client connections with the specified
            // port
            ServerSocket sock = new ServerSocket(port);
            while (true) {
                Socket client = sock.accept();

                // create a new thread every incoming client
                Thread workerThread = new Thread(new Worker(client));

                // execute the new executed thread
                workerThread.start();
            }
        } catch (IOException ie) {
            System.err.println(ie);
        }
    }
}
