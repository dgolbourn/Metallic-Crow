cmake_minimum_required (VERSION 3.15)

project (Metallic-Crow)

set_property (GLOBAL PROPERTY USE_FOLDERS ON)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4459 /wd4456 /wd4457")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4996")

find_package(jansson REQUIRED)
find_package(SDL2 REQUIRED)	
find_package(SDL2_ttf REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(ffmpeg REQUIRED)	
find_package(box2d REQUIRED)
find_package(lua REQUIRED)
find_package(boost REQUIRED)

include_directories ("${PROJECT_SOURCE_DIR}/src/audio")

set (AUDIO_SOURCES
    "${PROJECT_SOURCE_DIR}/src/audio/mix_exception.cc"
    "${PROJECT_SOURCE_DIR}/src/audio/mix_exception.h"
    "${PROJECT_SOURCE_DIR}/src/audio/mix_library.cc"
    "${PROJECT_SOURCE_DIR}/src/audio/mix_library.h"
    "${PROJECT_SOURCE_DIR}/src/audio/mix_thread.cc"
    "${PROJECT_SOURCE_DIR}/src/audio/mix_thread.h"
    "${PROJECT_SOURCE_DIR}/src/audio/music.cc"
    "${PROJECT_SOURCE_DIR}/src/audio/music.h"
    "${PROJECT_SOURCE_DIR}/src/audio/sound.cc"
    "${PROJECT_SOURCE_DIR}/src/audio/sound.h")

source_group ("Source Files\\audio" FILES ${AUDIO_SOURCES})

include_directories ("${PROJECT_SOURCE_DIR}/src/configuration")

set (CONFIGURATION_SOURCES
    "${PROJECT_SOURCE_DIR}/src/configuration/lua_exception.h"
    "${PROJECT_SOURCE_DIR}/src/configuration/lua_stack.cc"
    "${PROJECT_SOURCE_DIR}/src/configuration/lua_stack.h"
    "${PROJECT_SOURCE_DIR}/src/configuration/lua_stack_impl.h"
    "${PROJECT_SOURCE_DIR}/src/configuration/lua_command.cc"
    "${PROJECT_SOURCE_DIR}/src/configuration/lua_command.h"
	"${PROJECT_SOURCE_DIR}/src/configuration/lua_data.h"
    "${PROJECT_SOURCE_DIR}/src/configuration/lua_guard.cc"
    "${PROJECT_SOURCE_DIR}/src/configuration/lua_guard.h"
    "${PROJECT_SOURCE_DIR}/src/configuration/parse.cc"
    "${PROJECT_SOURCE_DIR}/src/configuration/parse.h"
    "${PROJECT_SOURCE_DIR}/src/configuration/version.cc"
    "${PROJECT_SOURCE_DIR}/src/configuration/version.h")

source_group ("Source Files\\configuration" FILES ${CONFIGURATION_SOURCES})

include_directories ("${PROJECT_SOURCE_DIR}/src/controller")

set (CONTROLLER_SOURCES
    "${PROJECT_SOURCE_DIR}/src/controller/controller.cc"
    "${PROJECT_SOURCE_DIR}/src/controller/controller.h"
    "${PROJECT_SOURCE_DIR}/src/controller/menu.h"
    "${PROJECT_SOURCE_DIR}/src/controller/menu.cc"
    "${PROJECT_SOURCE_DIR}/src/controller/player.h"
    "${PROJECT_SOURCE_DIR}/src/controller/player.cc"
    "${PROJECT_SOURCE_DIR}/src/controller/saves.h"
    "${PROJECT_SOURCE_DIR}/src/controller/saves.cc")

source_group ("Source Files\\controller" FILES ${CONTROLLER_SOURCES})

include_directories ("${PROJECT_SOURCE_DIR}/src/display")

set (DISPLAY_SOURCES
    "${PROJECT_SOURCE_DIR}/src/display/animation.cc"
    "${PROJECT_SOURCE_DIR}/src/display/animation.h"
    "${PROJECT_SOURCE_DIR}/src/display/bounding_box.cc"
    "${PROJECT_SOURCE_DIR}/src/display/bounding_box.h"
    "${PROJECT_SOURCE_DIR}/src/display/colour.cc"
    "${PROJECT_SOURCE_DIR}/src/display/colour.h"
    "${PROJECT_SOURCE_DIR}/src/display/flood_fill.h"
    "${PROJECT_SOURCE_DIR}/src/display/font.cc"
    "${PROJECT_SOURCE_DIR}/src/display/font.h"
    "${PROJECT_SOURCE_DIR}/src/display/font_impl.h"
    "${PROJECT_SOURCE_DIR}/src/display/img_exception.cc"
    "${PROJECT_SOURCE_DIR}/src/display/img_exception.h"
    "${PROJECT_SOURCE_DIR}/src/display/img_library.cc"
    "${PROJECT_SOURCE_DIR}/src/display/img_library.h"
    "${PROJECT_SOURCE_DIR}/src/display/modulation.cc"
    "${PROJECT_SOURCE_DIR}/src/display/modulation.h"
    "${PROJECT_SOURCE_DIR}/src/display/painter.cc"
    "${PROJECT_SOURCE_DIR}/src/display/painter.h"
    "${PROJECT_SOURCE_DIR}/src/display/rect.cc"
    "${PROJECT_SOURCE_DIR}/src/display/rect.h"
    "${PROJECT_SOURCE_DIR}/src/display/render.cc"
    "${PROJECT_SOURCE_DIR}/src/display/render.h"
    "${PROJECT_SOURCE_DIR}/src/display/sdl_exception.cc"
    "${PROJECT_SOURCE_DIR}/src/display/sdl_exception.h"
    "${PROJECT_SOURCE_DIR}/src/display/sdl_library.cc"
    "${PROJECT_SOURCE_DIR}/src/display/sdl_library.h"
    "${PROJECT_SOURCE_DIR}/src/display/shape.h"
    "${PROJECT_SOURCE_DIR}/src/display/surface.cc"
    "${PROJECT_SOURCE_DIR}/src/display/surface.h"
    "${PROJECT_SOURCE_DIR}/src/display/texture.cc"
    "${PROJECT_SOURCE_DIR}/src/display/texture.h"
    "${PROJECT_SOURCE_DIR}/src/display/ttf_exception.cc"
    "${PROJECT_SOURCE_DIR}/src/display/ttf_exception.h"
    "${PROJECT_SOURCE_DIR}/src/display/ttf_library.cc"
    "${PROJECT_SOURCE_DIR}/src/display/ttf_library.h"
    "${PROJECT_SOURCE_DIR}/src/display/window.cc"
    "${PROJECT_SOURCE_DIR}/src/display/window.h"
    "${PROJECT_SOURCE_DIR}/src/display/window_impl.h")

source_group ("Source Files\\display" FILES ${DISPLAY_SOURCES})

include_directories ("${PROJECT_SOURCE_DIR}/src/dynamics")

set (DYNAMICS_SOURCES
    "${PROJECT_SOURCE_DIR}/src/dynamics/body.cc"
    "${PROJECT_SOURCE_DIR}/src/dynamics/body.h"
    "${PROJECT_SOURCE_DIR}/src/dynamics/body_impl.h"
    "${PROJECT_SOURCE_DIR}/src/dynamics/body_impl_iterator.cc"
    "${PROJECT_SOURCE_DIR}/src/dynamics/body_impl_iterator.h"
    "${PROJECT_SOURCE_DIR}/src/dynamics/body_impl_pair.cc"
    "${PROJECT_SOURCE_DIR}/src/dynamics/body_impl.h"
    "${PROJECT_SOURCE_DIR}/src/dynamics/box_search.cc"
    "${PROJECT_SOURCE_DIR}/src/dynamics/box_search.h"
    "${PROJECT_SOURCE_DIR}/src/dynamics/collision_group.cc"
    "${PROJECT_SOURCE_DIR}/src/dynamics/collision_group.h"
    "${PROJECT_SOURCE_DIR}/src/dynamics/joint.cc"
    "${PROJECT_SOURCE_DIR}/src/dynamics/joint.h"
    "${PROJECT_SOURCE_DIR}/src/dynamics/light.cc"
    "${PROJECT_SOURCE_DIR}/src/dynamics/light.h"
    "${PROJECT_SOURCE_DIR}/src/dynamics/ray_search.cc"
    "${PROJECT_SOURCE_DIR}/src/dynamics/ray_search.h"
    "${PROJECT_SOURCE_DIR}/src/dynamics/world.cc"
    "${PROJECT_SOURCE_DIR}/src/dynamics/world.h"
    "${PROJECT_SOURCE_DIR}/src/dynamics/world_impl.h")

source_group ("Source Files\\dynamics" FILES ${DYNAMICS_SOURCES})

include_directories ("${PROJECT_SOURCE_DIR}/src/event")

set (EVENT_SOURCES
    "${PROJECT_SOURCE_DIR}/src/event/command.h"
    "${PROJECT_SOURCE_DIR}/src/event/event.cc"
    "${PROJECT_SOURCE_DIR}/src/event/event.h"
	"${PROJECT_SOURCE_DIR}/src/event/for_each.h"
    "${PROJECT_SOURCE_DIR}/src/event/queue.cc"
    "${PROJECT_SOURCE_DIR}/src/event/queue.h"
    "${PROJECT_SOURCE_DIR}/src/event/signal.cc"
    "${PROJECT_SOURCE_DIR}/src/event/signal.h"
    "${PROJECT_SOURCE_DIR}/src/event/timer.cc"
    "${PROJECT_SOURCE_DIR}/src/event/timer.h")

source_group ("Source Files\\event" FILES ${EVENT_SOURCES})

include_directories ("${PROJECT_SOURCE_DIR}/src/exception")

set (EXCEPTION_SOURCES
    "${PROJECT_SOURCE_DIR}/src/exception/exception.h"
    "${PROJECT_SOURCE_DIR}/src/exception/log.cc"
    "${PROJECT_SOURCE_DIR}/src/exception/log.h")

source_group ("Source Files\\exception" FILES ${EXCEPTION_SOURCES})

include_directories ("${PROJECT_SOURCE_DIR}/src/function")

set (FUNCTION_SOURCES
    "${PROJECT_SOURCE_DIR}/src/function/bind.h")

source_group ("Source Files\\function" FILES ${FUNCTION_SOURCES})

include_directories ("${PROJECT_SOURCE_DIR}/src/game")

set (GAME_SOURCES
    "${PROJECT_SOURCE_DIR}/src/game/actor.cc"
    "${PROJECT_SOURCE_DIR}/src/game/actor.h"
    "${PROJECT_SOURCE_DIR}/src/game/actor_impl.h"
    "${PROJECT_SOURCE_DIR}/src/game/choice.cc"
    "${PROJECT_SOURCE_DIR}/src/game/choice.h"
    "${PROJECT_SOURCE_DIR}/src/game/fade.cc"
    "${PROJECT_SOURCE_DIR}/src/game/fade.h"
    "${PROJECT_SOURCE_DIR}/src/game/feature.cc"
    "${PROJECT_SOURCE_DIR}/src/game/feature.h"
    "${PROJECT_SOURCE_DIR}/src/game/game_body.cc"
    "${PROJECT_SOURCE_DIR}/src/game/game_body.h"
    "${PROJECT_SOURCE_DIR}/src/game/game_joint.cc"
    "${PROJECT_SOURCE_DIR}/src/game/game_joint.h"
    "${PROJECT_SOURCE_DIR}/src/game/position.h"
    "${PROJECT_SOURCE_DIR}/src/game/subtitle.cc"
    "${PROJECT_SOURCE_DIR}/src/game/subtitle.h")

source_group ("Source Files\\game" FILES ${GAME_SOURCES})

include_directories ("${PROJECT_SOURCE_DIR}/src/memory")

set (MEMORY_SOURCES
    "${PROJECT_SOURCE_DIR}/src/memory/weak_ptr.h")

source_group ("Source Files\\memory" FILES ${MEMORY_SOURCES})

include_directories ("${PROJECT_SOURCE_DIR}/src/script")

set (SCRIPT_SOURCES
    "${PROJECT_SOURCE_DIR}/src/script/script.cc"
    "${PROJECT_SOURCE_DIR}/src/script/script.h"
    "${PROJECT_SOURCE_DIR}/src/script/script_actor.cc"
    "${PROJECT_SOURCE_DIR}/src/script/script_audio.cc"
    "${PROJECT_SOURCE_DIR}/src/script/script_choice.cc"
    "${PROJECT_SOURCE_DIR}/src/script/script_collision.cc"
    "${PROJECT_SOURCE_DIR}/src/script/script_fade.cc"
    "${PROJECT_SOURCE_DIR}/src/script/script_impl.h"
    "${PROJECT_SOURCE_DIR}/src/script/script_joint.cc"
    "${PROJECT_SOURCE_DIR}/src/script/script_stage.cc"
    "${PROJECT_SOURCE_DIR}/src/script/script_subtitle.cc"
    "${PROJECT_SOURCE_DIR}/src/script/script_timer.cc"
    "${PROJECT_SOURCE_DIR}/src/script/script_view.cc")

source_group ("Source Files\\script" FILES ${SCRIPT_SOURCES})

include_directories ("${PROJECT_SOURCE_DIR}/src/time")

set (TIME_SOURCES
    "${PROJECT_SOURCE_DIR}/src/time/now.cc"
    "${PROJECT_SOURCE_DIR}/src/time/now.h")

source_group ("Source Files\\time" FILES ${TIME_SOURCES})

add_executable (metallic-crow 
                "${PROJECT_SOURCE_DIR}/src/main.cc"
                ${AUDIO_SOURCES}
                ${CONFIGURATION_SOURCES}
                ${CONTROLLER_SOURCES}
                ${DISPLAY_SOURCES}
                ${DYNAMICS_SOURCES}
                ${EVENT_SOURCES}
                ${EXCEPTION_SOURCES}
                ${FILESYSTEM_SOURCES}
                ${FUNCTION_SOURCES}
                ${GAME_SOURCES}
                ${MEMORY_SOURCES}
                ${SCRIPT_SOURCES}
                ${TIME_SOURCES})

include_directories ("${SDL2_INCLUDE_DIRS}")
target_link_libraries(metallic-crow SDL2)
target_link_libraries(metallic-crow SDL2-image zlib libpng)
target_link_libraries(metallic-crow SDL2-ttf freetype)
target_link_libraries(metallic-crow SDL2-mixer libogg libvorbis libvorbisfile)
target_link_libraries(metallic-crow Box2D)
target_link_libraries(metallic-crow lua)
target_link_libraries(metallic-crow boost-filesystem)
target_link_libraries(metallic-crow boost-program-options)
target_link_libraries(metallic-crow boost-system)
