#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <cstddef>
#include <deque>
#include <string>

/**
 * @brief tokenizes user assembly program into tokens
 * @brief invalid tokens won't signal an error immediately, but will cause
 * MISSING_ARGUMENTS or INVALID_ATOM down the line
 */
std::deque<std::string> create_tokens(const std::string source_buffer);

/**
 * @brief finds the size of the next string
 * @details helper function of get_token_len. If a literal string is not
 * terminated before a newline, the token won't be added at all, which
 * allows a reliable "MISSING_ARGUMENTS" for SPRINT
 */
size_t get_string_token_length(const size_t i, const std::string source_buffer);

/**
 * @brief finds the size of the next token to extract
 * @details helper function for create_tokens. Will return SIZE_MAX if
 * it hits an invalid token, but won't signal an error immediatly.
 */
size_t get_token_len(const size_t i, const std::string source_buffer);

/**
 * @brief checks if i matches equivalent regex pattern: [$%|a-zA-Z0-9_-]
 * @details create_tokens helper function
 */
bool is_identifier_char(const char i);

/**
 * @brief moves buffer_idx in create_tokens past comments
 * @details create_tokens helper function
 */
void skip_comment(size_t &buffer_idx, const std::string source_buffer);

/**
 * @brief moves buffer_idx in create_tokens past whitespace
 * @details create_tokens helper function
 */
void skip_whitespace(size_t &buffer_idx, const std::string source_buffer);

#endif
