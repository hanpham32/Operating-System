/*
 * Han PHam
 * CSS 430 B Au 23 Operating System
 * Professor Robert Dimpsey
 * Worker Class
 */

import java.net.*;
import java.io.*;

public class Worker implements Runnable {
    private Socket client;

    public Worker(Socket client) {
        this.client = client;
    }

    @Override
    public void run() {
        try {
            PrintWriter pout = new PrintWriter(client.getOutputStream(), true);

            // Simulate 5000ms of work
            Thread.sleep(5000);

            // Write the Date to the socket
            pout.println(new java.util.Date().toString());

            client.close();
        } catch (IOException | InterruptedException e) {
            System.err.println(e);
        }
    }
}