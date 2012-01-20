<?php
/**
 * @file
 * Theme function implementations for the API module.
 */

/**
 * Theme function to generate reference links on function pages.
 *
 * @param $type
 *   The type of reference link i.e., calls or implementations.
 * @param $count
 *   The number of referenced items.
 * @param $branches
 *   An array of branches.
 * @param $function
 *  The function object being referenced.
 *
 * @return
 *   Markup for a link to the reference listing pages.
 */
function theme_api_function_reference_link($type, $count, $branches, $function) {
  // Create the link title.
  $action = ($type == 'calls') ? format_plural($count, 'calls', 'call') : format_plural($count, 'implements', 'implement');
  $link_title = format_plural($count, '1 function @action @name()', '@count functions @action @name()', array('@action' => $action, '@name' => $function->title));

  // Create the link path.
  $replaced_string = str_replace(API_FILEPATH_SEPARATOR, API_FILEPATH_SEPARATOR_REPLACEMENT, $function->file_name);
  $link_path = 'api/'. $branches[$function->branch_id]->project .'/'. $replaced_string .'/function/'. $type .'/'. $function->object_name .'/'. $branches[$function->branch_id]->branch_name;

  return '<h3>'. l($link_title, $link_path) .'</h3>';
}

