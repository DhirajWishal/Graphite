:: Copyright 2022 Dhiraj Wishal 
:: SPDX-License-Identifier: Apache-2.0
::
:: Xenon bootstrap file

:: Create the Build directory if it doesn't exist.
if not exist Build mkdir Build

:: Change directory to that directory and setup the CMake files.
call cd Build 
call cmake ..