#pragma once

#include <unordered_map>
#include <vector>

#include <argos3/plugins/robots/foot-bot/simulator/footbot_entity.h>

namespace argos {

Real centroid_of_centroids(unsigned long step, std::unordered_map<unsigned long, std::vector<CFootBotEntity *>> map);

}
