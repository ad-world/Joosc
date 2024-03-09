package redblacktree;

// https://www.javatpoint.com/red-black-tree-java

public class RedBlackNode {      
    public RedBlackNode leftChild;
    public RedBlackNode rightChild;  
    public int element;  
    public int color;  
  
    public RedBlackNode(int element) {  
        this.element = element;  
    }   
  
    public RedBlackNode(int element, RedBlackNode leftChild, RedBlackNode rightChild) {  
        this.element = element;  
        this.leftChild = leftChild;  
        this.rightChild = rightChild;  
        color = 1;  
    }      
}