
> I (Johnathan Nightingale) handled the original coding, distribution,
> maintenance, etc.
>
> Peter Tirsek (peter at tirsek dot com) filled me in on where the magical
> fairy number comes from (see beep.c).  He also tracked down the relevant
> kernel code that causes the ioctl()s to die under certain conditions
> (see the README or man page sections on ioctl).  He is truly a wonderful
> person.
>
> Andreas Hochsteger (e9625392 at student dot tuwien dot ac dot at) contributed
> several useful little patches, and was my inspiration for the -d/-D
> distinction.  He's also thrown pointers my way about code packaging, which
> are appreciated.
>
> Rick Franchuk (rickf at transpect dot net) came up with the idea of the -s and
> -c stdin hooks.  Since most people will be using beep to tell them when a new
> line pops up in log, or mail, or what-have-you, this was quite clever of him
> and though I really did mean to do it anyhow, he sent me the patch before I had
> bothered to write it, so he gets the credit.  :)
>
> Serge Winitzki (winitzki at erebus.phys.cwru.edu) suggested having beep take
> floats for frequency, instead of int's.
>
> The guys at freshmeat.net really deserve a big ol whack of credit too, for
> running a very cool site in general, and for running one so successfully that
> my little 4k program generated literally hundreds of emails in reply.  It gives
> one a great sense of community to see such an overwhelming response.
>
> In particular, fryguy[at]freshmeat (Ryan Weaver) rocks supremely, for packaging
> beep in RPM format, and maintaining said package until he decides not to.  I
> really did mean to make a spec for it myself, but hey, when freshmeat offers to
> maintain your packages, you'd have to have a *real* good reason to say no.
>
> Most of all - and I write this nearly 10 years later, so I have the benefit of
> history on my side here - I would like to thank Gerfried Fuchs, who has tirelessly
> maintained beep in debian, along with a stack of patches, years after I stopped
> hacking on it in earnest. He is amazingly dedicated, and has a new son whom we
> would all be fortunate to have follow in his father's footsteps.

Thanks to terom on Hacker News, Andrew Ayer, Richard Kettlewall,
rain-1 on github, Jakub Wilk, and Benjamin Grange for their help with
fixing
[CVE-2018-0492](https://cve.mitre.org/cgi-bin/cvename.cgi?name=CVE-2018-0492),
[CVE-2018-1000532](https://cve.mitre.org/cgi-bin/cvename.cgi?name=CVE-2018-1000532),
and the help fixing the fixes (https://github.com/johnath/beep/issues/11).

Thanks to Hanno Böck for the report on possible integer overflows
(https://github.com/johnath/beep/issues/13).

Thanks to Stefan Helmert for the `sig_atomic_t` idea.

Thanks to Dan Jacobson (jidanni on github) and kanliot for suggesting
documentation improvements especially for but not restricted to the
permissions setup (https://github.com/spkr-beep/beep/issues/4).

Thanks to Daniel Kamil Kozar (xavery on github) for reporting and
helping with fixing the sudo check when beep runs as non-root user
(https://github.com/spkr-beep/beep/issues/5).

Thanks to Walter B. Vaughan for reminding me that keeping a format
string in C macros using `__VA_ARGS__` is still a bad idea.

Thanks to Dan Jacobson for the idea and the perl script for converting
morse code to beeps via a beep command line.

Thanks to a1346054 for the fixes to spelling and whitespace.
