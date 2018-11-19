//
// Cliente.java
//
package login;
// Importamos las clases de CORBA necesarias
import org.omg.CORBA.ORB;
import org.omg.CosNaming.*;
import org.omg.CosNaming.NamingContextPackage.*;

// Importamos otras clases necesarias de Java
import java.io.*;
import java.util.*;
import javax.swing.*;
import javax.swing.event.*;
import java.awt.event.*;
import java.awt.*;
public class Cliente extends JFrame implements ChangeListener,ActionListener{

	private static LoginImplementacion h;

public static void main (String args[ ]) {
		// ******************************************************
	// ********* 	Implemetacion de CORBA *********************
	// ******************************************************
	try {
	// INICIAMOS ORB
	ORB orb = ORB.init(args, null);

	// LEEMOS EN ARCHIVO UBICACION DEL OBJETO REMOTO
	//FileInputStream file = new FileInputStream("login.ref");
	//BufferedReader in = new BufferedReader(new InputStreamReader(file));
	//String ref = in.readLine();

	// CREAMOS REFERENCIA AL OBJETO REMOTO
	//h = login.inicio_sesionHelper.narrow(orb.string_to_object(ref));
		org.omg.CORBA.Object objRef= orb.resolve_initial_references("NameService");
		NamingContextExt ncRef= NamingContextExtHelper.narrow(objRef);
		h= new LoginImplementacion(); 
		Scanner s= new Scanner();
		while(1){
			System.out.println("1- REGISTRO\n");
			System.out.println("2- LOGIN\n");
			int x = s.nextInt();
			switch (x){
				case 1:
					System.out.println("REGISTRO\n");
					System.out.println("Ingrese ID\n");
					String id = nextLine();
					System.out.println("Ingrese Dominio\n");
					String dom = nextLine();
					System.out.println("Ingrese contraseña\n");
					String pass= nextLine();
					System.out.println("Repita contraseña\n");
					String rpass= nextLine();
					id= id + "@" +dom;
					if (pass.Equals(rpass)){
						h.registrar(id,pass);
						System.out.println("Usuario Registrado Correctamente\n");
					} 
					else{
						System.out.println("Las contraseña no coinciden\n");
					}
					break;
				case 2:
					System.out.println("LOGIN\n");
					System.out.println("Ingrese usuario\n");
					String id = nextLine();
					System.out.println("Ingrese contraseña\n");
					String pass= nextLine();
					if(h.logear(id,pass))
						System.out.println("ACCESO CONCEDIDO: BIENVENIDO " +id+ " \n");
					else
						System.out.println("ACCESO DENEGADO\n");
					break;
				default:
					System.out.println("error");
			}	
		}
		



	}
	catch(Exception e) {
		System.out.println("ERROR : " + e);
		e.printStackTrace(System.out);
	}
}
}

