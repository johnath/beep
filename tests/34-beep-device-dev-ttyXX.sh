if ${BEEP} -f "$FREQ" -e /dev/ttyXX; then
    :
else
    echo "Error"
fi
