package joosc.ir.interpret;

import joosc.ir.ast.*;
import joosc.ir.visit.CheckCanonicalIRVisitor;

public class Main {

    public static void main(String[] args) {
        // Runs a simple program in the interpreter

        // IR roughly corresponds to the following:
        //     int a(int i, int j) {
        //         return i + 2 * j;
        //     }
        //     int b(int i, int j) {
        //         int x = a(i, j) + 1;
        //         return x + 20 * 5;
        //     }

        String arg0 = Configuration.ABSTRACT_ARG_PREFIX + 0;
        String arg1 = Configuration.ABSTRACT_ARG_PREFIX + 1;

        Stmt aBody = new Seq(new Move(new Temp("i"), new Temp(arg0)),
                                 new Move(new Temp("j"), new Temp(arg1)),

                                 new Return(new BinOp(BinOp.OpType.ADD,
                                         new Temp("i"),
                                         new BinOp(BinOp.OpType.MUL,
                                                 new Const(2),
                                                 new Temp("j")))));
        FuncDecl aFunc = new FuncDecl("a", 2, aBody);

        Stmt bBody =
                new Seq(new Move(new Temp("i"), new Temp(arg0)),
                          new Move(new Temp("j"), new Temp(arg1)),
                          new Move(new Temp("x"),
                                  new BinOp(BinOp.OpType.ADD,
                                          new Call(new Name("a"),
                                                  new Temp("i"),
                                                  new Temp("j")),
                                          new Const(1))),

                          new Return(new BinOp(BinOp.OpType.ADD,
                                  new Temp("x"),
                                  new BinOp(BinOp.OpType.MUL,
                                          new Const(20),
                                          new Const(5)))));
        FuncDecl bFunc = new FuncDecl("b", 2, bBody);

        CompUnit compUnit = new CompUnit("test");
        compUnit.appendFunc(aFunc);
        compUnit.appendFunc(bFunc);

        // IR interpreter demo
        {
            Simulator sim = new Simulator(compUnit);
            long result = sim.call("b", -1, 1);
            System.out.println("b(-1,1) evaluates to " + result);
        }

        // IR canonical checker demo
        {
            CheckCanonicalIRVisitor cv = new CheckCanonicalIRVisitor();
            System.out.print("Canonical? ");
            System.out.println(cv.visit(compUnit));
        }
    }
}
