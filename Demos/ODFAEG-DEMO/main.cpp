#include "application.h"
#include "odfaeg/Core/clock.h"
using namespace odfaeg::core;
using namespace odfaeg::math;
using namespace odfaeg::physic;
using namespace odfaeg::graphic;
using namespace odfaeg::audio;
using namespace sorrok;
using namespace std;
class C {
    public :
    C () : a(7), b(4.5), s("hello!") {}
    template <typename Ar>
    void serialize (Ar& ar) {
        ar(a);
        ar(b);
        ar(s);
    }
    void afficher () {
        std::cout<<"a : "<<a<<std::endl<<"b : "<<b<<std::endl<<"s : "<<s<<std::endl;
    }
    private :
    int a;
    float b;
    std::string s;
};
class A : public Registered<A> {

    public :
    A () {
        a = 5;
        b = 3.5f;
    }
    template <typename Ar>
    void vtserialize(Ar& ar) {
        ar(a);
        ar(b);
    }
    int& getA() {
        return a;
    }
    float& getB() {
        return b;
    }
    virtual std::string& getS() = 0;
    virtual void afficher() {
        std::cout<<"a : "<<a<<std::endl<<"b : "<<b<<std::endl;
    }
    virtual ~A() {}
    private :
    int a;
    float b;
};
class B : public A {
    public :
    enum E {
        Z
    };
    B () {}
    B (C& c) : A() {
        s = "Hello world!";
        e = Z;
        v.push_back(&c);
    }
    template <typename Ar>
    void vtserialize(Ar& ar) {
        A::vtserialize(ar);
        ar(s);
        ar(e);
        ar(v);
    }
    virtual void afficher() {
        A::afficher();
        std::cout<<"s : "<<s<<std::endl<<"e : "<<e<<std::endl;
        v[0]->afficher();
    }
    std::string& getS() {
        return s;
    }
    virtual ~B() {}
    private :
    E e;
    std::string s;
    std::vector<C*> v;
};
int main(int argc, char* argv[])
{
    EXPORT_CLASS_GUID(BoundingVolumeBoundingBox, BoundingVolume, BoundingBox)
    EXPORT_CLASS_GUID(EntityTile, Entity, Tile)
    EXPORT_CLASS_GUID(EntityTile, Entity, BigTile)
    EXPORT_CLASS_GUID(EntityWall, Entity, g2d::Wall)
    EXPORT_CLASS_GUID(EntityDecor, Entity, g2d::Decor)
    EXPORT_CLASS_GUID(EntityAnimation, Entity, Anim)
    EXPORT_CLASS_GUID(EntityHero, Entity, Hero)
    MyAppli app(sf::VideoMode(800, 600, 32), "Test odfaeg");
    return app.exec();
}




