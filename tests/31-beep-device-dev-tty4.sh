if ${BEEP} -f "$FREQ" -e /dev/tty4; then
    :
else
    echo "Error"
fi
