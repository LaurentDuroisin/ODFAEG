#include "../../../odfaeg/Driver/r600_asm.hpp"
void CppToGppParser::CppToGppPaser(unsigned int progAdr) {
    //Adresse du programme dans la VRAM.
    currentDataAdr = progAdr;
    currentInstAdr = progAdr;
    firstPageAdr = progAdr;
    currentPageAdr = progAdr;
    pagesToLoadAdr = 0;
    types = {"unsigned char", "char", "unsigned int", "int", "unsigned long", "long", "unsigned short", "short"};
}
void CppToGppParser::parseCppToGpp(std::string cppcode) {
    parsePreProcessor(cppcode);
    unsigned int blocSize = 0;
    parseCppCode(blocSize, cppcode, instByteCode.end(), datasByteCode.end());
    link();
}
void CppToGppParser::parsePreProcessor(std::string cppcode) {
    std::vector<std::string> instructions = core::split(cppcode, ";");
    unsigned int i  = 0;
    while (i < instructions.size()) {
        if (isStructureDecalration(instructions[i])) {
            int nbStructFound = 0;
            parseDataBlocs(instructions, i, nbStructFound);
            i++;
            if (nbStructFound > 0) {
                log<<"Error : missing end of struct declaration!"<<std::endl;
            } else if (nbStructFound < 0) {
                log<<"Error : missing struct declaration!"<<std::endl;
            }
        } else {
            parseData(instructions[i]);
            i++;
        }
    }
}
void CppToGppParser::parseDataBlocs(std::vector<std::string> codeblocs, unsigned int& i, int& nbStructFound, Struct* parent) {
    //Tant qu'on n'atteint pas la fin du bloc, on continue.
    while (codeblocs[i].find("}") == std::string::npos && i < codeblocks.size()) {
        //On crée la structure.
        if (isStructureDeclaration(codeblocs[i])) {
             std::vector<std::string>::iterator it;
             //Il faut faire ceci car il n'y a pas de ; après une déclaration de structure.
             for (unsigned int n = 0, it = codeblocs.begin(); n < i && it < codeblocs.end(); n++, it++);
             if (codeblocs[i].find("{") != std::string::npos) {
                 std::vector<std::string> params = core::split(codeblocs[i], "{");
                 //Structure non vide ?
                 if (params.size() > 1) {
                    codeblocs.insert(it, params.begin()+1, params.end());
                 }
                 params = core::split(params[0], " ");
                 Struct s;
                 s.name = params[0]+" "+params[1];
                 s.type = params[1];
                 types.push_back(s.type);
                 structs.insert(std::make_pair(s.type, s));
                 i++;
                 nbStructFound++;
                 //Structures imbriquées, il faut réappeler la fonction.
                 if (nbStructFound > 1) {
                    parseDataBlocs(codeblocks, i, nbStructFound, &structs.end());
                 }
            } else {
                log<<"Error : missing open bracket after structure declaration!"<<std::endl;
                return;
            }
        //Si ce n'est pas un structure, on compile l'instruction..
        } else {
             parseData(codeBlocs[i], &structs.end());
             i++;
        }
    }
    nbStructFound--;
}
void CppToGppParser::parseData(std::string instruction, Struct* s) {
    if (isVariableDeclaration(instructions)) {
        parseVariableDeclaration(instructions, s);
    }
    if (isFunctionDeclaration(instructions)) {
        parseFunction(instructions, s);
    }
}
void CppToGppParser::parseCppCode(unsigned int& blocSize, std::vector<std::string> code, std::vector<std::string>::iterator instIt, std::vector<std::string>::iterator datasIt) {
    std::vector<std::string> instructions = core::split(code, ";");
    for (unsigned int i = 0; i < instructions.size(); i++) {
        if (isLoop(instructions[i])) {
            parseLoop(instructions[i]);
        } else if (isControlInst(instructions[i])) {
            parseControlInt(instructions[i]);
        } else {
            parseVariableOperation(instructions[i]);
        }
    }
}
void CppToGppParser::link() {
    //Liage des fonctions.
    //Appel de la fonction main et de toutes les autres fonctions.
    linkFunctions();
    //Liage des pages.
    linkPages();
    //Liage des sauts.
    //Create the jump tables. (Saut d'instructions en fonction de la valeur d'une variable booléenne)
    linkJumpTables();
    //Liage des fonctions d'allocation dynamique.
    linkDynamicAllocations();
}
void CppToGppPaster::linkFunctions() {
    std::vector<std::string> instructions = source_code.split(";");
    for (unsigned int i = 0; i < instructions.size(); i++) {
        if (isFunctionCall(instructions)) {
            std::vector<std::string> parts = core::split("(");
            std::vector<std::string> args = core::split(",");
            std::string fct = parts[0]+"(";
            for (unsigned int a = 0; a < args.size(); a++) {
                fct += args[a];
                if (a == args.size() - 1) {
                    fct += ")"
                } else {
                    fct += ",";
                }
            }
            functionCall(fct, vars.find(fct).adr);
        }
    }
}
void CppToGppPaster::linkPages() {
    //Instructions à charger depuis la RAM pendant l'exécution du programme.
    //Addresses source.
    std::vector<std::string> params = {"uint32_t*", "RAM_INST_ADR"};
    staticAllocateGPUMemory(blocs.size() * sizeof(uint32_t), params, datasByteBode.begin());
    //Addresses de destination.
    params = {"uint32_t*", "VRAM_INST_ADR"};
    staticAllocateGPUMemory(blocs.size() * sizeof(uint32_t), params, datasByteBode.begin());
    //Nombre d'instructions à charger.
    params = {"uint32_t*", "SIZE"};
    staticAllocateGPUMemory(blocs.size() * sizeof(uint32_t), params, datasByteBode.begin());
    std::vector<uint32_t>::iterator it;
    for (unsigned int i = 0, i < blocs.size(); i++) {
        //Liage des pages.
        /*Il faut charger la page suivante à chaque fois que la position du bloc dépasse la taille maximale d'une page d'instructions et à la fin
        de l'appel du bloc d'instructions, il faut recharger la première page pour un appel suivant éventuel sur ce même bloc.
        Il faut aussi stopper le programme jusqu'à ce que la page d'instructions suivante soit chargée.
        Pour les fonctions récursives il faut charger tout le bloc de la fonction pour ne pas devoir copier la
        fonction à chaque fois qu'elle est appelée si elle est sur plusieurs pages ce qui serait contre performant.*/
        unsigned int offsetSum = 0;
        unsigned int position = blocs[i].firstInstAdr + PAGE_SIZE;
        for (unsigned int j = 0, it = instByteCode.begin(); j < position; j++, it++) {

        }
        if (blocs[i].size > PAGE_SIZE && !(blocs[i].type == Bloc::REC_FUNCTION)) {
            progSize += position;
            do {
                std::vector<uint32_t> byteCode;
                byteCode.push_back(EG_V_SQ_ALU_WORD1_OP2_SQ_OP2_INST_MOV);
                byteCode.push_back(C_SQ_CF_ALU_WORD0_ADDR(vars.find("VRAM_INST_ADR").adr + i * sizeof(uint32_t)));
                byteCode.push_back(V_SQ_ALU_SRC_LITERAL);
                byteCode.push_back(blocs[i].firstPageAdr);
                byteCode.push_back(EG_V_SQ_ALU_WORD1_OP2_SQ_OP2_INST_MOV);
                byteCode.push_back(C_SQ_CF_ALU_WORD0_ADDR(vars.find("RAM_INST_ADR").adr + i * sizeof(uint32_t)));
                byteCode.push_back(V_SQ_ALU_SRC_LITERAL);
                byteCode.push_back(position);
                instByteCode.insert(it, byteCode.begin(), byteCode.end());
                //Si l'on est arrivé sur la dernière page, la page peut ne pas être totalement remplie, il y a donc moins de données à charger.
                std::vector<uint32_t> byteCode;
                if (position - blocs[i].firstInstAdr + PAGE_SIZE + byteCode.size() + 4 + blocSize < blocs[i].size() + byteCode.size() + 4 + blocSize) {
                    byteCode.push_back(EG_V_SQ_ALU_WORD1_OP2_SQ_OP2_INST_MOV);
                    byteCode.push_back(C_SQ_CF_ALU_WORD0_ADDR(vars.find("SIZE").adr + i * sizeof(uint32_t)));
                    byteCode.push_back(V_SQ_ALU_SRC_LITERAL);
                    byteCode.push_back(blocs[i].size() - offsetSum);
                    instByteCode.insert(it, byteCode.begin(), byteCode.end());
                } else {
                    byteCode.push_back(EG_V_SQ_ALU_WORD1_OP2_SQ_OP2_INST_MOV);
                    byteCode.push_back(C_SQ_CF_ALU_WORD0_ADDR(vars.find("SIZE").adr + i * sizeof(uint32_t)));
                    byteCode.push_back(V_SQ_ALU_SRC_LITERAL);
                    byteCode.push_back(PAGE_SIZE + byteCode.size() + 1);
                    instByteCode.insert(it, byteCode.begin(), byteCode.end());
                }
                std::string code = R"(while (SIZE != 0) {}"
                                  )";
                unsigned int blocSize = 0;
                parseCppCode(blocSize, code, instByteCode.end());
                offsetSum += byteCode.size() + blocSize;
                position += PAGE_SIZE + byteCode.size() + blocSize;
                //Il faut remettre à jour le bloc vu que la taille des instructions du bloc à changé.
                blocs[i].size += byteCode.size() + blocSize;
                blocs[i].lastInstAdr += byteCode.size() + blocSize;
                if (position == blocs[i].firstInstAdr + PAGE_SIZE) {
                    if (blocs[i].type == FUNCTION)
                        blocs[i].firstPageSize = PAGE_SIZE + byteCode.size() + blocSize + 1;
                    else
                        blocs[i].firstPageSize = PAGE_SIZE + byteCode.size() + blocSize;
                }
                progSize += position;
            } while (position - blocs[i].firstInstAdr < blocs[i].size);
            //Copie la première instruction de la fonction dans la première page de la fonction.
            byteCode.push_back(EG_V_SQ_ALU_WORD1_OP2_SQ_OP2_INST_MOV);
            byteCode.push_back(C_SQ_CF_ALU_WORD0_ADDR(vars.find("VRAM_INST_ADR").adr + i * sizeof(uint32_t)));
            byteCode.push_back(V_SQ_ALU_SRC_LITERAL);
            byteCode.push_back(blocs[i].firstPageAdr);
            byteCode.push_back(EG_V_SQ_ALU_WORD1_OP2_SQ_OP2_INST_MOV);
            byteCode.push_back(C_SQ_CF_ALU_WORD0_ADDR(vars.find("RAM_INST_ADR").adr + i * sizeof(uint32_t)));
            byteCode.push_back(V_SQ_ALU_SRC_LITERAL);
            byteCode.push_back(blocs[i].firstInstAdr);
            std::string code = R"(while (SIZE != 0) {}"
                                  )";
            unsigned int blocSize = 0;
            parseCppCode(blocSize, code, instByteCode.end());
            std::vector<uint32_t> byteCode;
            byteCode.push_back(EG_V_SQ_ALU_WORD1_OP2_SQ_OP2_INST_MOV);
            byteCode.push_back(C_SQ_CF_ALU_WORD0_ADDR(vars.find("SIZE").adr + i * sizeof(uint32_t)));
            byteCode.push_back(V_SQ_ALU_SRC_LITERAL);
            byteCode.push_back(PAGE_SIZE + byteCode.size() + 1);
            byteCode.push_back(EG_V_SQ_CF_ALU_WORD1_SQ_CF_INST_ALU_POP_AFTER);
            offsetSum += byteCode.size() + blocSize;
            blocs[i].size += byteCode.size() + blocSize;
        } else {
            progSize += blocs[i].size();
            if (blocs[i].type == Bloc::REC_FUNCTION || blocs[i].type == Bloc::FUNCTION) {
                instByteCode.insert(it, EG_V_SQ_CF_ALU_WORD1_SQ_CF_INST_ALU_POP_AFTER);
                offsetSum += 1;
                blocs[i].size += 1;
                blocs[i].lastInstAdr += 1;
                blocs[i].firstPageSize = blocs[i].size + 1;
            } else {
                blocs[i].firstPageSize = blocs[i].size;
            }
        }
        progSize += offsetSum;
        //Il faut remettre à jour les adresses pour les blocs suivants car la taille du bloc courant à changé.
        for (unsigned int n = i+1; n < blocs.size(); n++) {
            blocs[n].firstInstAdr += offsetSum;
            blocs[n].firstPageAdr += offsetSum;
            blocs[n].lastInstAdr += offsetSum;
        }
    }
}
void CppToGppPaster::linkJumpTables() {
    datasByteCode.push_back(EG_V_SQ_CF_WORD1_SQ_CF_INST_JUMPTABLE);
    for (unsigned int i = 0, i < blocs.size(); i++) {
        if (blocs[i].type == Bloc::LOOP || blocs[i].type == Bloc::IF) {
            datasByteCode.push_back(C_SQ_CF_WORD0_JUMPTABLE_SEL);
            datasByteCode.push_back(i);
            //loop start
            datasByteCode.push_back(C_SQ_CF_WORD0_ADDR);
            datasByteCode.push_back(blocs[i].lastInstAdr - (blocs[i].firstInstAdr - blocs[i].firstPageAdr));
            //loop end.
            datasByteCode.push_back(C_SQ_CF_WORD0_ADDR);
            datasByteCode.push_back(blocs[i].firstInstAdr - (blocs[i].firstInstAdr - blocs[i].firstPageAdr));
        }
    }
}
void CppToGppParser::linkDynamicAllocations() {
    progSize += datasByteCode.size() + ? + ?;
    std::string code =
    "void* gmalloc (unsigned int typesize, unsigned int size) {
    "bool IS_MEM_FREE_FOUND = 0, MEM_STATE = 0, unsigned int adr = 0;"
    "   while(IS_MEM_FREE_FOUND == 0 && adr < MAX_ADR) {"
    "       IS_MEM_FREE_FOUND = 1;"
    "       unsigned int i;"
    "       for (i = 0; i < typesize * size && IS_MEM_FREE_FOUND; i++, adr+=2) {"
    "           memcpy(adr, MEM_STATE, 1);"
    "           /*Attention aux variables temporaires du programme, il ne faut pas que l'allocation dynamique vienne écrire dans les données du programme.*/"
    "           if (MEM_STATE != 0 || adr >= "+core::conversionIntString(progAdr)+" && adr < "+core::conversionIntString(progSize)+")";
    "               IS_MEM_FREE_FOUND = 0;"
    "           }"
    "       }"
    "       adr += (adr >= "+core::conversionIntString(progAdr)+" && adr < "+core::conversionIntString(proSize)+") ? "+core::conversionIntString(proSize)+" : 2;"
    "   }"
    "}"
    unsigned int blocSize = 0;
    parseCppCode(blockSize, code, instByteCode.end());
    std::vector<uint32_t>::reverse_iterator it;
    //On doit le faire à la fin lorsque la taille du programme est connue.
    std::vector<std::string> instructions = core::split(source_code, ";");
    for (int i = instructions.size() - 1; i >= 0; i--) {
        if (isDynamicAllocation(instructions[i])) {
            unsigned int typesize = dynAllocationStack.back().first;
            unsigned int size = dynAllocationStack.back().first;
            dynAllocationStack.pop_back();
            //Insertions des instructions d'appel de la fonction galloc.
        }
    }
}
void CppToGppParser::functionCall(std::string parentFunctionCall, uint32_t adr) {
    //Recherche du bloc de la fonction a appeler.
    unsigned int blocPos=0;
    for (unsigned int i = 0; i < blocs.size(); i++) {
        if (adr == blocs[i].firstPageAdr) {
            blocPos = i;
        }
    }
    //Recherche de l'endroit ou l'on doit insérer l'appel de la fonction dans le byte code.
    for (it = instByteCode.begin(); *it != EG_V_SQ_CF_WORD1_SQ_CF_INST_CALL && it != instByteCode.end(), it++) {

    }
    //Passage des paramètres à la fonction.
    std::vector<std::string> params = core::split(parentFunction, ",");
    for(unsigned int i = 0; i < params.size(); i++) {
        if (core::isDigit(params[i])) {
            affectationFromLiteral(4, params[i], it);
        } else {
            affectationFromVariable(4, params[i], it);
        }
    }
    std::vector<uint32_t> byteCode;
    *it = EG_V_SQ_CF_ALU_WORD1_SQ_CF_INST_ALU_PUSH_BEFORE;
    byteCode.push_back(it, EG_V_SQ_CF_WORD1_SQ_CF_INST_CALL);
    byteCode.push_back(it, C_SQ_CF_WORD0_ADDR(adr));
    instByteCode.insert(it, byteCode.begin(), byteCode.end());
    unsigned int offset = 3;
    for (unsigned int i = 0; i < instructions.size(); i+=2) {
        if (instructions.find("(") != std::string::npos) {
            std::vector<std::string> parts = core::split("(");
            std::vector<std::string> args = core::split(",");
            std::string fct = parts[0]+"(";
            for (unsigned int a = 0; a < args.size(); a++) {
                fct += args[a];
                if (a == args.size() - 1) {
                    fct += ")"
                } else {
                    fct += ",";
                }
            }
            adr = vars.find(fct).second.first;
            /*Si c'est un appel d'une grosse fonction récursive, comme les instructions  des deux fonctions
                sont chargées au même endroit dans la VRAM, on a besoin de mémoire supplémentaire pour ne pas effacer les instructions de la fonction appelante : il faut copier les pages.
                Faire des fonctions récursives trop grosse est contre-performant*/
            if (parentfunctionCall == fct) {
                blocs[blocPos].type = REC_FUNCION;
            } else {
                blocs[blocPos].type = FUNCION;
            }
            //Remise à jour du bloc et des blocs suivants vu que le taille du bloc à changé.
            blocs[blocPos].size += offset;
            blocs[blocPos].lastInstAdr += offset;
            for (unsigned int n = blocPos+1; n < blocs.size(); n++) {
                blocs[n].firstInstAdr += offset;
                blocs[n].lastInstAdr += offset;
            }
        }
    }
}
unsigned int CppToGppParser::getsizeof(std::string type, unsigned int& size) {
    if (type == "char" || type == "unsigned char")
        return 1;
    if (type == "short" || type == "unsigned short")
        return 2;
    if (type == "int" || type == "unsigned int")
        return 4;
    if (type == "long" || type == "unsigned long")
        return 8;
    if (type.find("*") != std::string::npos)
        return 4;
    Struct s = structs.find(type);
    for (unsigned int i = 0; i < s.vars.size(); i++) {
        size += getsizeof(s.vars[i].type, size);
    }
    for (unsigned int i = 0; i < s.children.size(); i++) {
        size += getsizeof(s.children[i].type, size);
    }
    return size;
}
bool CppToGppParser::isStructureDeclaration(std::string instruction) {
    if (instruction.find("(") == std::string::npos && instruction.find("(") != std::string::npos || instruction.find("class") != std::string::npos)
        return true;
    return false;
}
bool CppToGppParser::isVariableDeclaration(std::string instruction) {
    if (instruction.find("(") != std::string::npos || instruction.find("struct") != std::string::npos || instruction.find("class") != std::string::npos)
        return false;
    //Il peut y avoir plusieurs déclarations par instructions.
    std::vector<std::string> params = core::split(instruction, ",");
    if (params.size() > 1) {
        if (params[0] == "typedef") {
            types.push_back(params2[1]);
            return false;
        } else {
            for (unsigned int i = 0; i < types.size(); i++) {
                if (params[0] == types[i]) {
                    typeFound = true;
                }
            }
            if (!typeFound) {
                log<<"Error : unknow type : "<<params[0]<<std::endl;
                return false;
            }
            for (unsigned int i = 1; i < params.size(); i++) {
                //Il peut y avoir une déclaration et une affectation, on ignore l'affectation pour le moment.
                if(params[i].find("=")) {
                    unsigned int pos = params[i].find_first_of("=");
                    params[i].erase(pos, params[i].size() - pos);
                }
                std::vector<std::string> params2 = params[i].split(params[i], " ");
                if (!isVarNameValid(params2[1])) {
                    log<<"Error : variable names cannot be digits or c++ operators!"<<std::endl;
                    return false;
                }
            }
        }
    } else if (params.size() == 1) {
        log<<"Error : missing type name or variable name!"<<std::endl;
        return false;
    } else {
        return false;
    }
    return true;
}
void CppToGppParser::parseVariableDeclaration(std::string instruction, Struct* s) {
    //Il peut y avoir plusieurs déclarations par instructions.
    std::vector<std::string> params = instruction.split(instruction, ",");
    std::string type = params[0];
    for (unsigned int i = 1; i < params.size(); i++) {
        //Il peut y avoir une déclaration et une affectation, on ignore l'affectation pour le moment.
        if(params[i].find("=")) {
            unsigned int pos = params[i].find_first_of("=");
            params[i].erase(pos, params[i].size() - pos);
        }
        Var var;
        if (params[i].at(0) == "*") {
            params[i].erase(0, 1);
            var.name = params[i];
            if (type.find("*") != std::string::npos) {
                log<<"Error : pointer already declared in type name!"<std::endl;
                return false;
            }
            var.type = type+"*";
        }
        var.name = params[i];
        var.type = type;
        vars.insert(std::make_pair(name, var));
        if (s != nullptr) {
            s->vars.push_back(var);
        }
        staticAllocateGPUMemory(getsizeof(params2[0]), params2, datasByteCode.end());
    }
    return true;
}
void CppToGppParser::equalOperator(std::vector<std::string> params, std::vector<std::string>::iterator instIt, std::vector<std::string>::iterator datasIt) {
    std::vector<std::string> result = core::split(params, "=");
    std::vector<std::string> infos = core::split(result, " ");
    std::vector<std::string> args = core::split(params, "==");
    if (infos[0] == "bool")
        staticAllocateGPUMemory(1, result, datasIt);
    instByteCode.push_back(V_SQ_LDS_INST_AND);
    instByteCode.push_back(C_SQ_ALU_WORD0_SRC0_REL);
    instByteCode.push_back(vars.find(args[0]).first.second);
    instByteCode.push_back(C_SQ_ALU_WORD0_SRC1_REL);
    instByteCode.push_back(vars.find(args[1]).first.second);
    instByteCode.push_back(C_SQ_ALU_WORD1_DST_REL);
    instByteCode.push_back(vars.find(tmp1).first.second);
    instByteCode.push_back(V_SQ_LDS_INST_AND);
    instByteCode.push_back(C_SQ_ALU_WORD0_SRC0_REL);
    instByteCode.push_back(vars.find(args[0]).first.second);
    instByteCode.push_back(C_SQ_ALU_WORD0_SRC1_REL);
    instByteCode.push_back(vars.find(args[1]).first.second);
    instByteCode.push_back(C_SQ_ALU_WORD1_DST_REL);
    instByteCode.push_back(vars.find(tmp2).first.second);
}
void CppToGppParser::staticAllocateGPUMemory(unsigned int size, std::vector<std::string> params, std::vector<std::string>::iterator it) {
    //Stocke le nom, l'adresse et le type de la variable.
    //Adresse du premier octet = adresse courante + taille du mot.
    /*On a besoin de stocker les paramètres de la fonction pour retrouver son nom lors du passage de paramètres
    pour l'appeler plus tard*/
    std::vector<uint32_t> byteCode;
    for (unsigned int i = 0; i < size; i++) {
        //type d'information = un litéral.
        byteCode.push_back(it, V_SQ_ALU_SRC_LITERAL);
        //Adresse suivante = adresse courante + taille du mot.
        currentDataAdr++;
        //Initialisation de l'octet à 0.
        byteCode.push_back(0);
        vars.end().adr = currentDataAdr;
        //Adresse suivante = adresse courante + taille du mot.
        currentDataAdr++;
    }
    datasByteCode.insert(it, byteCode.begin(), byteCode.end());
}
//A parser en dernier pour ajouter à la pile des allocations dynamiques.
void CppToGppParser::dynamicAllocateGPUMemory(unsigned int typesize, unsigned int size, std::vector<std:string>::iterator it) {
    /*Dit juste que l'on va faire un appel à la fonction d'allocation dynamique mais on ne peut
    pas encore passer les paramètres parce que cette fonction n'est parsée qu'à la fin.*/
    dynAllocStack(std::make_pair(typesize, size));
}
void CppToGppParser::loopWhile(std::vector<string> params, std::vector<std::string>::iterator instIt, std::vector<std::string>::iterator datasIt) {
    unsigned int blocSize = 0;
    unsigned int pos1 = params[0].find_first_of("(");
    unsigned int pos2 = params[0].find_first_of(")");
    unsigned int size = pos2 - pos1;
    std::string str = params[0].substr(pos1, size);
    std::vector<std::string> conditions = core::split(str, ",");
    instByteCode.push_back(EG_V_SQ_CF_WORD1_SQ_CF_INST_LOOP_START);
    blocSize++;
    parseCppCode(blocSize);
    unsigned int pos1 = params[0].find_first_of("(");
    unsigned int pos2 = params[0].find_first_of(")");
    unsigned int size = pos2 - pos1;
    std::string str = params[0].substr(pos1, size);
    std::vector<std::string> conditions = core::split(str, ",");
    std::vector<std::string> params = {conditions[0], "TMP_COND_RES"};
    if (conditions[0].find("==") != std::string::npos) {
        equalOperator(params, it);
    }
    instByteCode.push_back(C_SQ_CF_WORD0_JUMPTABLE_SEL);
    instByteCode.push_back(blocs.size());
    instByteCode.push_back(S_SQ_CF_WORD0_JUMPTABLE_SEL(vars.find("TMP_COND_RES").address));
    instByteCode.push_back(EG_V_SQ_CF_WORD1_SQ_CF_INST_LOOP_END);
    blockSize++;
    createBloc(blocSize, params);
}
void CppToGppParser::affectationFromLiteral(unsigned int typesize, std::vector<std::string> params, unsigned int offset, unsigned int size, std::vector<std::string>::iterator it) {
    //Déplacement de la valeur des litéraux aux adresses des octets de la variable.
    for (unsigned int i = offset; i < size; i++) {
        for (unsigned int j = 0; j < typesize; j++) {
            //Déplacement des valeurs des octets aux adresses des octects à affecter dans la VRAM..
            instByteCode.push_back(EG_V_SQ_ALU_WORD1_OP2_SQ_OP2_INST_MOV);
            //type d'information = une adresse.
            instByteCode.push_back(C_SQ_CF_ALU_WORD0_ADDR(vars.find(params[0]).adr));
            //type d'information = un litéral.
            instByteCode.push_back(V_SQ_ALU_SRC_LITERAL);
            //Valeur de l'octet.
            instByteCode.push_back(std::hex(conversionStringInt(params[1]) << (j * 8) & 0xFF));
            currentInstAdr += 6;
        }
    }
}
void CppToGppParser::affectationFromAddress(unsigned int typesize, std::vector<std::string> params, unsigned int offset, unsigned int size, std::vector<std::string>::iterator it) {
    //Déplacement du contenu d'une variable dans une autre variable.
   for (unsigned int i = offset; i < size; i++) {
        for (unsigned int j = 0; j < typesize; j++) {
            //Déplacement des valeurs des octets aux adresses des octects à affecter dans la VRAM..
            instByteCode.push_back(EG_V_SQ_ALU_WORD1_OP2_SQ_OP2_INST_MOV);
            //type d'information = une adresse.
            instByteCode.push_back(C_SQ_CF_ALU_WORD0_ADDR(vars.find(params[0]).adr));
            //type d'information = un litéral.
            instByteCode.push_back(C_SQ_CF_ALU_WORD0_ADDR(vars.find(params[1]).adr));
            //Valeur de l'octet.
            instByteCode.push_back(vars.find(params[1]).val + i * typesize * 2 + j);
            currentInstAdr += 6;
        }
    }
}
void CppToGppParser::affectationPointer (std::vector<std::string> params, std::vector<std::string>::iterator it) {
    for (unsigned int i = 0; i < sizeof(uint32_t); i++) {
        //Déplacement des valeurs des octets aux adresses des octects à affecter dans la VRAM..
        instByteCode.push_back(EG_V_SQ_ALU_WORD1_OP2_SQ_OP2_INST_MOV);
        //type d'information = une adresse.
        instByteCode.push_back(C_SQ_CF_ALU_WORD0_ADDR(vars.find(params[0]).adr));
        //type d'information = un litéral.
        instByteCode.push_back(V_SQ_ALU_SRC_LITERAL);
        //Valeur de l'octet.
        instByteCode.push_back(C_SQ_CF_ALU_WORD0_ADDR(vars.find(params[1]).adr);
        currentInstAdr += 6;
    }
}
void CppToGppParser::createBloc(unsigned int size) {
    Bloc bloc;
    bloc.firstInstAdr = currentInstAdr - size;
    bloc.lastInstAdr = currentInstAdr;
    bloc.size = size;
    bloc.firstPageAdr = firstPageAdr;
    blocs.push_back(bloc);
    firstPageAdr += (i < PAGE_SIZE) ? size : PAGE_SIZE;
}
void CppToGppParser::parseFunction(std::vector<std::string> params, std::vector<std::pair<std::string, std::string>> args, std::vector<std::string>::iterator it) {
    //On copie le contenu des arguments de la fonction dans des variables temporaires.
    unsigned int blocSize = 0;
    std::vector<std::string> fParamsTypes = core::split(params[0], ",");
    //Stocke le nom, l'addresse et le type de la fonction.
    Function fct;
    fct.name = params[0];
    fct.type = params[1];
    fct.adr = currentPageAdr;
    functions.push_back(fct);
    //Allocation de la mémoire pour les paramètres de la fonction.
    for (unsigned int i = 0; i < fParamsTypes.size(); i++) {
        if (fParams[i].find("unsigned int")) {
            std::vector<std::string> farg {args[i].first, args[i].second};
            blocSize += 2 * sizeof(unsigned int);
            staticAllocateGPUMemory(sizeof(unsigned int), params, instByteCode.end(), true);
        }
    }
    //On parse le code source de la fonction.
    parseCppCode(blocSize);
    //Bloc d'instructions de la fonction.
    createBloc(blocSize);
}
