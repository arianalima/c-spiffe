# Minimum CMake required
cmake_minimum_required(VERSION 3.13)

add_executable(check_pem check_pem.c )

target_link_libraries(check_pem internal ${CHECK_LIBRARIES}
  OpenSSL::SSL
  spiffeid
  uriparser
  bundle
  jansson
  cjose
  subunit
  check_pic
  check
  rt
  m
  crypto
  pthread)

add_test(check_pem check_pem)
