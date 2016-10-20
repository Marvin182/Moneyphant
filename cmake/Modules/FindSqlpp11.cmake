# Try to find Sqlpp11 library and it's sqlite3 connector
#
# This will define the following variables::
#
#   SQLPP11_FOUND        - True if the system has the library.
#   SQLPP11_INCLUDE_DIRS - The directories which include the headers.
#   SQLPP11_LIBRARIES    - The libraries which need to be linked.

# Includes
find_path(SQLPP11_INCLUDE_DIRS NAMES sqlpp11/sqlpp11.h)
# Sqlpp11 uses Howard Hinnant's (header-only) date library.
find_package(HinnantDate REQUIRED)
set(SQLPP11_INCLUDE_DIRS ${SQLPP11_INCLUDE_DIRS} ${HinnantDate_INCLUDE_DIR})

# Libraries
find_library(SQLPP11_LIBRARIES NAMES sqlpp11-connector-sqlite3)

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set SQLPP11_FOUND to TRUE if all listed variables are TRUE
find_package_handle_standard_args(Sqlpp11 DEFAULT_MSG SQLPP11_LIBRARIES SQLPP11_INCLUDE_DIRS)
mark_as_advanced(SQLPP11_INCLUDE_DIRS SQLPP11_LIBRARIES)