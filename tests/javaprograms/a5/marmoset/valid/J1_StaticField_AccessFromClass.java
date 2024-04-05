// DISAMBIGUATION,TYPE_CHECKING,CODE_GENERATION
public class J1_StaticField_AccessFromClass {
    public static void main(String args[]) {
        System.out.println(test());
    }
	public J1_StaticField_AccessFromClass() { }
	public static int test() { 
		return Byte.MAX_VALUE - 4;
	}
}
