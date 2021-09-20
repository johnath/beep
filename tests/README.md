The beep testsuite
==================

Deficiencies
------------

  * Quick hack.

  * Does not test whether actual sound output happened. The different
    frequency might help a human detect a different melody, but that
    is no replacement for a properly automated test.


Devices
=======

```
[root@host ~]# echo blubb > /dev/input/blubb

[user@host ~]$ ls -lR /dev/tty0 /dev/vc/0 /dev/input/
ls: cannot access '/dev/vc/0': No such file or directory
crw--w----+ 1 root tty  4, 0 27. Dez 17:55 /dev/tty0

/dev/input/:
total 4
-rw-r--r--. 1 root root       6 28. Dez 01:55 blubb
drwxr-xr-x. 2 root root     140 27. Dez 17:55 by-id
drwxr-xr-x. 2 root root     180 27. Dez 19:19 by-path
crw-rw----. 1 root input 13, 64 27. Dez 17:55 event0
crw-rw----. 1 root input 13, 65 27. Dez 17:55 event1
crw-rw----. 1 root input 13, 74 27. Dez 17:55 event10
crw-rw----. 1 root input 13, 75 27. Dez 17:55 event11
crw-rw----. 1 root input 13, 76 27. Dez 17:55 event12
crw-rw----. 1 root input 13, 77 27. Dez 17:55 event13
crw-rw----. 1 root input 13, 78 27. Dez 17:55 event14
crw-rw----. 1 root input 13, 79 27. Dez 17:55 event15
crw-rw----. 1 root input 13, 80 27. Dez 17:55 event16
crw-rw----. 1 root input 13, 81 27. Dez 17:55 event17
crw-rw----. 1 root input 13, 82 27. Dez 17:55 event18
crw-rw----. 1 root input 13, 83 27. Dez 17:55 event19
crw-rw----. 1 root input 13, 66 27. Dez 17:55 event2
crw-rw----. 1 root input 13, 84 27. Dez 17:55 event20
crw-rw----. 1 root input 13, 85 27. Dez 19:19 event21
crw-rw----. 1 root input 13, 67 27. Dez 17:55 event3
crw-rw----. 1 root input 13, 68 27. Dez 17:55 event4
crw-rw----. 1 root input 13, 69 27. Dez 17:55 event5
crw-rw----. 1 root input 13, 70 27. Dez 17:55 event6
crw-rw----. 1 root input 13, 71 27. Dez 17:55 event7
crw-rw----. 1 root input 13, 72 27. Dez 17:55 event8
crw-rw----. 1 root input 13, 73 27. Dez 17:55 event9
crw-rw----. 1 root input 13, 63 27. Dez 17:55 mice
crw-rw----. 1 root input 13, 32 27. Dez 17:55 mouse0

/dev/input/by-id:
total 0
lrwxrwxrwx. 1 root root 9 27. Dez 17:55 usb-B16_b_02_USB-PS_2_Optical_Mouse-event-mouse -> ../event3
lrwxrwxrwx. 1 root root 9 27. Dez 17:55 usb-B16_b_02_USB-PS_2_Optical_Mouse-mouse -> ../mouse0
lrwxrwxrwx. 1 root root 9 27. Dez 17:55 usb-Burr-Brown_from_TI_USB_Audio_CODEC-event-if03 -> ../event2
lrwxrwxrwx. 1 root root 9 27. Dez 17:55 usb-Microsoft_Natural®_Ergonomic_Keyboard_4000-event-kbd -> ../event4
lrwxrwxrwx. 1 root root 9 27. Dez 17:55 usb-Microsoft_Natural®_Ergonomic_Keyboard_4000-if01-event-kbd -> ../event5

/dev/input/by-path:
total 0
lrwxrwxrwx. 1 root root  9 27. Dez 17:55 pci-0000:00:10.1-usb-0:1:1.3-event -> ../event2
lrwxrwxrwx. 1 root root  9 27. Dez 17:55 pci-0000:00:12.2-usb-0:2.3:1.0-event-mouse -> ../event3
lrwxrwxrwx. 1 root root  9 27. Dez 17:55 pci-0000:00:12.2-usb-0:2.3:1.0-mouse -> ../mouse0
lrwxrwxrwx. 1 root root  9 27. Dez 17:55 pci-0000:00:12.2-usb-0:2.4:1.0-event-kbd -> ../event4
lrwxrwxrwx. 1 root root  9 27. Dez 17:55 pci-0000:00:12.2-usb-0:2.4:1.1-event-kbd -> ../event5
lrwxrwxrwx. 1 root root 10 27. Dez 17:55 platform-eeepc-wmi-event -> ../event20
lrwxrwxrwx. 1 root root 10 27. Dez 19:19 platform-pcspkr-event-spkr -> ../event21
[user@host ~]$ _
```
