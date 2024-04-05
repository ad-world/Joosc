// CODE_GENERATION
public class J1e_divisionbyzero {
    public static void main(String args[]) {
        System.out.println(test());
    }

    public J1e_divisionbyzero () { }

    public static int test() {
	int i = 0;
    if (i/i > 0)
        return 123;
        return 123;
    }

}
