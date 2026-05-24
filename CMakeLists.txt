cmake_minimum_required(VERSION 3.11)
project(EchecsChaos)

set(CMAKE_CXX_STANDARD 17)

# --- 1. La nouvelle méthode propre pour trouver tes fichiers ---
# On fouille uniquement dans les dossiers Core et Graphics
file(GLOB_RECURSE CORE_SOURCES "Core/*.cpp" "Core/*.h")
file(GLOB_RECURSE GRAPHICS_SOURCES "Graphics/*.cpp" "Graphics/*.h")

# On regroupe tout avec ton main.cpp à la racine
set(SOURCES main.cpp ${CORE_SOURCES} ${GRAPHICS_SOURCES})

# --- 2. CRÉATION DE LA CIBLE ---
add_executable(EchecsChaos ${SOURCES})

# --- 3. INCLUDES ---
target_include_directories(EchecsChaos PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/Core/base 
    ${CMAKE_CURRENT_SOURCE_DIR}/Graphics
    ${CMAKE_CURRENT_SOURCE_DIR}/raylib/include 
)

# --- 4. LIBRARIES ---
target_link_directories(EchecsChaos PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/raylib/lib
)

# --- 5. LIAISON FINALE ---
target_link_libraries(EchecsChaos PRIVATE raylib opengl32 gdi32 winmm wininet)
# --- 6. RENDRE L'EXE AUTONOME (STATIC LINKING) ---
target_link_options(EchecsChaos PRIVATE -static-libgcc -static-libstdc++ -static)
