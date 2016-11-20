mark_as_advanced(SQLite3_LIBRARY SQLite3_INCLUDE_DIR) 

find_path(SQLite3_INCLUDE_DIR "sqlite3.h")

find_library(SQLite3_LIBRARY NAMES "sqlite3")

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SQLite3 DEFAULT_MSG SQLite3_LIBRARY SQLite3_INCLUDE_DIR)

