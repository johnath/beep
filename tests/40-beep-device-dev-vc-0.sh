if ${BEEP} -f "$FREQ" -e /dev/vc/0; then
    :
else
    echo "Error"
fi
