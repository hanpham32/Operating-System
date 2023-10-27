/*
 * Han Pham
 * CSS 430 B Au 23 Operating System
 * Professor Robert Dimpsey
 * Worker Class:
 * Handles client's request, simulates 5000ms work and sending the date
 */

import java.net.*;
import java.io.*;

public class Worker implements Runnable {

    // declare socket
    private Socket client;

    // constructor
    public Worker(Socket client) {
        this.client = client;
    }

    // executed when a thread started using an instance of this class
    @Override
    public void run() {
        try {

            // a PrintWriter object to send data to the client
            PrintWriter pout = new PrintWriter(client.getOutputStream(), true);

            // Simulate a work delay of 5000ms (5 seconds)
            try {
                Thread.sleep(5000);
            } catch (InterruptedException e) {
            }
            // send the current date and time
            pout.println(new java.util.Date().toString());

            // close client after sending the date
            client.close();
        } catch (IOException ie) {
            System.err.println(ie);
        }
    }
}