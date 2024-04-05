// PARSER_WEEDER,CODE_GENERATION
public class J1_ArrayCreateAndIndex {
    public static void main(String args[]) {
        System.out.println(test());
    }
    public J1_ArrayCreateAndIndex() {}
    public static int test() {
	return 123 + (new int[1])[0];
    }
}
