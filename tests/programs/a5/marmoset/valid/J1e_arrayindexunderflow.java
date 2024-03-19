// CODE_GENERATION
public class J1e_arrayindexunderflow {
    public static int i = 'a' - 'b';

    public J1e_arrayindexunderflow() { }

    public static int test() {
        short[] shorts = new short[1];
        int j = shorts[J1e_arrayindexunderflow.i];
        return 123;
    }

}
