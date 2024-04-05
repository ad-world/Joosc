// PARSER_WEEDER,CODE_GENERATION
public class J1_SimpleTypeArray {
    public static void main(String args[]) {
        System.out.println(test());
    }

    public J1_SimpleTypeArray(){}

	public static int test(){
		boolean[] a = new boolean[5];
		if (a[4] == false){
			return 123;
		}
		return 444;
	}
}

