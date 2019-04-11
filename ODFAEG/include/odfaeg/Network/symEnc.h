#ifndef SYM_ENC
#define SYM_ENC
#include <string>
#include <stdlib.h>
#include "../utilities.h"
class SymEnc {
    public :
        static std::string encrypt (std::string message);
        static std::string decrypt (std::string cyptedMess);
        static std::string getKey ();
        static void setKey(int k, int d);
    private :
        SymEnc();
        SymEnc(const SymEnc &se);
        static int k;
        static int d;
        static const unsigned int BLOC_SIZE = 100;
};
#endif
