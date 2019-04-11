#ifndef ODFAEG_BLENDING_HPP
#define ODFAEG_BLENDING_HPP
namespace odfaeg {
    namespace driver {
        class Blending : public PipelineStage {
            public :
                void load(core::any any);
                bool done();
                void addFragments(std::vector<Fragment> fragments);
                core::any getResults();
                void processBlending();
                ~Blending();
            private :
                std::thread t;
                std::atomic<bool> blendingDone, running;
                std::vector<Fragment> fragments;
                FrameBuffer& frameBuffer;
        };
    }
}
#endif
