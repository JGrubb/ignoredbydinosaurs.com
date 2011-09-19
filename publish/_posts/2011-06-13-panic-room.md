---
layout: post
title: A quick trip to the Panic Room
permalink: /2011/06/quick-trip-panic-room
---

I use Git.  I'm relatively new to the party and it's all I've ever used.  I tried to get SVN working for me back when I was first gettings started and it felt like hand-wiring a tube amplifier - slow, tedious, and you don't know if it's going to work until you're totally done.  I've more recently taken on a client who uses a nifty issue tracker called Jira, but they have their source checked in to SVN.  I thought I was going to to have to get familiar with it until I rediscovered a tool that comes with Git called git-svn.   It works pretty transparently after learning a couple of new commands.

This morning however, I tried something new and was greeted with this (after 5 hours of work).

<pre>
src/sites/all/modules/eloqua
580e5a6480dfae9ee8aa39e2ff14e4b3604d8827 
doesn't exist in the repository at /usr/local/git/libexec/git-core/git-svn line 4771
Failed to read object 580e5a6480dfae9ee8aa39e2ff14e4b3604d8827 
at /usr/local/git/libexec/git-core/git-svn line 573
</pre>

This is a Drupal site that I'm working on.  To make a long story short, I decided to pull a new module over via Git from drupal.org instead of using Drush like I always do.  I figured I might want to chip in on some of the development of this module while I'm already here.  Everything went fine, I added the .git folder within the module to .gitignore and went on my way.  After finishing up enough of the work to send it over to staging and attempting a <code>git svn dcommit</code> I get the horrifying error above.

I found <a href="http://de-co-de.blogspot.com/2009/02/git-svn-and-submodules.html">this post</a> that got me started down the right track.  I used a different approach, though.  I used the technique of rewriting the history with the info <a href="http://help.github.com/remove-sensitive-data/">found here</a>.  I'm pretty familiar with removing accidentally committed DB passwords, and wasn't familiar with the technique that he had commented out in that post.  Worked like a charm, and am now back on track.

To sum up, you delete the offending directory and then run something to the effect of 

<code>
git filter-branch --index-filter \
 'git rm --cached --ignore-unmatch path/to/the/formerly/misbehaving/module'
</code>

from the base path of the git repo.  By the way, that backslash doesn't do anything but allow you to wrap a command to two lines.  You can leave it out if you want and just type all that as one long line.

The culprit of all this is that git-svn specifically chokes on git repos below the main one, as is the case if you <code>git clone</code> a module straight off of d.o.  So, sorry kids, you'll have to contribute that code in some other way.