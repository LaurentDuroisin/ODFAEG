#include "bigInt.h"

using namespace std;

BigInt::BigInt(unsigned long long integer, bool positif, unsigned long long b) {
    this->positif = positif;
    this->base = b;
    nbChiffres = 0;
    if (integer > 0) {
        if (base < MAX_INT) {
            unsigned long long max = 1LL;
            while (integer >= max)
                max *= base;
            max /= base;
            while (max > 0) {
                unsigned long long c;
                c = integer / max;
                computeNbC(c);
                chiffres.push_back(c);
                integer %= max;
                max /= b;
            }
        } else {
            unsigned long long c = integer / base;
            computeNbC(c);
            chiffres.push_back(c);
            integer %= base;
            computeNbC(integer);
            chiffres.push_back(integer);
        }
    }
}
void BigInt::computeNbC (unsigned int c) {
    unsigned int nb = 10;
    unsigned int i = 1;
    while (c >= nb) {
        nb *= 10;
        i++;
    }

    nbChiffres += i;
}
unsigned int BigInt::getNbChiffres() {
    return nbChiffres;
}
void BigInt::push_back (Chiffre &c) {
    computeNbC(c);
    chiffres.push_back(c);
}
BigInt BigInt::generate (int nbBits) {

    BigInt n;
     do {
         n.clear();
         for (int i = 0; i < nbBits / 32; i++) {
             Chiffre c = rand() / (double) RAND_MAX * BASE;

             n.insert(c, 0);
         }

     } while (n.isNull());
     n.shorten();
     return n;
}

bool BigInt::isPositif () {
    return positif;
}
void BigInt::shorten () {

    vector<Chiffre>::iterator it;
    for (it = chiffres.begin(); it != chiffres.end(); ) {
        if (*it == 0) {
            it = chiffres.erase(it);
            nbChiffres--;
        } else
            break;
    }
}
BigInt& BigInt::operator= (const BigInt& bi ) {
    chiffres = bi.chiffres;
    base = bi.base;
    positif = bi.positif;
    return *this;
}
void BigInt::clear()
{
    chiffres.clear();
}
bool BigInt::isNull() const {
    return chiffres.empty();
}
Indice BigInt::size() const {
    return chiffres.size();
}
Chiffre BigInt::operator[] ( Indice i ) const {
    return ( i<0 || i>=chiffres.size() ? Chiffre(0) : chiffres[i] );
}
BigInt BigInt::operator++ (int i) {

    for( Indice i= chiffres.size() - 1; i>= 0; i-- ) {
        if (chiffres[i] == Chiffre(base -1) )
            chiffres[i]= Chiffre(0);
        else {
            ++(chiffres[i]);
            return *this;
        }

    }
    insert (Chiffre(1),  0);
    return *this;
}
void BigInt::insert(Chiffre c, int pos) {

    if (pos <= size()) {
        BigInt result(0, true, base);
        result.chiffres.resize(size() + 1, Chiffre(0));
        for (int i = 0, j = 0; i <= size(); i++, j++) {
            if (i != pos) {
                result.chiffres[i] = chiffres[j];
            } else {
                j--;
            }
        }

        result.chiffres[pos] = c;
        computeNbC(c);

        *this = result;
    }
}
BigInt BigInt::operator-- (int i) {
    bool ok = false;
    for( Indice i=chiffres.size() - 1; i>=0; i--)
        if ( chiffres[i] == Chiffre(0) )
            chiffres[i]= Chiffre(base - 1);
        else {
            --(chiffres[i]);
            shorten();
            ok = true;
        }
    if (!ok)
        cerr<<"Error : negative number"<<endl;
    return *this;
}
int BigInt::comparaison (const BigInt &a, const BigInt &b) const {
    if ( a.chiffres.size() > b.chiffres.size() )
        return +1;
    if ( a.chiffres.size() < b.chiffres.size() )
        return -1;

    for( Indice i= 0; i < a.size(); i++) {

        if ( a.chiffres[i] > b.chiffres[i] )
            return +1;
        if ( a.chiffres[i] < b.chiffres[i] )
            return -1;
    }
    return 0;
}
bool BigInt::operator== ( const BigInt& a ) const {
    return ( comparaison(*this, a) == 0 );
}
bool BigInt::operator!= ( const BigInt& a) const {
    return ( comparaison(*this, a) != 0 );
}
bool BigInt::operator< ( const BigInt& a ) const {
    return ( comparaison(*this,a) < 0 );
}
bool BigInt::operator<= ( const BigInt& a ) const {
    return ( comparaison(*this, a) <= 0 );
}
bool BigInt::operator> ( const BigInt& a) const {
    return ( comparaison(*this,a) > 0 );
}
bool BigInt::operator>= ( const BigInt& a) const {
    return ( comparaison(*this, a) >= 0 );
}
BigInt BigInt::opposite () const {
    BigInt result = *this;
    result.positif = !positif;
    return result;
}
BigInt BigInt::add (const BigInt& bi) const {

    BigInt somme(0, true, base);
    somme.clear();
    Indice taille = max(this->size(), bi.size());
    somme.chiffres.resize( taille, Chiffre(0) );

    Chiffre retenue= 0;
    Indice i, j;
    for(i=size() - 1, j = bi.size() - 1; i >= 0; i--, j--)  {

        unsigned long long temp = (unsigned long long) (*this)[i] + (unsigned long long) bi[j] + (unsigned long long) retenue;

        if ( temp < base) {
            somme.chiffres[i] = (unsigned int) temp;
            retenue = 0;
        }
        else {
            somme.chiffres[i]= (unsigned int) temp - base;
            retenue = 1;

        }
    }

    while (retenue != 0) {
        if (i >= 0) {
            unsigned long long temp = (unsigned long long) (*this)[i] + (unsigned long long) bi[i] + (unsigned long long) retenue;

            if ( temp < base) {
                somme.chiffres[i] = (unsigned int) temp;
                retenue = 0;
            } else {
                somme.chiffres[i]= (unsigned int) temp - base;

            }
            i--;
        } else {
            somme.insert(retenue, 0);
            retenue = 0;
        }
    }
    somme.shorten();
    return somme;
}

BigInt BigInt::operator+ (const BigInt &bi) const {
    BigInt a = *this;
    BigInt b = bi;
    BigInt result(0, true, base);
    if (positif && bi.positif) {
        if (bi.size() > size()) {
            a = bi;
            b = *this;
        }
        result = a.add(b);
    } else if (!positif && bi.positif) {
        if (*this >= bi) {
            result = a.sub(b);
            result.positif = false;
        } else {
            result = b.sub(a);
        }
    } else if (positif && !bi.positif) {
        if (*this >= bi) {
            result = a.sub(b);
        } else {
            result = b.sub(a);
            result.positif = false;
        }
    } else {
       if (bi.size() > size()) {
            a = bi;
            b = *this;
       }
       result = a.add(b);
       result.positif = false;
    }
    return result;
}
BigInt BigInt::operator+= (const BigInt &bi) {
    *this = *this + bi;
    return *this;
}
BigInt BigInt::sub (const BigInt &bi) const {
    BigInt diff(0, true, this->base);
    diff.clear();

    if (bi == 0)
        return *this;

    Indice taille= max( this->size(), bi.size() );
    diff.chiffres.resize( taille, Chiffre(0) );
    // Calculer la somme chiffre par chiffre en tenant compte des retenues
    Chiffre retenue= 0;
    Indice i, j;
    for(i=size() - 1, j = bi.size() - 1; i>=0; i--, j--)  {
        long long temp = (long long) ((unsigned long long) (*this)[i] - (unsigned long long) bi[j] - (unsigned long long) retenue);

        if ( temp >= 0 ) {
            diff.chiffres[i]= (unsigned int) temp;
            retenue = 0;
        }
        else {
            diff.chiffres[i]= (unsigned int) (temp + base);
            retenue = 1;
        }
    }
    while (retenue > 0) {
        long long temp = (long long) ((unsigned long long) (*this)[i] - (unsigned long long) bi[i] - (unsigned long long) retenue);

        if ( temp >= 0 ) {
            diff.chiffres[i]= (unsigned int) temp;
            retenue = 0;
        }
        else {
            diff.chiffres[i]= (unsigned int) (temp + base);
        }
        i--;
    }
    diff.shorten();
    return diff;
}
BigInt BigInt::operator- (const BigInt &bi) const {
    BigInt result(0, true, base);
    BigInt a = *this;
    BigInt b = bi;
    if (positif && bi.positif) {
        if (*this >= bi) {
            result = a.sub(b);
        } else {
            result = b.sub(a);
            result.positif = false;
        }
    } else if (!positif && bi.positif) {
        if (bi.size() > size()) {
            a = bi;
            b = *this;
        }
        result = a.add(b);
        result.positif = false;
    } else if (positif && !bi.positif) {
        if (bi.size() > size()) {
            a = bi;
            b = *this;
        }
        result = a.add(b);
    } else {
        if (*this >= bi) {
            result = a.sub(b);
            result.positif = false;
        } else {
            result.positif = true;
            result = b.sub(a);
        }
    }
    return result;
}
BigInt BigInt::operator-= (const BigInt &bi) {
    *this = *this - bi;
    return *this;
}
BigInt BigInt::addZeros (int n) {

    if (n < 0)
        return *this;
    if (isNull())
        return *this;
    BigInt result(0, positif, base);

    result.chiffres.resize(size() + n, Chiffre(0));
    nbChiffres += n;
    for (int i = 0; i < size(); i++) {
         result.chiffres[i] = chiffres[i];
    }
    return result;
}
BigInt BigInt::multiply (const BigInt &bi) const {

    BigInt produit(0, true, this->base);
    produit.clear();

    if ( this->size() == 0 || bi.size() == 0 ) {
        return produit;
    }

    Indice i = 0, j = bi.size() - 1;
    produit = scaleUp(bi.chiffres[i]).addZeros(j);

    while ( j > 0)
    {
       i++; j--;
       produit += scaleUp (bi.chiffres[i]).addZeros(j);

    }

    produit.shorten();
    return produit;
}
BigInt BigInt::karatsuba (const BigInt &bi) const {
    if (size() >= bi.size()) {
        Indice n = size() / 2;
        BigInt a = arraySub(0, size() - n);
        BigInt b = arraySub(size() - n, n);
        if (bi.size() > n) {
            BigInt c = bi.arraySub(0, bi.size() - n);
            BigInt d = bi.arraySub(bi.size() - n, n);
            BigInt ac = a * c;
            BigInt bd = b * d;
            BigInt ad_bc = (a + b) * (c + d) - ac - bd;
            ac = ac.addZeros(2*n);
            ad_bc = ad_bc.addZeros(n);
            return ac + ad_bc + bd;
        } else {
            BigInt aq = a * bi;
            BigInt bq = b * bi;
            aq = aq.addZeros(n);
            return aq + bq;
        }
    }
    return *this;
}
BigInt BigInt::operator* (const BigInt &bi) const {
    // D´el´eguer les petites multiplications `a la m´ethode scolaire

    BigInt result(0, true, base);
    if (size() < bi.size())
        result = bi * *this;
    else if (bi.size() < Karatsuba_Treshold)
        result = multiply(bi);
    else
        result = karatsuba(bi);
    if (!positif && bi.positif || positif && !bi.positif)
        result.positif = false;
    else
        result.positif = true;
    return result;
}
BigInt BigInt::operator*= (const BigInt &bi) {
    *this = *this * bi;
    return *this;
}


BigInt BigInt::operator/ (const BigInt &bi) const {
    if (bi == 0) {
        cerr<<"Error : b is null!";
        return 0;
    }
    if (bi > *this)
        return 0;
    BigInt result(0, true, base);
    BigInt reste(0, true, base);
    result = burnikel_ziegler(bi, reste);
    if (!positif && bi.positif || positif && !bi.positif)
        result.positif = false;
    else
        result.positif = true;
    return result;

}
BigInt BigInt::operator/= (const BigInt &bi) {
    *this = *this / bi;
    return *this;
}

BigInt BigInt::operator% (const BigInt& bi) const {

    if (bi == BigInt(0)) {
        cerr<<"Error : b is null!";
        return 0;
    }
    if (bi > *this)
        return *this;
    BigInt reste(0, true, base);
    burnikel_ziegler(bi, reste);
    reste.shorten();
    if (!positif && bi.positif || positif && !bi.positif)
        reste.positif = false;
    else
        reste.positif = true;
    return reste;
}
BigInt BigInt::pow (BigInt exp) {
    BigInt un(1, true, base);
    BigInt deux (2, true, base);
    if (exp.isNull()) {
        return un;
    } else if (exp % deux == 0) {
        BigInt a = pow(exp / deux);
        return (a * a);
    } else {
        return (*this) * pow (exp - un);
    }
}
BigInt BigInt::prodMod (const BigInt &b, const BigInt &n) const {
  if(isNull()) {
    return 0;
  } else {
    // a = 2 * q  + e
    BigInt q = *this / BigInt(2);
    BigInt e = *this % BigInt(2);
    BigInt r = BigInt(2) * q.prodMod(b, n) % n;
    return e == 0 ? r : (r + b) % n;
  }
}
BigInt BigInt::modOfPow (const BigInt exp, const BigInt mod) const {
    BigInt result = 1;

    BigInt e = exp, bs;
    bs = *this;
    while (e > 0) {

        if (e % 2 == 1)
            result = result * bs % mod;
        e /= 2;
        bs = bs * bs % mod;
    }
    return result;
}
BigInt BigInt::operator%= (const BigInt &bi) {
    *this = *this % bi;
    return *this;
}
BigInt BigInt::arraySub (int n, int length) const {
    BigInt result(0, true, this->base);

    if (n >= 0 && n + length <= size() && length  > 0) {
        result.chiffres.resize(length, Chiffre(0));
        for (int i = n, j = 0; j < length; i++, j++) {
            result.chiffres[j] = chiffres[i];
        }
    }

    result.shorten();
    return result;
}
BigInt BigInt::operator<< (int n) const {
    if (n < 0)
        return *this;
    if (isNull())
        return *this;
    BigInt result(0, true, base);

    result.chiffres.resize(size() + n, Chiffre(0));
    for (int i = 0; i < size(); i++) {
         result.chiffres.push_back(chiffres[i]);
    }
    return result;
}
BigInt BigInt::operator>> (int n) const {
    if (n < 0)
        return *this;
    if (isNull())
        return *this;
    if (n > size())
        n = size();
    BigInt result (0, true, base);
    result.chiffres.resize(size() - n, Chiffre(0));
    for (int i = 0; i < result.size(); i++)
        result.chiffres[i] = chiffres[i];
    return result;
}
BigInt BigInt::operator& (int n) const {
    BigInt a, b, bi(n);
    if (size() > bi.size()) {
        b = bi<<(size() - bi.size() - 1);
        a = *this;
    } else if (size() < bi.size()) {
        a = *this<<(bi.size() - size() - 1);
        b = bi;
    } else {
        a = *this;
        b = bi;
    }
    BigInt result (0, true, base);
    result.chiffres.resize(a.size(), Chiffre(0));
    for (int i = 0; i < a.size(); i++) {
        result.chiffres[i] = chiffres[i] & b.chiffres[i];
    }
    result.shorten();
    return result;
}
BigInt BigInt::scaleUp (unsigned int n) const {

    unsigned long long accu = 0;
    unsigned int retenue = 0;
    BigInt result(0, true, base);
    result.chiffres.resize(size() + 1, Chiffre(0));
    BigInt bs = BigInt(base, true, base);
    BigInt ni(n);

    if (ni >= 0 && ni < bs) {
        if (n == 0)
            return 0;
        for (int i = size(); i > 0; i--) {
            accu = (unsigned long long) chiffres[i-1] * n + (unsigned long long) retenue;
            result.chiffres[i] = (unsigned int) (accu % base);
            retenue = (unsigned int) (accu / base);
        }
        result.chiffres[0] = retenue;
        if (retenue == 0)
            result.shorten();
    }
    return result;
}
BigInt BigInt::scaleDown (BigInt &r, unsigned long long n) const {

    unsigned long long accu = 0;
    unsigned int retenue = 0;
    BigInt result = *this;
    BigInt bs = BigInt(base, true, base);
    BigInt ni(n, true, base);

    if (ni >= 0 && ni < bs * bs) {
        for (unsigned int i = 0; i < size(); i++) {
            accu = (unsigned long long) chiffres[i] + (unsigned long long) retenue * base;
            result.chiffres[i] = (unsigned int) (accu / n);
            retenue = (unsigned int) (accu % n);
        }
        r = BigInt(retenue, true, base);

    }
    result.shorten();
    return result;
}
BigInt BigInt::burnikel_ziegler(const BigInt &bi, BigInt &r) const {
    BigInt q(0, true, base);
    BigInt bs (base, true, base);

    if (bi.size() <= 2) {
        unsigned long long b2 = (bi.size() < 2) ? bi.chiffres[0] : bi.chiffres[0] * base + bi.chiffres[1];
        q = scaleDown(r, b2);
        return q;
    }

    Indice n = (bi.size() - 1) / 2;

    // D´ecouper a et b en deux moiti´es
    BigInt a0, a1;
    a0 = arraySub(size() - n, n);
    a1 = arraySub(0, size() - n);

    if (a1 >= bi) {

        BigInt q1, r1, q0, r0;
        q1 = a1.burnikel_ziegler (bi, r1);
        r1 = r1.addZeros(n);

        q0 = (r1 + a0).burnikel_ziegler (bi, r0);
        r = r0;
        q1 = q1.addZeros(n);
        return q1 + q0;
    } else {

        BigInt b0, b1, q1, r1, a0_r1, b0_q1;
        b0 = bi.arraySub(bi.size() - n, n);
        b1 = bi.arraySub(0, bi.size() - n);
        q1 = a1.burnikel_ziegler(b1, r1);
        r1 = r1.addZeros(n);
        a0_r1 = r1 + a0;
        b0_q1 = b0 * q1;
        if (a0_r1 >= b0_q1) {

            r = a0_r1 - b0_q1;
            return q1;
        } else {

            BigInt minus_x = b0_q1 - a0_r1;
            r = bi - minus_x;
            return q1 - BigInt(1, true, base);
        }
    }

}
BigInt BigInt::convert (unsigned long long b) const {

    BigInt newBi = *this;
    if (b != base) {

        newBi = BigInt (0, true, 10);
        BigInt bs = BigInt(base, true, 10);

        for (int i = 0; i < size(); i++) {
            BigInt exp (size() - i - 1, true, 10);
            BigInt c1 (chiffres[i], true, 10);
            BigInt c2 = c1 * bs.pow(exp);
            newBi += c2;
        }
        BigInt max(1, true, 10);
        bs = BigInt (b, true, 10);
        BigInt r(0, true, 10);

        while (newBi >= max) {
            max = max * bs;
        }

        max /= bs;

        newBi.shorten();
        BigInt a = newBi;
        newBi.clear();
        newBi.nbChiffres = 0;
        while (max > 0) {
            BigInt c;
            c = a / max;
            if (c == 0) {
                newBi.chiffres.push_back(Chiffre(0));
                newBi.computeNbC(0);
            } else {
                Chiffre somme = 0;
                for (int i = 0; i < c.size(); i++) {

                    somme += c.chiffres[i] * Math::power(10, c.size() - i - 1);

                }
                newBi.chiffres.push_back(somme);
                a %= max;
                newBi.computeNbC(somme);
            }
            max /= bs;
        }
        newBi.shorten();
        newBi.base = b;
        return newBi;

    }
    return newBi;
}

// Conversion valeur -> symbole pour la sortie
char symbole( Chiffre valeur, unsigned long long base) {

    return (( valeur>=0 && valeur<base ) ? chiffre[valeur] : '?');
}
// Op´erateur de sortie (afficher les chiffres, ou bien "0" pour une suite vide)
ostream& operator<< ( ostream& out, const BigInt& bi) {

    if ( bi.size()== 0 ) return ( out << 0 );
    if (!bi.positif)
            out<<"-";
    for( int i = 0; i < bi.size(); i++) {

        out<<bi.chiffres[i];
    }
    return out;
}

