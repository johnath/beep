if ${BEEP} blubb > /dev/null; then
    echo "beep should have exited with non-0, but exited with 0"
else
    : "The unhandled non-option argument has been detected"
fi
