// Copyright (c) Microsoft Corporation. All rights reserved.
// SPDX-License-Identifier: MIT

#include <az_json_state.h>

#include <stdio.h>

/*
static inline bool is_white_space(char c) {
  return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

static inline bool is_digit(char c) {
  return '0' <= c && c <= '9';
}

static az_json_value az_json_value_none = {
  .type = AZ_JSON_VALUE_NONE,
};

static az_json_value_parse_result az_json_value_parse_result_error = {
  .state = AZ_JSON_VALUE_ERROR,
  .value = AZ_JSON_VALUE_ERROR,
};

static inline az_json_value_parse_result az_json_value_parse_result_none(az_json_value_state state) {
  return (az_json_value_parse_result){
    .state = state,
    .value = az_json_value_none,
  };
}

static inline az_json_value_state az_json_none_parse(char c) {
  if (is_white_space(c)) {
    return (az_json_value_state){ .type = AZ_JSON_VALUE_NONE };
  }
  switch (c) {
    case '"':
      return (az_json_value_state){ .type = AZ_JSON_VALUE_STRING };
    case 'n':
      return (az_json_value_state){ .type = AZ_JSON_VALUE_NULL, .null_ = 0 };
    case 'f':
      return (az_json_value_state){ .type = AZ_JSON_VALUE_FALSE, .false_ = 0 };
    case 't':
      return (az_json_value_state){ .type = AZ_JSON_VALUE_FALSE, .true_ = 0 };
    case '{':
      return (az_json_value_state){ .type = AZ_JSON_VALUE_OBJECT };
    case '[':
      return (az_json_value_state){ .type = AZ_JSON_VALUE_ARRAY };
    default:
      return (az_json_value_state){ .type = AZ_JSON_VALUE_ERROR };
  }
}

static inline az_json_value_parse_result az_json_value_parse(az_json_value_state const state, char const c) {
  switch (state.type) {
    case AZ_JSON_VALUE_NONE:
      {
        if (is_white_space(c)) {
          return (az_json_value_parse_result){
            .state = { .type = AZ_JSON_VALUE_NONE },
            .value = az_json_value_none,
          };
        }
        switch (c) {
          case '"':
            return (az_json_value_parse_result) {
              .state = { .type = AZ_JSON_VALUE_STRING },
              .value = az_json_value_none,
            };
          case 'n':
            return (az_json_value_parse_result){
              .state = { .type = AZ_JSON_VALUE_NULL, .null_ = 0 },
              .value = az_json_value_none,
            };
          case 'f':
            return (az_json_value_parse_result){
              .state = { .type = AZ_JSON_VALUE_FALSE, .false_ = 0 },
              .value = az_json_value_none,
            };
          case 't':
            return (az_json_value_parse_result){
              .state = { .type = AZ_JSON_VALUE_FALSE, .true_ = 0 },
              .value = az_json_value_none,
            };
          case '{':
            return (az_json_value_parse_result){
              .state = { .type = AZ_JSON_VALUE_OBJECT },
              .value = az_json_value_none,
            };
          case '[':
            return (az_json_value_parse_result){
              .state = { .type = AZ_JSON_VALUE_ARRAY },
              .value = az_json_value_none,
            };
          default:
            return az_json_value_parse_result_error;
        }
      }
    case AZ_JSON_VALUE_ERROR:
    default:
      return az_json_value_parse_result_error;
  }
}

/*
static az_error read_keyword(az_cstr expected, az_cstr const input, size_t *const p_i) {
  size_t i = *p_i;
  if (input.len < i + expected.len) {
    return AZ_JSON_ERROR_UNEXPECTED_END;
  }
  for (size_t e = 0; e < expected.len; ++e, ++i) {
    if (expected.p[e] != input.p[i]) {
      *p_i = i;
      return AZ_JSON_ERROR_UNEXPECTED_SYMBOL;
    }
  }
  *p_i = i;
  return AZ_OK;
}

AZ_CSTR(ull, "ull");
AZ_CSTR(alse, "alse");
AZ_CSTR(rue, "rue");

// parsing number after the first digit and a sign.
static az_error az_json_parse_number(
  int sign,
  int first,
  az_cstr const s,
  size_t *const p_i,
  az_json_value *const out_value
) {
  out_value->type = AZ_JSON_VALUE_NUMBER;
  out_value->number = first;
  // TODO:
  return AZ_OK;
}

// parsing string after `"`.
static az_error az_json_parse_string(az_cstr const s, size_t *const p_i, az_json_value *const out_value) {
  size_t i = *p_i;
  out_value->type = AZ_JSON_VALUE_STRING;
  out_value->string.begin = i;
  // TODO: find a string end.
  while (true) {
    if (s.len <= i) {
      *p_i = i;
      return AZ_JSON_ERROR_UNEXPECTED_END;
    }
    char const c = s.p[i];
    ++i;
    if (c == '"') {
      break;
    }
    if (c < ' ') {
      *p_i = i;
      return AZ_JSON_ERROR_UNEXPECTED_SYMBOL;
    }
    if (c == '\\') {
      // TODO: handle escape.
    }
  }
  out_value->string.end = i - 1;
  *p_i = i;
  return AZ_OK;
}

az_error az_json_parse_value(az_cstr const s, size_t *const p_i, az_json_value *const out_value) {
  size_t i = *p_i;
  char c;

  // skip whitespace.
  do {
    if (s.len <= i) {
      *p_i = i;
      out_value->type = AZ_JSON_VALUE_NULL;
      return AZ_JSON_ERROR_UNEXPECTED_END;
    }
    c = s.p[i];
    ++i;
  } while (is_white_space(c));
  *p_i = i;

  if (is_digit(c)) {
    return az_json_parse_number(+1, c - '0', s, p_i, out_value);
  }
  switch (c) {
    case '{':
      out_value->type = AZ_JSON_VALUE_OBJECT;
      return AZ_OK;
    case '[':
      out_value->type = AZ_JSON_VALUE_ARRAY;
      return AZ_OK;
    case '-':
      // TODO: read the first number.
      return az_json_parse_number(-1, 0, s, p_i, out_value);
    case '"':
      return az_json_parse_string(s, p_i, out_value);
    case 'n':
      out_value->type = AZ_JSON_VALUE_NULL;
      return read_keyword(ull, s, p_i);
    case 'f':
      out_value->type = AZ_JSON_VALUE_FALSE;
      return read_keyword(alse, s, p_i);
    case 't':
      out_value->type = AZ_JSON_VALUE_TRUE;
      return read_keyword(rue, s, p_i);
  }

  out_value->type = AZ_JSON_VALUE_NULL;
  return AZ_JSON_ERROR_UNEXPECTED_SYMBOL;
}
*/