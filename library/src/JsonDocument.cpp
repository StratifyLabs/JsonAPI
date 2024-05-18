// Copyright 2016-2021 Tyler Gilbert and Stratify Labs, Inc; see LICENSE.md

#if defined __link
#include "xml2json.hpp"
#endif

#include <fs/DataFile.hpp>
#include <printer/Printer.hpp>
#include <var/Deque.hpp>

#include "json/JsonDocument.hpp"

using namespace var;
using namespace json;
using namespace fs;
using namespace printer;


namespace {
int write_file_data(const char *buffer, size_t buflen, void *data) {
  reinterpret_cast<const fs::File *>(data)->write(View(buffer, buflen));
  if (api::ExecutionContext::return_value() != buflen) {
    return -1;
  }
  return 0;
}

size_t read_file_data(void *buffer, size_t buflen, void *data) {
  return reinterpret_cast<const fs::File *>(data)
    ->read(View(buffer, buflen))
    .return_value();
}

} // namespace

#if defined __link
JsonValue JsonDocument::from_xml_string(const char *xml, IsXmlFlat is_flat) {
  API_RETURN_VALUE_IF_ERROR(JsonValue());
#if !defined __android
  std::string json_string = xml2json(xml, is_flat == IsXmlFlat::no);
  return from_string(json_string.c_str());
#else
  return JsonValue();
#endif
}

JsonValue
JsonDocument::load_xml(const fs::FileObject &input, IsXmlFlat is_flat) {
  fs::DataFile data_file
    = fs::DataFile().reserve(input.size()).write(input).move();
  return from_xml_string(data_file.data().add_null_terminator(), is_flat);
}
#endif

JsonValue JsonDocument::from_string(const StringView json) {
  API_RETURN_VALUE_IF_ERROR(JsonValue());
  JsonValue value;
  value.m_value = API_SYSTEM_CALL_NULL(
    "",
    JsonValue::api()
      ->loadb(json.data(), json.length(), json_flags(), &m_error.m_value));
  return value;
}

var::String JsonDocument::to_string(const JsonValue &value) const {
  API_RETURN_VALUE_IF_ERROR(String());

  u32 size = API_SYSTEM_CALL(
    "",
    JsonValue::api()->dumpb(value.m_value, 0, 0, json_flags()));
  if (size == 0) {
    return var::String();
  }
  var::String result;
  result.resize(size);

  if (
    JsonValue::api()
      ->dumpb(value.m_value, result.to_char(), result.capacity(), json_flags())
    == 0) {
    return var::String();
  }
  return result;
}

JsonValue JsonDocument::load(const fs::FileObject &file) {
  API_RETURN_VALUE_IF_ERROR(JsonValue());
  JsonValue value;
  value.m_value = API_SYSTEM_CALL_NULL(
    "",
    JsonValue::api()->load_callback(
      read_file_data,
      (void *)&file,
      json_flags(),
      &m_error.m_value));
  return value;
}

JsonDocument &
JsonDocument::save(const JsonValue &value, const fs::FileObject &file) {
  API_RETURN_VALUE_IF_ERROR(*this);
  API_SYSTEM_CALL(
    "",
    JsonValue::api()->dump_callback(
      value.m_value,
      write_file_data,
      (void *)&file,
      json_flags()));
  return *this;
}

const JsonDocument &JsonDocument::seek(
  const var::StringView path,
  const fs::FileObject &file) const {

  /*
   * for
   * {
   *   "name": "config",
   *   "config": {
   *     "name": "JsonAPI"
   *   }
   * }
   *
   * use seek /config to seek to { "name": "JsonAPI" }
   * use seek /name to seek to "config"
   *
   * Use
   *
   */

  enum states {
    state_find_key, // waiting for " to close the key
    state_read_key, // waiting for " to close the key
    state_read_value,
    state_read_string_value,
    state_find_colon
  };

  class Container {
    API_AC(Container, KeyString, key);
    API_AF(Container, JsonValue::Type, type, JsonValue::Type::null);
    API_AF(Container, size_t, location, 0);
    API_AF(Container, size_t, array_index, 0);
  };

  API_ASSERT(path.at(0) == '/');

  Deque<Container> container_stack;
  const StringViewList token_list = path.split("/");
  using InputBuffer = Array<char, 256>;
  KeyString current_key = "<root>";
  InputBuffer input_buffer;

  const auto check_token_match = [](
                                   const Deque<Container> &container_stack,
                                   const StringViewList &token_list) -> bool {
    if (token_list.count() != container_stack.count()) {
      return false;
    }
    // ignore first value "<root>" for container, blank for token_list
    int i = 0;
    bool result = true;
    for (const auto &entry : container_stack) {
      if (i && (entry.key() != token_list.at(i))) {
        // return false;
        result = false;
      }
      i++;
    }
    return result;
  };

  const auto check_pop
    = [](enum states state, char c, Deque<Container> &container_stack) {
        if (c == '}' || c == ']') {
          if (container_stack.count()) {
            container_stack.pop_back();
            if (container_stack.count()) {
              if (container_stack.back().type() == JsonValue::Type::object) {
                state = state_find_key;
              } else {
                state = state_read_value;
              }
            }
          }
        }
        return state;
      };

  size_t array_index = 0;
  int read_result;
  enum states state = state_read_value;
  char last = 0;

  do {
    size_t location = file.location();
    size_t offset = 0;

    input_buffer.fill(0);
    read_result = file.read(input_buffer).return_value();

    for (auto c : input_buffer) {
      switch (state) {
      case state_read_value: {
        const enum states current_state = state;
        const size_t count = container_stack.count();
        if (c == '{') {
          if (
            container_stack.count()
            && container_stack.back().type() == JsonValue::Type::array) {
            current_key.format("[%d]", array_index);
          }

          container_stack.push_back(Container()
                                      .set_array_index(array_index)
                                      .set_key(current_key)
                                      .set_location(location + offset)
                                      .set_type(JsonValue::Type::object));
          current_key.clear();
          array_index = 0;
          state = state_find_key;
        } else if (c == '[') {

          container_stack.push_back(Container()
                                      .set_key(current_key)
                                      .set_location(location + offset)
                                      .set_type(JsonValue::Type::array));
          current_key.clear();
          state = state_read_value;
        }

        if (count != container_stack.count()) {
          if (check_token_match(container_stack, token_list)) {
            file.seek(location + offset);
            return *this;
          }
          break;
        }

        if (c == ',') {
          current_key.clear();

          if (container_stack.back().type() == JsonValue::Type::object) {
            state = state_find_key;
          } else {
            state = state_read_value;
            array_index++;
          }
          break;
        }

        if (c == '"') {
          state = state_read_string_value;
          break;
        }

        if (
          (current_state != (state = check_pop(state, c, container_stack)))
          && container_stack.count()) {
          array_index = container_stack.back().array_index();
        }

        break;
      }
      case state_read_key:

        if (last != '\\' && c == '"') {
          state = state_find_colon;
        } else {
          current_key.append(c);
        }

        break;
      case state_find_colon:
        if (c == ':') {
          state = state_read_value;
        }
        break;

      case state_find_key:
        if (c == '"') {
          state = state_read_key;
          break;
        }
        state = check_pop(state, c, container_stack);

        break;

      case state_read_string_value:
        if (last != '\\' && c == '"') {
          state = state_read_value;
        }
        break;
      }

      last = c;
      offset++;
    }

  } while (read_result > 0);

  return *this;
}

bool JsonDocument::is_valid(
  const fs::FileObject &file,
  printer::Printer *printer) {
  api::ErrorGuard error_guard;
  JsonDocument document;
  document.load(file);
  if (is_error() && printer) {
    printer->object("JsonDocumentError", document.error());
  }
  return is_success();
}
