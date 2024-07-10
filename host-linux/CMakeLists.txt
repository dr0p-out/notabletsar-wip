cmake_minimum_required(VERSION 3.16...4.0)
project(NtsHostLinux LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(GNUInstallDirs)
include(CheckIPOSupported)

option(ENABLE_NLS "Enable native language support" ON)
option(ENABLE_LTO "Enable link time optimization" OFF)

# for Dialog
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")
# for IDE
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(libusb-1.0 REQUIRED IMPORTED_TARGET libusb-1.0)
find_package(Dialog REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Widgets)
if (ENABLE_NLS)
  find_package(Intl)
  find_package(Gettext)
  if (NOT Intl_FOUND OR NOT GETTEXT_FOUND)
    message(WARNING "Native language support will NOT be available")
  endif()
endif()

add_compile_options(-Wall -Wextra)
check_ipo_supported(RESULT HAVE_LTO OUTPUT LTO_ERR)
configure_file(config.h.in "${CMAKE_CURRENT_BINARY_DIR}/config.h" @ONLY)

add_executable(nts_host_linux
  l10n/utils.c
  uinput.c
  dlg_lsusb.c
  accessory.c
  host_info.cc
  ../common/adapt.c
  recv.c
  main.c)
target_include_directories(nts_host_linux PRIVATE
  "${CMAKE_CURRENT_BINARY_DIR}")
target_link_libraries(nts_host_linux PRIVATE
  PkgConfig::libusb-1.0 Dialog::Dialog Qt6::Widgets)
if (ENABLE_NLS AND Intl_FOUND AND GETTEXT_FOUND)
  target_compile_definitions(nts_host_linux PRIVATE ENABLE_NLS)
  target_link_libraries(nts_host_linux PRIVATE Intl::Intl)
  add_subdirectory(l10n)
endif()
if (ENABLE_LTO)
  if (HAVE_LTO)
    set_target_properties(nts_host_linux
      PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
  else()
    message(WARNING "Link time optimization is NOT available: ${LTO_ERR}")
  endif()
endif()

install(TARGETS nts_host_linux
        DESTINATION "${CMAKE_INSTALL_BINDIR}")
