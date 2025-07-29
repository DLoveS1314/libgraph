#ifndef GEOJSONGRAPHCONVERTER_H
#define GEOJSONGRAPHCONVERTER_H

#include <string>
#include "Graph.h"
#include "json.hpp"

class GeoJSONGraphConverter {
public:
    // 将GeoJSON字符串转换为Graph对象
    static int fromGeoJSON(Graph & graph,const std::string& geojson);

private:
    // 计算两点间的Haversine距离(单位:公里)
    static double haversineDistance(double lon1, double lat1, double lon2, double lat2);
    
    // 根据坐标生成唯一节点ID
    static std::string generateNodeId(double lon, double lat);
};

#endif // GEOJSONGRAPHCONVERTER_H