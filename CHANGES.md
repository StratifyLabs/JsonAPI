# Version 1.1.0

## New Features

- Add an iterator for `JsonArray` to support range-for loops
- Add an iterator for `JsonObject` to support range-for loops
- Add `find()` to `JsonValue` to find a sub-value based on a path e.g "top/[4]/next"
- Add method `native_value()` to `JsonValue` which can be used for object tracing and debugging.

## Bug Fixes

- Remove forward declaration of `Container` class which should not be there


# Version 1.0

Initial stable release with support for jansson 2.13.
