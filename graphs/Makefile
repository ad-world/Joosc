.PHONY: all clean

GRAPH_NAME="graph.gv"
IR_GRAPH_NAME="ir_graph.gv"

all :
	dot -Tpng -O ${GRAPH_NAME}
	dot -Tpng -O ${IR_GRAPH_NAME}

clean:
	rm -f ${GRAPH_NAME}* ${IR_GRAPH_NAME}*
