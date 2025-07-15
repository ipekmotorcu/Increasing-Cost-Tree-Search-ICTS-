# CMakeList.txt: ICTS1 için CMake projesi, kaynağı ekleyin ve tanımlayın
# projeye özgü mantık burada bulunur.
#

# Kaynağı bu projenin yürütülebilir dosyasına ekleyin.
add_executable (ICTS1 "ICTS1.cpp" "ICTS1.h" "Agent.h" "Agent.cpp" "main.cpp"  "Position.h" "Grid.h" "Grid.cpp" "HighLevel.h" "HighLevel.cpp" "LowLevel.h" "LowLevel.cpp"  "ICTNode.h" "ICTNode.cpp" "id.cpp" "AStar.cpp" "AStar.h" "id.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ICTS1 PROPERTY CXX_STANDARD 20)
endif()

# TODO: Gerekirse testleri ve yükleme hedeflerini ekleyin.
