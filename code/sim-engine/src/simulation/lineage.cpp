#include "lineage.h"

json Lineage::toJson()
{
    json lineage;
    lineage["id"] = id;
    json generations_json;
    for (auto g : generations)
        generations_json.push_back(g);
    lineage["generations"] = generations;
    json cells_json;
    for (auto c : cells)
        cells_json.push_back(c->toJson());
    lineage["cells"] = cells_json;
    return lineage;
}
