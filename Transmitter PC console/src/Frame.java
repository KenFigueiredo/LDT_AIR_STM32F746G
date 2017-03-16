import java.awt.BorderLayout;

import javax.swing.JFrame;
import javax.swing.UIManager;
import javax.swing.UnsupportedLookAndFeelException;

public class Frame {

	
	public Frame(){
		setLook();
		
		JFrame frame = new JFrame();
		
		frame.setLayout(new BorderLayout());
		
		//Parser p = new Parser();
		Console c = new Console();
		//SendBar s = new SendBar();
		Options o = new Options();
		
		
		frame.add(c, BorderLayout.CENTER);
		//frame.add(s, BorderLayout.PAGE_END);
		frame.add(o, BorderLayout.EAST);
		
		
			frame.setTitle("IR Console");
			frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
			frame.setSize(1200, 600);
			frame.setResizable(true);
			frame.setVisible(true);
			frame.setLocationRelativeTo(null);
		
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
}

