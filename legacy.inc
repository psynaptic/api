<?php

/**
 * Redirect deprecated API <=1.1 links.
 *
 * @param menu_tail
 *   One or more menu parts that follows the old style api/file/ path.
 */
function api_file_redirect() {
  $branches = api_get_branches();
  $branch = $branches[variable_get('api_default_branch', NULL)];

  $args = func_get_args();
  $suffix = '';
  $file = new StdClass();
  $file->object_type = 'file';
  $file->branch = $branch;
  $file->branch_name = $branch->branch_name;

  $end = count($args) - 1;
  if (in_array($args[$end], array('source', 'documentation'))) {
    $suffix = '/' . $args[$end];
    $end -= 1;
  }
  foreach ($branches as $b) {
    if ($b->branch_name === $args[$end]) {
      $branch = $b;
      $file->branch_name = $args[$end];
      $end -= 1;
      break;
    }
  }
  $file->object_name = implode('/', array_slice($args, 0, $end + 1));

  drupal_goto(api_url($file) . $suffix);
}
