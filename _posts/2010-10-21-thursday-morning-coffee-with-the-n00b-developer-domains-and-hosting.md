--- 
layout: post
title: Thursday morning coffee with the n00b developer - domains and hosting
created: 1287664919
permalink: /2010/10/thursday-morning-coffee-n00b-developer-domains-and-hosting
---
<p>Welcome back to another installment of the newby web developer series.  This morning's entry comes once again from our buddy Jimmy, who the two of you reading might know as the highly technically inclined CT robo-teacher.  He'll probably be reforming the state of education in your state soon.  </p>

<blockquote>What's up fool.

My friend wants me to build him a simple site using wordpress. He already owns the domain name.

My question is, once I build the site using MAMP, and get everything upload to our host, how does the domain name get transferred? Is that something we have to handle or the host handles.

Thanks,
Jimmy</blockquote>

<p>Calling me a fool is his way of showing affection.  So, the answer is that "it depends".  If your buddy went the "get your domain free with a year's webhosting" route that many larger webhosts encourage you to do, then you don't have to do anything.  If your buddy bought the domain on the open market through a registrar such as GoDaddy, you have to dive into the scary, subterranean world of DNS.</p>

<p>DNS stands for Domain Name System.  A perhaps less-than-100%-accurate description --</p>

<p>When you buy a domain name, it's sort of like buying a spot in the world's internet phone book.  That is to say that the address <a href="http://johnnygrubb.com/">http://johnnygrubb.com</a> is actually an "abstraction" of the information that a computer needs to complete your browser's request to get my webpage to come up.  What my computer needs to know is the IP address of the server where http://johnnygrubb.com lives.  The Domain Name System does that for you.</p>  

<p>If you need to look up the phone number of Newtown Chinese Restaurant in Newton, you go to the phone book, look for Newtown Chinese Restaurant and move your eye to the right to get the phone number.  If you want to go to any website, you enter the address in your browser, which takes the request and hits the nearest DNS server to find the actual IP address of the webhost so the requested page can be grabbed.  It's the job of the owner of the domain name to make sure that the "phone number" or IP address is correct.</p>

<p>If you've bought your domain name as part of a package, then you don't need to do anything.  The webhost actually owns the domain name and is letting you use it.  This has pros and cons obviously, the pros being that they handle all of this DNS stuff for you.</p>  

<p>If you've bought the domain name separately, you need to give that name a "phone number" so that DNS knows who to call when you request a page from that site.  That's done by getting deep into your Domain Control Panel (or something like depending on the registrar) and and finding the "Nameserver" controls.  This is where you give your domain name an address.  It's usually in an english readable form, for instance, one of the nameservers for my hosting account is <code>ns1.mediatemple.com</code>.  That means that when a request is made for a page on this site, DNS says "don't look at me, go talk to Media Temple's nameservers.  They know where to find that page."  Once the request gets to Media Temple, their nameservers say "the page you seek is at 64.207.129.18", which is the IP address of the actual server on which this site is plopped.</p>

<p>So, to extend the analogy a little further, a DNS lookup is actually more like that movie where the kids are trying to find the super-cool party, only to get to the party they have to drive all over town from one destination to the next before they are finally given the actual address.  For real fun, type this is your Terminal --</p>

<code>traceroute http://ignoredbydinosaurs.com</code>

<p>This command will show you all of the different "nodes" out there on the web that your request hits before it actually arrives at it's destination, my webhost.</p>

<p>So, the tl;dr is that if he bought the domain name through a webhost as part of a hosting package, you don't need to do anything.  If he bought the domain name on it's own, you need to go to the Domain Control Panel or the Domain Manager or whatever it's called where it was registered.  In there somewhere is a setting for the nameservers.  On the webhost's end, you'll find what their nameservers are (usually something like ns1.awesomewebhost.com and ns2.awesomewebhost.com) and put that information into the proper fields on the registrar end.  There's always at least 2 nameservers at every host because like all servers, they can go down and if they both go down your site will be unavailable.  It happens.</p>

<p>Changing nameserver information is like firing an MX missile.  There is no "undo".  There is no recalling the missile.  Once you push the button to change the nameservers, that change of address starts filtering out to the rest of the world's DNS servers so that the request can be sent to the proper IP address.  This filtering-out process can take anywhere from a few hours to a few days, depending.  Once it's done, the whole world will be pointed to the party on your server.</p>
