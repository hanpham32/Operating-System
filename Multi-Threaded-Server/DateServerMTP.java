/*
 * Han PHam
 * CSS 430 B Au 23 Operating System
 * Professor Robert Dimpsey
 * Multi-Threaded Java Pool DateServer Class
 */

import java.net.*;
import java.util.concurrent.*;
import java.io.IOException;

public class DateServerMTP {
    public static void main(String[] args) {
        if (args.length != 1) {
            System.out.println("Usage: java DateServerMTP <port>");
            System.exit(1);
        }

        int port = Integer.parseInt(args[0]);
        ExecutorService executor = Executors.newFixedThreadPool(10); // Example: Create a thread pool of 10

        try {
            ServerSocket sock = new ServerSocket(port);
            while (true) {
                Socket client = sock.accept();
                executor.execute(new Worker(client));
            }
        } catch (IOException ie) {
            System.err.println(ie);
        } finally {
            executor.shutdown();
        }
    }
}