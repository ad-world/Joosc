public class NoThisSuperExample {
    protected int value;

    public NoThisSuperExample() {
        // Valid constructor without explicit this() or super() calls
    }

    public void setValue(int value) {
        this.value = value;  // Valid usage of this
    }

    public NoThisSuperExample() {

    }
}
