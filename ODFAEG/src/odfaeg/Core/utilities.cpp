#include "../../../include/odfaeg/Core/utilities.h"
namespace odfaeg {
    namespace core {
        using namespace std;
        using namespace sf;
        std::vector<std::string> split (const std::string &input, const std::string &separator) {
            std::vector<std::string> output;
            std::string::size_type prev_pos = 0, pos = 0;

            while((pos = input.find(separator, pos)) != std::string::npos)
            {
                std::string substring( input.substr(prev_pos, pos-prev_pos) );

                output.push_back(substring);

                prev_pos = ++pos;
            }

            output.push_back(input.substr(prev_pos, pos-prev_pos)); // Last word

            return output;
        }

        float conversionStringFloat(std::string str)
        {
            std::stringstream ss(str);
            float f;
            ss >> f;
            return f;
        }

        std::string conversionFloatString(float f)
        {
            std::stringstream ss;
            ss << f;
            return ss.str();
        }

        Int32 conversionStringInt(std::string str)
        {
            std::stringstream ss(str);
            Int32 i;
            ss >> i;
            return i;
        }

        std::string conversionIntString(Int32 i)
        {
            std::stringstream ss;
            ss << i;

            return ss.str();
        }
        Int64 conversionStringLong(std::string str)
        {
            std::stringstream ss(str);
            Int64 i;
            ss >> i;
            return i;
        }

        std::string conversionLongString(Int64 i)
        {
            std::stringstream ss;
            ss << i;

            return ss.str();
        }
    }
}
