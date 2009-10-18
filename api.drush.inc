<?php
// $Id$

function api_drush_help($section) {
  switch ($section) {
    case 'drush:api reparse':
      return dt('Mark a file, branch, or everything for reparsing.');
  }
}

function api_drush_command() {
  return array(
    'api reparse' => array(
      'callback' => 'api_drush_reparse',
      'description' => 'Mark files to reparse.',
      'arguments' => array(
        'branch_or_file' => 'Optional branch name or file path.',
      ),
    ),
  );
}

function api_drush_reparse($branch_or_file = NULL) {
  if (is_null($branch_or_file)) {
    // Reparse all.
    db_query("UPDATE {api_file} SET modified = 52");
  }
  else {
    $branch_id = db_result(db_query("SELECT branch_id FROM {api_branch} WHERE branch_name = '%s'", $branch_or_file));
    if (!empty($branch_id)) {
      // Reparse a branch.
      db_query("UPDATE {api_file} f INNER JOIN {api_documentation} d ON d.object_type = 'file' AND d.did = f.did SET f.modified = 52 WHERE d.branch_id = %d", $branch_id);
    }
    else {
      // Reparse a file.
      db_query("UPDATE {api_file} f INNER JOIN {api_documentation} d ON d.object_type = 'file' AND d.did = f.did SET f.modified = 52 WHERE d.file_name = '%s'", $branch_or_file);
    }
  }
  drush_log(dt('Marked @number files for reparsing.', array('@number' => db_affected_rows())), 'ok');
}
