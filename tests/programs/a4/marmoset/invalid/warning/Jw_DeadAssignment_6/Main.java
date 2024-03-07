import java.util.Random;

public class Main {
    public Main() {
    }

    public static Random r = new Random();

    public boolean f(boolean c) {
        boolean b = true;
        for (; b | c; b = Main.r.nextBoolean()) {
            if (!b) {
                return true;
            } else {
                b = false; // dead assignment
            }
        }
        return b;
    }
}
