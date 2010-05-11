<?php
// $Id$

/**
 * @file api-class-page.tpl.php
 * Theme implementation to display a class overview.
 *
 * Available variables:
 * - $documentation: Documentation from the comment header of the class.
 * - $branch: Object with information about the branch.
 * - $class: Object with information about the class.
 * - $defined: HTML reference to file that defines this class.
 * - $is_admin: True or false.
 * - $logged_in: True or false.
 *
 * Available variables in the $branch object:
 * - $branch->project: The machine name of the branch.
 * - $branch->title: A proper title for the branch.
 * - $branch->directories: The local included directories.
 * - $branch->excluded_directories: The local excluded directories.
 *
 * Available variables in the $class object.
 * - $class->title: Display name.
 * - $class->object_type: For this template it will be 'class'.
 * - $class->branch_id: An identifier for the branch.
 * - $class->file_name: The path to the file in the source.
 * - $class->summary: A one-line summary of the object.
 * - $class->code: Escaped source code.
 * - $class->see: HTML index of additional references.
 *
 * @see api_preprocess_api_class_page().
 */
?>
<?php print $documentation ?>

<?php if (!empty($see)) { ?>
  <h3><?php print t('See also') ?></h3>
  <?php print $see ?>
<?php } ?>

<?php if (!empty($hierarchy)) { ?>
  <h3><?php print t('Hierarchy') ?></h3>
  <?php print $hierarchy ?>
<?php } ?>

<?php if (!empty($interfaces)) { ?>
  <h3><?php print t('Implemented Interfaces') ?></h3>
  <?php print $interfaces ?>
<?php } ?>

<?php if (!empty($constants)) { ?>
  <h3><?php print t('Constants') ?></h3>
  <?php print $constants ?>
<?php } ?>

<?php if (!empty($inherited_constants)) { ?>
  <h3><?php print t('Inherited Constants') ?></h3>
  <?php print $inherited_constants ?>
<?php } ?>

<?php if (!empty($properties)) { ?>
  <h3><?php print t('Properties') ?></h3>
  <?php print $properties ?>
<?php } ?>

<?php if (!empty($inherited_properties)) { ?>
  <h3><?php print t('Inherited Properties') ?></h3>
  <?php print $inherited_properties ?>
<?php } ?>

<?php if (!empty($methods)) { ?>
  <h3><?php print t('Methods') ?></h3>
  <?php print $methods ?>
<?php } ?>

<?php if (!empty($inherited_methods)) { ?>
  <h3><?php print t('Inherited Methods') ?></h3>
  <?php print $inherited_methods ?>
<?php } ?>

<?php print $defined; ?>
<?php print theme('ctools_collapsible', t('View code'), $code, $collapsed = TRUE) ?>

<?php if (!empty($related_topics)) { ?>
  <h3><?php print t('Related topics') ?></h3>
  <?php print $related_topics ?>
<?php } ?>
