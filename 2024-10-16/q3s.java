// Q2 : Convert string to upper case using client server program with TCP in Java

import java.io.*;
import java.net.*;

public class q3s {
    public static void main(String[] args) {
        try (ServerSocket serverSocket = new ServerSocket(6789)) {
            System.out.println("Server is running and waiting for clients...");
            while (true) {
                Socket clientSocket = serverSocket.accept();
                System.out.println("Client connected!");
                new ClientHandler(clientSocket).start();
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}

class ClientHandler extends Thread {
    private Socket clientSocket;
    public ClientHandler(Socket socket) {
        this.clientSocket = socket;
    }
    public void run() {
        try (
                BufferedReader inFromClient = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));
                DataOutputStream outToClient = new DataOutputStream(clientSocket.getOutputStream())
        ) {
            String clientInput;

            while ((clientInput = inFromClient.readLine()) != null) {
                System.out.println("Received from client: " + clientInput);
                String response = clientInput.toUpperCase();
                outToClient.writeBytes(response + "\n");
                System.out.println("Sent to client: " + response);
            }

            System.out.println("Client disconnected.");
        } catch (IOException e) {
            System.out.println("Connection with client lost.");
        } finally {
            try {
                clientSocket.close();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }
}
