//
// Cliente.java
//
package login;
// Importamos las clases de CORBA necesarias
import org.omg.CORBA.ORB;

// Importamos otras clases necesarias de Java
import java.io.*;
import java.util.*;
import javax.swing.*;
import javax.swing.event.*;
import java.awt.event.*;
import java.awt.*;
public class Cliente extends JFrame implements ChangeListener,ActionListener{

	private JLabel title,id,dom,pass,rpass,a;
	private JButton reg,log;
	private JTextField name,mail, pwd, rpwd;
	private JCheckBox check;
	private static inicio_sesion h;

public Cliente(){
	// ******************************************************
	// ********* 	Creacion de Grafica *********************
	// ******************************************************
	setLayout(null);
	setTitle("Objetos Distribuidos");
	getContentPane().setBackground(new Color(255,140,178));
	// -------------------------------------------
	// Labels ------------------------------------
	title = new JLabel("Login");
	title.setBounds(150,0,200,25);
	title.setFont(new Font("arial", 3, 24));
	add(title);
	id = new JLabel("ID");
	id.setBounds(30,50,60,25);
	id.setFont(new Font("arial", 2,16));
	add(id);
	dom = new JLabel("Dominio");
	dom.setBounds(230,50,150,25);
	dom.setFont(new Font("arial", 2, 16));
	add(dom);
	pass = new JLabel("Password");
	pass.setBounds(30,110,150,25);
	pass.setFont(new Font("arial", 2, 16));
	add(pass);
	rpass = new JLabel("Repetir Password");
	rpass.setBounds(30,180,200,25);
	rpass.setFont(new Font("arial", 2, 16));
	add(rpass);
	rpass.setVisible(false);
	a = new JLabel("@");
	a.setBounds(180,80,30,20); 
	a.setFont(new Font("arial", 2, 16));
	add(a);
	// -------------------------------------------
	// TextFields
	name= new JTextField();
	mail= new JTextField();
	pwd= new JTextField();
	rpwd= new JTextField();
	name.setBounds(20, 80 , 150 , 20);
	mail.setBounds(200, 80 , 150 , 20);
	pwd.setBounds(20, 150 , 330 , 20);
	rpwd.setBounds(20, 210 , 330 , 20);
	add(name);
	add(mail);
	add(pwd);
	add(rpwd);
	rpwd.setVisible(false);
	// -------------------------------------------
	// Buttons
	check= new JCheckBox("Soy nuevo");
	check.setBounds(120,250,200,30);
	check.addChangeListener(this);
	check.setBackground(new Color(255,140,178));
	add(check);
	reg = new JButton("Registrarse");
	reg.setBounds(50,300,150,30);
	reg.setEnabled(false);
	add(reg);
	reg.addActionListener(this);
	log = new JButton("Iniciar Sesion");
	log.setBounds(200,300,150,30);
	add(log);
	log.addActionListener(this);

}
public void stateChanged(ChangeEvent e){
	if(check.isSelected() == true){
		title.setText("Registrarse");
		rpass.setVisible(true);
		rpwd.setVisible(true);
		reg.setEnabled(true);
		log.setEnabled(false);
	}else{
		title.setText("Login");
		rpass.setVisible(false);
		rpwd.setVisible(false);
		reg.setEnabled(false);
		log.setEnabled(true);
	}
}
public void actionPerformed(ActionEvent e){
	if (e.getSource() == reg){
		String user= name.getText() + "@" + mail.getText();
		String pass = pwd.getText();
		if (pass.equals(rpwd.getText())){
			h.registrar(user,pass);
			JOptionPane.showMessageDialog(null,"Usuario creado correctamente","Correcto", JOptionPane.DEFAULT_OPTION);
			}
		else{
			JOptionPane.showMessageDialog(null,"Las contraseñas no coinciden","Ocurrio un error", JOptionPane.ERROR_MESSAGE);
			}
		pwd.setText("");
		rpwd.setText("");
	}
	if(e.getSource() == log){
		String user= name.getText() + "@" + mail.getText();
		String pass = pwd.getText();
		boolean request= h.logear(user,pass);
		if (request){
			JOptionPane.showMessageDialog(null,"Usuario Logeado correctamente","Correcto", JOptionPane.DEFAULT_OPTION);
			}
		else{
			JOptionPane.showMessageDialog(null,"usuario y/o contraseña incorrecto/s ","Ocurrio un error", JOptionPane.ERROR_MESSAGE);
			}
		pwd.setText("");
		rpwd.setText(""); 
	}
}
public static void main (String args[ ]) {
	Cliente cl= new Cliente();
	cl.setBounds(0,0,400,400);
	cl.setVisible(true);
	cl.setLocationRelativeTo(null);
		// ******************************************************
	// ********* 	Implemetacion de CORBA *********************
	// ******************************************************
	try {
	// INICIAMOS ORB
	ORB orb = ORB.init(args, null);

	// LEEMOS EN ARCHIVO UBICACION DEL OBJETO REMOTO
	FileInputStream file = new FileInputStream("login.ref");
	BufferedReader in = new BufferedReader(new InputStreamReader(file));
	String ref = in.readLine();

	// CREAMOS REFERENCIA AL OBJETO REMOTO
	h = login.inicio_sesionHelper.narrow(orb.string_to_object(ref));
	}
	catch(Exception e) {
		System.out.println("ERROR : " + e);
		e.printStackTrace(System.out);
	}
}
}

