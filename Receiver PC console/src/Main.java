import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.math.RoundingMode;
import java.net.InetAddress;
import java.net.UnknownHostException;
import java.text.DecimalFormat;
import java.util.ArrayList;

public class Main{
	
	private static Frame f;
	private static Thread t;
	private static boolean blocked, connectionActive;
	private static DecimalFormat df;
	
	public static void main(String[] args){
		df = new DecimalFormat("##.###");
		df.setRoundingMode(RoundingMode.DOWN);
		connectionActive = false;
		f = new Frame();
	}
		
		public static void setConnect(boolean b){
			connectionActive = b;
		}
		
		public static boolean getConnect(){
			return connectionActive;
		}
		
		public static void getMsg(String s){
			f.logLine(s);
		}
		
		public static void sendFile(File fl){
			threadedMain.sendFile(fl);		
		}
		
		public static void startThread(String IP, int port){
			t = new Thread(new threadedMain(IP,port));
			t.start();
		}
		
		public static void killThread(boolean src){
			threadedMain.closeConnect(src);
			blocked = true;			
		}
	
	public static class threadedMain implements Runnable{
		private BufferedReader userIn;
		private static TCPConnection tcp;
		private String IP;
		private int port;
		private boolean init;
		
		private ArrayList<String> keyWords;
		private static boolean isLoaded;
		
		public threadedMain(String ip, int port){	
			IP = ip;
			this.port = port;
			isLoaded = false;
			blocked = true;
			init = false;
			keyWords = new ArrayList<String>();
			setKeyWords();		
		}
			
			public static void closeConnect(boolean src){
				f.logLine("Client program closing");
				
				if(!src) f.setConnect();
				
				try {	
					tcp.sendMsgLn("EXIT");
					tcp.closeConnection();
				} catch (IOException e) {
					e.printStackTrace();
				}	
			}
		
			public void setBlocked(boolean b){
				blocked = b;
			}
			
			public void initConnection(){
				tcp = new TCPConnection(IP,port);
					try {
						tcp.init();
						tcp.startServerReader();
						f.logLine("Connected to server on " + IP + "[" + tcp.getSocket().getInetAddress().toString() + "]");
						tcp.sendMsgLn(InetAddress.getLocalHost().toString());
		
					}catch(UnknownHostException e){
						f.logLine("ERR: Server could not be found " + IP);
						killThread(false);
						
					}catch(IOException e){
						f.logLine("Couldn't establish I/O to the Server!");
						killThread(false);
					}	
					connectionActive = true;		 
			}
			
			public static void parseInput(String in){
				
				boolean keyword = false;
				
				if(in.length() == 0){
					keyword = true; 
				}
				
				if(in.length() == 3){			
					if(in.substring(0, 3).compareToIgnoreCase("cls") == 0){
						keyword = true;
						clearScreen();					
					}			
				}
				
				else if(in.equals("EXIT")){
					keyword = true;
					f.logLine("Disconnecting from server, please wait..."); 
					//tcp.sendMsgLn(in);
					killThread(false);
				}
				
				else if(in.equals("ALIGN")){
					keyword = true;
					f.logLine("Laser has been toggled for alignment."); 
					f.logLine("Please refer to the notification LED for status");
					tcp.sendMsgLn(in);
				}
				
				else if(in.length() >= 4){
					if(in.substring(0,4).compareToIgnoreCase("send") == 0){
						keyword = true;
						if(in.length() > 4){					
							String msg = in.substring(5);
							
							if(in.length() > 6){
								if(in.substring(4,7).compareToIgnoreCase("file") == 0){
									if(connectionActive){
										sendFile(f.getFile());
									}
								}
							}
								
							if(connectionActive){
								sendTextMsg(msg);
							}
							else{
								f.logLine("ERROR: Could not send msg (" + msg +") ethernet is not connected.");
							}
						}
						else{
							f.logLine("ERROR: No message attached to SEND command.");
						}
					}	
				}
				
				if(!keyword)
					f.logLine("ERROR: INVALID KEYWORD");
			}
			
			public static void sendFile(File fl){
				
				f.logLine("Beginning LDT file transfer (" + f.getPath() + ")...");
				f.logLine("Please wait...");
				
					int size = (int) fl.length(), parts = 1;
					int startIndex = 0, endIndex = 0, lim = 180;
					byte[] array = new byte[size];
					double percentDone = 0;
				
					try {
						FileInputStream fS = new FileInputStream(fl);
						fS.read(array);
					} catch (IOException e) {
						e.printStackTrace();
					}
					
					String str = new String(array);
					if(size > lim){ // if file size greater than lim split into smaller chunks
						parts = (int)(fl.length() / lim);
						if(fl.length() % lim > 0) 
							parts++;
					}
						
						tcp.sendMsgLn("MULTIFILE"); // send keyword to microcontroller
							try {
								Thread.sleep(500);
								tcp.sendMsgLn(f.getName()); // send filename
								Thread.sleep(500);
								tcp.sendMsgLn(""+parts);	// send total parts
							} catch (InterruptedException e) {
								e.printStackTrace();
							}
					
						
					for(int i = 0; i < parts; i++){
						startIndex = i * lim;
						endIndex = (i * lim) + lim;
						
						if(endIndex > size)	endIndex = size;	
						String s = str.substring(startIndex, endIndex);
						
							while(tcp.getBlock() == true){
								try {
									Thread.sleep(1);
								} catch (InterruptedException e) {
									e.printStackTrace();
								}
							}
							
						tcp.sendMsgLn(s);
						tcp.setBlock(true);				
	
						if(i > 0){
							percentDone = (i * 100) / parts;
							getMsg("Progress: " + df.format(percentDone) + "%");
						}	
					}
					
					f.logLine("Operation complete");
			}
			
			public static void sendTextMsg(String s){
				f.logLine("Beginning LDT message transfer (" + s + ")...");
				f.logLine("Please wait...");
				
				tcp.sendMsgLn("1WAYMSG"); //send ID op code
				
				if(s.length() > 180){
					s = s.substring(0, 180);
				}
	
				tcp.sendMsgLn(s); // send payload
				f.logLine("Operation Complete");
			}
			
			public static void clearScreen(){
				f.clearText();
			}
			
			private void setKeyWords(){
				keyWords.add("send");
				keyWords.add("LOAD");
				keyWords.add("cls");
				keyWords.add("EXIT");
			}
				
			public void run(){
				if(!init){
					f.logLine("Initializing connection, please wait...");
					initConnection();
					init = true;
				}
				
				if(!blocked){	
					return;
				}
		}
	}
	



}
