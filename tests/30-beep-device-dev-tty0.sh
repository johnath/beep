if ${BEEP} -f "$FREQ" -e /dev/tty0; then
    :
else
    echo "Error"
fi
