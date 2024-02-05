public class implicit_this_class_for_static_methods {
  public static int m1() {
    return 42;
  }
  public int m2() {
    return m1();
  }
}