//
// Created by Cangwang on 2024/5/9.
//

#pragma once

#include <layer/AbstractDrawer.h>
#include <layer/Layers.h>
#include <layer/ImageDrawer.h>
#include <layer/VideoDrawer.h>
#include <layer/VectorDrawer.h>
#if USE_TGFX
#include "src/main/cpp/tgfx/TextDrawer.h"
#endif
#include <layer/ShapeDrawer.h>
#include <layer/TextCpuDrawer.h>
#include <bean/comp.h>
#include <utils/layertype.h>

using namespace std;
namespace ymat {
    class VectorDrawer : public AbstractDrawer {
    public:
        ~VectorDrawer();

        void init() override;

        void draw(shared_ptr<Matrix> parentMatrix, shared_ptr<FrameInfo> frameInfo) override;

        void setLayers(vector<shared_ptr<Comp>> comps, vector<shared_ptr<SimpleLayerInfo>> infos);

        void destroy() override;

        shared_ptr<Layers> setLayer(vector<shared_ptr<Comp>> comps, int id, string type, bool isTrackMatte, bool findTrack);
        shared_ptr<SimpleLayerInfo> findMaskInfo(vector<shared_ptr<SimpleLayerInfo>> infos, int trackMatteLayer);
        void initImageProgram();
    private:
        vector<shared_ptr<Layers>> subLayers;
        shared_ptr<FrameInfo> viewFrameInfo = make_shared<FrameInfo>();
        shared_ptr<Matrix> viewMatrix;
    };
}