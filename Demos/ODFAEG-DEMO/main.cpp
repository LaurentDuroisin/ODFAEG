#include "application.h"
#include <SFML/OpenGL.hpp>
#include "odfaeg/Window/window.hpp"
#include "odfaeg/Window/x11Window.hpp"
#include "odfaeg/Window/context.hpp"
#include <SFML/Window/WindowStyle.hpp>
#include "odfaeg/Graphics/sprite.h"
using namespace odfaeg::core;
using namespace odfaeg::math;
using namespace odfaeg::physic;
using namespace odfaeg::graphic;
using namespace odfaeg::window;
using namespace odfaeg::audio;
using namespace sorrok;
class BigInt {
    public :
        BigInt() {
        }
        BigInt(std::string str) {
            fromStr(str);
        }
        static BigInt genRandom(unsigned int nbBits) {
            BigInt n;
            for (unsigned int i = 0; i < nbBits-1; i++) {
                bool bit = (int) Math::random(2);
                n.bits.push_back(bit);
            }
            n.bits.push_back(1);
            return n;
        }
        BigInt operator+= (BigInt& b) {
            (*this) = (*this) + b;
            return (*this);
        }
        BigInt operator+ (BigInt& b) {
            if (isNull())
                return b;
            if (b.isNull())
                return *this;
            BigInt s;
            if (bits.size() != b.bits.size()) {
                if (bits.size() > b.bits.size()) {
                    b.addZeros(bits.size() - b.bits.size());
                } else {
                    addZeros(b.bits.size() - bits.size());
                }
            }
            bool r = 0;
            for (unsigned int i = 0; i < bits.size(); i++) {
                bool bit = addBits(bits[i], b.bits[i], r);
                s.bits.push_back(bit);
            }
            s.bits.push_back(r);
            removeZeros();
            b.removeZeros();
            s.removeZeros();
            return s;
        }
        BigInt operator- (BigInt& b) {
            BigInt s, c;
            if (bits.size() > b.bits.size()) {
                b.addZeros(bits.size() - b.bits.size());
            }
            c = b.complAt1();
            c.removeZeros();
            c++;
            s = (*this) + c;
            s.bits.pop_back();
            s.removeZeros();
            b.removeZeros();
            return s;
        }
        BigInt operator++(int i) {
            BigInt one;
            one.bits.push_back(1);
            (*this) += one;
            return *this;
        }
        BigInt operator* (BigInt &b) {
            BigInt s;
            for (unsigned int i = 0; i < b.bits.size(); i++) {
                BigInt m;
                for (unsigned int j = 0; j < bits.size(); j++) {
                    bool bit = bits[j] & b.bits[i];
                    m.bits.push_back(bit);
                }
                m >>= i;
                s += m;
            }
            return s;
        }
        BigInt operator/ (BigInt &b) {
            BigInt r, q;
            for (int i = bits.size() - 1; i >= 0; i--) {
                r.bits.push_front(bits[i]);
                bool bit = !(r < b);
                q.bits.push_front(bit);
                r = (!bit) ? r : r - b;
            }
            q.removeZeros();
            return q;
        }
        BigInt operator% (BigInt &b) {
            BigInt r, q;
            for (int i = bits.size() - 1; i >= 0; i--) {
                r.bits.push_front(bits[i]);
                bool bit = !(r < b);
                q.bits.push_front(bit);
                r = (!bit) ? r : r - b;
            }
            q.removeZeros();
            return r;
        }
        BigInt operator>>= (unsigned int shiftLeft) {
            for (unsigned int i = 0; i < shiftLeft; i++) {
                bits.push_front(0);
            }
            return (*this);
        }
        bool isNull () {
            return bits.size() == 0;
        }
        bool operator< (BigInt& b) {
            if (bits.size() < b.bits.size())
                return true;
            if (bits.size() > b.bits.size())
                return false;
            for (int i = bits.size() - 1; i >= 0; i--) {
                if (bits[i] < b.bits[i])
                    return true;
                if (bits[i] > b.bits[i])
                    return false;
            }
            return false;
        }
        void fromStr(std::string str) {
            unsigned int n = 0;
            for (unsigned int i = 0; i < str.length(); i++) {
                unsigned int digit = conversionStringInt(str.substr(i, 1)) * Math::power(10, str.length() - i - 1);
                n += digit;
            }
            int p = 0;
            while (n > Math::power(2, p))
                p++;
            while (p != -1) {
                bool bit = n / (unsigned int) Math::power(2, p);
                bits.push_front(bit);
                n -= bit * (unsigned int) Math::power(2, p);
                p--;
            }
            removeZeros();
        }
        std::string getStr() {
            std::string s;
            unsigned int n=0;
            for (unsigned int i = 0; i < bits.size(); i++) {
                unsigned int digit = bits[i] * Math::power(2, i);
                n += digit;
            }
            return conversionIntString(n);
        }
    private :
        BigInt complAt1 () {
            BigInt c;
            for (unsigned int i = 0; i < bits.size(); i++) {
                c.bits.push_back(!bits[i]);
            }
            return c;
        }
        void addZeros(unsigned int nb) {
            for (unsigned int i = 0; i < nb; i++) {
                bits.push_back(0);
            }
        }
        void removeZeros() {
            while(!bits.empty() && !bits.back())
                bits.pop_back();
        }
        bool addBits(bool a, bool b, bool& r) {
            bool s = (a ^ b ^ r);
            r = (a & b) | (a & r) | (b & r);
            return s;
        }
        friend std::ostream& operator<< (std::ostream& out, const BigInt& bi);
        std::deque<bool> bits;
};
std::ostream& operator<< ( std::ostream& out, const BigInt& bi) {
    if ( bi.bits.size()== 0 ) return ( out << 0 );
    for(int i = bi.bits.size() - 1; i >= 0; i--) {
        out<<(bi.bits[i]);
    }
    return out;
}
int main(int argc, char* argv[])
{
    /*BigInt a("42");
    BigInt b("6");
    std::cout<<"a : "<<a.getStr()<<std::endl<<"b : "<<b.getStr()<<std::endl;
    BigInt s = a + b;
    std::cout<<"a + b : "<<s.getStr()<<std::endl;
    s = a - b;
    std::cout<<"a - b : "<<s.getStr()<<std::endl;
    s = a * b;
    std::cout<<"a * b : "<<s.getStr()<<std::endl;
    s  = a / b;
    std::cout<<"a / b : "<<s.getStr()<<std::endl;
    return 0;*/
    EXPORT_CLASS_GUID(BoundingVolumeBoundingBox, BoundingVolume, BoundingBox)
    EXPORT_CLASS_GUID(EntityTile, Entity, Tile)
    EXPORT_CLASS_GUID(EntityTile, Entity, BigTile)
    EXPORT_CLASS_GUID(EntityWall, Entity, g2d::Wall)
    EXPORT_CLASS_GUID(EntityDecor, Entity, g2d::Decor)
    EXPORT_CLASS_GUID(EntityAnimation, Entity, Anim)
    EXPORT_CLASS_GUID(EntityHero, Entity, Hero)
    EXPORT_CLASS_GUID(EntityMesh, Entity, Mesh)
    MyAppli app(sf::VideoMode(800, 600), "Test odfaeg");
    return app.exec();

}




