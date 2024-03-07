public class Jw_DeadAssignment_1 {
    public Jw_DeadAssignment_1() {}

    public static int test() {
        int ax = 0;
        int bx = ax = 123;
        ax = 456; // dead assignment to ax
        return bx;
    }
}
