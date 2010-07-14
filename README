Intro
-----

I just got so tired of being limited to printf("\a"); when I wanted a terminal
beep.  This program isn't supposed to be anything stupendous, it's just
supposed to get the job done.  Its intended purpose in life is to live inside
shell/perl scripts, and allow a little more granularity than you get with the
default terminal bell.  Maybe I'm the only one who thinks this is useful. :)

If for any reason you decide you need to, contact me:

johnath@johnath.com
http://johnath.com/

And beep can generally be found at:

http://johnath.com/beep/

For installation instructions, see INSTALL.
For copying and (non-)warranty information, see COPYING.
For usage information, check the man page.

There is a github repository of this code at: git://github.com/johnath/beep.git

A note about ioctl
------------------

As noted in the man page, some users are running into a situation where beep 
dies with a complaint from ioctl().  The reason for this, as Peter Tirsek was
nice enough to point out to me, stems from how the kernel handles beep's 
attempt to poke at (for non-programmers: ioctl is a sort of catch-all function
that lets you poke at things that have no other predefined poking-at mechanism)
the tty, which is how it beeps.  The short story is, the kernel checks that 
either:

- you are the superuser
- you own the current tty

What this means is that root can always make beep work (to the best of my 
knowledge!), and that any local user can make beep work, BUT a non-root remote
user cannot use beep in it's natural state.  What's worse, an xterm, or other
x-session counts, as far as the kernel is concerned, as 'remote', so beep 
won't work from a non-priviledged xterm either.  I had originally chalked this
up to a bug, but there's actually nothing I can do about it, and it really is 
a Good Thing that the kernel does things this way.  There is also a solution.

By default beep is not installed with the suid bit set, because that would 
just be zany.  On the other hand, if you do make it suid root, all your 
problems with beep bailing on ioctl calls will magically vanish, which is 
pleasant, and the only reason not to is that any suid program is a potential 
security hole.  Conveniently, beep is very short, so auditing it is pretty 
straightforward.

Decide for yourself, of course, but it looks safe to me - there's only one 
buffer and fgets doesn't let it overflow, there's only one file opening, and 
while there is a potential race condition there, it's with /dev/console.  If 
someone can exploit this race by replacing /dev/console, you've got bigger 
problems.  :)

So the quick solution is beep is not beeping when you want it to is:

$ su
Password:
# chmod 4755 /usr/bin/beep

(or wherever you put it)

The one snag is that this will give any little nitwit the ability to run beep 
successfully - make sure this is what you want.  If it isn't, a slightly more 
complex fix would be something like:

# chgrp beep /usr/bin/beep
# chmod 4750 /usr/bin/beep

and then add only beep-worthy users to the 'beep' group.

Playing Songs
-------------

A surprising number of people have sent in requests, or even patches, to help
beep play multiple, different sounds off a single invocation.  I had always
thought that if people wanted a more complex melody, they would just do 
something like:

$ cat << EOF > song.sh
#!/bin/sh
beep <first beep's options>
beep <second beep's options>
etc...
EOF

Nevertheless, because of repeated and vociferous demand, version 1.2 (and 
presumably all later versions) include the -n/--new switch which allows you to
use one command line to create multiple beeps.  Check the man page for 
details.  I have also had a couple people suggest that I encourage the 
development of such shell scripts/command lines, even collect the particularly
melodious ones.  Certainly if anyone feels like sending some to me, I will put
them somewhere visible, or even include them as a sample.  I think Dvorak's New
World Symphony, 4th Movement, for example, would make a lovely shell script.
I also wouldn't mind a rendition of BNL's If I had a million dollars.  But by
all means, be creative.

All files copyright (C) Johnathan Nightingale, 2002.
All files distributed under the GNU general public license.








