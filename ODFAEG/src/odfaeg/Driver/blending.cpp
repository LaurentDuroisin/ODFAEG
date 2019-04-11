#include "../../../include/odfaeg/Driver/blending.hpp"
namespace odfaeg {
    namespace driver {
        Blending::Blending(FrameBuffer& fb) : t(processBlending), fb(fb) {
            blendingDone.store(false);
        }
        bool Blending::done() {
            return blendingDone.load();
        }
        void Blending::load(core::any any) {
            addFragments(any.get<std::vector<Fragment>>());
        }
        void Blending::addFragments(std::vector<Fragment> fragments) {
            blendingDone.store(false);
            this->fragments = fragments;
        }
        any Blending::getResults() {
            blendingDone.store(false);
            return fragments;
        }
        void Blending::processBlending() {
            running.store(true);
            while(running.load()) {
                if (!blendingDone.load()) {
                    fragments.clear();
                    for (unsigned int i = 0; i < fragments.size(); i++) {
                        Color srcColor = fragments[i].getColor();
                        unsigned int index = fragments[i].getPosition().y * viewport.getSize().x + fragments[i].getPosition().y;
                        Color dstColor = frameBuffer[index];
                        if (srcColorBlending == GL_SRC_ALPHA) {
                            srcColor = Color(srcColor.r * scrColor.a, srcColor.g * srcColor.a, srcColor.b * scrColor.a, srcColor.a);
                        }
                        if (dstColorBlending == GL_ONE_MINUS_SRC_ALPHA) {
                            dstColor = Color(dstColor.r * (1 - srcColor.a), dstColor.g * (1 - srcColor.a), destColor.b * (1 - srcColor.a), dstColor.a);
                            dstColor.a = srcColor.a + dstColor.a * (1 - srcColor.a);
                        }
                        if (blendEq == GL_FUNC_ADD) {
                            frameBuffer[i] = srcColor + dstColor;
                        }
                    }
                }
            }
        }
        Blending::~Blending() {
            running.store(false);
        }
    }
}
