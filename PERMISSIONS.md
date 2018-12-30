Permission setup for beep
=========================

The way to regulate permissions to the speaker device suggested by the
`beep` package is to have the system administrator add all users who
should be able to run beep to the `beep` group.


Add user to `beep` group
------------------------

To add user `jane` to the `beep` group, the system administrator has
to run a command like

    # usermod jane -a -G beep

After having user `jane` log out (and after killing user `jane`'s
running `tmux` instances, killing `system --user` sessions for user
`jane`, or just plain rebooting), user `jane` can log back in and
check whether she now a `beep` group member:

    [jane@host ~]$ id
    uid=1000(jane) gid=1000(jane) groups=1000(jane),10(wheel),942(beep) context=unconfined_u:unconfined_r:unconfined_t:s0-s0:c0.c1023
    [jane@host ~]$ ./beep -f 220 -n -f 275 -n -f 330 -n -f 440 -n -f 550 -n -f 660 -n -f 880
    [jane@host ~]$ 
