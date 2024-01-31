public class closest_match_constructor_overloading {
  public A() {}
  public A(Object x, Object y) {}
  public A(Object x, A y) {}
  public void m() {
    new A(new A(), new A());
  }
}