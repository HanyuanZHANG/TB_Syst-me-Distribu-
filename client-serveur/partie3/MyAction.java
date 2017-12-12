import java.io.*;

public class MyAction{

    static String calRes(String message)
        throws IOException{
        String[] request = message.split("\\s+");
        if (request.length == 3){
            String lower = request[0].toLowerCase();
            if (lower.equals("add")){
                //System.out.println(lower);
                String res = Integer.toString(Integer.parseInt(request[1]) + Integer.parseInt(request[2])); 
                return res;       
            }
            else if (lower.equals("mul")){
                String res = Integer.toString(Integer.parseInt(request[1]) + Integer.parseInt(request[2])); 
                return res;       
            }
            else{
                return "cannot operate !!!";            
            }
        }else{
            return "erruer in expression !!!";        
        }
    }
}
