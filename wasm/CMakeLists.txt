add_executable(datasketches_hll hll_bindings.cpp)
target_link_libraries(datasketches_hll PRIVATE hll)
target_compile_options(datasketches_hll PRIVATE -fexceptions)
target_link_options(datasketches_hll PRIVATE
  --bind
  -fexceptions
  -sMODULARIZE=1
  -sEXPORT_ES6=0
  -sENVIRONMENT=node
  -sALLOW_MEMORY_GROWTH=1
  -sWASM_BIGINT=1
  -sEXPORT_NAME=DataSketchesHllModule
)
set_target_properties(datasketches_hll PROPERTIES OUTPUT_NAME "datasketches_hll" SUFFIX ".js")

add_custom_command(
  TARGET datasketches_hll POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
          ${CMAKE_CURRENT_SOURCE_DIR}/datasketches_hll.d.ts
          $<TARGET_FILE_DIR:datasketches_hll>/datasketches_hll.d.ts
)
