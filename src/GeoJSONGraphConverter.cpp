#include "../include/GeoJSONGraphConverter.h"
#include <cmath>
#include <unordered_map>
#include "../include/Node.h"
#include "../include/SimpleEdge.h"

// 地球半径(公里)
const double EARTH_RADIUS_KM = 6371.0;

// 角度转弧度
static double toRadians(double degrees) {
    return degrees * M_PI / 180.0;
}

// 实现Haversine距离计算
double GeoJSONGraphConverter::haversineDistance(double lon1, double lat1, double lon2, double lat2) {
    double dLat = toRadians(lat2 - lat1);
    double dLon = toRadians(lon2 - lon1);
    
    double a = sin(dLat/2) * sin(dLat/2) +
               cos(toRadians(lat1)) * cos(toRadians(lat2)) *
               sin(dLon/2) * sin(dLon/2);
    
    double c = 2 * atan2(sqrt(a), sqrt(1-a)); 
    return EARTH_RADIUS_KM * c; // 距离(公里)
}

// 生成节点ID (使用坐标的字符串表示)
std::string GeoJSONGraphConverter::generateNodeId(double lon, double lat) {
    // 保留6位小数确保精度
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%.6f,%.6f", lon, lat);
    return std::string(buffer);
}

// 从GeoJSON创建图
Graph GeoJSONGraphConverter::fromGeoJSON(const std::string& geojson) {
    Graph graph;
    std::unordered_map<std::string, Node*> nodeMap; // 缓存已创建的节点

    try {
        // 解析GeoJSON
        nlohmann::json j = nlohmann::json::parse(geojson);

        // 检查是否为FeatureCollection
        if (j["type"].get_string() != "FeatureCollection") {
            throw std::runtime_error("GeoJSON must be a FeatureCollection");
        }

        // 遍历所有要素
        for (const auto& feature : j["features"]) {
            // 只处理LineString类型的要素
            if (feature["geometry"]["type"].get_string() == "LineString") {
                const auto& coordinates = feature["geometry"]["coordinates"];

                // 遍历坐标点对，创建边
                for (size_t i = 0; i < coordinates.size() - 1; ++i) {
                    // 获取当前点和下一个点的坐标
                    double lon1 = coordinates[i][0].get_number();
                    double lat1 = coordinates[i][1].get_number();
                    double lon2 = coordinates[i+1][0].get_number();
                    double lat2 = coordinates[i+1][1].get_number();

                    // 生成节点ID
                    std::string id1 = generateNodeId(lon1, lat1);
                    std::string id2 = generateNodeId(lon2, lat2);

                    // 获取或创建节点
                    Node* node1 = nullptr;
                    Node* node2 = nullptr;

                    if (nodeMap.find(id1) == nodeMap.end()) {
                        node1 = &graph.makeNode(Node(id1));
                        nodeMap[id1] = node1;
                    } else {
                        node1 = nodeMap[id1];
                    }

                    if (nodeMap.find(id2) == nodeMap.end()) {
                        node2 = &graph.makeNode(Node(id2));
                        nodeMap[id2] = node2;
                    } else {
                        node2 = nodeMap[id2];
                    }

                    // 计算两点间距离作为边的权重
                    double distance = haversineDistance(lon1, lat1, lon2, lat2);

                    // 创建双向边(无向图)
                    graph.makeBiEdge<SimpleEdge>(*node1, *node2, distance);
                }
            }
        }
    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to parse GeoJSON: " + std::string(e.what()));
    }

    return graph;
}