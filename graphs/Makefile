.PHONY: all clean

GRAPH_NAME="graph.gv"
CFG_GRAPH_NAME="cfggraph.gv"

all :
	dot -Tpng -O ${GRAPH_NAME}
	dot -Tpng -O ${CFG_GRAPH_NAME}

clean:
	rm -f ${GRAPH_NAME}* ${CFG_GRAPH_NAME}*
