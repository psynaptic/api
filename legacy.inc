<?php

function api_file_redirect() {
  $args = func_get_args();
  $suffix = '';
  $file = new StdClass();
  $file->object_type = 'file';
  $file->branch_name = db_result(db_query("SELECT branch_name FROM {api_branch} WHERE branch_id = %d", variable_get('api_default_branch', NULL)));

  $end = count($args) - 1;
  if (in_array($args[$end], array('source', 'documentation'))) {
    $suffix = '/' . $args[$end];
    $end -= 1;
  }
  if (db_result(db_query("SELECT 1 FROM {api_branch} WHERE branch_name = '%s'", $args[$end]))) {
    $file->branch_name = $args[$end];
    $end -= 1;
  }
  $file->object_name = implode('/', array_slice($args, 0, $end + 1));

  drupal_goto(api_url($file) . $suffix);
}
