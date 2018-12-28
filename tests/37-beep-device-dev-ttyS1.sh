if ${BEEP} -f "$FREQ" -e /dev/ttyS1; then
    :
else
    echo "Error"
fi
