
aux_source_directory(. ALL_SRC)
# remove the lua.c/luac.c from the all file lists
SET(LIB_SRC ${ALL_SRC})
LIST(REMOVE_ITEM LIB_SRC lua.c luac.c)
add_library(lua ${LIB_SRC})

target_include_directories(lua PUBLIC .)
