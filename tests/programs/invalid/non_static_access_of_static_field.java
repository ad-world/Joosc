public class non_static_access_of_static_field {
  public static int x;
  public int m() {
    A a = new A();
    return a.x;
  }
}