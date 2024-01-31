public class super_field_access {
  public A() {}
  public int f;
}

public class B extends A {
  public B() {}
  public int m() { return super.f; }
}