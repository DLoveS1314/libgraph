#include "GeoJSONGraphConverter.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <cmath>
#include "Graph.h"
#include "Node.h"
#include "SimpleEdge.h"
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
using json = nlohmann::json;

// Haversine formula implementation
// Earth radius in kilometers
constexpr double EARTH_RADIUS = 6371.0;

double GeoJSONGraphConverter::haversineDistance(double lon1, double lat1, double lon2, double lat2) {
    double dLat = (lat2 - lat1) * M_PI / 180.0;
    double dLon = (lon2 - lon1) * M_PI / 180.0;
    lat1 = lat1 * M_PI / 180.0;
    lat2 = lat2 * M_PI / 180.0;
    double a = pow(sin(dLat / 2), 2) + pow(sin(dLon / 2), 2) * cos(lat1) * cos(lat2);
    double c = 2 * atan2(sqrt(a), sqrt(1 - a));
    return EARTH_RADIUS * c;
}

// 生成UUID的辅助函数
#include <random>
#include <iomanip>
std::string generateUUID() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(0, 15);
    static std::uniform_int_distribution<> dis2(8, 11);
    std::stringstream ss;
    int i;
    ss << std::hex;
    for (i = 0; i < 8; i++) ss << dis(gen);
    ss << "-";
    for (i = 0; i < 4; i++) ss << dis(gen);
    ss << "-4";
    for (i = 0; i < 3; i++) ss << dis(gen);
    ss << "-" << dis2(gen);
    for (i = 0; i < 3; i++) ss << dis(gen);
    ss << "-";
    for (i = 0; i < 12; i++) ss << dis(gen);
    return ss.str();
}

std::string GeoJSONGraphConverter::generateNodeId(double lon, double lat) {
    std::ostringstream oss;
    // 容差是1e-6 大概1cm左右
    oss << std::fixed << std::setprecision(6) << lon << "_" << lat;
    return oss.str();
}

int  GeoJSONGraphConverter::fromGeoJSON( Graph & graph, const std::string& geojson) {

    json j = json::parse(geojson);
    if (!j.contains("features")) return 0;
    // 添加进度条
    std::cout << "正在转换GeoJSON数据到图结构..." << std::endl;
    size_t total = j["features"].size();
    size_t count = 0;
    for (const auto& feature : j["features"]) {
        // 进度条显示
        if (count % 100 == 0 || count == total - 1) {
            int percent = static_cast<int>(100.0 * count / total);
            std::cout << "\r解析进度: [" << std::string(percent/2, '=') << std::string(50-percent/2, ' ') << "] " << percent << "%" << std::flush;
        }
        ++count;
        if (!feature.contains("geometry")) continue;
        // if (!feature.contains("id")) continue; // 确保每个Feature都有ID
        // std::string featureId = feature["id"];
        const auto& geometry = feature["geometry"];
        if (!geometry.contains("type") || !geometry.contains("coordinates")) continue;
        std::string type = geometry["type"];
        if (type == "LineString") {
            const auto& coords = geometry["coordinates"];
         
            for (size_t i = 1; i < coords.size(); ++i) {
                double lon1 = coords[i-1][0], lat1 = coords[i-1][1];
                double lon2 = coords[i][0], lat2 = coords[i][1];
                std::string id1 = generateNodeId(lon1, lat1);
                std::string id2 = generateNodeId(lon2, lat2);
                // 查找或创建节点1
                Node* node1 = graph.findNodeById(id1);
                if (!node1) {
                    node1 = &graph.makeNode(Node(id1, lon1, lat1));
                    // 可在此扩展Node存储经纬度
                }
                // 查找或创建节点2
                Node* node2 = graph.findNodeById(id2);
                if (!node2) {
                    node2 = &graph.makeNode(Node(id2, lon2, lat2));
                    // 可在此扩展Node存储经纬度
                }
                //std::cout << "Creating edge from " << node1_1.getId() << " to " << node2_1.getId() << std::endl;
                // std::cout <<  " Creating edge from " << node1->getId() << " to " << node2->getId() << std::endl;

                double dist = haversineDistance(lon1, lat1, lon2, lat2);
              
                graph.makeBiEdge<SimpleEdge>(*node1, *node2, dist);
            }
        }
        // 可扩展支持Point、Polygon等
    }
    std::cout << std::endl; // 完成后换行
    return 1;
}
