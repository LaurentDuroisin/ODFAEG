#include "odfaegCreatorStateExecutor.hpp"
#include "application.hpp"
using namespace odfaeg::core;
using namespace odfaeg::math;
using namespace odfaeg::graphic;
ODFAEGCreatorStateExecutor::ODFAEGCreatorStateExecutor(ODFAEGCreator* app) : app(app) {
}
bool ODFAEGCreatorStateExecutor::doState(State& state) {
    if (state.getName() == "ADDREMOVESHAPE") {
        std::string str = state.getParameter("ADDREMOVESHAPE").getValue<std::string>();
        std::istringstream iss(str);
        ITextArchive ia(iss);
        unsigned int id;
        ia(id);
        Shape* shape;
        ia(shape);
        app->addShape(shape);
    }
    if (state.getName() == "SCHANGEXPOS") {
        float newXPos = state.getParameter("NEWVALUE").getValue<float>();
        Shape* shape = state.getParameter("OBJECT").getValue<Shape*>();
        shape->setPosition(Vec3f(newXPos,shape->getPosition().y, shape->getPosition().z));
    }
    if (state.getName() == "SCHANGEYPOS") {
        float newYPos = state.getParameter("NEWVALUE").getValue<float>();
        Shape* shape = state.getParameter("OBJECT").getValue<Shape*>();
        shape->setPosition(Vec3f(shape->getPosition().x, newYPos, shape->getPosition().z));
    }
    if (state.getName() == "SCHANGEZPOS") {
        float newZPos = state.getParameter("NEWVALUE").getValue<float>();
        Shape* shape = state.getParameter("OBJECT").getValue<Shape*>();
        shape->setPosition(Vec3f(shape->getPosition().x, shape->getPosition().y, newZPos));
    }
    if (state.getName() == "SCHANGERCOLOR") {
        unsigned int color = state.getParameter("NEWVALUE").getValue<unsigned int>();
        Shape* shape = state.getParameter("OBJECT").getValue<Shape*>();
        shape->setFillColor(sf::Color(color, shape->getFillColor().g, shape->getFillColor().b, shape->getFillColor().a));
    }
    if (state.getName() == "SCHANGEGCOLOR") {
        unsigned int color = state.getParameter("NEWVALUE").getValue<unsigned int>();
        Shape* shape = state.getParameter("OBJECT").getValue<Shape*>();
        shape->setFillColor(sf::Color(shape->getFillColor().r, color, shape->getFillColor().b, shape->getFillColor().a));
    }
    if (state.getName() == "SCHANGEBCOLOR") {
        unsigned int color = state.getParameter("NEWVALUE").getValue<unsigned int>();
        Shape* shape = state.getParameter("OBJECT").getValue<Shape*>();
        shape->setFillColor(sf::Color(shape->getFillColor().r, shape->getFillColor().g, color, shape->getFillColor().a));
    }
    if (state.getName() == "SCHANGEACOLOR") {
        unsigned int color = state.getParameter("NEWVALUE").getValue<unsigned int>();
        Shape* shape = state.getParameter("OBJECT").getValue<Shape*>();
        shape->setFillColor(sf::Color(shape->getFillColor().r, shape->getFillColor().g, shape->getFillColor().b, color));
    }
    if (state.getName() == "SCHANGETEXTURE") {
        Texture* texture = state.getParameter("NEWVALUE").getValue<Texture*>();
        Shape* shape = state.getParameter("OBJECT").getValue<Shape*>();
        shape->setTexture(texture);
    }
    if (state.getName() == "SCHANGEXTEXCOORD") {
        int texCoordX = state.getParameter("NEWVALUE").getValue<int>();
        Shape* shape = state.getParameter("OBJECT").getValue<Shape*>();
        shape->setTextureRect(sf::IntRect(texCoordX,shape->getTextureRect().top,shape->getTextureRect().width,shape->getTextureRect().height));
    }
    if (state.getName() == "SCHANGEYTEXCOORD") {
        int texCoordY = state.getParameter("NEWVALUE").getValue<int>();
        Shape* shape = state.getParameter("OBJECT").getValue<Shape*>();
        shape->setTextureRect(sf::IntRect(shape->getTextureRect().left,texCoordY,shape->getTextureRect().width,shape->getTextureRect().height));
    }
    if (state.getName() == "SCHANGEWTEXCOORD") {
        int texCoordW = state.getParameter("NEWALUE").getValue<int>();
        Shape* shape = state.getParameter("OBJECT").getValue<Shape*>();
        shape->setTextureRect(sf::IntRect(shape->getTextureRect().left,shape->getTextureRect().top,texCoordW,shape->getTextureRect().height));
    }
    if (state.getName() == "SCHANGEXTEHCOORD") {
        int texCoordH = state.getParameter("NEWVALUE").getValue<int>();
        Shape* shape = state.getParameter("OBJECT").getValue<Shape*>();
        shape->setTextureRect(sf::IntRect(shape->getTextureRect().left,shape->getTextureRect().top,shape->getTextureRect().width,texCoordH));
    }
}
bool ODFAEGCreatorStateExecutor::undoState(State& state) {
    if (state.getName() == "ADDREMOVESHAPE") {
        std::string str = state.getParameter("ADDREMOVESHAPE").getValue<std::string>();
        std::istringstream iss(str);
        ITextArchive ia(iss);
        unsigned int id;
        ia(id);
        app->removeShape(id);
    }
    if (state.getName() == "SCHANGEXPOS") {
        float newXPos = state.getParameter("OLDVALUE").getValue<float>();
        Shape* shape = state.getParameter("OBJECT").getValue<Shape*>();
        shape->setPosition(Vec3f(newXPos,shape->getPosition().y, shape->getPosition().z));
    }
    if (state.getName() == "SCHANGEYPOS") {
        float newYPos = state.getParameter("OLDVALUE").getValue<float>();
        Shape* shape = state.getParameter("OBJECT").getValue<Shape*>();
        shape->setPosition(Vec3f(shape->getPosition().x, newYPos, shape->getPosition().z));
    }
    if (state.getName() == "SCHANGEZPOS") {
        float newZPos = state.getParameter("OLDVALUE").getValue<float>();
        Shape* shape = state.getParameter("OBJECT").getValue<Shape*>();
        shape->setPosition(Vec3f(shape->getPosition().x, shape->getPosition().y, newZPos));
    }
    if (state.getName() == "SCHANGERCOLOR") {
        unsigned int color = state.getParameter("OLDVALUE").getValue<unsigned int>();
        Shape* shape = state.getParameter("OBJECT").getValue<Shape*>();
        shape->setFillColor(sf::Color(color, shape->getFillColor().g, shape->getFillColor().b, shape->getFillColor().a));
    }
    if (state.getName() == "SCHANGEGCOLOR") {
        unsigned int color = state.getParameter("OLDVALUE").getValue<unsigned int>();
        Shape* shape = state.getParameter("OBJECT").getValue<Shape*>();
        shape->setFillColor(sf::Color(shape->getFillColor().r, color, shape->getFillColor().b, shape->getFillColor().a));
    }
    if (state.getName() == "SCHANGEBCOLOR") {
        unsigned int color = state.getParameter("OLDVALUE").getValue<unsigned int>();
        Shape* shape = state.getParameter("OBJECT").getValue<Shape*>();
        shape->setFillColor(sf::Color(shape->getFillColor().r, shape->getFillColor().g, color, shape->getFillColor().a));
    }
    if (state.getName() == "SCHANGEACOLOR") {
        unsigned int color = state.getParameter("OLDVALUE").getValue<unsigned int>();
        Shape* shape = state.getParameter("OBJECT").getValue<Shape*>();
        shape->setFillColor(sf::Color(shape->getFillColor().r, shape->getFillColor().g, shape->getFillColor().b, color));
    }
    if (state.getName() == "SCHANGETEXTURE") {
        Texture* texture = state.getParameter("OLDVALUE").getValue<Texture*>();
        Shape* shape = state.getParameter("OBJECT").getValue<Shape*>();
        shape->setTexture(texture);
    }
    if (state.getName() == "SCHANGEXTEXCOORD") {
        int texCoordX = state.getParameter("OLDVALUE").getValue<int>();
        Shape* shape = state.getParameter("OBJECT").getValue<Shape*>();
        shape->setTextureRect(sf::IntRect(texCoordX,shape->getTextureRect().top,shape->getTextureRect().width,shape->getTextureRect().height));
    }
    if (state.getName() == "SCHANGEYTEXCOORD") {
        int texCoordY = state.getParameter("OLDVALUE").getValue<int>();
        Shape* shape = state.getParameter("OBJECT").getValue<Shape*>();
        shape->setTextureRect(sf::IntRect(shape->getTextureRect().left,texCoordY,shape->getTextureRect().width,shape->getTextureRect().height));
    }
    if (state.getName() == "SCHANGEWTEXCOORD") {
        int texCoordW = state.getParameter("NEWALUE").getValue<int>();
        Shape* shape = state.getParameter("OBJECT").getValue<Shape*>();
        shape->setTextureRect(sf::IntRect(shape->getTextureRect().left,shape->getTextureRect().top,texCoordW,shape->getTextureRect().height));
    }
    if (state.getName() == "SCHANGEXTEHCOORD") {
        int texCoordH = state.getParameter("OLDVALUE").getValue<int>();
        Shape* shape = state.getParameter("OBJECT").getValue<Shape*>();
        shape->setTextureRect(sf::IntRect(shape->getTextureRect().left,shape->getTextureRect().top,shape->getTextureRect().width,texCoordH));
    }
}
