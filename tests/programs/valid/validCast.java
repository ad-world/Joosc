public class validCast {
    public validCast() {}

    public static int test() {
        int hello;
        char[] abcdef = new char[5];
        int goodbye = (int) hello;
        goodbye = (validCast) hello;
        return 8;
    }
}