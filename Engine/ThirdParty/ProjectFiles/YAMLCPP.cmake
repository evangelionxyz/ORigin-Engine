add_library(YAMLCPP STATIC
    ${THIRD_PARTY_DIR}/yaml-cpp/include/yaml-cpp/anchor.h
    ${THIRD_PARTY_DIR}/yaml-cpp/include/yaml-cpp/binary.h
    ${THIRD_PARTY_DIR}/yaml-cpp/include/yaml-cpp/contrib/anchordict.h
    ${THIRD_PARTY_DIR}/yaml-cpp/include/yaml-cpp/contrib/graphbuilder.h
    ${THIRD_PARTY_DIR}/yaml-cpp/include/yaml-cpp/depthguard.h
    ${THIRD_PARTY_DIR}/yaml-cpp/include/yaml-cpp/dll.h
    ${THIRD_PARTY_DIR}/yaml-cpp/include/yaml-cpp/emitfromevents.h
    ${THIRD_PARTY_DIR}/yaml-cpp/include/yaml-cpp/emitter.h
    ${THIRD_PARTY_DIR}/yaml-cpp/include/yaml-cpp/emitterdef.h
    ${THIRD_PARTY_DIR}/yaml-cpp/include/yaml-cpp/emittermanip.h
    ${THIRD_PARTY_DIR}/yaml-cpp/include/yaml-cpp/emitterstyle.h
    ${THIRD_PARTY_DIR}/yaml-cpp/include/yaml-cpp/eventhandler.h
    ${THIRD_PARTY_DIR}/yaml-cpp/include/yaml-cpp/exceptions.h
    ${THIRD_PARTY_DIR}/yaml-cpp/include/yaml-cpp/mark.h
    ${THIRD_PARTY_DIR}/yaml-cpp/include/yaml-cpp/node/convert.h
    ${THIRD_PARTY_DIR}/yaml-cpp/include/yaml-cpp/node/detail/impl.h
    ${THIRD_PARTY_DIR}/yaml-cpp/include/yaml-cpp/node/detail/iterator.h
    ${THIRD_PARTY_DIR}/yaml-cpp/include/yaml-cpp/node/detail/iterator_fwd.h
    ${THIRD_PARTY_DIR}/yaml-cpp/include/yaml-cpp/node/detail/memory.h
    ${THIRD_PARTY_DIR}/yaml-cpp/include/yaml-cpp/node/detail/node.h
    ${THIRD_PARTY_DIR}/yaml-cpp/include/yaml-cpp/node/detail/node_data.h
    ${THIRD_PARTY_DIR}/yaml-cpp/include/yaml-cpp/node/detail/node_iterator.h
    ${THIRD_PARTY_DIR}/yaml-cpp/include/yaml-cpp/node/detail/node_ref.h
    ${THIRD_PARTY_DIR}/yaml-cpp/include/yaml-cpp/node/emit.h
    ${THIRD_PARTY_DIR}/yaml-cpp/include/yaml-cpp/node/impl.h
    ${THIRD_PARTY_DIR}/yaml-cpp/include/yaml-cpp/node/iterator.h
    ${THIRD_PARTY_DIR}/yaml-cpp/include/yaml-cpp/node/node.h
    ${THIRD_PARTY_DIR}/yaml-cpp/include/yaml-cpp/node/parse.h
    ${THIRD_PARTY_DIR}/yaml-cpp/include/yaml-cpp/node/ptr.h
    ${THIRD_PARTY_DIR}/yaml-cpp/include/yaml-cpp/node/type.h
    ${THIRD_PARTY_DIR}/yaml-cpp/include/yaml-cpp/noexcept.h
    ${THIRD_PARTY_DIR}/yaml-cpp/include/yaml-cpp/null.h
    ${THIRD_PARTY_DIR}/yaml-cpp/include/yaml-cpp/ostream_wrapper.h
    ${THIRD_PARTY_DIR}/yaml-cpp/include/yaml-cpp/parser.h
    ${THIRD_PARTY_DIR}/yaml-cpp/include/yaml-cpp/stlemitter.h
    ${THIRD_PARTY_DIR}/yaml-cpp/include/yaml-cpp/traits.h
    ${THIRD_PARTY_DIR}/yaml-cpp/include/yaml-cpp/yaml.h
    ${THIRD_PARTY_DIR}/yaml-cpp/src/binary.cpp
    ${THIRD_PARTY_DIR}/yaml-cpp/src/collectionstack.h
    ${THIRD_PARTY_DIR}/yaml-cpp/src/contrib/graphbuilder.cpp
    ${THIRD_PARTY_DIR}/yaml-cpp/src/contrib/graphbuilderadapter.cpp
    ${THIRD_PARTY_DIR}/yaml-cpp/src/contrib/graphbuilderadapter.h
    ${THIRD_PARTY_DIR}/yaml-cpp/src/convert.cpp
    ${THIRD_PARTY_DIR}/yaml-cpp/src/depthguard.cpp
    ${THIRD_PARTY_DIR}/yaml-cpp/src/directives.cpp
    ${THIRD_PARTY_DIR}/yaml-cpp/src/directives.h
    ${THIRD_PARTY_DIR}/yaml-cpp/src/emit.cpp
    ${THIRD_PARTY_DIR}/yaml-cpp/src/emitfromevents.cpp
    ${THIRD_PARTY_DIR}/yaml-cpp/src/emitter.cpp
    ${THIRD_PARTY_DIR}/yaml-cpp/src/emitterstate.cpp
    ${THIRD_PARTY_DIR}/yaml-cpp/src/emitterstate.h
    ${THIRD_PARTY_DIR}/yaml-cpp/src/emitterutils.cpp
    ${THIRD_PARTY_DIR}/yaml-cpp/src/emitterutils.h
    ${THIRD_PARTY_DIR}/yaml-cpp/src/exceptions.cpp
    ${THIRD_PARTY_DIR}/yaml-cpp/src/exp.cpp
    ${THIRD_PARTY_DIR}/yaml-cpp/src/exp.h
    ${THIRD_PARTY_DIR}/yaml-cpp/src/indentation.h
    ${THIRD_PARTY_DIR}/yaml-cpp/src/memory.cpp
    ${THIRD_PARTY_DIR}/yaml-cpp/src/node.cpp
    ${THIRD_PARTY_DIR}/yaml-cpp/src/node_data.cpp
    ${THIRD_PARTY_DIR}/yaml-cpp/src/nodebuilder.cpp
    ${THIRD_PARTY_DIR}/yaml-cpp/src/nodebuilder.h
    ${THIRD_PARTY_DIR}/yaml-cpp/src/nodeevents.cpp
    ${THIRD_PARTY_DIR}/yaml-cpp/src/nodeevents.h
    ${THIRD_PARTY_DIR}/yaml-cpp/src/null.cpp
    ${THIRD_PARTY_DIR}/yaml-cpp/src/ostream_wrapper.cpp
    ${THIRD_PARTY_DIR}/yaml-cpp/src/parse.cpp
    ${THIRD_PARTY_DIR}/yaml-cpp/src/parser.cpp
    ${THIRD_PARTY_DIR}/yaml-cpp/src/ptr_vector.h
    ${THIRD_PARTY_DIR}/yaml-cpp/src/regex_yaml.cpp
    ${THIRD_PARTY_DIR}/yaml-cpp/src/regex_yaml.h
    ${THIRD_PARTY_DIR}/yaml-cpp/src/regeximpl.h
    ${THIRD_PARTY_DIR}/yaml-cpp/src/scanner.cpp
    ${THIRD_PARTY_DIR}/yaml-cpp/src/scanner.h
    ${THIRD_PARTY_DIR}/yaml-cpp/src/scanscalar.cpp
    ${THIRD_PARTY_DIR}/yaml-cpp/src/scanscalar.h
    ${THIRD_PARTY_DIR}/yaml-cpp/src/scantag.cpp
    ${THIRD_PARTY_DIR}/yaml-cpp/src/scantag.h
    ${THIRD_PARTY_DIR}/yaml-cpp/src/scantoken.cpp
    ${THIRD_PARTY_DIR}/yaml-cpp/src/setting.h
    ${THIRD_PARTY_DIR}/yaml-cpp/src/simplekey.cpp
    ${THIRD_PARTY_DIR}/yaml-cpp/src/singledocparser.cpp
    ${THIRD_PARTY_DIR}/yaml-cpp/src/singledocparser.h
    ${THIRD_PARTY_DIR}/yaml-cpp/src/stream.cpp
    ${THIRD_PARTY_DIR}/yaml-cpp/src/stream.h
    ${THIRD_PARTY_DIR}/yaml-cpp/src/streamcharsource.h
    ${THIRD_PARTY_DIR}/yaml-cpp/src/stringsource.h
    ${THIRD_PARTY_DIR}/yaml-cpp/src/tag.cpp
    ${THIRD_PARTY_DIR}/yaml-cpp/src/tag.h
    ${THIRD_PARTY_DIR}/yaml-cpp/src/token.h
)

target_include_directories(YAMLCPP PRIVATE ${THIRD_PARTY_DIR}/yaml-cpp/include)

if (CMAKE_BUILD_TYPE STREQUAL Debug)
    set_target_properties(YAMLCPP PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${BUILD_DIR}/Debug/Binaries/ThirdParty
        LIBRARY_OUTPUT_DIRECTORY ${BUILD_DIR}/Debug/Binaries/ThirdParty
        RUNTIME_OUTPUT_DIRECTORY ${BUILD_DIR}/Debug/Binaries/ThirdParty
    )
elseif (CMAKE_BUILD_TYPE STREQUAL Release)
    set_target_properties(YAMLCPP PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${BUILD_DIR}/Release/Binaries/ThirdParty
        LIBRARY_OUTPUT_DIRECTORY ${BUILD_DIR}/Release/Binaries/ThirdParty
        RUNTIME_OUTPUT_DIRECTORY ${BUILD_DIR}/Release/Binaries/ThirdParty
    )
elseif (CMAKE_BUILD_TYPE STREQUAL Dist)
    set_target_properties(YAMLCPP PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${BUILD_DIR}/Dist/Binaries/ThirdParty
        LIBRARY_OUTPUT_DIRECTORY ${BUILD_DIR}/Dist/Binaries/ThirdParty
        RUNTIME_OUTPUT_DIRECTORY ${BUILD_DIR}/Dist/Binaries/ThirdParty
    )
endif()