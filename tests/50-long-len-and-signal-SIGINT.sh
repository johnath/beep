: REQUIRES_HARDWARE

ts_begin="$(date +%s)"

${BEEP} -f "$FREQ" -l 10000 &
pid="$!"

sleep 0.2
kill -s INT "$pid"

wait "$pid"
retcode="$?"

ts_end="$(date +%s)"
ts_delta="$(expr "$ts_end" - "$ts_begin")"
# echo "ts_delta=$ts_delta"

if test "$ts_delta" -le 2; then
    echo "Signal has aborted beep: ${retcode}"
else
    echo "Signal has apparently NOT aborted beep: ${retcode}"
fi
