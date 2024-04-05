// CODE_GENERATION
public class J1e_nullarrayindexing {
    public static void main(String args[]) {
        System.out.println(test());
    }

    protected static byte[] arr;

    public J1e_nullarrayindexing() { }

    public static int test() {
        int i = 0;
        J1e_nullarrayindexing.arr[i] = (byte) 8;
        return 123;
    }

}
