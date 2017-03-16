import java.io.BufferedReader;
import java.io.DataOutputStream;
import java.io.File;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;
import java.net.SocketException;
import java.net.UnknownHostException;
import java.util.ArrayList;

public class TCPConnection {
	private String server;
	private int portNum;
	private boolean bufferControl;
	
	private Socket socket;
	private BufferedReader in;
	private DataOutputStream out;
	private PrintWriter pw;
	private ServerReader serv;
	private ArrayList<String> keywords;
		
		public TCPConnection(String s, int p){
				server = s;
				portNum = p;
				bufferControl = false;
				keywords = new ArrayList<String>();
				populateKeywords();
			}
			
			public class ServerReader extends Thread{
				private BufferedReader in;
				private ArrayList<String> buffer;
				private String lastMsg = "";
				
				public ServerReader(BufferedReader serv){
					this.in = serv;
					this.buffer = new ArrayList<String>();
					Main.getMsg("Server message thread initialized");
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
							
							if(!keywords.contains(lastMsg)){								
								Main.getMsg("S: "+lastMsg);
								buffer.add(lastMsg);							
							}
							
							else{
								//!(lastMsg.equals("ACK")) && !(lastMsg.equals("BLKRLS"))
								if(lastMsg.equals("BLKRLS")){
									bufferControl = false;
								}								
							}
											
						} catch (IOException e) {
							Main.getMsg("Connection closed.");
							try {
								this.join(0);
							} catch (InterruptedException e1) {
								e1.printStackTrace();
							}
						}
					}
					
					Main.getMsg("***** Server closing connection ******");
					try {
						closeConnection();
					} catch (IOException e) {
						e.printStackTrace();
					}
				}
	
			}

			public void init() throws UnknownHostException, IOException{
				socket = new Socket(server,portNum);
				in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
				out = new DataOutputStream(socket.getOutputStream());
				pw = new PrintWriter(socket.getOutputStream(),true);			
			}
			
			public void closeConnection() throws IOException{
				in.close();
				out.close();
				socket.close();				
			}
			
			public void populateKeywords(){
				keywords.add("ACK");
				keywords.add("BLKRLS");
				keywords.add("OPCOMPLETE");
				keywords.add("key:!");
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
			
			public void sendMsg(String s){
				pw.print(s);
			}
					
			public void sendMsgLn(String s){
				pw.println(s);
				pw.flush();				
			}
			
			public void setBlock(boolean b){
				bufferControl = b;
			}
					
			public boolean getBlock(){
				return bufferControl;
			}
			
			public ArrayList<String> getServerBuffer(){
				return serv.getBuffer();
			}
			
			public int getServerBufferSize(){
				return serv.getBufferSize();
			}
}
