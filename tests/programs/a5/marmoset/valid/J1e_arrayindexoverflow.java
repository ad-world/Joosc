// CODE_GENERATION
public class J1e_arrayindexoverflow {
    public static char i = 'a';

    public J1e_arrayindexoverflow() { }

    public static int test() {
        short[] shorts = new short[1];
        int j = shorts[J1e_arrayindexoverflow.i];
        return 123;
    }

}
