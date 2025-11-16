#include <cctype> // isalnum
#include <cstdint> // SIZE_MAX
#include <cstddef> // size_t
#include <deque>
#include <string>

#include "tokenizer.h"

// made newlines cause early termination of tokenize_buffer, which allows
//      for reliable "Missing arguments" for SPRINT without needing another
//      retval enum for tokenization

size_t get_token_len(size_t i, std::string source_buffer) {
        size_t token_len = 1, buff_len = source_buffer.length();
        bool is_quote_token = source_buffer[i] == '\"';
        bool escaped_quote = false;
        while (i + token_len < buff_len) {
                char curr = source_buffer[i + token_len];
                if (is_quote_token) {
                        if (curr == '\n')
                                return SIZE_MAX;
                        token_len++;
                        // ensure newlines and escaped quotes are tokenized
                        if (escaped_quote) {
                                // escaped_quote only lasts till the next char
                                escaped_quote = false;
                                if (curr == '\"')
                                        continue;
                        }
                        if (curr == '\\')
                                escaped_quote = true;
                        else if (curr == '\"')
                                break;
                } else {
                        if (is_identifier_char(curr))
                                token_len++;
                        else
                                break;
                }
        }
        if (i + token_len == buff_len)
                return SIZE_MAX; // basically just -1
        return token_len;
}

bool is_identifier_char(const char i) {
        // equivalent regex pattern: [$%|a-zA-Z0-9_]
        if (i == '$' || i == '%' || i == ':' || i == '_' || i == '-')
                return true;
        if (isalnum(i))
                return true;
        return false;
} 

void skip_comment(size_t &i, const std::string source_buffer) {
        size_t buff_len = source_buffer.length();
        while (i < buff_len) {
                if (source_buffer[i] != '\n')
                        i++;
                else
                        break;
        }
}

void skip_whitespace(size_t &i, const std::string source_buffer) {
        size_t buff_len = source_buffer.length();
        while (i < buff_len) {
                if (isspace(source_buffer[i]))
                        i++;
                else
                        break;
        }
}

std::deque<std::string> tokenize_buffer(const std::string source_buffer) {
        std::deque<std::string> tokens = {};
        size_t buff_idx = 0;
        size_t buff_len = source_buffer.length();
        while (buff_idx < buff_len) {
                skip_whitespace(buff_idx, source_buffer);
                if (buff_idx == buff_len)
                        return tokens;
                size_t token_len = 0;
                std::string curr_token = "";
                char first_char = source_buffer[buff_idx];
                switch (first_char) {
                case ';':
                        skip_comment(buff_idx, source_buffer);
                        if (buff_idx == buff_len)
                                return tokens;
                        break;
                default:
                        token_len = get_token_len(buff_idx, source_buffer);
                        // prevent unterminated quotes or symbosl that
                        // go to EOF from becoming tokens
                        if (token_len == SIZE_MAX)
                                return tokens;
                        curr_token = source_buffer.substr(buff_idx, token_len);
                        tokens.push_back(curr_token);
                        buff_idx += token_len;
                        break;
                }
                buff_idx++;
        }
        return tokens;
}
