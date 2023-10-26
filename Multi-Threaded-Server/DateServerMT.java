/*
 * Han PHam
 * CSS 430 B Au 23 Operating System
 * Professor Robert Dimpsey
 * Multi-Threaded DataServer Class
 */

import java.net.*;
import java.io.*;

public class DateServerMT {
    public static void main(String[] args) {
        if (args.length != 1) {
            System.out.println("Usage: java DateServerMT <port>");
            System.exit(1);
        }

        int port = Integer.parseInt(args[0]);

        try {
            ServerSocket sock = new ServerSocket(port);
            while (true) {
                Socket client = sock.accept();
                Thread workerThread = new Thread(new Worker(client));
                workerThread.start();
            }
        } catch (IOException ie) {
            System.err.println(ie);
        }
    }
}
