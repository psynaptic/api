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
