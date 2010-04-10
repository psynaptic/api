<table id="api-function-signature">
  <thead>
    <tr><th class="branch"><?php print t('Versions') ?></th><th></th></tr>
  </thead>
  <tbody>
    <?php foreach ($signatures as $branch => $signature) { ?>
      <?php if ($signature['active']) { ?>
        <tr class="active">
          <td class="branch"><?php print $branch ?></td>
          <td class="signature"><code><?php print $signature['signature'] ?></code></td>
        </tr>
      <?php } else if ($signature['status']) { ?>
        <tr>
          <td class="branch"><?php print l($branch, $signature['url']) ?></td>
          <td class="signature"><code><?php print l($signature['signature'], $signature['url'], array('html' => TRUE)) ?></code></td>
        </tr>
      <?php } else { ?>
        <tr>
          <td class="branch"><?php print $branch ?></td>
          <td class="signature"><code><?php print $signature['signature'] ?></code></td>
        </tr>
      <?php } ?>
    <?php } ?>
  </tbody>
</table>

<?php print $documentation ?>

<?php if (!empty($parameters)) { ?>
<h3><?php print t('Parameters') ?></h3>
<?php print $parameters ?>
<?php } ?>

<?php if (!empty($return)) { ?>
<h3><?php print t('Return value') ?></h3>
<?php print $return ?>
<?php } ?>

<?php if (!empty($see)) { ?>
<h3><?php print t('See also') ?></h3>
<?php print $see ?>
<?php } ?>

<?php if (!empty($related_topics)) { ?>
<h3><?php print t('Related topics') ?></h3>
<?php print $related_topics ?>
<?php } ?>

<?php print $call ?>

<h3><?php print t('Code'); ?></h3>
<?php print $defined; ?>
<?php print $code; ?>
