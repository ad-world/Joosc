// DISAMBIGUATION,TYPE_CHECKING,CODE_GENERATION
public class J1_staticMethodInvocation {
    public static void main(String args[]) {
        System.out.println(test());
    }

    public static int bar() {
	return 123;
    }

    public J1_staticMethodInvocation() {}


    public static int test() {
	return J1_staticMethodInvocation.bar();
    }


}
