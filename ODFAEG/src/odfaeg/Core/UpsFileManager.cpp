
namespace odfaeg {
using namespace std;
using namespace odfaeg;
UpsFileManager::UpsFileManager(std::string path)
{
    if(!path.empty())
        read(path);
    line=0;
}

void UpsFileManager::read(std::string path)
{
    std::ifstream fichier(path.c_str(), std::ios::in);  // on ouvre le fichier en lecture

    if(fichier)  // si l'ouverture a réussi
    {
        std::string contenu;  // déclaration d'une chaîne qui contiendra la ligne lue
        bool start=false;
        int i = 0;
        while(getline(fichier, contenu)) // on met dans "contenu" la ligne
        {
            //Insére le UpsFileManager en coupant les commentaire
            if(start)
                if(contenu.length() > 0 && contenu.at(0) !='#') {
                    code.push_back(contenu);
                }


            if(contenu=="START")
            {
                start=true;
            };
        }
        fichier.close();  // on ferme le fichier
    }
    else  // sinon
        std::cerr << "Impossible d'ouvrir le fichier :  " <<path<<std::endl;
    //Display();
}

void UpsFileManager::save(std::string path)
{
    std::ofstream file( path.c_str(), std::ios::out );
    file<<"START\n";
    for(unsigned int i=0;i<code.size();i++)
    file<<code[i]+"\n";
}

std::string UpsFileManager::getLine()
{
    line++;
    return code[line-1];
}


std::vector<int> UpsFileManager::conversionStringInts(std::string str)
{
    std::vector<string> tabStrings = split(str, "*");
    std::vector<int> tabInts;
    for(unsigned int i=0; i<tabStrings.size(); i++)
    {
        tabInts.push_back(conversionStringInt(tabStrings[i]));
    }
    return tabInts;
}
std::vector<float> UpsFileManager::conversionStringFloats (std::string str) {
    std::vector<string> tabStrings = split(str, "*");
    std::vector<float> tabFloats;
    for(unsigned int i=0; i<tabStrings.size(); i++)
    {
        tabFloats.push_back(conversionStringFloat(tabStrings[i]));
    }
    return tabFloats;
}
void UpsFileManager::Display()
{
    for(unsigned int i=0; i<code.size(); i++)
    {
        std::cout<<code[i]<<std::endl;
    }
}
}
