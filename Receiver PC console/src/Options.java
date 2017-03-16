import java.awt.BorderLayout;
import java.awt.Desktop;
import java.awt.Dimension;
import java.awt.FlowLayout;
import java.awt.Font;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;

import javax.swing.AbstractButton;
import javax.swing.BoxLayout;
import javax.swing.JButton;
import javax.swing.JFileChooser;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JSeparator;
import javax.swing.JTextArea;
import javax.swing.JTextField;
import javax.swing.ScrollPaneConstants;

@SuppressWarnings("serial")
public class Options extends JPanel implements ActionListener{
	
	private JTextField ipTA, portTA;
	private JLabel path;
	private JButton connect, browseButton, sendButton;
	private boolean buttonState;
	private JFileChooser fileChooser;
	private File loadedFile;
	
	public Options(){
		fileChooser = new JFileChooser();
		this.setLayout(new BorderLayout());
		
		JPanel jp = new JPanel();
			jp.setLayout(new BoxLayout(jp,BoxLayout.Y_AXIS));
				jp.add(initConnection());
				jp.add(new JSeparator(JSeparator.HORIZONTAL));
				//jp.add(initBrowse());
				jp.add(new JSeparator(JSeparator.HORIZONTAL));
				jp.add(initReadMe());
		this.add(jp, BorderLayout.CENTER);
		this.add(new JSeparator(JSeparator.VERTICAL), BorderLayout.WEST);
		this.add(new JSeparator(JSeparator.VERTICAL), BorderLayout.EAST);
		buttonState  = false;
	}
		
		private JPanel initConnection(){
			JPanel main = new JPanel(new FlowLayout());
			main.setLayout(new BoxLayout(main, BoxLayout.Y_AXIS));
			
				JPanel connectionTxt = new JPanel();
				connectionTxt.setLayout(new GridLayout(1,0,0,0));				
					JLabel conLab = new JLabel("Connection Options");
						conLab.setFont(new Font("Consolas",Font.BOLD,16));							
				connectionTxt.add(conLab);
					
				JPanel sub = new JPanel(new FlowLayout());
					JPanel ipJP = new JPanel(new FlowLayout());
						JLabel ipLab = new JLabel("IP:");
						ipTA = new JTextField(10);
						ipTA.setText("192.168.0.80");
					ipJP.add(ipLab);
					ipJP.add(ipTA);
					
					JPanel portJP = new JPanel(new FlowLayout());
						JLabel portLab = new JLabel("Port:");
						portTA = new JTextField(6);
						portTA.setText("5419");
					portJP.add(portLab);
					portJP.add(portTA);
					
					JPanel conJP = new JPanel(new FlowLayout());
						connect = new JButton("Connect");
						connect.setVerticalTextPosition(AbstractButton.CENTER);
						connect.setHorizontalTextPosition(AbstractButton.CENTER);
						connect.setEnabled(true);
						connect.addActionListener(this);
						connect.setPreferredSize(new Dimension(100, 20));
					conJP.add(connect);
						
				sub.add(ipJP);
				sub.add(portJP);
				sub.add(conJP);
			
			main.add(connectionTxt);
			main.add(sub);
				
			return main;			
		}
		
		private JPanel initBrowse(){
			JPanel main = new JPanel();
			main.setLayout(new BoxLayout(main, BoxLayout.Y_AXIS));
				JPanel sub = new JPanel();
					JLabel browseLab = new JLabel("File Loaded:");
					sub.add(browseLab);
					path = new JLabel ("No file selected.");
					sub.add(path);
				main.add(sub);
				
				JPanel sub2 = new JPanel();				
					browseButton = new JButton("Browse");
						browseButton.setVerticalTextPosition(AbstractButton.CENTER);
						browseButton.setHorizontalTextPosition(AbstractButton.CENTER);
						browseButton.setEnabled(true);
						browseButton.addActionListener(this);
						browseButton.setPreferredSize(new Dimension(100, 20));
					sub2.add(browseButton);
					sendButton = new JButton("Send");
						sendButton.setVerticalTextPosition(AbstractButton.CENTER);
						sendButton.setHorizontalTextPosition(AbstractButton.CENTER);
						sendButton.setEnabled(false);
						sendButton.addActionListener(this);
						sendButton.setPreferredSize(new Dimension(100, 20));
					sub2.add(sendButton);
				main.add(sub2);
			return main;	
		}
		
		private JPanel initReadMe(){
			JPanel readMe = new JPanel(new BorderLayout());
			JScrollPane scroll = new JScrollPane(readInManual());
				scroll.setVerticalScrollBarPolicy(ScrollPaneConstants.VERTICAL_SCROLLBAR_ALWAYS);
				scroll.getVerticalScrollBar().setUnitIncrement(16);
				scroll.setPreferredSize(new Dimension(200,600));
			readMe.add(scroll, BorderLayout.CENTER);
			
			return readMe;
		}
		
		public JTextArea readInManual(){
			
			BufferedReader br;
			JTextArea manual = new JTextArea();
			try {
				br = new BufferedReader(new FileReader("README.txt"));
				manual.read(br, "manual");
				br.close();
			} catch (FileNotFoundException e) {
				manual.setText("ERROR: ReadMe not found!");
			} catch (IOException e) {
				manual.setText("ERROR: README.txt could not be read!");
			}
			
			manual.setFont(new Font("Consolas",Font.PLAIN,18));
			manual.setEditable(false);
			manual.setLineWrap(true);
			manual.setWrapStyleWord(true);
			return manual;
		} 
			
			public boolean isConnected(){
				return buttonState;
			}
			
			public String getIP(){
				return ipTA.getText();
			}
			
			public String getPort(){
				return portTA.getText();
			}
			
			public String getPath(){
				return path.getText();
			}
			
			public File getFile(){
				return loadedFile;
			}

			@Override
			public void actionPerformed(ActionEvent event) {
				
				Object src = event.getSource();
					if(src.equals(connect)){
						handleConnectPress();
					}
					
					else if(src.equals(browseButton)){
						handleBrowsePress();
					}
					
					else if(src.equals(sendButton)){
						handleSendPress();
					}
				
			}
			
			public void handleConnectPress(){
				if(!buttonState){
					Main.startThread(ipTA.getText(), Integer.parseInt(portTA.getText()));
					connect.setText("Disconnect");
					ipTA.setEnabled(false);
					portTA.setEnabled(false);
					buttonState = true;
				}
				else{
					Main.killThread(true);
					connect.setText("Connect");
					ipTA.setEnabled(true);
					portTA.setEnabled(true);
					buttonState = false;
				}
			}
			
			public void setConnect(){
				if(buttonState){
					connect.setText("Connect");
					ipTA.setEnabled(true);
					portTA.setEnabled(true);
					buttonState = false;
				}
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
			
			public void handleBrowsePress(){				
		        int returnVal = fileChooser.showOpenDialog(this);
		            if (returnVal == JFileChooser.APPROVE_OPTION) {
		                loadedFile = fileChooser.getSelectedFile();
		                path.setText(loadedFile.getName());
		                sendButton.setEnabled(true);
		                
		                
	//		                try {
	//							Desktop.getDesktop().open(loadedFile);
	//						} catch (IOException e) {
	//							e.printStackTrace();
	//						}
		            } 
			}
	
			public void handleSendPress(){
				Main.sendFile(loadedFile);
			}
}




