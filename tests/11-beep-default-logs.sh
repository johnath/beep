: REQUIRES_HARDWARE
env BEEP_LOG_LEVEL=999 ${BEEP} 2>&1 | awk '/ as fd=[1-9][0-9]*/ { fds[$7]=fd_idx++; } /beep_drivers_register/ { drivers[$5]=drv_idx++; } { for (drv in drivers) { gsub(drv, "<DRV" drivers[drv] ">"); }; for (fd in fds) { gsub(fd, "fd=<FD" fds[fd] ">"); }; print; } BEGIN { driver="xyzxyzxyzxyz"; }'
