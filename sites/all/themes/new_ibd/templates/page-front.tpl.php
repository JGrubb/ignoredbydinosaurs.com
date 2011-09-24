<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="<?php print $language->language ?>" lang="<?php print $language->language ?>" dir="<?php print $language->dir ?>">
  <head>
    <?php print $head ?>
    <?php print $styles ?>
    <title><?php print $head_title ?></title>
    <meta name=viewport content="width=device-width,initial-scale=1">
    <script type="text/javascript">

      var _gaq = _gaq || [];
      _gaq.push(['_setAccount', 'UA-8646459-1']);
      _gaq.push(['_trackPageview']);

      (function() {
        var ga = document.createElement('script'); ga.type = 'text/javascript'; ga.async = true;
        ga.src = ('https:' == document.location.protocol ? 'https://ssl' : 'http://www') + '.google-analytics.com/ga.js';
        var s = document.getElementsByTagName('script')[0]; s.parentNode.insertBefore(ga, s);
      })();

    </script>
  </head>
  <body <?php print drupal_attributes($attr) ?>>

  <?php print $skipnav ?>

  <?php if ($header): ?>
    <div id='header'><div class='limiter clear-block'>
			<div class="container_12">
				<div class="grid_12">
      		<?php print $header; ?>
				</div>
			</div>
    </div></div>
  <?php endif; ?>

  <div id='branding'><div class='limiter clear-block'>
		<div class="wrap">
			<div>
    <?php if ($site_name): ?><div class='logo'><a href="<?php print $base_path ?>"><?php print $site_name ?></a></div><?php endif; ?>
			</div>
			<div id="nav">
				<?php if ($primary_links): print theme('links', $primary_links, array()); endif;?>
			</div>
		</div>
  </div></div>

  <div id='page'><div class='limiter clear-block'>
		<div class="wrap">
	    <div id='main' class='clear-block grid_12'>
				<?php if ($help || ($show_messages && $messages)): ?>
			    <div id='console'><div class='limiter clear-block'>
			      <?php print $help; ?>
			      <?php if ($show_messages && $messages): print $messages; endif; ?>
			    </div></div>
			  <?php endif; ?>
	        <?php if ($tabs) print $tabs ?>
	        <?php if ($tabs2) print $tabs2 ?>
	        <div id='content' class='clear-block'>
	          <div class="home-copy">
	            <?php print $content ?>
	          </div>
	          <div class="slideshow">
	            <?php print $right ?>
	          </div>
	          <div class="clear"></div>
	          <div class="wrap">
	            <?php print $content_bottom ?>
	          </div>
	        </div>
	      </div>
	    </div>
		</div>
  </div></div>

  <div id="footer"><div class='limiter clear-block'>
		<div class="wrap">
			<div>
		    <?php print $feed_icons ?>
		    <?php print $footer ?>
		    <?php print $footer_message ?>
		    <p id="copyright">All content &copy; <?php print date('Y'); ?> Ibd Web Development</p>
			</div>
		</div>
  </div></div>
  <?php print $scripts ?>
  <?php print $closure ?>

  </body>
</html>
