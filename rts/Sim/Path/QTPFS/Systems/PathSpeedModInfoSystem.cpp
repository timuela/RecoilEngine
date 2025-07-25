/* This file is part of the Spring engine (GPL v2 or later), see LICENSE.html */

#include "PathSpeedModInfoSystem.h"

#include <cassert>
#include <limits>

#include "Map/ReadMap.h"

#include "Sim/Misc/GlobalSynced.h"
#include "Sim/MoveTypes/MoveDefHandler.h"
#include "Sim/MoveTypes/MoveMath/MoveMath.h"
#include "Sim/Path/IPathManager.h"
#include "Sim/Path/QTPFS/Components/PathSpeedModInfo.h"
#include "Sim/Path/QTPFS/NodeLayer.h"
#include "Sim/Path/QTPFS/PathManager.h"
#include "Sim/Path/QTPFS/Registry.h"

#include "System/Ecs/EcsMain.h"
#include "System/Ecs/Utils/SystemGlobalUtils.h"
#include "System/Log/ILog.h"
#include "System/TimeProfiler.h"
#include "System/Threading/ThreadPool.h"
#include "System/SpringMath.h"

#include "System/Misc/TracyDefs.h"


using namespace SystemGlobals;
using namespace QTPFS;

// Tracking a crash in live, so need optimizations turned down here for awhile.
// #ifdef __GNUC__
// #pragma GCC push_options
// #pragma GCC optimize ("O0")
// #endif

void ScanForPathSpeedModInfo(int frameModulus) {
    RECOIL_DETAILED_TRACY_ZONE;
    auto& comp = systemGlobals.GetSystemComponent<PathSpeedModInfoSystemComponent>();
    auto layersView = registry.view<NodeLayerSpeedInfoSweep>();
    auto pm = dynamic_cast<QTPFS::PathManager*>(pathManager);
    int dataChunk = frameModulus;

    assert(layersView.size() <= MoveDefHandler::MAX_MOVE_DEFS);

    // Initialization
    if (frameModulus <= 0) {
        layersView.each([&layersView, pm](QTPFS::entity entity){
                auto& layer = layersView.get<NodeLayerSpeedInfoSweep>(entity);
                auto& nodeLayer = pm->GetNodeLayer(layer.layerNum);
                layer.updateCurMaxSpeed = (-std::numeric_limits<float>::infinity());
                layer.updateMaxNodes = nodeLayer.GetMaxNodesAlloced();
                layer.updateInProgress = true;
                layer.updateCurSumSpeed = 0.f;
                layer.updateNumLeafNodes = 0.f;
            });
        dataChunk = 0;
    }
    
    // One thread per layer: get maximum speed mod from the nodes walked thus far.
    for_mt(0, layersView.size(), [&layersView, &comp, dataChunk, pm](int idx){
        QTPFS::entity entity = layersView.storage<NodeLayerSpeedInfoSweep>()[idx];
        auto& layer = layersView.get<NodeLayerSpeedInfoSweep>(entity);

        const int idxBeg = ((dataChunk + 0) * layer.updateMaxNodes) / comp.refreshTimeInFrames;
	    const int idxEnd = ((dataChunk + 1) * layer.updateMaxNodes) / comp.refreshTimeInFrames;

        // if (layer.layerNum == 2) {
        //     LOG("Searching %d: %d/%d", dataChunk, layer.updateMaxNodes, comp.refreshTimeInFrames);
        // }

        // TODO: store speed mods in a separate component? Allows for SSE perhaps?
        auto& nodeLayer = pm->GetNodeLayer(layer.layerNum);
        for (int i = idxBeg; i < idxEnd; ++i) {
            auto* curNode = nodeLayer.GetPoolNode(i);
            if (curNode->IsLeaf()) {
                float speedMod = curNode->GetSpeedMod();
                layer.updateCurMaxSpeed = std::max(layer.updateCurMaxSpeed, speedMod);
                layer.updateCurSumSpeed += speedMod;
                layer.updateNumLeafNodes++;
            }

            // if (layer.layerNum == 2) {
            //     LOG("node: %d max(%f,%f)", i, layer.updateCurMaxSpeed, curNode->GetSpeedMod());
            // }
        }
    });

    // Finished search
    if (dataChunk == comp.refreshTimeInFrames + (-1))
        layersView.each([&comp, &layersView](QTPFS::entity entity){
            auto& layer = layersView.get<NodeLayerSpeedInfoSweep>(entity);
            comp.relSpeedModinfos[layer.layerNum].max = layer.updateCurMaxSpeed;
            comp.relSpeedModinfos[layer.layerNum].mean = layer.updateCurSumSpeed / layer.updateNumLeafNodes;
            layer.updateInProgress = false;

            // if (layer.layerNum == 2) {
            //     LOG("Finished Search - result is %f", comp.relSpeedModinfos[layer.layerNum]);
            // }
            });
}

// #ifdef __GNUC__
// #pragma GCC pop_options
// #endif

void InitLayers() {
    RECOIL_DETAILED_TRACY_ZONE;
    std::vector<QTPFS::entity> layers((size_t)moveDefHandler.GetNumMoveDefs());
    QTPFS::registry.create<decltype(layers)::iterator>(layers.begin(), layers.end());

    int counter = 0;
    std::for_each(layers.begin(), layers.end(), [&counter](QTPFS::entity entity){
        auto& layer = QTPFS::registry.emplace<NodeLayerSpeedInfoSweep>(entity);
        layer.layerNum = counter++;
        layer.updateCurMaxSpeed = 0.f;
        // LOG("%s: added %x:%x entity for layer %d '%s'", __func__, entt::to_integral(entity), entt::to_version(entity)
        //     , layer.layerNum, moveDefHandler.GetMoveDefByPathType(layer.layerNum)->name.c_str());
    });
}

void ScanWholeMap()
{
    auto& comp = systemGlobals.GetSystemComponent<PathSpeedModInfoSystemComponent>();

    // Scan whole map for initial max speed for hCost
    comp.refreshTimeInFrames = 1;
    ScanForPathSpeedModInfo(-1);
    comp.refreshTimeInFrames = GAME_SPEED * 30;
}

void PathSpeedModInfoSystem::Init()
{
    RECOIL_DETAILED_TRACY_ZONE;

    if (systemGlobals.IsSystemActive<PathSpeedModInfoSystemComponent>()) {
        ScanWholeMap();
        return;
    }

    auto& comp = systemGlobals.CreateSystemComponent<PathSpeedModInfoSystemComponent>();
    auto pm = dynamic_cast<QTPFS::PathManager*>(IPathManager::GetInstance(QTPFS_TYPE));

    InitLayers();

    systemUtils.OnUpdate().connect<&PathSpeedModInfoSystem::Update>();

    ScanWholeMap();

    comp.startRefreshOnFrame = NEXT_FRAME_NEVER;
    comp.refeshDelayInFrames = GAME_SPEED;

    comp.state = PathSpeedModInfoSystemComponent::STATE_READY;
}

void PathSpeedModInfoSystem::Update()
{
    SCOPED_TIMER("ECS::PathSpeedModInfoSystem::Update");

    auto& comp = systemGlobals.GetSystemComponent<PathSpeedModInfoSystemComponent>();
    auto nextFrameNum = gs->frameNum + 1;

    switch (comp.state) {
    case (PathSpeedModInfoSystemComponent::STATE_UPDATING):
        ScanForPathSpeedModInfo(gs->frameNum % comp.refreshTimeInFrames);
        if (nextFrameNum % comp.refreshTimeInFrames == 0)
            comp.state = PathSpeedModInfoSystemComponent::STATE_READY;
        break;
    case (PathSpeedModInfoSystemComponent::STATE_READY):
        if ( (nextFrameNum >= comp.startRefreshOnFrame) && (nextFrameNum % comp.refreshTimeInFrames == 0) ) {
            comp.state = PathSpeedModInfoSystemComponent::STATE_UPDATING;
            comp.startRefreshOnFrame = NEXT_FRAME_NEVER;
        }
        break;
    default:
        assert(false);
    }
}

void PathSpeedModInfoSystem::Shutdown() {
    RECOIL_DETAILED_TRACY_ZONE;
    systemUtils.OnUpdate().disconnect<&PathSpeedModInfoSystem::Update>();

    registry.view<NodeLayerSpeedInfoSweep>()
            .each([](QTPFS::entity entity){ registry.destroy(entity); });
}
