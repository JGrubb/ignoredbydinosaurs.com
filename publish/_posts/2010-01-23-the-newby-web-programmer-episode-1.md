--- 
layout: post
title: The newby web programmer.  Episode 1.
created: 1264279987
---
This post is for my buddy Jimmy.  He's a teacher in CT and has set up a Wordpress.com site for his classroom.  Of course the parents love it, and he's the young techno-hip teacher in the school so his principal has allowed him to go and set up a new site for his school, whose hideous and outdated site was left for dead by the side of the road several years ago, apparently.  He just sent me an email asking about downloading Wordpress.  I responded by asking why because downloading and using Wordpress isn't the piece of cake that you might think it is if you've never dealt with web servers and databases before.  So I decided to make this public so that he and I and we may refer to it from now on.  Besides, this hasn't been blogged about nearly enough...

When I first downloaded Ruby on Rails I was a bit perplexed.  It came with a README that gave these installation instructions that made no sense to me at all.  I didn't know anything about webservers at the time, or for a few months afterward, so here's what I learned.  This applies to Wordpress as well.

Wordpress and Ruby on Rails are both (more or less) database-backed content management systems.  (If you know anything about Rails, you go right ahead and light my comments section up.)  Drupal is, too.  This means that the post that you're reading right now isn't stored as a word document or a text file on a file system as you know it on your computer.  It's stored as plain text in a database.  Search the term here for a brief newby explanation from my ongoing programmer journal/book.  What this means is that you have to hook wordpress up to a database server to have your content served.  You also need to hook it up to a webserver to have your content served to and from a browser.  This sounds complicated because it kind of is until you do it once or twice.

If you are a Mac user go Google MAMP, and download it.  the AMP part of MAMP stand for Apache -the webserver, MySQL - the database, and PHP - the coding language that Wordpress is written in.  Pardon my grammar.  Install it.  Now you have a webserver and a database server on your computer, so you can play with Wordpress now.  In the MAMP directory somewhere is a folder called htDocs, which is the root of your webserver.  

If you make a subdirectory under htdocs called Wordpress, unzip and copy all the files from a Wordpress download, and start MAMP, you can then point your browser to http://localhost:8888/Wordpress and install wordpress in a way that you can then use.  You also need to create a database.  Use the PHPMyAdmin feature of MAMP for this.  Just figure it out.  You won't break anything.  Create a database and give WordPress the name of that database.  you're good to go with Wordpress on your "local machine".

Comments/Questions are down there.

More to come.
