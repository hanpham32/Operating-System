/**
 * Created by rtdimpsey on 4/13/17.
 * Simulated 5000ms work time
 */
import java.net.*;
import java.io.*;

public class DateServer430
{
    public static void main(String[] args)
    {
        try
        {
            ServerSocket sock = new ServerSocket(6013);
            while (true)
            {
                Socket client = sock.accept();
                PrintWriter pout = new PrintWriter(client.getOutputStream(), true);
		try
		{
		    Thread.sleep(5000);
		}
		catch (InterruptedException e) {}
                pout.println(new java.util.Date().toString());
                client.close();
            }
        }
        catch (IOException ie) {}
    }
}
