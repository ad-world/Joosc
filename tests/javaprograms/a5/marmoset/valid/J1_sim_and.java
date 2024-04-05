// CODE_GENERATION
public class J1_sim_and {
    public static void main(String args[]) {
        System.out.println(test());
    }
    public J1_sim_and() {}

    public static int test() {
	return J1_sim_and.sim_and(65535-128, 255-4);
    }


    static public int sim_and(int x, int y) {
	if (x==0 || y==0) return 0;
	boolean bit0 = (x%2!=0 && y%2!=0);
	int result = J1_sim_and.sim_and(x/2, y/2)*2;
	if (bit0) result = result+1;
	return result;
    }
}
