// CODE_GENERATION
public class J1_sim_or {
    public J1_sim_or() {}

    public static int test() {
	return J1_sim_or.sim_or(3*3*3, 1*2*3*4*5);
    }


    public static int sim_or(int x, int y) {
	if (x==0 && y==0) return 0;
	boolean bit0 = (x%2!=0 || y%2!=0);
	int result = J1_sim_or.sim_or(x/2, y/2)*2;
	if (bit0) result = result+1;
	return result;
    }
}
