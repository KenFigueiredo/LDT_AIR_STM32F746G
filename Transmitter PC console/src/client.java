

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.InetAddress;
import java.net.UnknownHostException;

public class client {
	
	private static final String IP = "192.168.0.80";
	private static final int port = 5419;
	private static Frame frame;
	
	
	public static void main(String[] args){
		
		frame = new Frame();
		
//		TCPconnection tcp = new TCPconnection(IP,port);
//		BufferedReader userIn = new BufferedReader(new InputStreamReader(System.in));
//
//			try {
//				tcp.init();
//				tcp.startServerReader();
//				System.out.println("Connected to server on " + IP + "[" + tcp.getSocket().getInetAddress().toString() + "]");
//				tcp.sendMsgln(InetAddress.getLocalHost().toString());
//
//			}catch(UnknownHostException e){
//				System.err.println("Server could not be found " + IP);
//				System.exit(1);
//			}catch(IOException e){
//				System.err.println("Couldn't establish I/O to the Server!");
//				System.exit(1);
//			}	
//			
//			String userInput = "";
//			while(!userInput.equals("EXIT")){
//				try {
//					userInput = userIn.readLine();
//					tcp.sendMsgln(userInput);
//				} catch (IOException e) {
//					e.printStackTrace();
//				}
//			}
//			
//			System.out.println("Client program closing");
//			try {
//				tcp.closeConnection();
//			} catch (IOException e) {
//				e.printStackTrace();
////			}
	}

	public static String toBinaryString(String in){
			byte b[] = in.getBytes();
			String str = "";
			for(int i = 0; i < b.length; i++){
				for(int j = 0; j < 7; j++){
					str += "" + ((b[i] >> j) & 1);
				}				
			}
			return str;
		}

}
