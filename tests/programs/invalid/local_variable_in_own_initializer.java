public class local_variable_in_own_initializer {
  public A() {}
  public void m() {
    int x  = (x = 1) + x;
  }
}