#include <iostream>
#include <RiscVM/ArgParser.hpp>

RiscVM::ArgParser::ArgParser(const std::vector<Param>& params)
    : m_Params(params)
{
}

void RiscVM::ArgParser::Parse(const int argc, const char* const* argv)
{
    Filename = argv[0];

    Args.clear();
    Flags.clear();
    Options.clear();

    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];
        bool flag_or_option = false;
        for (const auto& [name, desc, aliases, is_flag] : m_Params)
        {
            size_t j;
            for (j = 0; j < aliases.size(); ++j)
                if (arg == aliases[j])
                    break;
            if (j == aliases.size())
                continue;

            flag_or_option = true;
            if (is_flag)
            {
                Flags[name] = true;
                break;
            }

            Options[name] = argv[++i];
            break;
        }

        if (flag_or_option)
            continue;

        Args.push_back(arg);
    }
}

const std::string& RiscVM::ArgParser::Get(const std::string& name, const std::string& def)
{
    if (!Options.contains(name))
        return def;
    return Options[name];
}

void RiscVM::ArgParser::Print()
{
    std::cout << "RiscVM" << std::endl;
    std::cout << Filename << " [OPTIONS|FLAGS] <ARGS>..." << std::endl;

    int w0 = 0, w1 = 0, w2 = 0;
    for (const auto& [name, desc, aliases, is_flag] : m_Params)
    {
        if (is_flag)
            continue;

        std::string x_alias;
        for (const auto& alias : aliases)
            x_alias += alias + ", ";
        x_alias.erase(x_alias.size() - 2);

        w0 = std::max(w0, (int)name.size());
        w1 = std::max(w1, (int)x_alias.size());
        w2 = std::max(w2, (int)desc.size());
    }

    for (const auto& [name, desc, aliases, is_flag] : m_Params)
    {
        if (!is_flag)
            continue;

        std::string x_alias;
        for (const auto& alias : aliases)
            x_alias += alias + ", ";
        x_alias.erase(x_alias.size() - 2);

        w0 = std::max(w0, (int)name.size());
        w1 = std::max(w1, (int)x_alias.size());
        w2 = std::max(w2, (int)desc.size());
    }

    std::cout << "OPTIONS" << std::endl;
    for (const auto& [name, desc, aliases, is_flag] : m_Params)
    {
        if (is_flag)
            continue;

        std::string x_alias;
        for (const auto& alias : aliases)
            x_alias += alias + ", ";
        x_alias.erase(x_alias.size() - 2);

        fprintf(stdout, "\t%-*s %-*s %*s\n", w0, name.c_str(), w1, x_alias.c_str(), w2, desc.c_str());
    }
    std::cout << "FLAGS" << std::endl;
    for (const auto& [name, desc, aliases, is_flag] : m_Params)
    {
        if (!is_flag)
            continue;

        std::string x_alias;
        for (const auto& alias : aliases)
            x_alias += alias + ", ";
        x_alias.erase(x_alias.size() - 2);

        fprintf(stdout, "\t%-*s %-*s %*s\n", w0, name.c_str(), w1, x_alias.c_str(), w2, desc.c_str());
    }
    fflush(stdout);
}
