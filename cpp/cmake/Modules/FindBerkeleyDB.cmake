

FIND_PATH(BerkeleyDB_INCLUDE_DIR db_cxx.h
        /usr/local/include/db4
        /usr/local/include
        /usr/include/db4
        /usr/include
        )


FIND_LIBRARY(BerkeleyDB_LIBRARY
        NAMES db
        PATHS /usr/lib /usr/local/lib
        )
FIND_LIBRARY(BerkeleyDB_CXX_LIBRARY
        NAMES db_cxx
        PATHS /usr/lib /usr/local/lib
        )
FIND_LIBRARY(BerkeleyDB_STL_LIBRARY
        NAMES db_stl
        PATHS /usr/lib /usr/local/lib
        )

IF (BerkeleyDB_LIBRARY AND BerkeleyDB_CXX_LIBRARY AND BerkeleyDB_STL_LIBRARY AND BerkeleyDB_INCLUDE_DIR)
    SET(BerkeleyDB_LIBRARIES ${BerkeleyDB_LIBRARY} ${BerkeleyDB_CXX_LIBRARY} ${BerkeleyDB_STL_LIBRARY})
    SET(DB_FOUND "YES")
ELSE (BerkeleyDB_LIBRARY AND BerkeleyDB_INCLUDE_DIR)
    SET(DB_FOUND "NO")
ENDIF (BerkeleyDB_LIBRARY AND BerkeleyDB_CXX_LIBRARY AND BerkeleyDB_STL_LIBRARY AND BerkeleyDB_INCLUDE_DIR)


IF (DB_FOUND)
    MESSAGE(STATUS "Found BerkeleyDB: ${BerkeleyDB_LIBRARIES} ${BerkeleyDB_INCLUDE_DIR}")
ELSE (DB_FOUND)
    MESSAGE("1: ${BerkeleyDB_LIBRARY} 2:${BerkeleyDB_CXX_LIBRARY} 3:${BerkeleyDB_STL_LIBRARY} 4:${BerkeleyDB_INCLUDE_DIR}")
    MESSAGE(FATAL_ERROR "Could not find BerkeleyDB library")
ENDIF (DB_FOUND)

# Deprecated declarations.
SET (NATIVE_DB_INCLUDE_PATH ${BerkeleyDB_INCLUDE_DIR} )
GET_FILENAME_COMPONENT (NATIVE_DB_LIB_PATH ${BerkeleyDB_LIBRARY} PATH)

MARK_AS_ADVANCED(
        BerkeleyDB_LIBRARY
        BerkeleyDB_INCLUDE_DIR
)