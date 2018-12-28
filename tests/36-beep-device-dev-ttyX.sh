if ${BEEP} -f "$FREQ" -e /dev/ttyX; then
    :
else
    echo "Error"
fi
