// PARSER_WEEDER,CODE_GENERATION
public class J1_NegativeCharCast {
    public static void main(String args[]) {
        System.out.println(test());
    }

    public J1_NegativeCharCast(){}

       public static int test() {

	   return (char)-123456 - 7493;
       }
}
