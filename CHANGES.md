# Version 1.3.0

## New Features

- Remove virtual functions from `json::JsonValue`
  - Make `create()` functions static
- Add `recursive` to `json::JsonObject::UpdateFlags`

## Bug Fixes

- Remove `json::JsonValue::to_value()` which returned `*this`
- Remove copy constructor and operator from `json::JsonObject` this was creating an extra reference and possibly leaking memory

# Version 1.2.0

## New Features

- Update `CMakeLists.txt` to require setup of SDK in a super-project
- Add an iterator for `JsonArray` to support range-for loops
- Add an iterator for `JsonObject` to support range-for loops
- Add `find()` to `JsonValue` to find a sub-value based on a path e.g "top/[4]/next"

## Bug Fixes

- None

# Version 1.1.0

## New Features

- Add method `native_value()` to `JsonValue` which can be used for object tracing and debugging.

## Bug Fixes

- Remove forward declaration of `Container` class which should not be there


# Version 1.0

Initial stable release with support for jansson 2.13.
