<?php
// $Id: gmaps-views-view-row-marker-node.tpl.php,v 1.1.2.1 2009/12/03 12:29:46 xmarket Exp $
/**
 * @file gmaps-views-view-row-marker-node.tpl.php
 * Default simple view template to display a single node.
 *
 * Rather than doing anything with this particular template, it is more
 * efficient to use a variant of the node.tpl.php based upon the view,
 * which will be named node-view-VIEWNAME.tpl.php. This isn't actually
 * a views template, which is why it's not used here, but is a template
 * 'suggestion' given to the node template, and is used exactly
 * the same as any other variant of the node template file, such as
 * node-NODETYPE.tpl.php
 *
 * @ingroup views_templates
 */
?>
<?php print $node; ?>
<?php if ($comments): ?>
  <?php print $comments; ?>
<?php endif; ?>