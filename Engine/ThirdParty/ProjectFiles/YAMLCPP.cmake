add_library(YAMLCPP STATIC
    ${THIRD_PARTY_DIR}/YAMLCPP/include/yaml-cpp/anchor.h
    ${THIRD_PARTY_DIR}/YAMLCPP/include/yaml-cpp/binary.h
    ${THIRD_PARTY_DIR}/YAMLCPP/include/yaml-cpp/contrib/anchordict.h
    ${THIRD_PARTY_DIR}/YAMLCPP/include/yaml-cpp/contrib/graphbuilder.h
    ${THIRD_PARTY_DIR}/YAMLCPP/include/yaml-cpp/depthguard.h
    ${THIRD_PARTY_DIR}/YAMLCPP/include/yaml-cpp/dll.h
    ${THIRD_PARTY_DIR}/YAMLCPP/include/yaml-cpp/emitfromevents.h
    ${THIRD_PARTY_DIR}/YAMLCPP/include/yaml-cpp/emitter.h
    ${THIRD_PARTY_DIR}/YAMLCPP/include/yaml-cpp/emitterdef.h
    ${THIRD_PARTY_DIR}/YAMLCPP/include/yaml-cpp/emittermanip.h
    ${THIRD_PARTY_DIR}/YAMLCPP/include/yaml-cpp/emitterstyle.h
    ${THIRD_PARTY_DIR}/YAMLCPP/include/yaml-cpp/eventhandler.h
    ${THIRD_PARTY_DIR}/YAMLCPP/include/yaml-cpp/exceptions.h
    ${THIRD_PARTY_DIR}/YAMLCPP/include/yaml-cpp/mark.h
    ${THIRD_PARTY_DIR}/YAMLCPP/include/yaml-cpp/node/convert.h
    ${THIRD_PARTY_DIR}/YAMLCPP/include/yaml-cpp/node/detail/impl.h
    ${THIRD_PARTY_DIR}/YAMLCPP/include/yaml-cpp/node/detail/iterator.h
    ${THIRD_PARTY_DIR}/YAMLCPP/include/yaml-cpp/node/detail/iterator_fwd.h
    ${THIRD_PARTY_DIR}/YAMLCPP/include/yaml-cpp/node/detail/memory.h
    ${THIRD_PARTY_DIR}/YAMLCPP/include/yaml-cpp/node/detail/node.h
    ${THIRD_PARTY_DIR}/YAMLCPP/include/yaml-cpp/node/detail/node_data.h
    ${THIRD_PARTY_DIR}/YAMLCPP/include/yaml-cpp/node/detail/node_iterator.h
    ${THIRD_PARTY_DIR}/YAMLCPP/include/yaml-cpp/node/detail/node_ref.h
    ${THIRD_PARTY_DIR}/YAMLCPP/include/yaml-cpp/node/emit.h
    ${THIRD_PARTY_DIR}/YAMLCPP/include/yaml-cpp/node/impl.h
    ${THIRD_PARTY_DIR}/YAMLCPP/include/yaml-cpp/node/iterator.h
    ${THIRD_PARTY_DIR}/YAMLCPP/include/yaml-cpp/node/node.h
    ${THIRD_PARTY_DIR}/YAMLCPP/include/yaml-cpp/node/parse.h
    ${THIRD_PARTY_DIR}/YAMLCPP/include/yaml-cpp/node/ptr.h
    ${THIRD_PARTY_DIR}/YAMLCPP/include/yaml-cpp/node/type.h
    ${THIRD_PARTY_DIR}/YAMLCPP/include/yaml-cpp/noexcept.h
    ${THIRD_PARTY_DIR}/YAMLCPP/include/yaml-cpp/null.h
    ${THIRD_PARTY_DIR}/YAMLCPP/include/yaml-cpp/ostream_wrapper.h
    ${THIRD_PARTY_DIR}/YAMLCPP/include/yaml-cpp/parser.h
    ${THIRD_PARTY_DIR}/YAMLCPP/include/yaml-cpp/stlemitter.h
    ${THIRD_PARTY_DIR}/YAMLCPP/include/yaml-cpp/traits.h
    ${THIRD_PARTY_DIR}/YAMLCPP/include/yaml-cpp/yaml.h
    ${THIRD_PARTY_DIR}/YAMLCPP/src/binary.cpp
    ${THIRD_PARTY_DIR}/YAMLCPP/src/collectionstack.h
    ${THIRD_PARTY_DIR}/YAMLCPP/src/contrib/graphbuilder.cpp
    ${THIRD_PARTY_DIR}/YAMLCPP/src/contrib/graphbuilderadapter.cpp
    ${THIRD_PARTY_DIR}/YAMLCPP/src/contrib/graphbuilderadapter.h
    ${THIRD_PARTY_DIR}/YAMLCPP/src/convert.cpp
    ${THIRD_PARTY_DIR}/YAMLCPP/src/depthguard.cpp
    ${THIRD_PARTY_DIR}/YAMLCPP/src/directives.cpp
    ${THIRD_PARTY_DIR}/YAMLCPP/src/directives.h
    ${THIRD_PARTY_DIR}/YAMLCPP/src/emit.cpp
    ${THIRD_PARTY_DIR}/YAMLCPP/src/emitfromevents.cpp
    ${THIRD_PARTY_DIR}/YAMLCPP/src/emitter.cpp
    ${THIRD_PARTY_DIR}/YAMLCPP/src/emitterstate.cpp
    ${THIRD_PARTY_DIR}/YAMLCPP/src/emitterstate.h
    ${THIRD_PARTY_DIR}/YAMLCPP/src/emitterutils.cpp
    ${THIRD_PARTY_DIR}/YAMLCPP/src/emitterutils.h
    ${THIRD_PARTY_DIR}/YAMLCPP/src/exceptions.cpp
    ${THIRD_PARTY_DIR}/YAMLCPP/src/exp.cpp
    ${THIRD_PARTY_DIR}/YAMLCPP/src/exp.h
    ${THIRD_PARTY_DIR}/YAMLCPP/src/indentation.h
    ${THIRD_PARTY_DIR}/YAMLCPP/src/memory.cpp
    ${THIRD_PARTY_DIR}/YAMLCPP/src/node.cpp
    ${THIRD_PARTY_DIR}/YAMLCPP/src/node_data.cpp
    ${THIRD_PARTY_DIR}/YAMLCPP/src/nodebuilder.cpp
    ${THIRD_PARTY_DIR}/YAMLCPP/src/nodebuilder.h
    ${THIRD_PARTY_DIR}/YAMLCPP/src/nodeevents.cpp
    ${THIRD_PARTY_DIR}/YAMLCPP/src/nodeevents.h
    ${THIRD_PARTY_DIR}/YAMLCPP/src/null.cpp
    ${THIRD_PARTY_DIR}/YAMLCPP/src/ostream_wrapper.cpp
    ${THIRD_PARTY_DIR}/YAMLCPP/src/parse.cpp
    ${THIRD_PARTY_DIR}/YAMLCPP/src/parser.cpp
    ${THIRD_PARTY_DIR}/YAMLCPP/src/ptr_vector.h
    ${THIRD_PARTY_DIR}/YAMLCPP/src/regex_yaml.cpp
    ${THIRD_PARTY_DIR}/YAMLCPP/src/regex_yaml.h
    ${THIRD_PARTY_DIR}/YAMLCPP/src/regeximpl.h
    ${THIRD_PARTY_DIR}/YAMLCPP/src/scanner.cpp
    ${THIRD_PARTY_DIR}/YAMLCPP/src/scanner.h
    ${THIRD_PARTY_DIR}/YAMLCPP/src/scanscalar.cpp
    ${THIRD_PARTY_DIR}/YAMLCPP/src/scanscalar.h
    ${THIRD_PARTY_DIR}/YAMLCPP/src/scantag.cpp
    ${THIRD_PARTY_DIR}/YAMLCPP/src/scantag.h
    ${THIRD_PARTY_DIR}/YAMLCPP/src/scantoken.cpp
    ${THIRD_PARTY_DIR}/YAMLCPP/src/setting.h
    ${THIRD_PARTY_DIR}/YAMLCPP/src/simplekey.cpp
    ${THIRD_PARTY_DIR}/YAMLCPP/src/singledocparser.cpp
    ${THIRD_PARTY_DIR}/YAMLCPP/src/singledocparser.h
    ${THIRD_PARTY_DIR}/YAMLCPP/src/stream.cpp
    ${THIRD_PARTY_DIR}/YAMLCPP/src/stream.h
    ${THIRD_PARTY_DIR}/YAMLCPP/src/streamcharsource.h
    ${THIRD_PARTY_DIR}/YAMLCPP/src/stringsource.h
    ${THIRD_PARTY_DIR}/YAMLCPP/src/tag.cpp
    ${THIRD_PARTY_DIR}/YAMLCPP/src/tag.h
    ${THIRD_PARTY_DIR}/YAMLCPP/src/token.h
)

target_include_directories(YAMLCPP PRIVATE ${THIRD_PARTY_DIR}/YAMLCPP/include)

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