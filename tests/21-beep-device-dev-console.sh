if ${BEEP} -f "$FREQ" -e /dev/console; then
    :
else
    echo "Error"
fi
