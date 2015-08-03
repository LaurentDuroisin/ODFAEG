#include "../../../include/odfaeg/Core/UpsMapLoader.h"
namespace odfaeg {
using namespace odfaeg;
using namespace sf;
using namespace std;
UpsMapLoader::UpsMapLoader(std::string path, Map &map):UpsFileManager(path)
{
    success = createMap(map);
}

UpsMapLoader::UpsMapLoader(Map &map):UpsFileManager("")
{
    createCode(map);
}
bool UpsMapLoader::isSuccess () {
    return success;
}
bool UpsMapLoader::createMap(Map &map)
{

    line = 0;
    int id = conversionStringInt(getLine());
    string name = getLine();
    int version = conversionStringInt(getLine());


    vector<string> tab_path_image;
    vector<int> tab_id_image;
    //Seach image's path
    while(code[line]!="END_LOAD_IMAGE") {
        string line = getLine();
        vector<string> subStrings = split (line, "*");
        string path = subStrings[0];
        int tabId = conversionStringInt(subStrings[1]);
        tab_path_image.push_back(path);
        tab_id_image.push_back(tabId);
    }
    line++;
    for (unsigned int i = 0; i < tab_path_image.size(); i++)
        Application::app->getCache().resourceManager<Texture>("TextureManager").fromFile(tab_path_image[i]);
    tab_path_image.clear();
    tab_id_image.clear();
    map.setId(id);
    map.setName(name);
    map.setVersion(version);
    std::vector<Tile*> animsTiles;
    std::vector<Tile*> groundTiles;
    std::vector<Tile*> wallsTiles;
    std::vector<Tile*> decorTiles;
    //Load Tile

    while(code[line]!="END_LOAD_TILE")
    {
        //Line set the path, and img_manager search with information
        int imageId = conversionStringInt(getLine());
        //SubRect
        std::vector<int> tni = conversionStringInts(getLine());
        IntRect subRect (tni[0],tni[1],tni[2],tni[3]);
        //Size
        tni = conversionStringInts(getLine());
        Vec2f size(tni[0],tni[1]);
        tni = conversionStringInts(getLine());
        string type = Entity::getTypeOfInt(tni[1]);
        Vec2f position (tni[2], tni[3]);
        Tile *tile;
        string pType = Entity::getTypeOfInt(tni[8]);

        tile = new Tile (Application::app->getCache().resourceManager<Texture>("TextureManager").getResourceById(imageId), position, size, subRect, 0);
        tile->setType(type);
        tile->setId(tni[0]);
        tile->setZOrder(tni[4]);

        tile->setRotation(tni[5]);
        tile->setOrigin(Vec2f(tni[6], tni[7]));
        if (tile->getParent()->getType() == "E_GROUND") {
            groundTiles.push_back(tile);
        } else if (tile->getParent()->getType() == "E_WALL") {

            wallsTiles.push_back(tile);
        } else if (tile->getParent()->getType() == "E_DECOR") {
            decorTiles.push_back(tile);
        } else if (tile->getParent()->getType() == "E_ANIMATION") {
            animsTiles.push_back(tile);
        }
    }
    line++;

    while (code[line] != "END_LOAD_TILES_GROUND") {
        std::vector<int> tni = conversionStringInts(getLine());
        TileGround* tileGround = new TileGround(Vec2f (0, 0), 0);
        for (unsigned int i = 0; i < groundTiles.size(); i++) {
            if (tni[1] == groundTiles[i]->getId()) {
                tileGround->addTile(groundTiles[i]);

            }
            tileGround->setId(tni[0]);
            Vec2f position (tni[2], tni[3]);
            Vec2f size (tni[4], tni[5]);
            int angle = tni[6];
            //tileGround->setPosition(position);
            tileGround->setSize(size);
            tileGround->setRotation(angle);
            map.addEntity(tileGround);
        }
    }
    line++;
    while (code[line] != "END_LOAD_TILES_DECOR") {
        std::vector<int> tni = conversionStringInts(getLine());
        Vec2f shadCenter;
        shadCenter.x = tni[2];
        shadCenter.y = tni[3];

        for (unsigned int i = 0; i < decorTiles.size(); i++) {
            if (tni[1] == decorTiles[i]->getId()) {
                Decor *decor = new Decor(decorTiles[i], AmbientLight::getAmbientLight());
                decor->setShadowCenter(shadCenter);
                decor->setId(tni[0]);
                Vec2f position(tni[4], tni[5]);
                Vec2f size (tni[6], tni[7]);
                int angle = tni[8];
               // decor->setPosition(position);
                decor->setSize(size);
                decor->setRotation(angle);
                map.addEntity(decor);
            }
        }
    }
    line++;
    while (code[line] != "END_LOAD_ANIMATION") {
        std::vector<int> tni = conversionStringInts(getLine());
        int id = tni[0];
        Vec2f pos (tni[1], tni[2]);
        Vec2f size (tni[3], tni[4]);
        int fri = tni[5];
        float fr = 1.f / (float) fri;
        Anim *anim = new Anim (fr, pos, size, 0);
        anim->setId(id);
        while (code[line] != "END_LOAD_TILES_ANIMATION") {
            vector<int> tni = conversionStringInts(getLine());
            for (unsigned int i = 0; i < tni.size(); i++) {
                for (unsigned int j = 0; j < animsTiles.size(); j++) {
                    if (tni[i] == animsTiles[j]->getId()) {
                        anim->addEntity(animsTiles[j]);
                    }
                }
            }
        }
        map.addEntity(anim);
        line++;

    }
    animsTiles.clear();
    line++;
    while (code[line]!="END_WALL") {
        std::vector<int> tni = conversionStringInts(getLine());
        int id = tni[0];
        int type = tni[2];

        for (unsigned int i = 0; i < wallsTiles.size(); i++) {
            if (tni[1] == wallsTiles[i]->getId()) {
                Vec2f position(tni[3], tni[4]);
                Vec2f size (tni[5], tni[6]);
                int angle = tni[7];
                Wall *w = new Wall(type, 80, wallsTiles[i], AmbientLight::getAmbientLight());
                w->setId(id);
                //w->setPosition(position);
                w->setSize(size);
                w->setRotation(angle);
                map.addEntity(w);
            }
        }

    }

    line++;
    while (code[line]!="END_LIGHT") {

         std::vector<float> tni = conversionStringFloats(getLine());

         Vec2f lightPos (tni[0], tni[1]);
         Vec2f lightDir (tni[2], tni[3]);
         int lightRadius = tni[4];
         int lightIntensity = tni[5];
         int lightHeight = tni[6];
         int r = tni[7];
         int g = tni[8];
         int b = tni[9];
         Color lightColor (r, g, b);
         EntityLight *l;
         if (tni[10] == 0) {
            l = new PonctualLight(lightPos, lightRadius, lightRadius * 0.5f, 0, lightIntensity, lightColor, 16, 0);

         } else if (tni[10] == 1) {
            l = new DirectionnalLight(lightPos, lightDir, lightHeight, lightRadius, lightRadius * 0.5f, lightIntensity, lightColor, 16, 0);
         }
         map.addEntity(l);

    }
    line++;
    vector<BoundingPolygon*> polys;
    while (code[line] != "END_COLLISION") {
        BoundingPolygon *poly = new BoundingPolygon();
        vector<int> tni = conversionStringInts(getLine());
        for (unsigned int i = 0; i < tni.size(); i+=2) {
           Vec2f *point = new Vec2f (tni[i], tni[i+1]);
           poly->addPoint(point);
        }
        polys.push_back(poly);
    }
    for (unsigned int i = 0; i < polys.size(); i++) {
        vector<odfaeg::CellMap*> casesMap =  map.getCasesMap();
        for (unsigned int j = 0; j < casesMap.size(); j++) {
            if (casesMap[j] != NULL && *polys[i] == *casesMap[j]->getPolygon()) {
                casesMap[j]->setPassable(false);
            }
        }
    }
    for (unsigned int i = 0; i < polys.size(); i++)
        delete polys[i];
    polys.clear();
    return true;

}

int UpsMapLoader::getId()
{
    return conversionStringInt(code[0]);
}
std::string UpsMapLoader::getMapName()
{
    return code[1];
}
std::string UpsMapLoader::getMapVersion()
{
    return code[2];
}

void UpsMapLoader::createCode(Map &map)
{
    code.clear();
    createHead(map);
    create(map);
}

void UpsMapLoader::create(Map &map)
{
    std::vector<Entity*> tab_entities = map.getEntities("*");
    std::vector<Tile*> tab_tiles;
    for (unsigned int i = 0; i < tab_entities.size(); i++) {
        std::vector<Entity*> children;
        map.getChildren(tab_entities[i], children, "E_TILE");
        for (unsigned int j = 0; j < children.size(); j++) {
            tab_tiles.push_back(static_cast<Tile*> (children[j]));
        }
    }
    std::vector<Entity*> tab_tGround = map.getEntities("E_GROUND");
    std::vector<Entity*> tab_tDecor = map.getEntities("E_DECOR");
    std::vector<Entity*> tab_anims = map.getEntities("E_ANIM");
    std::vector<Entity*> tab_lights = map.getEntities("E_LIGHT");
    std::vector<Entity*> tab_walls = map.getEntities ("E_WALL");

    //Usualy Information
    code.push_back("#");
    code.push_back("#");
    code.push_back("#");
    //Path images
    createPathImage(map);
    //Load Reference Tile

    createRefTiles(tab_tiles);
    createRefTGround(tab_tGround);
    createRefTDecor(tab_tDecor);
    createRefAnims(tab_anims);
    createRefLightAndWalls(tab_lights, tab_walls);
    createRefCollisions (map);

    code.push_back("#");    //Post Tile


    code.push_back("#");
    code.push_back("#");
    code.push_back("END_LOAD_MAP");
}

void UpsMapLoader::createHead(Map &map)
{
    code.push_back("#ID:");
    code.push_back(conversionIntString(map.getId()));
    code.push_back("#Nom");
    code.push_back(map.getName());
    code.push_back("#Version");
    code.push_back(conversionIntString(map.getVersion()));
}

void UpsMapLoader::createPathImage(Map &map)
{
    std::vector<std::string> tab = map.getPaths();

    std::vector<std::string>::iterator it;
    code.push_back("#Load Images");
    unsigned int id = 0;
    for(it = tab.begin(); it != tab.end(); it++)
    {
        code.push_back("#");
        ostringstream oss;
        oss<<*it<<"*"<<id;
        code.push_back(oss.str());
        id++;
    }
    code.push_back("#");
    code.push_back("END_LOAD_IMAGE");
    code.push_back("#");
}

void UpsMapLoader::createRefTiles(std::vector<Tile*> &tab)
{
    code.push_back("#Reference Tile");
    if(tab.empty()) code.push_back("#No Tile in Memory");
    else
    {
        for(unsigned int i=0; i<tab.size(); i++)
        {
            code.push_back("#Tile ");
            code.push_back("#Image id :");
            code.push_back(conversionIntString(Application::app->getCache().resourceManager<Texture>("TextureManager").getIdByResource(tab[i]->getTexture())));
            code.push_back("#SubRect :");
            code.push_back(conversionIntString(tab[i]->getTextureRect().left)+"*"+conversionIntString(tab[i]->getTextureRect().top)+"*"+conversionIntString(tab[i]->getTextureRect().width)+"*"+conversionIntString(tab[i]->getTextureRect().height));
            code.push_back("#Size :");
            code.push_back(conversionIntString(tab[i]->getSize().x)+"*"+conversionIntString(tab[i]->getSize().y));
            code.push_back("#Id*type*pos.x*pos.y*zOrder*angle*center.x*center.y*pType : ");
            code.push_back(conversionIntString(tab[i]->getId())+"*"+conversionIntString(tab[i]->getTypeInt())+"*"+conversionIntString(tab[i]->getPosition().x)
                           +"*"+conversionIntString(tab[i]->getPosition().y)+"*"+conversionIntString(tab[i]->getZOrder())
                           +"*"+conversionIntString(tab[i]->getRotation())+"*"+conversionIntString(tab[i]->getOrigin().x)
                           +"*"+conversionIntString(tab[i]->getOrigin().y)+"*"+conversionIntString(Entity::getIntOfType(tab[i]->getParent()->getType())));

        }
    }
    tab.clear();
    code.push_back("END_LOAD_TILE");
}
void UpsMapLoader::createRefTGround(vector<Entity*> &tab) {
    code.push_back("#Reference tile ground");
    if (tab.empty()) code.push_back("#No tiles ground in Memory");
    else {
        for (unsigned int i = 0; i < tab.size(); i++) {
            TileGround *tg = static_cast<TileGround*>(tab[i]);
            code.push_back("#Id*TileId*pos.x*pos.y*size.x*size.y*rotation");
            code.push_back(conversionIntString(tg->getId())+"*"+
                           conversionIntString(tg->getTiles()[0]->getId())+"*"+
                           conversionIntString(tg->getPosition().x)+"*"+
                           conversionIntString(tg->getPosition().y)+"*"+
                           conversionIntString(tg->getSize().x)+"*"+
                           conversionIntString(tg->getSize().y)+"*"+
                           conversionIntString(tg->getRotation()));
        }
    }
    code.push_back("END_LOAD_TILES_GROUND");
}
void UpsMapLoader::createRefTDecor(vector<Entity*> &tab) {
    code.push_back("#Reference tile decor");
    if (tab.empty()) code.push_back("#No tiles decor in Memory");
    else {
        for (unsigned int i = 0; i < tab.size(); i++) {
            Decor *dec = static_cast<Decor*> (tab[i]);
            code.push_back("#Id*TileId*ShadCenter.x*ShadCenter.y*pos.x*pos.y*size.x*size.y*rotation");
            code.push_back(conversionIntString(dec->getId())+"*"+
                           conversionIntString(dec->getTile()->getId())+"*"+
                           conversionIntString(dec->getShadowCenter().x)+"*"+
                           conversionIntString(dec->getShadowCenter().y)+"*"+
                           conversionIntString(dec->getPosition().x)+"*"+
                           conversionIntString(dec->getPosition().y)+"*"+
                           conversionIntString(dec->getSize().x)+"*"+
                           conversionIntString(dec->getSize().y)+"*"+
                           conversionIntString(dec->getRotation()));
        }
    }
    code.push_back("END_LOAD_TILES_DECOR");
}
void UpsMapLoader::createRefAnims(vector<Entity*> &tab) {
    code.push_back("#Reference anims");
    if (tab.empty()) code.push_back("#No animations in Memory");
    else {
        for (unsigned int i = 0; i < tab.size(); i++) {
            Anim* anim = static_cast<Anim*> (tab[i]);
            code.push_back("#Id*pos.x*pos.y*size.x*size.y*fr");
            float fr = anim->getFrameRate();
            int fri = (int) (1.f / fr);
            code.push_back(conversionIntString(anim->getId())+"*"+conversionIntString(anim->getPosition().x)
                               +"*"+conversionIntString(anim->getPosition().y)+"*"+conversionIntString(anim->getSize().x)
                               +"*"+conversionIntString(anim->getSize().y)+"*"+conversionIntString(fri));
            code.push_back("#TilesId");
            string line = "";
            for (unsigned int j = 0; j < anim->getEntities().size(); j++) {
                if ((j + 1) % 10 == 0 || j == anim->getEntities().size() - 1) {
                    line += conversionIntString(anim->getEntities()[j]->getId());
                    code.push_back(line);
                    line = "";
                } else {
                    line += (conversionIntString(anim->getEntities()[j]->getId())+"*");
                }
            }
            code.push_back("END_LOAD_TILES_ANIMATION");
        }

    }
    tab.clear();
    code.push_back("END_LOAD_ANIMATION");
}
void UpsMapLoader::createRefLightAndWalls (std::vector<Entity*> &lights, std::vector<Entity*> &walls) {
    code.push_back("#Reference walls");

    if (walls.empty()) code.push_back("#No walls in memory!");
    else {
        for (unsigned int i = 0; i < walls.size(); i++) {
            Wall *wall = static_cast<Wall*> (walls[i]);
            code.push_back("#Id*TileId*type*pos.x*pos.y*size.x*size.y*rotation : ");
            code.push_back(conversionIntString(wall->getId())+"*"+
                           conversionIntString(wall->getEntityId())+"*"+
                           conversionIntString(wall->getWallType())+"*"+
                           conversionIntString(wall->getPosition().x)+"*"+
                           conversionIntString(wall->getPosition().y)+"*"+
                           conversionIntString(wall->getSize().x)+"*"+
                           conversionIntString(wall->getSize().y)+"*"+
                           conversionIntString(wall->getRotation()));
        }
    }
    walls.clear();
    code.push_back("END_WALL");
    code.push_back("#Reference lights");
    if (lights.empty()) code.push_back("#No lights in memory!");
    else {
        for (unsigned int i = 0; i < lights.size(); i++) {
            EntityLight *light = static_cast<EntityLight*> (lights[i]);
            if (light->getType() == "E_DIRECTIONNAL_LIGHT") {
                DirectionnalLight *pLight = static_cast<DirectionnalLight*> (light);
                code.push_back("#Center.x*center.y*dir.x*dir.y*radius*intensity*height*color.r*color.g*color.b*type");
                code.push_back(conversionIntString(pLight->getCenter().x)+"*"+conversionIntString(pLight->getCenter().y)+"*"+conversionFloatString(pLight->getDir().x)+"*"+conversionFloatString(pLight->getDir().y)
                +"*"+conversionIntString(pLight->getSize().x * 0.5f)+"*"+conversionIntString(pLight->getIntensity())+"*"+conversionIntString(pLight->getHauteur())+"*"+conversionIntString(pLight->getColor().r)+"*"
                +conversionIntString(pLight->getColor().g)+"*"+conversionIntString(pLight->getColor().b)+"*"+conversionIntString(Entity::getIntOfType(pLight->getType())));
            } else if (light->getType() == "E_PONCTUAL_LIGHT") {
                PonctualLight *pLight = static_cast<PonctualLight*> (light);
                code.push_back("#Center.x*center.y*radius*intensity*height*color.r*color.g*color.b*type");
                code.push_back(conversionIntString(pLight->getCenter().x)+"*"+conversionIntString(pLight->getCenter().y)+"*"+conversionIntString(pLight->getSize().x * 0.5f)+"*"+conversionIntString(pLight->getIntensity())+"*"+conversionIntString(pLight->getHeight())+"*"+conversionIntString(light->getColor().r)+"*"
                +conversionIntString(pLight->getColor().g)+"*"+conversionIntString(pLight->getColor().b)+"*"+conversionIntString(Entity::getIntOfType(pLight->getType())));
            }

        }
    }
    lights.clear();
    code.push_back("END_LIGHT");
}
void UpsMapLoader::createRefCollisions (Map &map) {
    code.push_back ("#Reference collisions, positions of polygons with collision");
    string line = "";
    for (unsigned int i = 0; i < map.getCasesMap().size(); i++) {
        if (map.getCasesMap()[i] != NULL && !map.getCasesMap()[i]->isPassable()) {
            CellMap *cm = map.getCasesMap()[i];
            for (int j = 0; j < 4;j++) {
                if (j != 3) {
                    line += conversionIntString(cm->getPolygon()->getPoint(j).x)+"*"+conversionIntString(cm->getPolygon()->getPoint(j).y)+"*";
                } else {
                    line += conversionIntString(cm->getPolygon()->getPoint(j).x)+"*"+conversionIntString(cm->getPolygon()->getPoint(j).y);
                }
            }
            code.push_back(line);
            line = "";
        }
    }
    code.push_back("END_COLLISION");
}
}

