cmake_minimum_required(VERSION 2.8.3)
project(swri_console)

set(BUILD_DEPS
  rosbag_storage 
  roscpp 
  rosgraph_msgs
  )

set(RUNTIME_DEPS
  rosbag_storage
  roscpp
  rosgraph_msgs
  )
 
find_package(catkin REQUIRED COMPONENTS ${BUILD_DEPS})
find_package(Qt4 REQUIRED)

catkin_package()

include(${QT_USE_FILE})
include_directories(include 
  ${catkin_INCLUDE_DIRS} 
  ${QT_INCLUDE_DIR} )
add_definitions(${QT_DEFINITIONS})

set(QT_USE_QTCORE TRUE)
set(QT_USE_QTGUI TRUE)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(src)

# Add Qt Designer UI files to this list.
qt4_wrap_ui(SRC_FILES 
  ui/console_window.ui
  )

# Add header files containing Q_OBJET declaration to this list.
qt4_wrap_cpp(SRC_FILES
  include/swri_console/bag_source.h
  include/swri_console/bag_source_backend.h
  include/swri_console/console_master.h
  include/swri_console/console_window.h
  include/swri_console/log_database.h
  include/swri_console/log_list_model.h
  include/swri_console/log_list_widget.h
  include/swri_console/node_list_model.h
  include/swri_console/node_list_widget.h
  include/swri_console/ros_source.h
  include/swri_console/ros_source_backend.h
  include/swri_console/session_list_delegate.h
  include/swri_console/session_list_model.h
  include/swri_console/session_list_widget.h
  include/swri_console/settings_keys.h
  )

# Add extra resources to this list.
qt4_add_resources(RCC_SRCS 
  resources/images.qrc
  )

# Add source files to this list.
LIST(APPEND SRC_FILES  
  src/bag_source.cpp
  src/bag_source_backend.cpp
  src/console_master.cpp
  src/console_window.cpp
  src/log.cpp
  src/log_database.cpp
  src/log_list_model.cpp
  src/log_list_widget.cpp
  src/main.cpp
  src/node_list_model.cpp
  src/node_list_widget.cpp
  src/register_meta_types.cpp 
  src/ros_source.cpp
  src/ros_source_backend.cpp
  src/session_list_delegate.cpp
  src/session_list_model.cpp
  src/session_list_widget.cpp
  src/session.cpp
  src/settings_keys.cpp
  )

add_executable(swri_console 
  ${SRC_FILES} 
  ${RCC_SRCS} 
  )
target_link_libraries(swri_console 
  ${QT_LIBRARIES}
  ${catkin_LIBRARIES}
  )
set_target_properties(swri_console
  PROPERTIES COMPILE_FLAGS "-std=c++0x")



install(TARGETS swri_console
  RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
  LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
)

