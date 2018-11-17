//
// Servidor.java
//
package login;

// IMPORTAMOS LAS CLASES DE CORBA NECESARIAS
import java.util.*;
import java.io.*;
import org.omg.CORBA.ORB;
import org.omg.PortableServer.*;
import org.omg.PortableServer.POA;

class LoginImplementacion extends inicio_sesionPOA
{	
	private ArrayList<User> users= new ArrayList();
	
	public boolean logear(String user, String pass)
	{
		System.out.println("\nLogear \nusuario= " + user +" password= " + pass);
		for (User x : users){
			if (x.getUsr().equals(user)){
				if (x.getPwd().equals(pass)){
					System.out.println("Acceso concedido");
					return true;
					}
				}
		}
		System.out.println("Acceso Denegado");
		return false;
		
	}
	public boolean registrar(String user, String pass){
		User usr= new User();
		usr.registrar(user,pass);
		users.add(usr);
		System.out.println("\nRegistrar \nusuario= " + user +" password= " + pass);
		System.out.println(users.size());
		return true;
		
	}
}
class User {
	public String pwd,usr;
	
	public void registrar(String user, String pass){
		pwd = pass;
		usr = user;
	}
	public String getUsr(){
		return usr;
	}
	
	public String getPwd(){
		return pwd;
	}

}
public class servidor {
public static void main (String args[ ]) {
	try {
		
		// INICIAMOS ORB Y POA
		ORB orb = ORB.init(args, null);
		POA rootPOA = POAHelper.narrow(orb.resolve_initial_references("RootPOA"));
		rootPOA.the_POAManager().activate();

		// CREAMOS LOS OBJETOS QUE DARAN SERVICIO
		// Y LOS CONECTAMOS AL ORB
		LoginImplementacion objlogin = new LoginImplementacion();
		inicio_sesion ss = objlogin._this(orb);

		// CREAMOS UNA CADENA CON LA IDENTIFICACION DEL OBJETO
		String ref = orb.object_to_string(ss);

		// VOLCAMOS LA IDENTIFICACION A UN ARCHIVO PARA
		// PERMITIR AL CLIENTE HALLAR EL OBJETO REMOTO
		java.io.FileOutputStream file = new java.io.FileOutputStream("login.ref");
		java.io.PrintWriter out = new java.io.PrintWriter(file);
		out.println(ref);
		out.flush();
		file.close();

		// ESPERAMOS INVOCACIONES DE LOS CLIENTES
		System.out.println("Servidor listo y a la espera de conexiones ...\n");
		orb.run();
			
		System.out.println("Servidor apagandose ...\n");

	} catch(Exception e) {
		System.out.println("ERROR : " + e);
		e.printStackTrace(System.out);
	}
}
}
