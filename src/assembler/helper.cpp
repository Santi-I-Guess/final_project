#include <cstdint>
#include <string>
#include <sstream>

#include "../common_values.h"
#include "helper.h"

bool is_valid_atom(const Atom_Type atom_type, const std::string &token) {
        bool first, second;
        int16_t aux_value;
        std::string aux_string;
        std::stringstream aux_stream;
        std::string stripped_token;
        switch (atom_type) {
        case LABEL: /* checked earlier to avoid adding label_map as parameter */
                return true;
        case LITERAL_INT:
                if (token.front() != '$')
                        return false;
                stripped_token = token.substr(1, token.length() - 1);
                return is_valid_i16(stripped_token);
        case LITERAL_STR:
                first = token.front() == token.back();
                second = token.front() == '\"';
                return first && second;
        case MNEMONIC:
                return BLUEPRINTS.find(token) != BLUEPRINTS.end();
        case RAM_ADDR:
                stripped_token = token.substr(2, token.length() - 3); // remove [$]
                if (!is_valid_i16(stripped_token))
                        return false;
                aux_stream << stripped_token;
                aux_stream >> aux_value;
                return (aux_value >= 0) && (aux_value < STACK_START);
        case REGISTER:
                // general purpose registers only
                if (REGISTER_TABLE.find(token) == REGISTER_TABLE.end())
                        return false;
                return REGISTER_TABLE.at(token) < 10; // RZ, first 8 & RSP
        case SOURCE:
                if (token.front() == '$' && token.size() > 1)
                        return is_valid_atom(LITERAL_INT, token);
                if (token.front() == '[' && token.size() > 3)
                        return is_valid_atom(RAM_ADDR, token);
                if (token.front() == '%' && token.size() > 1)
                        return is_valid_atom(STACK_OFFSET, token);
                if (token.front() == 'R' || token.front() == 'C')
                        return REGISTER_TABLE.find(token) != REGISTER_TABLE.end();
                return false;
        case STACK_OFFSET:
                if (token.front() != '%')
                        return false;
                stripped_token = token.substr(1, token.length() - 1);
                return is_valid_i16(stripped_token);
        }
        return true;
}

bool is_valid_i16(const std::string &token) {
        std::stringstream the_stream(token);
        int32_t value = 0;
        the_stream >> value;
        if (the_stream.fail())
                return false;
        if (value > (int32_t)INT16_MAX)
                return false;
        if (value < (int32_t)INT16_MIN)
                return false;
        return true;
}

bool is_identifier_char(const char i) {
        if (i == '$' || i == '%' || i == '.'
                || i == ':' || i == '_' || i == '-'
                || i == '[' || i == ']')
                return true;
        if (isalnum(i))
                return true;
        return false;
} 
