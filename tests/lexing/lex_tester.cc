#include <iostream>
#include "parsing/bison/driver.h"
#include "parsing/bison/parser.hh"
using namespace std;

int main() {
    Driver drv;
    int line = 1;

    for ( bool space = false; true; space = true ) {
        yy::parser::symbol_type symbol = yylex(drv);

        if ( line != drv.location.end.line ) {
            cout << endl;
            line = drv.location.end.line;
        } else if ( space ) {
            cout << " ";
        }

        cout << yy::parser::symbol_name(symbol.kind());

      if ( symbol.kind() == yy::parser::symbol_kind::S_YYEOF ) { cout << endl; break; }
    }
}
