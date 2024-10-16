import java.io.*;
import java.net.*;

public class q3c {
    public static void main(String[] args) {
        try (Socket socket = new Socket("localhost", 6789)) {
            System.out.println("Connected to the server!");

            BufferedReader userInput = new BufferedReader(new InputStreamReader(System.in));
            DataOutputStream outToServer = new DataOutputStream(socket.getOutputStream());
            BufferedReader inFromServer = new BufferedReader(new InputStreamReader(socket.getInputStream()));

            System.out.print("Enter a string: ");
            String sentence = userInput.readLine();

            outToServer.writeBytes(sentence + "\n");

            String response = inFromServer.readLine();
            System.out.println("Received from server: " + response);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}