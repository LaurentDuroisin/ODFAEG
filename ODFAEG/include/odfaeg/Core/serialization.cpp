#include "serialization.h"
namespace odfaeg {
template <typename B>
std::map<std::string, B*(*)(void)> BaseFact<B>::types = std::map<std::string, B*(*)(void)>();
}











