public class invalidCast {
    public invalidCast() {}

    public static int test() {
        int hello;
        char[] abcdef = new char[5];
        int goodbye = (abcdef[0]) hello;
        return 8;
    }
}