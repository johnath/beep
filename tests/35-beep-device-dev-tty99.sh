if ${BEEP} -f "$FREQ" -e /dev/tty99; then
    :
else
    echo "Error"
fi
