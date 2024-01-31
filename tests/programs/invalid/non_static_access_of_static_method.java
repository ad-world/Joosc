public class non_static_access_of_static_method {
  public static int m1() {
    return 42;
  }
  public int m2() {
    A a = new A();
    return a.m1();
  }
}