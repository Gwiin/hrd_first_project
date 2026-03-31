# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/jyhwang0631/hrd_first_project/pico-sdk/tools/pioasm"
  "/home/jyhwang0631/hrd_first_project/PICO/build/pioasm"
  "/home/jyhwang0631/hrd_first_project/PICO/build/pioasm-install"
  "/home/jyhwang0631/hrd_first_project/PICO/build/pico-sdk/src/rp2_common/pico_cyw43_driver/pioasm/tmp"
  "/home/jyhwang0631/hrd_first_project/PICO/build/pico-sdk/src/rp2_common/pico_cyw43_driver/pioasm/src/pioasmBuild-stamp"
  "/home/jyhwang0631/hrd_first_project/PICO/build/pico-sdk/src/rp2_common/pico_cyw43_driver/pioasm/src"
  "/home/jyhwang0631/hrd_first_project/PICO/build/pico-sdk/src/rp2_common/pico_cyw43_driver/pioasm/src/pioasmBuild-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/jyhwang0631/hrd_first_project/PICO/build/pico-sdk/src/rp2_common/pico_cyw43_driver/pioasm/src/pioasmBuild-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/jyhwang0631/hrd_first_project/PICO/build/pico-sdk/src/rp2_common/pico_cyw43_driver/pioasm/src/pioasmBuild-stamp${cfgdir}") # cfgdir has leading slash
endif()
