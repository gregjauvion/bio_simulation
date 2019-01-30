#include "cell.h"

/* Serialization */
json Cell::toJson()
{
    json cell;
    cell["id"] = id;
    cell["volume"]=volume;
    cell["volume_birth"]=volume_birth;
    cell["quantities"] = quantities;
    cell["growth_formula"] = growth_formula->toString();
    cell["division_trigger"] = division_trigger->toString();
    cell["volume_share"] = volume_share->toString();
    return cell;
}
