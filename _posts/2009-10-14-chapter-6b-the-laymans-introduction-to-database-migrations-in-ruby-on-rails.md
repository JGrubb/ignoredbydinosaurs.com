--- 
layout: post
title: Chapter 6b - the layman's introduction to database migrations in Ruby on Rails
created: 1255569570
---
This is not for programmers.  This is for myself, because when I first started poking at Rails 6 months ago, I didn't have any idea why I needed to edit a migration file, much less what a migration was, except that it must have something to do with a database.  I only knew that because of the command

`$>rake db:migrate`

that I was told to perform and the fact that it had the letters "db" in it.

So here's the deal.  This morning I talked about <a href="http://ignoredByDinosaurs.com/2009/10/chapter-6a-version-control/">version control</a> and it's place in the life cycle of your application.  What version control does is keep track of the changes that you make to your applications source code from start to finish (whenever that is).  The only problem with this is that a big chunk of the data <em>and</em> functionality of a modern web application doesn't reside in the app's source code, it resides in the database, and nobody has invented an efficient version control system for a database yet.  Some friends of mine have tipped me off to different methods that they've used - mostly around writing down DB schema changes into a text file that then gets placed under version control with the rest of the source.  That's not entirely efficient, though, is it?

Ruby on Rails has come up with a system called "migrations".  Actually, someone surely came up with this way before Rails was invented, but I'm new here, so feel free to correct me in the comments section down there.  Let's say you create a new rails app (This is directly from the awesome book "<a href="http://www.amazon.com/Agile-Web-Development-Rails-Third/dp/1934356166/ref=sr_1_1?ie=UTF8&s=books&qid=1255546866&sr=8-1">Agile Web Development with Rails</a>" by a bunch of smart folks, and this example is the beginning of building a shopping cart system) :

`$>rails newapp`

This generates a load of boilerplate code which makes up the bones of any Rails app.  That means that instead of spending the first week of the development cycle sitting there writing low-level code that provides primordial programmatic structure, you can spend it writing code that actually does something that you (or your client) can interact with.  Let's keep it as simple as possible and say that you are going to have three fields that will be stored in the database to start. Rails let's you run this scaffold generating command:

`$>ruby script/generate scaffold product title:string description:text image_url:string`

This command runs a ruby script that builds a LOT of code for you, so much that you're only minutes away from tinkering around and you only started a few minutes before that.    This command creates the "products model", which is how a programmer says that they are going to start by creating some place for the admin to store the products that they want to sell online.  The fields that were initially created were for the name of the product, the description, and the URL where you're storing the picture of the product.  Don't worry about it too much, but take a look at the command up there, it'll make sense if you know what a string is.  Just know that the data entered into these fields is going to get stored in a database.  Wait, how are we gonna store this stuff in our database?  Have we even created the tables for the database yet?  No.  Rails does it for us.  Here's how.

One of the files that is created when we run that scaffold generator is called a "migration file".  This migration file is what actually creates the tables in the database, the columns in the tables, and can even populate the tables with data (used for testing), depending on what you do to the migration file.  There is a command (or method if you're OOP savvy) created when the scaffold is run called the "up method".  That's the one that changes your database.  There is also a "down method", which is what undoes the changes to your database.  When you run the good old `rake db:migrate` command Rails goes applies the appropriate changes to your database for you, and provides a way to undo them later.  So, your client comes in and wants to add a Price column to the store.  In the old days, you'd execute this SQL statement :

`ALTER TABLE products ADD column price DECIMAL (8,2) NOT NULL;`

Now, what if you wanted to undo that?  That's a command you ran there, not some source code that you can save and delete later if you change a few things.  You can't place SQL statements under version control.  You'd have to go and type that statement into the text file that was under version control, and then hope that you could effectively backtrack to it later if you wanted.  If your application and it's database have come a ways since that command you're going to have a good time figuring out how to undo it.  Rails doesn't make you do that.  In fact, you almost never are going to talk directly to your database in Rails.  Instead, you go to the most recent migration file (which <em>is</em> source code and thus under version control) and alter a line to look like this:

`add_column :products, :price, :decimal,
:precision => 8, :scale => 2, :default => 0`

Then you run a script that alters the database for you.  Below this add column instruction, or "up method", you add the remove column instruction, or "down method":

`remove_column :products, :price`

This is the undo button for your database and any schema changes along the way.  Since this command is under version control, and since you're not directly interacting with your database, and since Rails knows how and which migrations to apply and in what order if you ever want to go back to a previous version, you've just made your life a LOT easier.  Now if you only understood what the hell you were talking about!!

For a vastly better explanation, <a href="http://guides.rubyonrails.org/migrations.html">try here</a>.  

Good luck.
