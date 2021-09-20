: REQUIRES_HARDWARE

if (sleep 2; echo moo; sleep 2; echo foo; sleep 1; echo erk; sleep 1; echo bla) | ${BEEP} -f 880 -n -f 440 -s -n -f 659; then
    :
else
    echo "Error"
fi
