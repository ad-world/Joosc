public class NoThisSuperExample {
    private int value;

    public NoThisSuperExample() {
        // Valid constructor without explicit this() or super() calls
    }

    public void setValue(int value) {
        this.value = value;  // Valid usage of this
    }
}
