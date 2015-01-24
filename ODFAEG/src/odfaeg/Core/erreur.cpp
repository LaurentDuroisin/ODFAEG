#include "../../../include/odfaeg/Core/erreur.h"
namespace odfaeg {
    namespace core {
        Erreur::Erreur(int numero=0, const std::string &phrase="", int niveau=0) throw()
                 :m_numero(numero),m_phrase(phrase),m_niveau(niveau)
        {}

        const char* Erreur::what() const throw()
        {
            return m_phrase.c_str();
        }

        int Erreur::getNiveau() const throw()
        {
             return m_niveau;
        }

        Erreur::~Erreur() throw()
        {}
    }
}


