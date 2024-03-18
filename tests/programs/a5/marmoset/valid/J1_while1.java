// REACHABILITY,CODE_GENERATION
public class J1_while1 {
    public J1_while1() {}
    public static int m(int x) {
	while (x>0) x=x-1;
	return x;
    }
    public static int test() {
	return J1_while1.m(17)+123;
    }
}


