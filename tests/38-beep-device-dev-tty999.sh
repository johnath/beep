if ${BEEP} -f "$FREQ" -e /dev/tty999; then
    :
else
    echo "Error"
fi
