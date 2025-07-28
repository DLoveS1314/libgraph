#include <iostream>
#include <string>
#include "../include/GeoJSONGraphConverter.h"
#include "../include/Graph.h"

// 示例GeoJSON路网数据
const std::string SAMPLE_GEOJSON = R"(
{
    "type": "FeatureCollection",
    "features": [
        {
            "type": "Feature",
            "geometry": {
                "type": "LineString",
                "coordinates": [
                    [116.3974, 39.9083],  // 北京天安门
                    [116.4074, 39.9083],  // 东移1公里
                    [116.4074, 39.9183]   // 北移1公里
                ]
            }
        },
        {
            "type": "Feature",
            "geometry": {
                "type": "LineString",
                "coordinates": [
                    [116.4074, 39.9183],  // 连接到前一条线的终点
                    [116.4174, 39.9183]   // 东移1公里
                ]
            }
        }
    ]
}
)";

void testGeoJSONConversion() {
    try {
        // 转换GeoJSON到图
        Graph graph = GeoJSONGraphConverter::fromGeoJSON(SAMPLE_GEOJSON);

        // 输出图信息
        std::cout << "成功创建图结构:\n";
        std::cout << "节点数量: " << graph.getNodes().size() << std::endl;

        // 输出节点ID列表
        std::cout << "节点ID列表:\n";
        for (const auto* node : graph.getNodes()) {
            std::cout << "- " << node->getId() << std::endl;
        }

        // 演示最短路径查询
        if (graph.getNodes().size() >= 2) {
            auto it = graph.getNodes().begin();
            Node* startNode = *it;
            std::advance(it, graph.getNodes().size() - 1);
            Node* endNode = *it;

            auto path = graph.findShortestPathDijkstra(*startNode, *endNode);
            std::cout << \n从 "" << startNode->getId() << "" 到 "" << endNode->getId() << "" 的最短路径:\n";
            for (const auto* edge : path) {
                std::cout << "- 边权重: " << edge->getWeight() << "km\n";
            }
        }

    } catch (const std::exception& e) {
        std::cerr << "转换失败: " << e.what() << std::endl;
        return 1;
    }

}