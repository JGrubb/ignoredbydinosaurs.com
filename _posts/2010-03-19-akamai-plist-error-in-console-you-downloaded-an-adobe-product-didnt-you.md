--- 
layout: post
title: Akamai plist error in console - you downloaded an Adobe product, didn't you?
created: 1269012726
---
<p>Inaugurating a new "Geek" category for this post with the hope that I can prune back most of the current categories into Geek and Non-Geek.  Anyway =></p>

<p>You're probably on a Macintosh and noticed it hanging momentarily every ten seconds or so.  You probably recently downloaded Photoshop or Dreamweaver or some other Adobe product that made you download and install the Akamai download manager before it would let you download and install the Adobe product you wanted in the first place.  Being conscientious about organization and maintenance of the files in your computer you probably deleted the Akamai download manager after you used it.  You went on your merry way and are probably enjoying your Adobe product.  At some point you realized this really annoying hang that your computer has been doing.  You went to the Console app and saw a list of error messages a mile long about a call to some plist feature that OS X couldn't find.  You have glimpsed the trail of your enemy.  A brief Google search turned up a few dated clues.  You've found yourself here.  Welcome.  Let's kill that fucker, shall we?</p>

<p>Here's the error -</p>

<code>3/19/10 12:10:10 PM	com.apple.launchd.peruser.501[398] (com.akamai.client.plist[606]) Bug: launchd_core_logic.c:4103 (23932):13 <br />
3/19/10 12:10:10 PM	com.apple.launchd.peruser.501[398] (com.akamai.client.plist[606]) posix_spawn("/Applications/Akamai/loader.pl", ...): No such file or directory<br />
3/19/10 12:10:10 PM	com.apple.launchd.peruser.501[398] (com.akamai.client.plist[606]) Exited with exit code: 1<br />
3/19/10 12:10:10 PM	com.apple.launchd.peruser.501[398] (com.akamai.client.plist) Throttling respawn: Will start in 10 seconds
</code>

<p>Well, thanks a lot.  You check out your Library folders and can't find a plist item for Akamai.  You can't find anything for <code>com.apple.launchd</code>.  Spotlight doesn't find anything for Akamai.  Spotlight doesn't find anything for <code>com.apple.launched.peruser</code>.  The trail is going cold.  This is why you hate "download managers".  Aren't you capable of managing your own downloads?  </p>

<p>Well, the sad news is that since you're a conscientious, but perhaps not completely and utterly thorough computer user, you didn't dive deep into the README that was provided in the Akamai folder.  There was actually some useful info buried in there.  Here's the fix.</p>

<p>First, you have to go back to Adobe and pick out any trial product to use (again).  This will cause the Akamai thing to be reinstalled on your system, which is all you're after.  It'll download that goofy Akamai thing again to a disk image which you then install.  It will then start downloading the Adobe whatever.  You can stop the download at that point.  Now open up the Terminal and paste this command -</p>

<code>/Applications/Akamai/admintool uninstall -force</code>

<p>This is the only proper way to uninstall that stupid thing that you didn't want on your computer in the first place and for which they don't even provide an uninstaller, just a little hint in a README buried in the Akamai folder.  Restart your computer and hope it forgets all about it.  Carry on...</p>

<p>Props to the <a href="http://discussions.apple.com/thread.jspa?messageID=11243793#11243793">Apple Forum</a> for providing the answer.</p>
