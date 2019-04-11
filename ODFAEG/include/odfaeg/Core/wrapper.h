#ifndef ODFAEG_WRAPPER
#define ODFAEG_WRAPPER

namespace odfaeg {
    class OTextArchive;
template <typename D>
class Wrapper : public D {
public :
    Wrapper(D* derived) {
    }
    virtual D& get() {
        return *derived;
    }
private :
    D* derived;
};
template <>
class Wrapper<OTextArchive>;

}
#endif // ODFAEG_WRAPPER
