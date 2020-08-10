#!/usr/bin/env bash

echo `sed -nr 's/^ *ID *= *(\w+)/\1/p' /etc/os-release`

