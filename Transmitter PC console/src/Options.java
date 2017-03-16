import java.awt.Dimension;
import java.awt.FlowLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.ItemEvent;
import java.awt.event.ItemListener;
import java.util.ArrayList;
import java.util.Arrays;

import javax.swing.AbstractButton;
import javax.swing.Box;
import javax.swing.BoxLayout;
import javax.swing.JButton;
import javax.swing.JComboBox;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JTextField;

@SuppressWarnings("serial")
public class Options extends JPanel implements ActionListener{
	
	private JButton connect;
	private boolean buttonOnOff;
	private JTextField textField, textField2;
	
	public Options(){
		this.setLayout(new BoxLayout(this, BoxLayout.Y_AXIS));
		
		buttonOnOff = false;
		
		JPanel jp1 = new JPanel(new FlowLayout(FlowLayout.LEFT,20,0));	
			JPanel subPanel1 = new JPanel(new FlowLayout());
				JLabel lab1 = new JLabel("IP:");
				textField = new JTextField(10);
				textField.addActionListener(this);
				textField.setText("192.168.0.80");
			subPanel1.add(lab1);
			subPanel1.add(textField);
			
			JPanel subPanel2 = new JPanel(new FlowLayout());
				JLabel lab2 = new JLabel("PORT:");
				textField2 = new JTextField(5);
				textField2.addActionListener(this);
				textField2.setText("5419");
			subPanel2.add(lab2);
			subPanel2.add(textField2);
			
		jp1.add(subPanel1);
		jp1.add(subPanel2);

		JPanel jp3 = new JPanel(new FlowLayout());
			JTextField text3 = new JTextField(20);
			text3.addActionListener(this);
			JButton browse = new JButton("Browse");
			browse.setVerticalTextPosition(AbstractButton.CENTER);
			browse.setHorizontalTextPosition(AbstractButton.CENTER);
			browse.setEnabled(true);
			browse.addActionListener(this);
		jp3.add(text3);
		jp3.add(browse);
			
		JPanel jp4 = new JPanel(new FlowLayout());
			connect = new JButton("Connect");
			connect.setVerticalTextPosition(AbstractButton.CENTER);
			connect.setHorizontalTextPosition(AbstractButton.CENTER);
			connect.setEnabled(true);
			connect.addActionListener(this);
		jp4.add(connect);
		
		jp1.setMaximumSize(jp1.getPreferredSize());
		jp3.setMaximumSize(jp3.getPreferredSize());
		jp4.setMaximumSize(jp4.getPreferredSize());
		
		this.add(Box.createRigidArea(new Dimension(0,200)));
		this.add(jp1);
		this.add(jp3);
		this.add(jp4);
	}
	
	@Override
	public void actionPerformed(ActionEvent ev) {

//		if(!buttonOnOff){
//			Parser.connect();
//			connect.setText("Disconnect");
//			baudRate.setEnabled(false);
//			buttonOnOff = true;
//		}
//		else{
//			Parser.disconnect();
//			connect.setText("Connect");
//			baudRate.setEnabled(true);
//			buttonOnOff = false;
//		}	
	}
}
