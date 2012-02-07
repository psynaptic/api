<?php

/**
 * Provides a base class for testing the API module.
 *
 * Sets up a branch with the sample files, and parses this branch and the
 * default PHP function branch. Note that the branch is set up using the
 * api_save_branch() function directly, not using the admin interface.
 */
class ApiTestCase extends DrupalWebTestCase {
  private $default_branch;

  /**
   * Overrides DrupalWebTestCase::setUp().
   *
   * Sets up the sample branch and updates this and the default PHP branch.
   */
  function setUp() {
    $this->default_branch = variable_get('api_default_branch', NULL);
    variable_del('api_default_branch');
    parent::setUp('job_queue', 'grammar_parser', 'api');

    include drupal_get_path('module', 'api') .'/api.admin.inc';
    include drupal_get_path('module', 'api') .'/parser.inc';

    // Make a branch for sample code.
    $branch = new stdClass();
    $branch->type = 'files';
    $branch->project = 'test';
    $branch->branch_name = '6';
    $branch->title = 'Testing 6';
    $branch->status = 1;
    $branch->data = array(
      'directories' => drupal_get_path('module', 'api') .'/tests/sample',
      'excluded_directories' => drupal_get_path('module', 'api') .'/tests/sample/to_exclude',
    );
    api_save_branch($branch);
    variable_set('api_default_branch', $branch->branch_id);

    // Parse the code.
    api_update_all_branches();
    while (job_queue_dequeue()) { }
    api_shutdown();

    cache_clear_all('variables', 'cache');
    variable_init();
    api_get_branches(TRUE);
    menu_rebuild();
    $this->assertEqual(variable_get('api_default_branch', 99), $branch->branch_id, 'Variable for default branch is set correctly');
  }

  /**
   * Overrides DrupalWebTestCase::tearDown().
   *
   * Ensures that the default branch doesn't get overridden by tests.
   */
  function tearDown() {
    parent::tearDown();
    // Aparently SimpleTest is leaky sometimes.
    variable_set('api_default_branch', $this->default_branch);
  }

  /**
   * Returns the branch set up by this base testing class.
   */
  function getBranch() {
    $branches = api_get_branches();
    return reset($branches);
  }
}
