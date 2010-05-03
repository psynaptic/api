<?php
// $Id$

/**
 * @file api-file-page.tpl.php
 * Theme implementation for the summary page of a file.
 *
 * Available variables:
 * - $documentation: Documentation from the comment header of the file.
 * - $file: Object with information about the file.
 * - $is_admin: True or false.
 * - $logged_in: True or false.
 * - $user: user object.
 *
 * Available variables in the $file object:
 * - $file->title: Display name.
 * - $file->summary: Short summary.
 * - $file->documentation: HTML formatted comments.
 * - $file->code: HTML formatted source code.
 * - $file->classes: List of classes, if any.
 * - $file->constants: List of constants, if any.
 * - $file->globals: List of globals, if any.
 * - $file->functions: List of functions, if any.
 * - $file->see: Related api objects.
 */
?>
<?php if (!empty($file->version)) { ?>
<p><?php print t('Version') ?> <?php print $file->version ?></p>
<?php } ?>

<?php print $documentation ?>

<?php if (!empty($see)) { ?>
<h3><?php print t('See also') ?></h3>
<?php print $see ?>
<?php } ?>

<?php if (!empty($interfaces)) { ?>
<h3><?php print t('Interfaces') ?></h3>
<?php print $interfaces ?>
<?php } ?>
<?php if (!empty($classes)) { ?>
<h3><?php print t('Classes') ?></h3>
<?php print $classes ?>
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
<?php if (!empty($related_topics)) { ?>
<h3><?php print t('Related topics') ?></h3>
<?php print $related_topics ?>
<?php } ?>
