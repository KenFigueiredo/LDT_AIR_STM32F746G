import java.awt.FlowLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.AbstractButton;
import javax.swing.JButton;
import javax.swing.JPanel;
import javax.swing.JTextField;

@SuppressWarnings("serial")
public class SendBar extends JPanel implements ActionListener{
	
	private JTextField textField;
	private JButton button;
		
		public SendBar(){
			
			this.setLayout(new FlowLayout(FlowLayout.LEFT));
			
			textField = new JTextField(115);
			textField.addActionListener(this);
			
			button = new JButton("Send");
			button.setVerticalTextPosition(AbstractButton.CENTER);
			button.setHorizontalTextPosition(AbstractButton.CENTER);
			button.setEnabled(true);
			button.addActionListener(this);
			
			this.add(textField);
			this.add(button);
		}

		@Override
		public void actionPerformed(ActionEvent e) {
			String text = textField.getText();
			Console.log(text);
			//Parser.parseInput(text);			
			textField.setText("");	
		}
		
		public String getField(){
			return textField.getText();
		}
}
