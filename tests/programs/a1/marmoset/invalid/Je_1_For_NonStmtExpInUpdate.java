// JOOS1:PARSER_WEEDER,PARSER_EXCEPTION,SYNTAX_ERROR
// JOOS2:PARSER_WEEDER,PARSER_EXCEPTION,SYNTAX_ERROR
// JAVAC:UNKNOWN
// 
/**
 * Parser/weeder:
 * - Non-statement-expression not allowed in update clause of for loop
 */
public class Je_1_For_NonStmtExpInUpdate {

    public Je_1_For_NonStmtExpInUpdate() {}

    public static int test() {
    for (boolean b=false;b;true||true)
    	return 123;
    }

}
