#!/bin/bash
rm -f ./parse_input.hpp
rm -f ./parse_input.cpp
~/bin/semparse ./language.semparse-grammar +output_header=parse_input.hpp +output_code=parse_input.cpp +append
