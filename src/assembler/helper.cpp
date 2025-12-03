#include <cstdint>
#include <string>
#include <sstream>

#include "../common_values.h"
#include "helper.h"

bool is_valid_atom(const Atom_Type atom_type, const std::string token) {
        bool first, second;
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
        case REGISTER:
                // general purpose registers only
                if (REGISTER_TABLE.find(token) == REGISTER_TABLE.end())
                        return false;
                return REGISTER_TABLE.at(token) < 8;
        case SOURCE:
                if (is_valid_atom(LITERAL_INT, token))
                        return true;
                if (REGISTER_TABLE.find(token) != REGISTER_TABLE.end())
                        return true;
                return is_valid_atom(STACK_OFFSET, token);
        case STACK_OFFSET:
                if (token.front() != '%')
                        return false;
                stripped_token = token.substr(1, token.length() - 1);
                return is_valid_i16(stripped_token);
        }
        return true;
}

bool is_valid_i16(const std::string token) {
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
        if (i == '$' || i == '%' || i == '.' || i == ':' || i == '_' || i == '-')
                return true;
        if (isalnum(i))
                return true;
        return false;
} 
