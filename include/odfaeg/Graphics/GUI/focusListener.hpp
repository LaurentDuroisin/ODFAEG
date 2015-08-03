#ifndef ODFAEG_FOCUS_LISTENER
#define ODFAEG_FOCUS_LISTENER
namespace odfaeg {
    namespace graphic {
        namespace gui {
            class TextArea;
            class FocusListener {
                public :
                virtual void gaignedFocus(TextArea* textArea) = 0;
            };
        }
    }
}
#endif // ODFAEG_FOCUS_LISTENER
