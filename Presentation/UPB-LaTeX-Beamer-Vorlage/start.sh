#!/bin/bash
xrandr --output VGA1 --left-of LVDS1
pdf-presenter-console -d 30 -l 15 -s presentation.pdf
