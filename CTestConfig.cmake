## This file should be placed in the root directory of your project.
## Then modify the CMakeLists.txt file in the root directory of your
## project to incorporate the testing dashboard.
## # The following are required to uses Dart and the Cdash dashboard
##   ENABLE_TESTING()
##   INCLUDE(CTest)
set(CTEST_PROJECT_NAME "Ides Compiler")
set(CTEST_NIGHTLY_START_TIME "01:00:00 UTC")

set(CTEST_DROP_METHOD "http")
set(CTEST_DROP_SITE "dashboard.ides-lang.com")
set(CTEST_DROP_LOCATION "/submit.php?project=Ides+Compiler")
set(CTEST_DROP_SITE_CDASH TRUE)

set(CTEST_UPDATE_TYPE "None")

