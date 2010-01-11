<?php print $documentation ?>

<?php if (!empty($see)) { ?>
<h3><?php print t('See also') ?></h3>
<?php print $see ?>
<?php } ?>

<?php print $defined; ?>
<?php print $code; ?>

<?php if (!empty($related_topics)) { ?>
<h3><?php print t('Related topics') ?></h3>
<?php print $related_topics ?>
<?php } ?>
