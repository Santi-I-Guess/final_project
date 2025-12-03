#include <cctype>
#include <cstddef>
#include <vector>
#include <string>

#include "../common_values.h"
#include "tokenizer.h"

std::vector<Token> create_tokens(const std::string source_buffer) {
        std::vector<Token> tokens = {};
        size_t buff_idx = 0;
        size_t buff_len = source_buffer.size();
        int line_num = 1;
        // line_num should increase only if it's in the same step that
        //      mutates buff_idx or token_idx
        while (buff_idx < buff_len) {
        next_lexeme:
                char curr = source_buffer[buff_idx];
                // increase buff_idx until not on whitespace
                while (buff_idx < buff_len) {
                        curr = source_buffer[buff_idx];
                        if (isspace(curr)) {
                                if (curr == '\n')
                                        line_num++;
                                buff_idx++;
                        } else {
                                break;
                        }
                }
                if (buff_idx == buff_len)
                        return tokens;
                // increase buff_idx until not on a comment
                if (curr == ';') {
                        while (buff_idx < buff_len) {
                                curr = source_buffer[buff_idx];
                                buff_idx++;
                                if (curr == '\n') {
                                        line_num++;
                                        break;
                                }
                        }
                        if (buff_idx == buff_len)
                                return tokens;
                        goto next_lexeme;
                }
                size_t token_len = 0;
                if (curr == '\"') {
                        // if token is a string
                        token_len++;
                        bool skip_quote = false;
                        while (((buff_idx + token_len) < buff_len)) {
                                curr = source_buffer[buff_idx + token_len];
                                if (curr == '\n')
                                        break;
                                token_len++;
                                if (curr == '\"' && !skip_quote)
                                        break;
                                skip_quote = false;
                                // to allow for escaped quotes
                                if (curr == '\\')
                                        skip_quote = true;
                        }
                        if (buff_idx == buff_len)
                                break;
                        Token curr_token;
                        curr_token.data = source_buffer.substr(buff_idx, token_len);
                        curr_token.type = T_STRING_LIT;
                        curr_token.line_num = line_num;
                        tokens.push_back(curr_token);
                        buff_idx += token_len;
                } else if (is_identifier_char(source_buffer[buff_idx])) {
                        // if token is normal
                        token_len++;
                        while (((buff_idx + token_len) < buff_len) && (curr != '\n')) {
                                curr = source_buffer[buff_idx + token_len];
                                if (is_identifier_char(curr))
                                        token_len++;
                                else
                                        break;
                        }
                        if (buff_idx == buff_len)
                                break;
                        Token curr_token;
                        curr_token.data = source_buffer.substr(buff_idx, token_len);
                        curr_token.type = T_MNEMONIC; // default type
                        curr_token.line_num = line_num;
                        for (size_t i = 0; i < curr_token.data.length(); ++i) {
                                if (!isupper(curr_token.data.at(i)))
                                        curr_token.type = T_LABEL_REF;
                        }
                        if (curr_token.data.front() == '$')
                                curr_token.type = T_INTEGER_LIT;
                        else if (curr_token.data.front() == '%')
                                curr_token.type = T_STACK_OFF;
                        else if (curr_token.data.back() == ':')
                                curr_token.type = T_LABEL_DEF;
                        else if (REGISTER_TABLE.find(curr_token.data) != REGISTER_TABLE.end())
                                curr_token.type = T_REGISTER;
                        else if (curr_token.type != T_MNEMONIC)
                                curr_token.type = T_LABEL_REF;
                        tokens.push_back(curr_token);
                        buff_idx += token_len;
                } else {
                        // skip undesired character, like commas
                        buff_idx++;
                }
        }
        return tokens;
};

bool is_identifier_char(const char i) {
        if (i == '$' || i == '%' || i == ':' || i == '_' || i == '-')
                return true;
        if (isalnum(i))
                return true;
        return false;
} 
