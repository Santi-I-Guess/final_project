#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <cstddef> // size_t
#include <deque>
#include <string>

// tokenize_buffer helper function
size_t get_token_len(size_t buffer_idx, const std::string source_buffer);

// tokenize_buffer helper function
bool is_identifier_char(const char i);

// tokenize_buffer helper function
void skip_comment(size_t &buffer_idx, const std::string source_buffer);

// tokenize_buffer helper function
void skip_whitespace(size_t &buffer_idx, const std::string source_buffer);

// deque instead of deque to make assembling easier
// creates tokens
std::deque<std::string> tokenize_buffer(const std::string source_buffer);

#endif
