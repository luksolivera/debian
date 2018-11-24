//
// Servidor.java
//
import login.*;

// IMPORTAMOS LAS CLASES DE CORBA NECESARIAS
import java.util.*;
import java.io.*;
import org.omg.CORBA.ORB;
import org.omg.CosNaming.*;
import org.omg.CosNaming.NamingContextPackage.*;
import org.omg.PortableServer.*;
import org.omg.PortableServer.POA;

class LoginImplementacion extends inicio_sesionPOA
{	
	private ArrayList<User> users= new ArrayList();
	private ORB orb;
	public void setORB(ORB orb_val){
		orb =orb_val;
	}
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
		objlogin.setORB(orb);
		
		org.omg.CORBA.Object ref = rootPOA.servant_to_reference(objlogin);
		inicio_sesion href = inicio_sesionHelper.narrow(ref);
		
		org.omg.CORBA.Object objRef= orb.resolve_initial_references("NameService");
		NamingContextExt ncRef= NamingContextExtHelper.narrow(objRef);
		
		String name = "inicio_sesion";
		NameComponent path[]= ncRef.to_name(name);
		ncRef.rebind(path,href); 

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
