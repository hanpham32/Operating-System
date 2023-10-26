/*
 * Han PHam
 * CSS 430 B Au 23 Operating System
 * Professor Robert Dimpsey
 * Multi-Threaded Pool DateServer Class
 */

import java.net.*;
import java.io.*;
import java.util.concurrent.*;

public class DateServerMTP {
    public static void main(String[] args) {
        if (args.length != 1) {
            System.out.println("Usage: java DateServerMTP <port>");
            System.exit(1);
        }

        int port = Integer.parseInt(args[0]);
        ExecutorService executor = Executors.newFixedThreadPool(10);

        try {
            ServerSocket sock = new ServerSocket(port);

            while (true) {
                Socket client = sock.accept();
                Worker worker = new Worker(client);
                executor.execute(worker);
            }
        } catch (IOException ioe) {
            System.err.println(ioe);
        } finally {
            if (!executor.isShutdown()) {
                executor.shutdown();
            }
        }
    }
}
