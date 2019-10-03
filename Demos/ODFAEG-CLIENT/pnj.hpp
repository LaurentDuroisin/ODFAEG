#ifndef SORROK_PNJ
#define SORROK_PNJ
namespace sorrok {
    class Pnj : public Caracter {
        public :
            Pnj();
        private :
            std::vector<Quest> quests;
    };
}
#endif
