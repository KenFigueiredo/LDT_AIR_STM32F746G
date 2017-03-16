import java.awt.BorderLayout;
import java.awt.event.WindowEvent;
import java.awt.event.WindowListener;
import java.io.File;

import javax.swing.JFrame;
import javax.swing.JPanel;
import javax.swing.JSeparator;
import javax.swing.UIManager;
import javax.swing.UnsupportedLookAndFeelException;

@SuppressWarnings("serial")
public class Frame extends JFrame implements WindowListener{
	
	private JPanel display, options;
	
	public Frame(){
		setLook();
		addWindowListener(this);
		
		display = new Display();
		options = new Options();
		
		this.setLayout(new BorderLayout());
		JSeparator HR = new JSeparator(JSeparator.HORIZONTAL);
		
		this.add(HR, BorderLayout.NORTH);
		this.add(display, BorderLayout.CENTER);
		this.add(options,  BorderLayout.EAST);
		
		JSeparator HR2 = new JSeparator(JSeparator.HORIZONTAL);
		this.add(HR2, BorderLayout.SOUTH);
		
		this.setTitle("LDT Receiver");
		this.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		this.setVisible(true);
		this.setSize(1200, 600);
		this.setResizable(true);
		this.setLocationRelativeTo(null);
	}
		
		public boolean getConnection(){
			return ((Options) options).isConnected();
		}
		
		public String getIP(){
			return ((Options) options).getIP();
		}
		
		public int getPort(){
			return Integer.parseInt(((Options) options).getPort());
		}
		
		public void setConnect(){
			((Options)options).setConnect();
		}
		
		public String getPath(){
			return ((Options)options).getPath();
		}
		
		public File getFile(){
			return ((Options)options).getFile();
		}
		
		private void setLook(){
			try {
				UIManager.setLookAndFeel(UIManager.getSystemLookAndFeelClassName());
			}catch (ClassNotFoundException e){
				e.printStackTrace();
			}catch(UnsupportedLookAndFeelException e){
				e.printStackTrace();
			}catch(InstantiationException e){
				e.printStackTrace();
			}catch(IllegalAccessException e){
				e.printStackTrace();
			}	
		}
		
		public void logLine(String s){
			((Display) display).logLine(s);
		}
		
		public void log(String s){
			((Display) display).log(s);
		}

		public void clearText(){
			((Display) display).clearText();
		}

		@Override
		public void windowActivated(WindowEvent arg0) {
			// TODO Auto-generated method stub
			
		}

		@Override
		public void windowClosed(WindowEvent arg0) {			
		}

		@Override
		public void windowClosing(WindowEvent arg0) {
			logLine("Please wait, connection closing...");
			if(Main.getConnect()){
				Main.killThread(false);
			}			
			logLine("Connection closed, exiting program.");
			return;
		}

		@Override
		public void windowDeactivated(WindowEvent arg0) {
			// TODO Auto-generated method stub
			
		}

		@Override
		public void windowDeiconified(WindowEvent arg0) {
			// TODO Auto-generated method stub
			
		}

		@Override
		public void windowIconified(WindowEvent arg0) {
			// TODO Auto-generated method stub
			
		}

		@Override
		public void windowOpened(WindowEvent arg0) {
			// TODO Auto-generated method stub
			
		}
}
