public class validCast {
    public validCast() {}

    public static int test() {
        int hello = 2;
        char[] abcdef = new char[5];
        int goodbye = (int) hello;
        goodbye = (validCast) hello;
        return 8;
    }
}