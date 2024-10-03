#include <RiscVM/Assembler.hpp>

RiscVM::Token& RiscVM::Assembler::Next()
{
    enum State
    {
        State_Skip,
        State_Comment,
        State_Symbol,
        State_Bin,
        State_Oct,
        State_Dec,
        State_Hex,
        State_Char,
    };

    State state = State_Skip;
    std::string value;

    while (m_C >= 0 || state != State_Skip)
    {
        switch (state)
        {
        case State_Skip:
            switch (m_C)
            {
            case ';':
                state = State_Comment;
                break;

            case '\'':
                state = State_Char;
                break;

            case '\n':
                m_C = Get();
                return m_Token = {.Type = TokenType_NewLine};

            case '.':
                m_C = Get();
                if (isalpha(m_C))
                {
                    state = State_Symbol;
                    value += '.';
                    continue;
                }
                return m_Token = {.Type = TokenType_Dot};

            case ',':
                m_C = Get();
                return m_Token = {.Type = TokenType_Comma};

            case '(':
                m_C = Get();
                return m_Token = {.Type = TokenType_ParenOpen};

            case ')':
                m_C = Get();
                return m_Token = {.Type = TokenType_ParenClose};

            case '-':
                m_C = Get();
                return m_Token = {.Type = TokenType_Minus};

            case '0':
                m_C = Get();
                switch (m_C)
                {
                case 'b':
                    state = State_Bin;
                    break;
                case 'x':
                    state = State_Hex;
                    break;
                default:
                    value += '0';
                    state = State_Oct;
                    continue;
                }
                break;

            default:
                if (isdigit(m_C))
                {
                    state = State_Dec;
                    continue;
                }
                if (isalpha(m_C) || m_C == '_')
                {
                    state = State_Symbol;
                    continue;
                }
                break;
            }
            break;

        case State_Comment:
            if (m_C == '\n' || m_C < 0)
            {
                state = State_Skip;
                continue;
            }
            break;

        case State_Symbol:
            if (isalnum(m_C) || m_C == '_')
            {
                value += static_cast<char>(m_C);
                break;
            }
            if (m_C == ':')
            {
                m_C = Get();
                return m_Token = {.Type = TokenType_Label, .Value = value};
            }
            return m_Token = {.Type = TokenType_Symbol, .Value = value};

        case State_Bin:
            {
                if (m_C == '0' || m_C == '1')
                {
                    value += static_cast<char>(m_C);
                    break;
                }
                const auto imm = static_cast<uint32_t>(std::stoul(value, nullptr, 2));
                return m_Token = {
                    .Type = TokenType_Immediate,
                    .Immediate = imm,
                };
            }

        case State_Oct:
            {
                if ('0' <= m_C && m_C <= '7')
                {
                    value += static_cast<char>(m_C);
                    break;
                }
                const auto imm = static_cast<uint32_t>(std::stoul(value, nullptr, 8));
                return m_Token = {
                    .Type = TokenType_Immediate,
                    .Immediate = imm,
                };
            }

        case State_Dec:
            {
                if (isdigit(m_C))
                {
                    value += static_cast<char>(m_C);
                    break;
                }
                const auto imm = static_cast<uint32_t>(std::stoul(value, nullptr, 10));
                return m_Token = {
                    .Type = TokenType_Immediate,
                    .Immediate = imm
                };
            }

        case State_Hex:
            {
                if (isxdigit(m_C))
                {
                    value += static_cast<char>(m_C);
                    break;
                }
                const auto imm = static_cast<uint32_t>(std::stoul(value, nullptr, 16));
                return m_Token = {
                    .Type = TokenType_Immediate,
                    .Immediate = imm,
                };
            }

        case State_Char:
            if (m_C == '\'' || m_C < 0)
            {
                m_C = Get();
                return m_Token = {.Type = TokenType_Char, .Value = value};
            }
            value += static_cast<char>(m_C);
            break;
        }

        m_C = Get();
    }

    return m_Token = {.Type = TokenType_EOF};
}
