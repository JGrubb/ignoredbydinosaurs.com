---
layout: post
title: Migrating the blog from Drupal to Jekyll
meta_desc: The not altogether tortuous process of extracting my content from Drupal and migrating it to Jekyll, the static site generator.
---
##Reasons for leaving Drupal, a preamble

I'd had this website on Drupal since some time in July.  If you look through the [archives](http://ignoredbydinosaurs.com/archive/), you'll notice a relative dearth of posts from this time period.  Drupal just has a way of sucking all the fun out of blogging.  It's very, very slow for one thing.  I had a lot of trouble integrating the site with the Disqus comment system that handled all of my Wordpress comments before I made the move so I was forced to use Drupal's comment system.  I'd written a couple of posts in the "didn't find much in Google about it, so I decided to become the authoritative voice on it" vein, and had a ton of comments on one of them.  Those comments appear to be gone now, even though I devoted an entire day to exporting them out of Drupal into Disqus with a [solution that someone came up with](http://drupal.org/node/269010 "Migrate from Drupal comment.module to Disqus comments | drupal.org").

Basically, Drupal is a beast.  If you have a project that you are trying to build that involves users with accounts, and different levels of access to the content based on those roles, go with Drupal.  For anything else, anything simpler, stay away.  Drupal gives the impression of being somehow more user friendly since you can configure these massively complex sites without actually coding much of anything, but is that really a good thing??  Drupal's inherent dependence on [stashing so much configuration in the database](http://developmentseed.org/blog/2009/jul/09/development-staging-production-workflow-problem-drupal "The Development -&gt; Staging -&gt; Production Workflow Problem in Drupal | Development Seed") will be the death of the project if it's not figured out, and I personally don't think anything short of a MAJOR rewrite is going to sort it out.  The major Drupal rewrite that's about to drop on the world some time has taken 2 years to get even close to the door, and a rewrite that would effectively fix this particular issue would also effectively rewrite the entire philosophy of being able to build a site in the browser, arguably the whole reason for Drupal existence in the first place.

I haven't even gotten to the part about moving Drupal to Git.  Why not put the whole thing up in GitHub?  Have you heard of Rails?  It's doing pretty well, and I'd wager that a large part of the reason for that is how easy it is to dive in and contribute to open source on GitHub.  I've never once seen it even mentioned to move Drupal to GitHub.

anyway...

##Reasons for migrating to Jekyll, regular ramble

First off, it really wasn't that hard, so skip ahead if you wish.  I spent a good several hours on Github researching other sites that people had going on the platform out there.  I've been studying a whole lot of Ruby lately, so it was down to Jekyll or a few other simple solutions.  I started building a solution out of Sinatra, but decided that deployment was probably going to be more of a headache than I felt like dealing with.

I loved the idea of a static HTML site since one of my main gripes about Drupal was how many times I could count the little page load indicator going around on Chrome.  This site is blazing fast now, so yay for that.

I love the idea that there are no security updates, no databases to backup, no crufty markup that comes from where exactly?  Basically, there's very little tradeoff.  It's mostly win.  So, on to the show.

##Migrating from Drupal to Jekyll, the meat

One of the things that I didn't understand about Jekyll was that it doesn't really generate a site template for you (you get used to that when dealing with Ruby).  You have to build that part or Jekyll won't do anything at all.  It's easy enough to get started though, just Google or [borrow from other GitHubbers](https://github.com/mojombo/jekyll/wiki/Sites).  The tricky part was liberating my posts from Drupal, which was made vastly easier by [this fellow having written a Drupal migrator](https://github.com/mattdipasquale/jekyll/blob/master/lib/jekyll/migrators/drupal.rb) only a few weeks ago.

I followed all of the instructions on [this page](https://github.com/mojombo/jekyll/wiki/Blog-Migrations) for getting the migrators to work, but kept getting an error message that ruby couldn't find the specified file or something like that.  So an hour or two of fiddling around with the migrator file in my Jekyll directory and finally changing the command to something like this -

`ruby -r '~/PLAY/jekyll/_import/drupal' -e 'Jekyll::Drupal.process( "#{ENV["DB"]}", "#{ENV["USER"]}", "#{ENV["PASS"]}")'`


where `~/PLAY/jekyll` is the root of my jekyll install in order to get ruby to read the migrator file that was there instead of trying to find one that wasn't.  I'm sure the instructions will work fine for someone who knows more than me, but hey it worked.

##to give back unto the community...

TODO - add a bit into the Drupal migrator that also liberates the URL aliases from the DB, as the author of the current migrator apparently used the stock Drupal URL scheme (node/\*).  Jekyll has an easy facility for setting the permalinks for your posts, but going through every post to make sure they were right was needlessly tedious in hindsight.