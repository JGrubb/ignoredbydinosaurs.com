<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="<?php print $language->language ?>" lang="<?php print $language->language ?>" dir="<?php print $language->dir ?>">
  <head>
    <?php print $head ?>
    <?php print $styles ?>
    <!--[if IE]>
      <link type="text/css" rel="stylesheet" media="all" href="/sites/all/themes/ibd/ie.css" />
    <![endif]-->
    <title><?php print $head_title ?></title>
  </head>
  <body <?php print drupal_attributes($attr) ?>>
  <div id="header">
  <div class="container_12">
  	<div class="grid_12">

  <?php print $skipnav ?>

  <?php if ($help || ($show_messages && $messages)): ?>
    <div id='console'><div class='limiter clear-block'>
      <?php print $help; ?>
    </div></div>
  <?php endif; ?>

  <?php if ($header): ?>
    <div id='header-region'><div class='limiter clear-block'>
      <?php print $header; ?>
    </div></div>
  <?php endif; ?>

  <div id='branding'><div class='limiter clear-block'>
    <?php if ($site_name): ?><h1 class='site-name'><?php print $site_name ?></h1><?php endif; ?>
    <?php if ($search_box): ?><div class="block block-theme"><?php print $search_box ?></div><?php endif; ?>
  </div></div>
  	</div>
  </div>
  </div>

  <div id='page'><div class='limiter clear-block container_12'>
  
    <div id='navigation'><div class='limiter clear-block'>
    <?php if (isset($primary_links)) : ?>
      <?php print theme('links', $primary_links, array('class' => 'links primary-links')) ?>
    <?php endif; ?>
    <?php if (isset($secondary_links)) : ?>
      <?php print theme('links', $secondary_links, array('class' => 'links secondary-links')) ?>
    <?php endif; ?>
  </div></div>
  
    <?php if ($content_top): ?>
      <div id='content-top' class='grid_12'><?php print $content_top ?></div>
    <?php endif; ?>

    <?php if ($left): ?>
      <div id='left' class='clear-block grid_4'><?php print $left ?></div>
    <?php endif; ?>
    
	<?php if ($show_messages && $messages): print $messages; endif; ?>
	
	<?php if ($is_front): ?>
    <div id='main' class='clear-block grid_8'>
        <?php if ($mission): print '<div id="mission">'. $mission .'</div>'; endif; ?>
        <?php if ($title): ?><h1 class='page-title'><?php print $title ?></h1><?php endif; ?>
        <?php if ($tabs) print $tabs ?>
        <?php if ($tabs2) print $tabs2 ?>
        <div id='content' class='clear-block'><?php print $content ?></div>
    </div>
    <?php else: ?>
    <div id='main' class='clear-block grid_8'>
        <?php if ($mission): print '<div id="mission">'. $mission .'</div>'; endif; ?>
        <?php if ($title): ?><h1 class='page-title'><?php print $title ?></h1><?php endif; ?>
        <?php if ($tabs) print $tabs ?>
        <?php if ($tabs2) print $tabs2 ?>
        <div id='content' class='clear-block'><?php print $content ?></div>
    </div>
    <?php endif; ?>

    <?php if ($right): ?>
      <div id='right' class='clear-block grid_4'><?php print $right ?></div>
    <?php endif; ?>
  </div></div>
	  

  <div id="footer"><div class='limiter clear-block container_12'>
    <div class="grid_12">
    <?php print $feed_icons ?>
    <?php print $footer ?>
    <?php print $footer_message ?>
    </div>
  </div></div>

  <?php print $scripts ?>
  <script type="text/javascript" src="http://use.typekit.com/dqg8end.js"></script>
  <script type="text/javascript">try{Typekit.load();}catch(e){}</script>
  <script type="text/javascript" src="/new-ibd/sites/all/themes/ibd/js/twitter.js"></script>
  <?php print $closure ?>

  </body>
</html>
