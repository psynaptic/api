<?php
// $Id$

/**
 * @file api-group-page.tpl.php
 * Theme implementation for summarizing code that in a group.
 *
 * Available variables:
 * - $classes: List of classes, if any.
 * - $constants: List of constants, if any.
 * - $globals: List of globals, if any.
 * - $functions: List of functions, if any.
 * - $files: List of functions, if any.
 * - $defined: HTML reference to file that defines this group.
 * - $see: Related api objects.
 */
?>
<?php print $documentation ?>

<?php if (!empty($see)) { ?>
<h3><?php print t('See also') ?></h3>
<?php print $see ?>
<?php } ?>

<?php if (!empty($constants)) { ?>
<h3><?php print t('Constants') ?></h3>
<?php print $constants ?>
<?php } ?>
<?php if (!empty($globals)) { ?>
<h3><?php print t('Globals') ?></h3>
<?php print $globals ?>
<?php } ?>
<?php if (!empty($functions)) { ?>
<h3><?php print t('Functions') ?></h3>
<?php print $functions ?>
<?php } ?>
<?php if (!empty($files)) { ?>
<h3><?php print t('Files') ?></h3>
<?php print $files ?>
<?php } ?>

<?php print $defined; ?>
