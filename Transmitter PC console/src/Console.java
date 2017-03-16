import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.Font;

import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTextArea;
import javax.swing.ScrollPaneConstants;
import javax.swing.text.DefaultCaret;

public class Console extends JPanel {
	private static JTextArea textArea;

	public Console(){
			this.setLayout(new BorderLayout());
			this.setSize(new Dimension(600,300));
			textArea = initText();
			this.add(initScroll(textArea),BorderLayout.CENTER);
	}
	
	private JScrollPane initScroll(JTextArea tA){
		JScrollPane scroll = new JScrollPane(tA);
		scroll.setVerticalScrollBarPolicy(ScrollPaneConstants.VERTICAL_SCROLLBAR_AS_NEEDED);
		scroll.getVerticalScrollBar().setUnitIncrement(16);
		scroll.setHorizontalScrollBarPolicy(ScrollPaneConstants.HORIZONTAL_SCROLLBAR_NEVER);
		return scroll;
	}
	
	private JTextArea initText(){
		JTextArea tA = new JTextArea();
		DefaultCaret caret = (DefaultCaret)tA.getCaret();
		caret.setUpdatePolicy(DefaultCaret.ALWAYS_UPDATE);
		
		tA.setEditable(false);
		tA.setFont(new Font("Sans-Serif",Font.BOLD,14));
		tA.setBackground(Color.BLACK);
		tA.setForeground(Color.WHITE);
		tA.setText("> ");
		tA.setLineWrap(true);
		return tA;
	}
	
	public static void log(String s){
		textArea.append(s + "\n> ");
	}
	
	public static void log2(String s){
		textArea.append(s);
	}
	
	public static void clearText(){
		textArea.setText("> ");
	}

}
