#ifndef ODFAEG_PIPELINESTAGE_HPP
#define ODFAEG_PIPELINESTAGE_HPP
namespace odfaeg {
    namespace driver {
        class PipelineStage {
            public :
            virtual void load (core::any any) = 0;
            virtual bool done() = 0;
            virtual core::any getResults() = 0;
        };
    }
}
#endif
