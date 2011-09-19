--- 
layout: post
title: My little pony's first shell script
created: 1285763400
meta_desc: The one where I get all proud of myself because I wrote a Bash script that works.
---
So I've been doing a lot of CMS work lately, both Drupal and Wordpress.  The Wordpress gigs seem to come and go within a week, and are mostly just banging together a theme with varying amounts of complexity.  I'm a recent convert to the 960.gs CSS framework, which makes the absolute quickest work of laying out a website.  It's a thing of beauty and I've actually <a href="http://github.com/JGrubb/Base-WP-theme">created my own base theme</a> to make the work even quicker, but that's for another post.  

Every one of these gigs requires the exact same setup - downloading various bits of software from all over the internet and copying them to specific places on my hard drive and then wiring them all together.  It's a time consuming and utterly repetitive process, which makes it perfect for a computer to do.  Even more perfect is that I'm already on a computer when I'm doing it!  Wow...

I'd already taken to doing much of this work from the "command line", and it only occurred to me yesterday that if I were doing the exact same series of commands from the command line every time I set one of these things up, isn't that something I could write a "script" to do for me?  For those of you - a "script" is exactly what it sounds like.  In the most basic sense all computer programs are scripts, be they <a href="http://ignoredbydinosaurs.com/2009/03/chapter1b">compiled scripts or interpreted scripts</a>.  You must tell the computer what to do in no uncertain terms at all times for all possible scenarios.  Failure to do so is a Bad Thing.  So in this sense, I'm writing a series of terminal commands in the same order every time.  I had no idea until yesterday that it was as simple as saving those commands into a file and running that file to get what I want done.

So, about 20-30 minutes worth of downloading, copying, and renaming has been reduced to about 20-30 seconds, depending on bandwidth.  Here's the script, in case you're curious --

`
#!/bin/sh

wget http://wordpress.org/latest.tar.gz<br />
tar xzvf latest.tar.gz<br />
rm latest.tar.gz<br />
echo "Name of directory?"<br />
read directory<br />
mv wordpress $directory<br />
cd $directory/wp-content/themes<br />
wget http://github.com/JGrubb/Base-WP-theme/tarball/master<br />
tar xzvf JGrubb-Base-WP-theme-0efd724.tar.gz<br />
rm JGrubb-Base-WP-theme-0efd724.tar.gz<br />
mv JGrubb-Base-WP-theme-0efd724 ibd<br />

`

Now I'm off to write a script to do the same thing for me for Drupal...