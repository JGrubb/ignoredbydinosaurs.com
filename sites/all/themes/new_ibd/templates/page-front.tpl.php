<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="<?php print $language->language ?>" lang="<?php print $language->language ?>" dir="<?php print $language->dir ?>">
  <head>
    <?php print $head ?>
    <?php print $styles ?>
    <title><?php print $head_title ?></title>
		<script type="text/javascript" src="http://use.typekit.com/dqg8end.js"></script>
		<script type="text/javascript">try{Typekit.load();}catch(e){}</script>
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
		<div class="container_12">
			<div class="grid_12">
    <?php if ($site_name): ?><div class='logo'><a href="<?php print $base_path ?>"><?php print $site_name ?></a></div><?php endif; ?>
			</div>
			<div id="nav" class="grid_12">
				<?php if ($primary_links): print theme('links', $primary_links, array()); endif;?>
			</div>
		</div>
  </div></div>

  <div id='page'><div class='limiter clear-block'>
		<div class="container_12">
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
	          <div class="grid_6 alpha home-copy">
	            <?php print $content ?>
	          </div>
	          <div class="grid_6 omega">
	            <?php print $right ?>
	          </div>
	          <div class="grid_12 alpha omega">
	            <?php print $content_bottom ?>
	          </div>
	        </div>
	      </div>
	    </div>
		</div>
  </div></div>

  <div id="footer"><div class='limiter clear-block'>
		<div class="container_12">
			<div class="grid_12">
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
