--- 
layout: post
title: Chapter2 - Databases
created: 1238979456
---
This part of the blog is going to be tough to write.  Not because I don't have much to say on the topic, but because I have frequent pangs of "dude, you don't really know anything about any of this stuff".  Then the other side of my brain speaks up. "Dude, it's cool.  They know you're not a professor.  That's the point of this whole thing, right?  If someone wants hard facts and a professionally written tutorial on computer science, they'd probably go somewhere else.  If they're here, they're either a friend or an understanding onlooker, and either way they're probably well aware that you're just a bass player.  Nobody really expects that much from the bass player."  So, I'm on I 70 headed east across Kansas after selling the <em>shit </em>out of the Ogden theater this weekend.

And now, my initial impressions on the theory and practice of the database.

What the hell is a database?  Being a visual kind of thinker, I've been trying to come up with some sort of relatable analogy to help myself understand exactly what it is.

"It's a series of tables, populated with data, and the tables relate to each other."

Okay.  What do you do with it?

"Well, once you have the data in your database, you can run queries on it."

That sounds like a lot of fun!  But what the hell does that mean?

"Well, queries are like a request that you make.  Say you have a database that's populated with every song that RRE has ever played at every show in every venue.  If you wanted to know what you played at a given venue the previous time that you were there, then you could query the database to give you the set list from that show."

Well, that's cool.

"Yeah, but say you wanted to know how many times you've played Mighty River.  You could query the same database and get an answer on that, too.  Say you wanted to know the venues where you played it.  You could query that, too."

Okay, I get it.  So a database is kind of like a big spreadsheet?

"Sort of, but not really.  You see a spreadsheet is basically one big monolithic sheet that stores your data in rows and columns; like a database, but a database is made up of many of those sheets, and the collection of those sheets and the way that they relate to each other, and the queries that you can then make of the data is what makes it a more useful tool than just a big sheet with some numbers on it."

Okay.  So, how about an example?

For this, we shall turn to the web.  Actually, never mind because this is a blog which means that you don't need to turn at all.  Let's press Ctrl-T and get a fresh tab going in your browser and go on over to <a href="http://amazon.com">Amazon</a>.  Mac users have the option of Cmd-clicking the link to open in a new browser tab, so you don't have to navigate away from me.

Okay.  You see all that stuff on the page?  Just about everything there is stored in a big-ass database - all the text, all the ads, all the pictures.  Now, why is this important?  For that, we'll turn to my bookshelf and it's copy of <em>The Complete and Unabridged History of The Internet</em>.  I see here in chapter two after all this non-sense about universities and the DoD that once upon a time there was no such thing as the World Wide Web.  I'll skip to the part where then there was a WWW.  The first websites on the newborn baby web were mostly authored in straight HTML, an interpreted language used to help your computer assemble some data on the other end into a web page.  A skilled HTML coder can control every single element on the page that's being displayed down to the very pixel (until it's opened in another browser, but that's another chapter).  The HTML basically tells your browser what text to display, how to format it, what the background color ought to be, where to place the pictures and to what they should link.  This is understandably tedious and time consuming work, but you gotta do it if you want your website to look right.

Somewhere along the way, websites started getting much more complex.  Some genius decided to make a big online store where you could get pretty much anything, from books to socks to computers to whatever.  Now, imagine if you will, the task of hand coding every page.  For every item and every view and every picture and every review there was some army of poor bastards on the other end furiously coding away to keep up with the inflow of new inventory/users/reviews.  I recently read about some guy who worked for an online newspaper, and he supposedly spent 14 hours a day coding HTML for the site before he figured out that there was a better way to get your steady flow of content up there.

The answer lies in scripting language and a database.

A scripting language is basically the same thing as an interpreted language in as much as it's not compiled, or even thought of until "runtime".  At runtime, the computer gives instructions to something, probably your browser, which then passes the request along to the code.  It's at that time (runtime) that the code is crunched into the binary code that your computer can actually do something with on the processor level.  This is hyper-simplified.

What this means in the Amazon example is that instead of hand-coding every page, why not just build a template that gets populated with the data that's requested at the time?  Then you don't have to rebuild the wheel every single time a page request is made.  Well, how do you build a template?  With a scripting language, of course!

I'll just cut to the bird-eye view.  When you go to Amazon, and it knows that it's you, and it spits out all that stuff that you should buy based on past purchases, that's because all of that info is stored in a database.  When you click on an item and go to that items page, and the page looks just like the last one, that's because all that info is stored in a database.  There's a table in there for every one of you, and for every item on the site.  The table for you has all of your info in it, including past purchases, addresses, billing info, etc.  The table for each item has the description, pictures, reviews, links, etc.

The point is that each of these tables is filled out in a standard way with all of their pertinent data.  Why this is cool is that some automated process can then do something with it, like fill out a web page, or display your customer profile.  The scripting language is what automates the process.  It provides the instructions for the web server on the other end of your computer to "dynamically" make up the HTML needed to display the pictures and profile info and whatever else in a cohesive way across the entire website.  Dynamically means that you don't need an army of HTMLers because the computer is writing the HTML at the time of the page request.  Every different kind of page on the site (say reviews vs. the home page) has it's own scripted template that's called up when the appropriate request is made.  Kinda cool, no?

So, a database is somewhere between MS Excel and that big Tupperware bin where you keep all your summer clothes.  We'll dive into the beauty of doing things this way later.

A few database driven websites:

<a href="http://www.archive.org/search.php?query=collection%3ARailroadEarth&amp;sort=-date">Archive.org</a>
<a href="http://twitter.com">Twitter</a>
<a href="http://ignoredbydinosaurs.com/blog">This blog</a>
<a href="http://messageboard.tapeop.com/index.php?sid=d5ce53c0f1b60db5b462e65e841b6cfa">Any message forum</a>

Extra credit : Start paying attention to URLs, especially whenever the letters .php are used.  PHP is a very widely used scripting language for making database driven websites, and if .php is included somewhere on your address bar, you're looking at data that's stored in a database.
