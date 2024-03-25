// CODE_GENERATION
public class J1e_arraylengthneg {

    public J1e_arraylengthneg() { }

    public static int test() {
        int i = 1;
        while (i + 10 >= 0) {
            i = i - 1;
            int[] ints = new int[i];
        }
        return 123;
    }

}
