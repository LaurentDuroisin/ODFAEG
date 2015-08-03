#include "symEnc.h"
int SymEnc::k = 0;
int SymEnc::d = 0;
using namespace std;
string SymEnc::encrypt(string message) {
    string cryptedMess = "";
    for (unsigned int i = 0; i < message.size(); i++) {
        int ascii = message.at(i);
        int nb = ascii * k + d;
        string nbString = conversionIntString(nb);
        for (unsigned int i = nbString.size(); i < BLOC_SIZE; i++)
            nbString.insert(0, "0");
        cryptedMess += nbString;
    }
    return cryptedMess;
}
string SymEnc::decrypt(string cryptedMess) {
    string message = "";
    unsigned int size = cryptedMess.size() / BLOC_SIZE;
    for (unsigned int i = 0; i < size; i++) {
        string nbCodeStr = cryptedMess.substr(i * BLOC_SIZE, BLOC_SIZE);
        while (nbCodeStr.size() > 1 && nbCodeStr.at(0) == '0') {
            nbCodeStr.erase(0, 1);
        }
        int nb = (conversionStringInt(nbCodeStr) - d) / k;
        char ascii = (char) nb;
        message += ascii;
    }

    return message;
}
string SymEnc::getKey () {
    ostringstream oss;
    oss<<conversionIntString(k)<<"-"<<conversionIntString(d);
    return oss.str();
}
void SymEnc::setKey (int k, int d) {
    SymEnc::k = k;
    SymEnc::d = d;
}
