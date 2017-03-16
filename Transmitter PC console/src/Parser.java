//import java.io.File;
//import java.io.FileInputStream;
//import java.io.FileNotFoundException;
//import java.io.IOException;
//import java.nio.charset.StandardCharsets;
//import java.util.ArrayList;
//
//public class Parser {
//	
//	private ArrayList<byte[]> packetStream;
//	private static ArrayList<String> keyWords;
//	private static String commPort, baudRate;
//	
//	private static boolean isConnected;
//	
//		public Parser(){		
//			packetStream = null;
//			keyWords = new ArrayList<String>();
//			setKeyWords();
//			commPort = "COM3"; // by default
//			baudRate = "9600"; // by default
//			isConnected = false;
//		}
//		
//		private void setKeyWords(){
//			keyWords.add("SEND");
//			keyWords.add("cls");
//		}
//		
//		public static byte[] stringToByte(String s){
//			byte[] a = s.getBytes(StandardCharsets.US_ASCII);
//			return a;
//		}
//		
//		public static void loadVideo(){
//			File f = new File("src/ArcherVid.mp4");
//			
//			int lim = (int) f.length();
//			byte[] array = new byte[lim/2];
//			
//			try{
//				FileInputStream fis = new FileInputStream(f);
//				
//				for(int i = 0; i < lim/2; i++){
//					fis.read(array, i, i+1);
//					com.writeData(array,0);
//				}
//				System.out.println("Starting video read in #1");
//				
//				
//				
//				System.out.println("Starting video read in #2");
//				array = new byte[lim/2];
//				fis.read(array, (lim/2), lim);
//				com.writeData(array,0);				
//				
//				fis.close();
//			} catch (FileNotFoundException e){
//				Console.log("File Not Found.");
//				e.printStackTrace();
//			} catch (IOException e){
//				Console.log("Error reading file");
//				e.printStackTrace();
//			}
//			
//			Console.log("Video has been converted to byte array and is currently in memory.");
//			//return array;
//		}
//		
//		public static void parseInput(String in){
//			
//			boolean keyword = false;
//			
//			if(in.length() == 0){
//				keyword = true; // allow manual clear without error spam
//			}
//			
//			if(in.length() == 3){			
//				if(in.substring(0, 3).compareToIgnoreCase("cls") == 0){
//					keyword = true;
//					clearScreen();					
//				}			
//			}
//			else if(in.length() >= 4){
//				if(in.substring(0,4).compareToIgnoreCase("send") == 0){
//					keyword = true;
//					if(in.length() > 4){					
//						String msg = in.substring(5);
//						
//						if(in.length() > 6){
//							if(in.substring(4,7).compareToIgnoreCase("vid") == 0){
//								if(isConnected){
//									Console.log("Attempting to send Video");
//									loadVideo();
//								}
//							}
//						}
//							
//						if(isConnected){
//							Console.log2("Attempting to send msg (" + msg +")");
//							com.writeData(stringToByte(msg),1);
//						}
//						else{
//							Console.log("ERROR: Could not send msg (" + msg +") serial port is not connected.");
//						}
//					}
//					else{
//						Console.log("ERROR: No message attached to SEND command.");
//					}
//				}	
//			}
//			
//			if(!keyword)
//				Console.log("ERROR: INVALID KEYWORD");
//			
//			
//		}
//		
//		public static void clearScreen(){
//			Console.clearText();
//		}
//						
//		public static void connect(){
//			Console.log("Attempting to Connect to: " + commPort);
//			com.setBaud(baudRate);
//			openSerialComm(com);
//			isConnected = true;
//		}
//		
//		public static void disconnect(){
//			Console.log("Attempting to Disconnect from: " + commPort);
//			closeSerialComm(com);
//			isConnected = false;
//		}
//		
//		private static void openSerialComm(Communicator com){				
//				com.connect(commPort);
//				com.initIOStream();
//				com.initListener();
//		}
//		
//		private static void closeSerialComm(Communicator com){
//			com.disconnect();
//		}
//		
//		public static void setCommPort(String s){
//			commPort = s; 
//		}
//		
//		public static void setBaudRate(String s){
//			baudRate = s;
//		}
//		
//		public static void refreshList(){
//			com.resetPortNames();
//			com.searchForPorts();
//		}
//		
//		public static ArrayList<String> getPorts(){
//			return com.getPortNames();
//		}
//}
