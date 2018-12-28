if ${BEEP} -f "$FREQ" -e /dev/tty63; then
    :
else
    echo "Error"
fi
