#!/bin/env bash

javac src/App.java -d dist
cd dist
java App
