#include <iostream>

#define TESTING

#include "../include/Graph.h"
#include "../include/SimpleEdge.h"

#include <algorithm>
#include <chrono>


/*-----------------------------------------------------------------------------------------------*/

template <class T>
double getExecutionSpeed(T func) 
{
    using namespace std::chrono;
    auto t1 = high_resolution_clock::now();
    func();
    auto t2 = high_resolution_clock::now();
    return duration_cast<duration<double>>(t2 - t1).count();
}


/*-----------------------------------------------------------------------------------------------*/

class GraphTesting {

public:

    GraphTesting() 
    {
        // Make some nodes and Edges
        Node& rMunich = g.makeNode(Node("Munich"));
        Node& rHamburg = g.makeNode<Node>("Hamburg");
        Node& rBerlin = g.makeNode<Node>("Berlin");
        Node& rFrankfurt = g.makeNode<Node>("Frankfurt");

        g.makeEdge<SimpleEdge>(rBerlin, rHamburg, 450);
        g.makeEdge<SimpleEdge>(rHamburg, rBerlin, 450);
        // You can make the edges bidirectional, too:
        g.makeBiEdge<SimpleEdge>(rBerlin, rMunich, 650);
        g.makeBiEdge<SimpleEdge>(rBerlin, rFrankfurt, 590);

        // find the shortest path
        auto path = g.findShortestPathDijkstra(rHamburg, rMunich);
        for (Edge* pEdge : path) {
            // dynamic_cast to you Edge type is useful, if you have multiple different types of edges.
            SimpleEdge* pMyEdge = dynamic_cast<SimpleEdge*>(pEdge);
            if (pMyEdge != NULL) {
                std::cout << pEdge->toString() << std::endl;
            }
        }
    }

    /* TEST: Nodes should be sorted by ID*/
    void testNodeOrder()
    {
        std::cout << "testNodeOrder: ";

        if (!std::is_sorted(g.m_nodes.begin(), g.m_nodes.end(),
                [](Node* pFirst, Node* pSecond) -> bool { 
                    return pFirst->getId() < pSecond->getId();
                }))
        {
            std::cout << "The Nodes are not sorted correctly!" << std::endl;
            return;
        }

        std::cout << "OK" << std::endl;
    }


    void testRouting()
    {
        std::cout << "testRouting: ";

        Node* p1 = g.findNodeById("Hamburg");
        Node* p2 = g.findNodeById("Frankfurt");

        auto path = g.findShortestPathDijkstra(*p1, *p2);
        if (path.empty()) {
            std::cout << "Shortest Path not found!" << std::endl;
            return;
        }

        std::cout << "OK" << std::endl;
    }


    void measSearchSpeed() {
        
        std::vector<double> execTimes;

        execTimes.push_back(getExecutionSpeed([&]() {
            g.findNodeById("Berlin");
        }));

        execTimes.push_back(getExecutionSpeed([&]() {
            g.findNodeById("Hamburg");
        }));

        execTimes.push_back(getExecutionSpeed([&]() {
            g.findNodeById("Konstanz");
        }));

        execTimes.push_back(getExecutionSpeed([&]() {
            g.findNodeById("Munich");
        }));

        std::cout << "Search times: ";
        for (double time : execTimes) {
            std::cout << time << "s, ";
        }
        std::cout << std::endl;
    }


private:

    Graph g;
};


/*-----------------------------------------------------------------------------------------------*/

int main2()
{
    GraphTesting gt;

    std::cout << "---- Test results: --------------" << std::endl;
    gt.testNodeOrder();
    gt.testRouting();

    std::cout << "---- Time measurements: ---------" << std::endl;
    gt.measSearchSpeed();

    return 0;
}

int main1()
{
  // You can subclass Node, in order to add functionallity to the nodes.
  Node& rMunich = g.makeNode(Node("Munich"));
  Node& rHamburg = g.makeNode<Node>("Hamburg");
  Node& rBerlin = g.makeNode<Node>("Berlin");
  Node& rFrankfurt = g.makeNode<Node>("Frankfurt");

  // SimpleEdge is useful for some cases, but you can also subclass Edge.
  g.makeEdge<SimpleEdge>(rBerlin, rHamburg, 450);
  g.makeEdge<SimpleEdge>(rHamburg, rBerlin, 450);
  // You can make the edges bidirectional, too:
  g.makeBiEdge<SimpleEdge>(rBerlin, rMunich, 650);
  g.makeBiEdge<SimpleEdge>(rBerlin, rFrankfurt, 590);

  // find the shortest path between any type of nodes, regarding the weight of your edges
  auto path = g.findShortestPathDijkstra(rHamburg, rMunich);
  for (Edge* pEdge : path) {
      // dynamic_cast to your Edge type is useful, if you have multiple different types of edges.
      SimpleEdge* pMyEdge = dynamic_cast<SimpleEdge*>(pEdge);
      if (pMyEdge != NULL) {
          std::cout << pEdge->toString() << std::endl;
      }
  }

  return 0;
}
int main()
{
    // Uncomment the following line to run the testing main
    // return main2();

    // Otherwise, run the example main
    return main1();
}
/*-----------------------------------------------------------------------------------------------*/
