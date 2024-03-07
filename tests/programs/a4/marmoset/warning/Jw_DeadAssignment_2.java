public class Jw_DeadAssignment_2 {
    public Jw_DeadAssignment_2() {}

    public static int test() {
        int a = 0;
        int b = 2;
        a = new Jw_DeadAssignment_2().a; // dead assignment
        if (b > 0) {
            a = 2;
            b = 5;
        } else {
            for (a = 0; a < 10 ; a = a + 1)
                b = b + 1;
        }
        return a + b;
    }

    protected byte a = (byte) 3;
}
