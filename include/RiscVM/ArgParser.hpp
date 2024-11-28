#pragma once

#include <map>
#include <string>
#include <vector>

namespace RiscVM
{
    struct Param
    {
        std::string Name;
        std::string Desc;
        std::vector<std::string> Aliases;
        bool IsFlag = true;
    };

    class ArgParser
    {
    public:
        explicit ArgParser(const std::vector<Param>& params);

        void Parse(int argc, const char* const* argv);
        const std::string& Get(const std::string&, const std::string& = {});
        void Print();

        std::string Filename;
        std::vector<std::string> Args;
        std::map<std::string, bool> Flags;
        std::map<std::string, std::string> Options;

    private:
        std::vector<Param> m_Params;
    };
}
