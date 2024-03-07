public class Jw_DeadAssignment_4 {
    public Jw_DeadAssignment_4() {}

    public int f(int z) {
        while (z < 1000) {
            int y = 10;
            if (z > 0) {
                return y;
            } else {
                y = z; // dead assignment
                z = z + 1;
            }
        }
        int y = z;
        return y;
    }
}
