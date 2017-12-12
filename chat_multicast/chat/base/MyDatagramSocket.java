import java.net.*;
import java.io.*;

/**
 * A subclass of DatagramSocket which contains 
 * methods for sending and receiving messages
 * @author M. L. Liu
 */
public class MyDatagramSocket extends DatagramSocket {
   static final int MAX_LEN = 100;  
   MyDatagramSocket(int portNo)  throws SocketException{
     super(portNo);
   }
   public void sendMessage(InetAddress receiverHost, int receiverPort,                  
                           String message) throws IOException {
   		          	
         byte[ ] sendBuffer = message.getBytes( );                                     
         DatagramPacket datagram = 
            new DatagramPacket(sendBuffer, sendBuffer.length, 
                               receiverHost, receiverPort);
         this.send(datagram);
   } // end sendMessage

   public String receiveMessage()
		  throws IOException {		
      byte[ ] receiveBuffer = new byte[MAX_LEN];
      DatagramPacket datagram =
        new DatagramPacket(receiveBuffer, MAX_LEN);
      //this.setSoTimeout(3000);
      this.receive(datagram);
      String message = new String(receiveBuffer);
      return message;
   } //end receiveMessage


   public void writeInfo(String sender, int port){
        try {
                BufferedWriter out = new BufferedWriter(new FileWriter(sender));
                InetAddress addr = InetAddress.getLocalHost();
                String hostname = addr.getHostAddress();
                out.write(hostname);
                System.out.println(port);
                out.newLine();
                out.write(Integer.toString(port));
                out.close();
            } catch (IOException e) {
            }
    }
   public String[] getInfo(String receiver){
        try {
                BufferedReader in = new BufferedReader(new FileReader(receiver));
                String machine;
                String port;
                machine = in.readLine();
                port = in.readLine();
                System.out.println(machine);
                System.out.println(port);
                in.close();
                return new String[] { machine, port };
            } catch (IOException e) {
                throw new RuntimeException("Error while reading");
            }
    }
} //end class

