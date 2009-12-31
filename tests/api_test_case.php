<?php
// $Id$

class ApiTestCase extends DrupalWebTestCase {
  function setUp() {
    parent::setUp('api', 'job_queue');

    include drupal_get_path('module', 'api') .'/api.admin.inc';
    include drupal_get_path('module', 'api') .'/parser.inc';

    // Make a branch for sample code.
    $branch->type = 'files';
    $branch->branch_name = '6';
    $branch->title = 'Testing 6';
    $branch->data = array(
      'directories' => drupal_get_path('module', 'api') .'/tests/sample',
    );
    api_save_branch($branch);

    // Parse the code.
    api_update_all_branches();
    while (job_queue_dequeue()) { }
  }
}
