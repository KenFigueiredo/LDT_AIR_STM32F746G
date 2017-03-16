import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.Font;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.AbstractButton;
import javax.swing.JButton;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTextArea;
import javax.swing.JTextField;
import javax.swing.ScrollPaneConstants;
import javax.swing.text.DefaultCaret;

@SuppressWarnings("serial")
public class Display extends JPanel implements ActionListener{
	
	private JTextArea display;
	private JTextField sendBar;
	private JButton sendButton;
	
	public Display(){
		this.setLayout(new BorderLayout());
		this.setSize(new Dimension(600,300));
		display = initText();
		sendBar = initField();
		initButton();
		this.add(initScroll(display),BorderLayout.CENTER);
		JPanel jp = new JPanel();
			jp.add(sendBar);
			jp.add(sendButton);
		//this.add(jp, BorderLayout.SOUTH);
	}
	
		private JTextArea initText(){
			JTextArea tA = new JTextArea();
			DefaultCaret caret = (DefaultCaret)tA.getCaret();
			caret.setUpdatePolicy(DefaultCaret.ALWAYS_UPDATE);
			
			tA.setEditable(false);
			tA.setFont(new Font("Sans-Serif",Font.BOLD,24));
			tA.setBackground(Color.BLACK);
			tA.setForeground(Color.WHITE);
			tA.setText("> ");
			tA.setLineWrap(true);
			return tA;
		}
		
		private JTextField initField(){
			JTextField s = new JTextField();
			s.setColumns(75);
			return s;
		}
		
		private void initButton(){
			sendButton = new JButton("Send");
			sendButton.setVerticalTextPosition(AbstractButton.CENTER);
			sendButton.setHorizontalTextPosition(AbstractButton.CENTER);
			sendButton.setEnabled(true);
			sendButton.addActionListener(this);
			sendButton.setPreferredSize(new Dimension(100, 20));
		}
		
		private JScrollPane initScroll(JTextArea tA){
			JScrollPane scroll = new JScrollPane(tA);
			scroll.setVerticalScrollBarPolicy(ScrollPaneConstants.VERTICAL_SCROLLBAR_AS_NEEDED);
			scroll.getVerticalScrollBar().setUnitIncrement(16);
			scroll.setHorizontalScrollBarPolicy(ScrollPaneConstants.HORIZONTAL_SCROLLBAR_NEVER);
			return scroll;
		}
		
		public void logLine(String s){
			display.append(s + "\n> ");
		}
		
		public void log(String s){
			display.append(s);
		}

		public void clearText(){
			display.setText("> ");
		}

		@Override
		public void actionPerformed(ActionEvent event) {
				Object src = event.getSource();
				if(src.equals(sendButton) || src.equals(sendBar)){
					handleSendPress();
				}
		}
		
		private void handleSendPress(){
			String text = sendBar.getText();
			logLine(text);
			Main.threadedMain.parseInput(text);			
			sendBar.setText("");
		}
}
