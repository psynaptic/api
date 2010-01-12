<?php if (!empty($branch)) { ?>
  <h3><?php print l(t('Topics'), 'api/' . $branch->project . '/groups/' . $branch->branch_name); ?></h3>
  <?php print api_page_listing($branch, 'group', FALSE); ?>
  <h3><?php print l(t('Files'), 'api/' . $branch->project . '/files/' . $branch->branch_name); ?></h3>
  <h3><?php print l(t('Globals'), 'api/' . $branch->project . '/globals/' . $branch->branch_name); ?></h3>
  <h3><?php print l(t('Constants'), 'api/' . $branch->project . '/constants/' . $branch->branch_name); ?></h3>
  <h3><?php print l(t('Functions'), 'api/' . $branch->project . '/functions/' . $branch->branch_name); ?></h3>
  <h3><?php print l('API Search', 'api/search/' . $branch->branch_name); ?></h3>
  <?php print api_switch_project($branch); ?>
<?php } ?>

<p class="api-no-mainpage"><em><?php print t('A main page for this branch has not been indexed. A documentation comment with <code>@mainpage {title}</code> needs to exist, or has not been indexed yet. For Drupal core, this is available in the <a href="http://cvs.drupal.org/viewvc.py/drupal/contributions/docs/developer/">developer documentation</a> in the contributions repository.'); ?></em></p>
