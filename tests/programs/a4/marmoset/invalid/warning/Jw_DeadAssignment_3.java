public class Jw_DeadAssignment_3 {
    public Jw_DeadAssignment_3() {}

    public static int test() {
        int y = 0;
        int x = 9;
        y = Jw_DeadAssignment_3.f(); // dead assignment
        int z = 5 - x;
        if (x > 0) {
            y = 11;
        }
        return z;
    }

    public static int f() {
        return 8;
    }
}
