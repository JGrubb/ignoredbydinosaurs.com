--- 
layout: post
title: Installing Drush on Media Temple Grid Server
created: 1279291850
---
[Drush](http://drupal.org/project/drush "Drush | drupal.org") is a tool for working on Drupal websites.  It's technically filed away on Drupal.org as a "module", but it's not exactly a module.  It's more like an add-on for your webserver setup, either [development](http://ignoredbydinosaurs.com/2010/01/newby-web-programmer-episode-1) or production, that adds some really useful tools for managing your Drupal website.  For instance, those of you familiar with Wordpress might be surprised to know that Drupal has no built in facility for updating either the core Drupal code or any of the add-ons (which go by the name "Plug-ins" in Wordpress and "Modules" in Drupal).  I don't have any statistics to back this up, but I think the average Drupal site is built with many more of these open-source-and-constantly-shifting contributions than the typical Wordpress site.  

What all that means is that Drupal is a pain in the ass to maintain.  When one of your modules has an update, you get a message that tells you so.  These messages come all the time.  They never stop, actually.  And there's no way to make them go away except to go and download the latest version(s) and upload it/them to the server over FTP, hope nothing went wrong with the transfer, and then go and update your DB, more or less manually.  Over time, slackness can set in.

I've been reading about it for several weeks because the most interesting thing it does (to me now) is take care of updating your stuff.  The downside for some is that it's a command line tool.  However, if you like the command line because it makes you feel like a real programmer, you're in luck!  `drush up`  updates your site, modules and all by downloading them straight from the Drupal CVS repository, and then running any DB updates for you.  "Magic", you might say had you been deprived of this after having tasted Wordpress' sweet waters.

I haven't even used it yet, but I spent the last two days figuring our how to install it, and found the answer scattered over 3 or 4 different places.  Maybe I'm the only one, but I figured I'd make it a little easier for the next guy.

* [Make sure you know how to use SSH](http://kb.mediatemple.net/questions/16/Connecting+via+SSH+to+your+(gs)+Grid-Service "Connecting via SSH to your (gs) Grid-Service") on your grid server account, and log in.
* You will now be at the command line.  You want to make sure you are in the home directory, so type:<br />
`cd ~/`
* At this point you'll create a folder for stuff that you install to live in.  Type:<br />
`mkdir bin`, and then switch into that directory:<br />
`cd bin`
* This command fetches it from drupal.org for you:<br />
`wget http://ftp.drupal.org/files/projects/drush-All-versions-3.0-beta1.tar.gz`<br />
in tarball form.
* Untar:
`tar xzvf drush-All-versions-3.0-beta1.tar.gz`
* Switch into the new `drush` folder:<br />
`cd drush`
* Take a look at the contents of that folder by typing `ls` and make sure everything looks like this:
`LICENSE.txt  commands  drush.api.php  drush.info  drush_logo-black.png  example.drushrc.php
README.txt   drush     drush.bat      drush.php   example.drush.inc     includes
`
* Note the path to that folder by typing:
`pwd`.  <br />
You'll get something back like: `/home/XXXXX/users/.home/bin/drush`.<br />
The drush command is at the end of that path, and you have to let the server know that now.<br />
Note that everything before `bin` can be replaced with `~/`.
* Switch back to your FTP client and root around in your server for the "home" folder.  It's going to be back toward the root:<br />`cd ~/users`
* You may or may not see a file named `.bash_profile`.  Note that it might be hidden depending on your FTP client.  If you are sure that it's not there, create a file and give it that name.
* MT runs PHP4 on the command line by default.  You have to specifically tell it to run Drush with PHP5.  Thus, paste this into the file you just created (`.bash_profile`).<br />
`alias drush='/usr/bin/php5 ~/bin/drush/drush.php'`
* `source .bash_profile` to reload the `.bash_profile` file.

God willing, if you type in `drush` at this point you are going to get a long list of newly available commands.

Note that this took me over a month of poking and prodding to get working correctly.  Specifically, in contrast to all the Drush installation tutorials I found out there, I had to specifically direct it to the `drush.php` file, rather than the generic `drush` wrapper file that the tutorials directed me to use when aliasing the Drush command.  As usually YMMV.

Good luck!

EDIT - September 8, 2010

I've been having issues with updating the Drupal core with Drush.  `drush up` works just fine to update any modules, but for some reason I get an error that Drush needs PHP 5.2 when trying to update the core.  It seems to kick out of php5 when it gets to Drupal core.  I've tried running the full `php5 /home/70298/users/.home/bin/drush/drush.php up` and it still says that my cli version of PHP is too old.  Any suggestions/workarounds/fixes are most welcome.  Thanks, JG.

EDIT - November 8, 2010
After filing a ticket with Media Temple's Awesome Customer Support, they pointed me to a <a href="http://wiki.mediatemple.net/w/GS:Allow_multiple_open_basedir_directories">knowledge base article on configuring multiple open basedir directories</a>.  I haven't had a chance to go through it yet and update the instructions above, but if and when this becomes absolutely necessary to use Drush, I certainly will.
