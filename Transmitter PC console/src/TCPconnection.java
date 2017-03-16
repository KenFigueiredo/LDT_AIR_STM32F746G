

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;
import java.net.UnknownHostException;
import java.util.ArrayList;

public class TCPconnection {
	private String server;
	private int portNum;
	
	private Socket socket;
	private BufferedReader in;
	private PrintWriter out;
	private ServerReader serv;
		
		public TCPconnection(String s, int p){
				server = s;
				portNum = p;
			}
			
			public class ServerReader extends Thread{
				private BufferedReader in;
				private ArrayList<String> buffer;
				private String lastMsg = "";
				
				public ServerReader(BufferedReader serv){
					this.in = serv;
					this.buffer = new ArrayList<String>();
					System.out.println("Server message thread initialized");
				}
				
				public String read() throws IOException{
					String msg;
					while((msg = in.readLine()) != null){
						return msg;
					}
					return null;
				}
				
				public ArrayList<String> getBuffer(){
					return buffer;
				}
				
				public int getBufferSize(){
					return buffer.size();
				}
				
				public void run(){
					while(lastMsg != null){
						try {
							lastMsg = read();
							System.out.println("> " + lastMsg);
							buffer.add(lastMsg);
						
						} catch (IOException e) {
							this.stop();
						}
					}
				}
	
			}

			public void init() throws UnknownHostException, IOException{
				socket = new Socket(server,portNum);
				in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
				out = new PrintWriter(socket.getOutputStream(),true);	
			}
			
			public void closeConnection() throws IOException{
				in.close();
				out.close();
				socket.close();				
			}
			
			public void startServerReader(){
				serv = new ServerReader(in);
				serv.start();
			}
			
			public String getServerMsg() throws IOException{
				String serverMsg;
				while((serverMsg = in.readLine()) != null){
					return serverMsg;
				}
				return null;
			}
			
			public Socket getSocket(){
				return socket;
			}
					
			public void sendMsgln(String s){
				out.println(s);
			}
			
			public void sendMsg(String s){
				out.print(s);
			}
			
			public ArrayList<String> getServerBuffer(){
				return serv.getBuffer();
			}
			
			public int getServerBufferSize(){
				return serv.getBufferSize();
			}
}
