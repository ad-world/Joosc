// TYPE_CHECKING,CODE_GENERATION
public class J1_BigShortFromByteInit {
    public static void main(String args[]) {
        System.out.println(test());
    }

    public J1_BigShortFromByteInit(){}

	public static int test() {
		short x = (byte)123456;
		return x + 59;
	}
}

