#pragma once

#include <controllers/footbot/segregation_footbot_controller.h>

#include <argos3/core/utility/math/rng.h>
#include <argos3/plugins/robots/foot-bot/simulator/footbot_entity.h>

#include <loop_functions/segregation_loop_function.h>

using namespace argos;

class GauciLoopFunction : public SegregationLoopFunction {

  Real CostAtStep(unsigned long step, std::unordered_map<unsigned long, std::vector<CFootBotEntity *>> map) override;

};

