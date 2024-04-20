public class simple_static_set {
    public simple_static_set() {}

    public static int x = 120;

    public static int test() {
        simple_static_set.x = 123;
        return simple_static_set.x;
    }
}
