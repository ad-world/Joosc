public class synchronized_statement {
  public int x;
  public void m() {
    synchronized(x) {
      x = x-1;
    }
}