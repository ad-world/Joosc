public class Main {
    // foo's someMethod should not be seen as incorrectly trying to replace bar's someMethod
    // since A.String and B.String are different types.
    public Main() {}

    public static int test() {
	    return 123;
    }
}
