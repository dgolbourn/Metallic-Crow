cmake_minimum_required (VERSION 2.6)

project (Metallic-Crow)

set_property (GLOBAL PROPERTY USE_FOLDERS ON)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")

set (CMAKE_GENERATOR_TOOLSET "CTP_Nov2013" CACHE STRING "Platform Toolset" FORCE)

set (CMAKE_VS_PLATFORM_TOOLSET "CTP_Nov2013" CACHE STRING "Platform Toolset" FORCE)

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

include_directories ("${PROJECT_SOURCE_DIR}/src/c")

set (C_SOURCES
	"${PROJECT_SOURCE_DIR}/src/c/cstd_exception.h"
	"${PROJECT_SOURCE_DIR}/src/c/va_list.h")

source_group ("Source Files\\c" FILES ${C_SOURCES})

include_directories ("${PROJECT_SOURCE_DIR}/src/configuration")

set (CONFIGURATION_SOURCES
	"${PROJECT_SOURCE_DIR}/src/configuration/json.cc"
	"${PROJECT_SOURCE_DIR}/src/configuration/json.h"
	"${PROJECT_SOURCE_DIR}/src/configuration/json_exception.h"
	"${PROJECT_SOURCE_DIR}/src/configuration/json_iterator.cc"
	"${PROJECT_SOURCE_DIR}/src/configuration/json_iterator.h"
	"${PROJECT_SOURCE_DIR}/src/configuration/lua_exception.h"
	"${PROJECT_SOURCE_DIR}/src/configuration/lua_stack.cc"
	"${PROJECT_SOURCE_DIR}/src/configuration/lua_stack.h"
	"${PROJECT_SOURCE_DIR}/src/configuration/lua_stack_impl.h"
	"${PROJECT_SOURCE_DIR}/src/configuration/lua_command.cc"
	"${PROJECT_SOURCE_DIR}/src/configuration/lua_command.h"
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
	"${PROJECT_SOURCE_DIR}/src/display/sdl_texture.cc"
	"${PROJECT_SOURCE_DIR}/src/display/sdl_texture.h"
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
	"${PROJECT_SOURCE_DIR}/src/dynamics/collision.cc"
	"${PROJECT_SOURCE_DIR}/src/dynamics/collision.h"
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
	"${PROJECT_SOURCE_DIR}/src/event/command.cc"
	"${PROJECT_SOURCE_DIR}/src/event/command.h"
	"${PROJECT_SOURCE_DIR}/src/event/event.cc"
	"${PROJECT_SOURCE_DIR}/src/event/event.h"
	"${PROJECT_SOURCE_DIR}/src/event/queue.cc"
	"${PROJECT_SOURCE_DIR}/src/event/queue.h"
	"${PROJECT_SOURCE_DIR}/src/event/signal.cc"
	"${PROJECT_SOURCE_DIR}/src/event/signal.h"
	"${PROJECT_SOURCE_DIR}/src/event/switch.h"
	"${PROJECT_SOURCE_DIR}/src/event/sync.cc"
	"${PROJECT_SOURCE_DIR}/src/event/sync.h"
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
	"${PROJECT_SOURCE_DIR}/src/game/make_body.cc"
	"${PROJECT_SOURCE_DIR}/src/game/make_body.h"
	"${PROJECT_SOURCE_DIR}/src/game/position.h"
	"${PROJECT_SOURCE_DIR}/src/game/scene.cc"
	"${PROJECT_SOURCE_DIR}/src/game/scene.h"
	"${PROJECT_SOURCE_DIR}/src/game/scenery.cc"
	"${PROJECT_SOURCE_DIR}/src/game/scenery.h"
	"${PROJECT_SOURCE_DIR}/src/game/screen.cc"
	"${PROJECT_SOURCE_DIR}/src/game/screen.h"
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
	"${PROJECT_SOURCE_DIR}/src/script/script_resource.cc"
	"${PROJECT_SOURCE_DIR}/src/script/script_scenery.cc"
	"${PROJECT_SOURCE_DIR}/src/script/script_screen.cc"
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
				${C_SOURCES}
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

include_directories ("${PROJECT_SOURCE_DIR}/lib/SDL2/include")
include_directories ("${PROJECT_SOURCE_DIR}/lib/SDL2/src/render")
add_library(SDL2 SHARED IMPORTED)
set_property(TARGET SDL2 PROPERTY IMPORTED_LOCATION "${PROJECT_SOURCE_DIR}/bin/shared/SDL2.dll")
set_property(TARGET SDL2 PROPERTY IMPORTED_IMPLIB "${PROJECT_SOURCE_DIR}/bin/shared/SDL2.lib")
target_link_libraries(metallic-crow SDL2)

include_directories ("${PROJECT_SOURCE_DIR}/lib/SDL2_image")
add_library(SDL2-image SHARED IMPORTED)
set_property(TARGET SDL2-image PROPERTY IMPORTED_LOCATION "${PROJECT_SOURCE_DIR}/bin/shared/SDL2_image.dll")
set_property(TARGET SDL2-image PROPERTY IMPORTED_IMPLIB "${PROJECT_SOURCE_DIR}/bin/shared/SDL2_image.lib")

add_library(zlib SHARED IMPORTED)
set_property(TARGET zlib PROPERTY IMPORTED_LOCATION "${PROJECT_SOURCE_DIR}/bin/shared/zlib.dll")
set_property(TARGET zlib PROPERTY IMPORTED_IMPLIB "${PROJECT_SOURCE_DIR}/bin/shared/zlib.lib")

add_library(libpng SHARED IMPORTED)
set_property(TARGET libpng PROPERTY IMPORTED_LOCATION "${PROJECT_SOURCE_DIR}/bin/shared/libpng16.dll")
set_property(TARGET libpng PROPERTY IMPORTED_IMPLIB "${PROJECT_SOURCE_DIR}/bin/shared/libpng16.lib")

target_link_libraries(metallic-crow SDL2-image zlib libpng)

include_directories ("${PROJECT_SOURCE_DIR}/lib/SDL2_ttf")
add_library(SDL2-ttf SHARED IMPORTED)
set_property(TARGET SDL2-ttf PROPERTY IMPORTED_LOCATION "${PROJECT_SOURCE_DIR}/bin/shared/SDL2_ttf.dll")
set_property(TARGET SDL2-ttf PROPERTY IMPORTED_IMPLIB "${PROJECT_SOURCE_DIR}/bin/shared/SDL2_ttf.lib")

add_library(freetype SHARED IMPORTED)
set_property(TARGET freetype PROPERTY IMPORTED_LOCATION "${PROJECT_SOURCE_DIR}/bin/shared/freetype.dll")
set_property(TARGET freetype PROPERTY IMPORTED_IMPLIB "${PROJECT_SOURCE_DIR}/bin/shared/freetype.lib")
target_link_libraries(metallic-crow SDL2-ttf freetype)

include_directories ("${PROJECT_SOURCE_DIR}/lib/SDL2_mixer")
add_library(SDL2-mixer SHARED IMPORTED)
set_property(TARGET SDL2-mixer PROPERTY IMPORTED_LOCATION "${PROJECT_SOURCE_DIR}/bin/shared/SDL2_mixer.dll")
set_property(TARGET SDL2-mixer PROPERTY IMPORTED_IMPLIB "${PROJECT_SOURCE_DIR}/bin/shared/SDL2_mixer.lib")

add_library(libogg SHARED IMPORTED)
set_property(TARGET libogg PROPERTY IMPORTED_LOCATION "${PROJECT_SOURCE_DIR}/bin/shared/libogg.dll")
set_property(TARGET libogg PROPERTY IMPORTED_IMPLIB "${PROJECT_SOURCE_DIR}/bin/shared/libogg.lib")

add_library(libvorbis SHARED IMPORTED)
set_property(TARGET libvorbis PROPERTY IMPORTED_LOCATION "${PROJECT_SOURCE_DIR}/bin/shared/libvorbis.dll")
set_property(TARGET libvorbis PROPERTY IMPORTED_IMPLIB "${PROJECT_SOURCE_DIR}/bin/shared/libogg.lib")

add_library(libvorbisfile SHARED IMPORTED)
set_property(TARGET libvorbisfile PROPERTY IMPORTED_LOCATION "${PROJECT_SOURCE_DIR}/bin/shared/libvorbisfile-3.dll")
set_property(TARGET libvorbisfile PROPERTY IMPORTED_IMPLIB "${PROJECT_SOURCE_DIR}/bin/shared/libvorbisfile-3.lib")
target_link_libraries(metallic-crow SDL2-mixer libogg libvorbis libvorbisfile)

include_directories ("${PROJECT_SOURCE_DIR}/lib/box2d/Box2D")
add_library(Box2D STATIC IMPORTED)
set_property(TARGET Box2D PROPERTY IMPORTED_LOCATION_RELEASE "${PROJECT_SOURCE_DIR}/bin/static/Box2D.lib")
set_property(TARGET Box2D PROPERTY IMPORTED_LOCATION_DEBUG "${PROJECT_SOURCE_DIR}/bin/static/Box2D_d.lib")
target_link_libraries(metallic-crow Box2D)

include_directories ("${PROJECT_SOURCE_DIR}/lib/lua/src")
include_directories ("${PROJECT_SOURCE_DIR}/lib/lua")
add_library(lua STATIC IMPORTED)
set_property(TARGET lua PROPERTY IMPORTED_LOCATION_RELEASE "${PROJECT_SOURCE_DIR}/bin/static/lua.lib")
set_property(TARGET lua PROPERTY IMPORTED_LOCATION_DEBUG "${PROJECT_SOURCE_DIR}/bin/static/lua_d.lib")
target_link_libraries(metallic-crow lua)

include_directories ("${PROJECT_SOURCE_DIR}/lib/jansson/src")
include_directories ("${PROJECT_SOURCE_DIR}/lib/jansson/win32")
add_library(jansson STATIC IMPORTED)
set_property(TARGET jansson PROPERTY IMPORTED_LOCATION_RELEASE "${PROJECT_SOURCE_DIR}/bin/static/jansson.lib")
set_property(TARGET jansson PROPERTY IMPORTED_LOCATION_DEBUG "${PROJECT_SOURCE_DIR}/bin/static/jansson_d.lib")
target_link_libraries(metallic-crow jansson)

include_directories ("${PROJECT_SOURCE_DIR}/lib/boost")

add_library(boost-filesystem STATIC IMPORTED)
set_property(TARGET boost-filesystem PROPERTY IMPORTED_LOCATION_RELEASE "${PROJECT_SOURCE_DIR}/bin/static/libboost_filesystem-vc120-mt-1_56.lib")
set_property(TARGET boost-filesystem PROPERTY IMPORTED_LOCATION_DEBUG "${PROJECT_SOURCE_DIR}/bin/static/libboost_filesystem-vc120-mt-gd-1_56.lib")
target_link_libraries(metallic-crow boost-filesystem)

add_library(boost-program-options STATIC IMPORTED)
set_property(TARGET boost-program-options PROPERTY IMPORTED_LOCATION_RELEASE "${PROJECT_SOURCE_DIR}/bin/static/libboost_program_options-vc120-mt-1_56.lib")
set_property(TARGET boost-program-options PROPERTY IMPORTED_LOCATION_DEBUG "${PROJECT_SOURCE_DIR}/bin/static/libboost_program_options-vc120-mt-gd-1_56.lib")
target_link_libraries(metallic-crow boost-program-options)

add_library(boost-system STATIC IMPORTED)
set_property(TARGET boost-system PROPERTY IMPORTED_LOCATION_RELEASE "${PROJECT_SOURCE_DIR}/bin/static/libboost_system-vc120-mt-1_56.lib")
set_property(TARGET boost-system PROPERTY IMPORTED_LOCATION_DEBUG "${PROJECT_SOURCE_DIR}/bin/static/libboost_system-vc120-mt-gd-1_56.lib")
target_link_libraries(metallic-crow boost-system)
