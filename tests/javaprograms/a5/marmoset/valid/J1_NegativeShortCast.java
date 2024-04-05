// PARSER_WEEDER,CODE_GENERATION
public class J1_NegativeShortCast {
    public static void main(String args[]) {
        System.out.println(test());
    }

    public J1_NegativeShortCast(){}

       public static int test() {

	   return (short)-123456 - 7493;
       }
}

