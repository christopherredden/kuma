#!/usr/bin/env bash
/usr/local/Cellar/bison/3.0.4/bin/bison -v -d -o kuma_parser.cpp kuma.y
/usr/local/Cellar/flex/2.5.39/bin/flex -o kuma_tokens.cpp kuma.l
