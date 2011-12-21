<?php

/**
 * @file
 * Drush commands for API module.
 */

/**
 * Implementation of hook_drush_help().
 */
function api_drush_help($section) {
  switch ($section) {
    case 'drush:api-reparse':
      return dt('Mark a file, branch, or everything for reparsing.');
  }
}

/**
 * Implementation of hook_drush_command().
 */
function api_drush_command() {
  return array(
    'api-reparse' => array(
      'callback' => 'api_drush_reparse',
      'description' => 'Mark files to reparse.',
      'arguments' => array(
        'branch_or_file' => 'Optional branch identifier or file path relative to the common directory of its branch.',
      ),
    ),
    'api-list-branches' => array(
      'callback' => 'api_drush_list_branches',
      'description' => 'List API branches.',
    ),
  );
}

/**
 * Flag a branch, or all branches, to be reparsed on the next cron run.
 *
 * @param $branch_or_file
 *   (optional) Identifier of the branch to reparse, or name of a single file to
 *   reparse. If omitted all branches will be reparsed. File name must include
 *   the path relative to the common path to the directories indexed by this
 *   branch.
 */
function api_drush_reparse($branch_or_file = NULL) {
  $num = api_mark_for_reparse($branch_or_file);

  drush_log(dt('Marked @number files for reparsing.', array('@number' => $num)), 'ok');
}

/**
 * List all branches.
 */
function api_drush_list_branches() {
  $branches = api_get_branches();
  drush_log(dt('API Branches:'), 'ok');
  $rows = array(array(dt('ID'), dt('Project'), dt('Title'), dt('Name'), dt('Type'), dt('Location'), dt('Status')));
  foreach ($branches as $branch) {
    if (isset($branch->directories)) {
      $location = $branch->directories;
    } else {
      $location = isset($branch->summary) ? $branch->summary : '';
    }
    $rows[] = array(
      $branch->branch_id,
      $branch->project,
      $branch->title,
      $branch->branch_name,
      $branch->type,
      $location,
      $branch->status ? dt('Enabled') : dt('Disabled'),
    );
  }
  drush_print_table($rows, TRUE);
}
