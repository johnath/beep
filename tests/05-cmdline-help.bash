${BEEP} --help | sed 1q
if test "x${PIPESTATUS[*]}" = "x0 0"; then
    :
else
    echo "Error exit, should not happen"
fi
