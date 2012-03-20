<?php

/**
 * @file
 * Theme implementation: shows a list of functions, with source and description.
 *
 * Available variables:
 * - $function['function']: Function link.
 * - $function['file']: File link.
 * - $function['description']: Function description.
 */
?>
<dl class="api-functions">
<?php foreach ($functions as $function) { ?>
  <dt><?php print $function['function'] ?> <small>in <?php print $function['file'] ?></small></dt>
  <dd><?php print $function['description'] ?></dd>
<?php } ?>
</dl>
