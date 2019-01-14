if ${BEEP} --blubb > /dev/null; then
    echo "beep should not have succeeded"
else
    echo "Error: Good."
fi
