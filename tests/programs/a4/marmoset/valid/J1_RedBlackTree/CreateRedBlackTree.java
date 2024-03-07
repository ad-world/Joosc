package redblacktree;

// https://www.javatpoint.com/red-black-tree-java

public class CreateRedBlackTree {
    protected static RedBlackNode nullNode = new RedBlackNode(0);
    protected RedBlackNode current;
    protected RedBlackNode parent;
    protected RedBlackNode header;
    protected RedBlackNode grand;
    protected RedBlackNode great;

    protected static int RED = 0;
    protected static int BLACK = 1;

    public CreateRedBlackTree(int header) {
        this.header = new RedBlackNode(header);
        this.header.leftChild = CreateRedBlackTree.nullNode;
        this.header.rightChild = CreateRedBlackTree.nullNode;
    }

    public void removeAll() {
        header.rightChild = CreateRedBlackTree.nullNode;
    }

    public boolean checkEmpty() {
        return header.rightChild == CreateRedBlackTree.nullNode;
    }

    public void insertNewNode(int newElement) {
        current = parent = grand = header;
        CreateRedBlackTree.nullNode.element = newElement;

        while (current.element != newElement) {
            great = grand;
            grand = parent;
            parent = current;

            if (newElement < current.element)
                current = current.leftChild;
            else
                current = current.rightChild;

            if (current.leftChild.color == CreateRedBlackTree.RED && current.rightChild.color == CreateRedBlackTree.RED)
                handleColors(newElement);
        }

        if (current != CreateRedBlackTree.nullNode)
            return;

        current = new RedBlackNode(newElement, CreateRedBlackTree.nullNode, CreateRedBlackTree.nullNode);

        if (newElement < parent.element)
            parent.leftChild = current;
        else
            parent.rightChild = current;
        handleColors(newElement);
    }

    protected void handleColors(int newElement) {
        current.color = CreateRedBlackTree.RED;
        current.leftChild.color = CreateRedBlackTree.BLACK;
        current.rightChild.color = CreateRedBlackTree.BLACK;

        if (parent.color == CreateRedBlackTree.RED) {
            grand.color = CreateRedBlackTree.RED;

            if (newElement < grand.element && grand.element != newElement && newElement < parent.element)
                parent = performRotation(newElement, grand);
            current = performRotation(newElement, great);
            current.color = CreateRedBlackTree.BLACK;
        }

        header.rightChild.color = CreateRedBlackTree.BLACK;
    }

    protected RedBlackNode performRotation(int newElement, RedBlackNode parent) {
        if (newElement < parent.element)
            if (newElement < parent.leftChild.element)
                return parent.leftChild = rotationWithLeftChild(parent.leftChild);
            else
                return parent.leftChild = rotationWithRightChild(parent.leftChild);
        else if (newElement < parent.rightChild.element)
            return parent.rightChild = rotationWithLeftChild(parent.rightChild);
        else
            return parent.rightChild = rotationWithRightChild(parent.rightChild);
    }

    protected RedBlackNode rotationWithLeftChild(RedBlackNode node2) {
        RedBlackNode node1 = node2.leftChild;
        node2.leftChild = node1.rightChild;
        node1.rightChild = node2;
        return node1;
    }

    protected RedBlackNode rotationWithRightChild(RedBlackNode node1) {
        RedBlackNode node2 = node1.rightChild;
        node1.rightChild = node2.leftChild;
        node2.leftChild = node1.leftChild;
        return node2;
    }

    public int nodesInTree() {
        return nodesInTree(header.rightChild);
    }

    protected int nodesInTree(RedBlackNode node) {
        if (node == CreateRedBlackTree.nullNode)
            return 0;
        else {
            int size = 1;
            size = size + nodesInTree(node.leftChild);
            size = size + nodesInTree(node.rightChild);
            return size;
        }
    }

    public boolean searchNode(int value) {
        return searchNode(header.rightChild, value);
    }

    protected boolean searchNode(RedBlackNode node, int value) {
        boolean check = false;
        while ((node != CreateRedBlackTree.nullNode) && check != true) {
            int nodeValue = node.element;
            if (value < nodeValue)
                node = node.leftChild;
            else if (value > nodeValue)
                node = node.rightChild;
            else {
                check = true;
                return check;
            }
            check = searchNode(node, value);
        }
        return check;
    }

    public void preorderTraversal() {
        preorderTraversal(header.rightChild);
    }

    protected void preorderTraversal(RedBlackNode node) {
        if (node != CreateRedBlackTree.nullNode) {
            char c = 'R';
            if (node.color == 1)
                c = 'B';
            System.out.print(node.element + "" + c + " ");
            preorderTraversal(node.leftChild);
            preorderTraversal(node.rightChild);
        }
    }

    public void inorderTraversal() {
        inorderTraversal(header.rightChild);
    }

    protected void inorderTraversal(RedBlackNode node) {
        if (node != CreateRedBlackTree.nullNode) {
            inorderTraversal(node.leftChild);
            char c = 'R';
            if (node.color == 1)
                c = 'B';
            System.out.print(node.element + "" + c + " ");
            inorderTraversal(node.rightChild);
        }
    }

    public void postorderTraversal() {
        postorderTraversal(header.rightChild);
    }

    protected void postorderTraversal(RedBlackNode node) {
        if (node != CreateRedBlackTree.nullNode) {
            postorderTraversal(node.leftChild);
            postorderTraversal(node.rightChild);
            char c = 'R';
            if (node.color == 1)
                c = 'B';
            System.out.print(node.element + "" + c + " ");
        }
    }
}
