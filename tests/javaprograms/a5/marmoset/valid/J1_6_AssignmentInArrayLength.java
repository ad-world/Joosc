// TYPE_CHECKING,CODE_GENERATION
public class J1_6_AssignmentInArrayLength {
    public static void main(String args[]) {
        System.out.println(test());
    }
	public J1_6_AssignmentInArrayLength() {}
	
	public static int test() {
		int[] a = null;
		return (a = new int[123]).length;
	}
}
