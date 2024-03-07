public class Jw_DeadAssignment_5 {
    public Jw_DeadAssignment_5() {}

    public int f() {
        int y = 0;
        for (int z=0 ;
            y < 8 ;
            z = y)  // dead assignment
        {
            y = 1 + y;
        }
        return y;
    }
}
