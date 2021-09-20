: REQUIRES_HARDWARE

if (sleep 2; echo moo; sleep 2; echo foo; sleep 1; echo erk; sleep 1; echo bla) | ${BEEP} -s -f 880 -n -f 440 -n -f 659 -s ; then
    :
else
    echo "Error"
fi
