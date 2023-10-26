/*
 * Han PHam
 * CSS 430 B Au 23 Operating System
 * Professor Robert Dimpsey
 * Worker Class:
 * Handles client's request, simulates 5000ms work and sending the date
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
            // Simulated 5000ms work
            try {
                Thread.sleep(5000);
            } catch (InterruptedException e) {
            }
            pout.println(new java.util.Date().toString());
            client.close();
        } catch (IOException ie) {
            System.err.println(ie);
        }
    }
}