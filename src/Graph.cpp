#include "../include/Graph.h"
#include "json.hpp"

#include <map>
#include <limits>
#include <queue>
#include <functional>
#include <unordered_set>  // 需要包含头文件
#include <fstream>        // 需要包含头文件
//-------------------------------------------------------------------------------------------------

Graph::~Graph() 
{ 
    // free all nodes and edges
    for (Edge* pEdge : m_edges) delete pEdge;
    for (Node* pNode : m_nodes) delete pNode;
}


//-------------------------------------------------------------------------------------------------

bool Graph::remove(const Edge& rEdge)
{
    auto it = std::find(m_edges.begin(), m_edges.end(), &rEdge);
    if (it != m_edges.end()) {
        delete *it;
        m_edges.erase(it);
        return true;
    }

    return false;
}


//-------------------------------------------------------------------------------------------------

bool Graph::remove(const Node& rNode)
{
    auto it = std::find(m_nodes.begin(), m_nodes.end(), &rNode);
    if (it != m_nodes.end()) {
        // delete all edges that are connected with the given node
        auto eIt = m_edges.begin();
        while (eIt != m_edges.end()) {
            if ((*eIt)->isConnectedTo(rNode)) {
                delete *eIt;
                eIt = m_edges.erase(eIt);
            }
            else {
                eIt++;
            }
        }
        // delete the node
        delete *it;
        m_nodes.erase(it);
        return true;
    }
    return false;
}


//-------------------------------------------------------------------------------------------------

Node* Graph::findNodeById(const std::string& id)
{
    auto it = m_nodes.begin();
    // just need to check the lower bound, since nodes are sorted by id.
    while (it != m_nodes.end() && (*it)->getId() < id) {
        it++;
    }

    // return the node, if found
    if (it != m_nodes.end() && (*it)->getId() == id) {
        return *it;
    }

    return NULL;
}


//-------------------------------------------------------------------------------------------------

Graph::tEdges Graph::findEdges(const Node& rSrc, const Node& rDst)
{
    tEdges ret;

    for (auto it = m_edges.begin(); it != m_edges.end(); it++) {
        if ((&(*it)->getSrcNode() == &rSrc) && (&(*it)->getDstNode() == &rDst)) {
            ret.push_back(*it);
        }
    }

    return ret;
}


//-------------------------------------------------------------------------------------------------

Graph::tEdges Graph::findEdges(const std::string& srcId, const std::string& dstId)
{
    Node* pSrc = findNodeById(srcId);
    if (pSrc == NULL) {
        return tEdges();
    }

    Node* pDst = findNodeById(dstId);
    if (pDst == NULL) {
        return tEdges();
    }

    return findEdges(*pSrc, *pDst);
}


//-------------------------------------------------------------------------------------------------

std::string Graph::toString() const
{
	std::string result;

	for (auto it = m_edges.begin(); it != m_edges.end(); it++)
	{
		result += (*it)->toString() + "\n";
	}

	return result;
}


//-------------------------------------------------------------------------------------------------

void Graph::saveAsDot(const std::string& rFilename) const {
    // std::ofstream ofs(rFilename);
    // ofs << "digraph G {\n";
    // // 输出所有节点（可加属性）
    // for (const Node* node : m_nodes) {
    //     ofs << "    \"" << node->getId() << "\"";
    //     ofs << " [label=\"" << node->getId() << "\"]";
    //     ofs << ";\n";
    // }
    // // 输出所有边
    // for ( Edge* edge : m_edges) {
    //     if (pMyEdge) {
    //         ofs << "    \"" << edge->getSrcNode().getId() << "\" -> \"" << pMyEdge->getDstNode().getId() << "\"";
    //         ofs << " [label=\"" << pMyEdge->getWeight() << "\"]";
    //         ofs << ";\n";
    //     }
    // }
    // ofs << "}\n";
    // ofs.close();
}


//-------------------------------------------------------------------------------------------------

/**
* This is based on https://en.wikipedia.org/wiki/Dijkstra%27s_algorithm
*/
Graph::tDijkstraMap Graph::findDistancesDijkstra(
        const Node& rSrcNode, const Node* pDstNode, Node** pFoundDst)
{
    tDijkstraMap nodeTable; // 保存每个节点的最短距离和路径信息
    std::list<Node*> Q;     // 待访问节点集合（未确定最短距离的节点）

    // 查找源节点指针
    auto srcIt = std::find(m_nodes.begin(), m_nodes.end(), &rSrcNode);
    if (srcIt == m_nodes.end()) {
        throw InvalidNodeException("source node is not in the graph");
    }
    Node* pSrc = *srcIt;

    // 查找目标节点指针（如果有）
    Node* pDst = NULL;
    if (pDstNode != NULL) {
        auto dstIt = std::find(m_nodes.begin(), m_nodes.end(), pDstNode);
        if (dstIt == m_nodes.end()) {
            throw InvalidNodeException("destination node is not in the graph");
        }
        pDst = *dstIt;
    }

    // 初始化所有节点的距离为无穷大，前驱为NULL，并加入Q
    for (Node* pNode : m_nodes) {
        Q.push_back(pNode);
        nodeTable[pNode] = { std::numeric_limits<double>::max(), NULL, NULL };
    }
    // 源节点距离设为0
    nodeTable[pSrc].distance = 0;

    // 主循环：每次选出Q中距离最小的节点u
    while (!Q.empty()) {
        Node* u = Q.front();
        
        for (Node* pCurrentNode : Q) {
            if (nodeTable[pCurrentNode].distance < nodeTable[u].distance) {
                u = pCurrentNode;
            }
        }

        // 如果u是目标节点，提前返回
        if (u == pDst) {
            *pFoundDst = u;
            return nodeTable;
        }

        // 从Q中移除u，表示u的最短距离已确定
        Q.remove(u);
        // 如果起点v_0 到目标点u之间的某一条路径是最短路径，
        // 那么在该路径上面，任何一个点u ′ 到 v_0 的路径都是u ′ 到 v_0 的最短路径。
        // 遍历u的所有出边，尝试更新邻居节点v的距离
        for (Edge* pOutEdge : u->getOutEdges()) {
            Node* v = &pOutEdge->getDstNode();
            double newDistance = nodeTable[u].distance + pOutEdge->getWeight();
            tDijkstraInfo& vEntry = nodeTable[v];
            // 如果通过u到v的距离更短，则更新v的距离和前驱信息
            if (newDistance < vEntry.distance) {
                vEntry.distance = newDistance;
                vEntry.prevNode = u;
                vEntry.prevEdge = pOutEdge;
            }
        }
    }
    // 如果没有找到目标节点，返回所有节点的最短距离
    *pFoundDst = NULL;
    return nodeTable;
}



using HeapEntry = std::pair<Node*, double>;
struct CompareDist {
        bool operator()(const HeapEntry& lhs, const HeapEntry& rhs) const {
            return lhs.second > rhs.second;  // 最小堆：距离小的优先
        }
    };

/**
* This is based on https://en.wikipedia.org/wiki/Dijkstra%27s_algorithm 使用优先队列进行优化
*/
Graph::tDijkstraMap Graph::findDistancesDijkstraV1(
        const Node& rSrcNode, const Node* pDstNode, Node** pFoundDst)
{


    tDijkstraMap nodeTable;  // 存储最短路径信息
    std::priority_queue<HeapEntry, std::vector<HeapEntry>, CompareDist> minHeap;
    std::unordered_set<Node*> visited;  // 跳过已确定最短路径的节点
    // 找到源节点指针
    auto srcIt = std::find(m_nodes.begin(), m_nodes.end(), &rSrcNode);
    if (srcIt == m_nodes.end()) {
        throw InvalidNodeException("source node is not in the graph");
    }
    Node* pSrc = *srcIt;

    // 检查目标节点（可选）
    Node* pDst = nullptr;
    if (pDstNode != nullptr) {
        auto dstIt = std::find(m_nodes.begin(), m_nodes.end(), pDstNode);
        if (dstIt == m_nodes.end()) {
            throw InvalidNodeException("destination node is not in the graph");
        }
        pDst = *dstIt;
    }

    // 初始化所有节点为“未知距离”
    for (Node* pNode : m_nodes) {
        nodeTable[pNode] = { std::numeric_limits<double>::max(), nullptr, nullptr };
    }

    // 初始化源节点
    nodeTable[pSrc].distance = 0.0;
    minHeap.push({ pSrc, 0.0 });

    // 主循环
    while (!minHeap.empty()) {
        Node* u = minHeap.top().first;
        minHeap.pop();

        if (visited.count(u)) {
            continue;  // 已处理过
        }
        visited.insert(u);

        // 如果到达目标节点，提前返回
        if (u == pDst) {
            *pFoundDst = u;
            return nodeTable;
        }

        // 遍历所有出边
        for (Edge* pOutEdge : u->getOutEdges()) {
            Node* v = &pOutEdge->getDstNode();
            double newDistance = nodeTable[u].distance + pOutEdge->getWeight();
            tDijkstraInfo& vInfo = nodeTable[v];

            if (newDistance < vInfo.distance) {
                vInfo.distance = newDistance;
                vInfo.prevNode = u;
                vInfo.prevEdge = pOutEdge;
                minHeap.push({v, newDistance});
            }
        }
    }

    // 未找到目标节点
    *pFoundDst = nullptr;
    return nodeTable;
}


//-------------------------------------------------------------------------------------------------

Graph::tPath Graph::findShortestPathDijkstra(const Node& rSrc, const Node& rDst,bool useV1)
{
    Graph::tPath path;
    Node* currentNode;
    tDijkstraMap nodeTable; // Declare nodeTable here
    if (useV1) {
        // 使用优化版本
        nodeTable = findDistancesDijkstraV1(rSrc, &rDst, &currentNode);
    } else {
        // 使用原始版本
        nodeTable = findDistancesDijkstra(rSrc, &rDst, &currentNode);
    }
    // currentNode will be NULL, if no path was found.
    // tDijkstraMap nodeTable = findDistancesDijkstra(rSrc, &rDst, &currentNode);
    
    // insert the path to a deque
    if (currentNode != NULL) {
        while (nodeTable[currentNode].prevNode != NULL) {
            tDijkstraInfo& curr = nodeTable[currentNode];
            path.push_front(curr.prevEdge);
            currentNode = curr.prevNode;
        }
    }

    return path;
}


//-------------------------------------------------------------------------------------------------

void Graph::saveAsJson(const std::string& filename) const {
    nlohmann::json j;
    // 节点
    for (const Node* node : m_nodes) {
        j["nodes"].push_back({
            {"id", node->getId()},
            {"lon", node->getLon()},
            {"lat", node->getLat()}
        });
    }
    // 边
    for ( Edge* edge : m_edges) {
        j["edges"].push_back({
            {"src", edge->getSrcNode().getId()},
            {"dst", edge->getDstNode().getId()},
            {"weight", edge->getWeight()}
        });
    }
    std::ofstream ofs(filename);
    ofs << j.dump(2);
    ofs.close();
}

void Graph::loadFromJson(const std::string& filename) {
    std::ifstream ifs(filename);
    nlohmann::json j;
    ifs >> j;
    ifs.close();
    // 清空原有数据（如有需要可补充）
    m_nodes.clear();
    m_edges.clear();
    // 加载节点
    for (const auto& nodej : j["nodes"]) {
        std::string id = nodej["id"];
        double lon = nodej["lon"];
        double lat = nodej["lat"];
        makeNode(Node(id, lon, lat));
    }
    // 加载边
    for (const auto& edgej : j["edges"]) {
        std::string src = edgej["src"];
        std::string dst = edgej["dst"];
        double weight = edgej["weight"];
        Node* srcNode = findNodeById(src);
        Node* dstNode = findNodeById(dst);
        
        if (srcNode && dstNode) {
            makeBiEdge<SimpleEdge>(*srcNode, *dstNode, weight);
        }
    }
}
//-------------------------------------------------------------------------------------------------
