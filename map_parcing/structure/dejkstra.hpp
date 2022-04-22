#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <sqlite3.h>
#include <stdio.h>

struct graph {
	struct graphShadingEdge {
		uint64_t fineness; //Êðóïíîñòü äîðîãè, ò.å. ÷åì áîëåå êðóïíàÿ äîðîãà òåì áîëüøå ýòà âåëè÷èíà 
		double shading; // Çàòåíåííîñòü äîðîãè âûðàæåííàÿ â äëèíå íåçàòåíåííîé ÷àñòè
		double length; // Äëèíà äîðîãè 
		uint64_t node; //Âåðøèíà êîíöà ðåáðà
		uint64_t prevNode; //âåðøèíà íà÷àëà ðåáðà
	};

	struct graphNode {
		double x;
		double y;

	};
	struct graphRoute {
		std::vector<graphNode> Nodes;
		double shading;
	};
	struct weightNode {
		uint64_t index;
		int fineness;
	};

    std::string db_path;
	
	std::vector<graphShadingEdge> Edges;
	std::vector<graphNode> Nodes;
	std::vector<std::vector<weightNode>> adjacencyMatrix;
	graphShadingEdge getShadingEdge(uint64_t fineness, uint64_t Node1, uint64_t Node2);
	graphNode getNode(uint64_t Node);// --map
	double getShading(graphNode Node1, graphNode Node2);// -shading
	graphRoute getRoute(uint64_t startNode, uint64_t endNode);// ---output
	std::vector<weightNode> getadjacencyMatrix(uint64_t Node);// -map
	double getLength2(graphNode Node1, graphNode Node2);
	graph();
    ~graph();
};

graph::graph(): db_path("../database/shadow.db") {

}

graph::~graph() {
    
}
