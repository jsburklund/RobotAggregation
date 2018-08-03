#pragma once

#include <unordered_map>
#include <vector>

#include <argos3/core/utility/datatypes/datatypes.h>
#include <argos3/plugins/robots/foot-bot/simulator/footbot_entity.h>

namespace argos {

Real cluster_metric(unsigned long step, std::unordered_map<unsigned long, std::vector<CFootBotEntity *>> map);

}
