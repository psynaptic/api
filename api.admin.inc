<?php
// $Id$

function api_page_admin_form() {
  $branches = api_get_branches();
  $form = array();

  if (count($branches) > 0) {
    $form['branches'] = array(
      '#theme' => 'api_branch_table',
    );
    foreach ($branches as $branch) {
      $form['branches'][$branch->branch_id]['default_branch'] = array(
        '#type' => 'radio',
        '#default_value' => variable_get('api_default_branch', NULL),
        '#return_value' => $branch->branch_id,
      );
      $form['branches'][$branch->branch_id]['project'] = array(
        '#value' => $branch->project,
      );
      $form['branches'][$branch->branch_id]['title'] = array(
        '#value' => l($branch->title, 'admin/settings/api/branches/'. $branch->branch_id),
      );
      $form['branches'][$branch->branch_id]['branch_name'] = array(
        '#value' => $branch->branch_name,
      );
    }
  }
  else {
    $form['branches'] = array(
      '#value' => '<p><em>'. t('There are no branches yet. You can <a href="@url">add a branch</a>.', array('@url' => url('admin/settings/api/branches/new'))) .'</em></p>',
    );
  }

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save changes'),
  );

  return $form;
}

function theme_api_branch_table($element) {
  $header = array(
    t('Default'),
    t('Project'),
    t('Page label'),
    t('URL label'),
  );
  $rows = array();
  foreach (element_children($element) as $key) {
    $row = array();
    foreach (element_children($element[$key]) as $child) {
      $row[] = drupal_render($element[$key][$child]);
    }
    $rows[] = $row;
  }
  return theme('table', $header, $rows);
}

function api_page_admin_form_submit($form, &$form_state) {
  if (!empty($form_state['values']['default_branch'])) {
    variable_set('api_default_branch', $form_state['values']['default_branch']);
  }

  // We may have menu changes, so clear the menu cache for all users.
  cache_clear_all('*', 'cache_menu', TRUE);

  drupal_set_message(t('Changes saved.'));
}

function api_admin_new_branch_page() {
  return '<h3>' . t('Select branch type') . '</h3>' . theme('links', array(
    'files' => array(
      'title' => t('Files'),
      'href' => 'admin/settings/api/branches/new/files',
    ),
    'php' => array(
      'title' => t('PHP functions'),
      'href' => 'admin/settings/api/branches/new/php',
    ),
  ));
}

function api_branch_edit_form($form_state, $branch_id = NULL) {
  $branches = api_get_branches();
  if (is_numeric($branch_id)) {
    $branch = $branches[$branch_id];
  }
  else {
    $branch->type = $branch_id;
    $branch->project = '';
    $branch->branch_name = '';
    $branch->title = '';
    $branch->directories = '';
    $branch->excluded_directories = '';
    $branch->status = TRUE;
  }
  $form = array(
    '#branch' => $branch,
  );

  $form['project'] = array(
    '#title' => t('Project'),
    '#type' => 'textfield',
    '#default_value' => $branch->project,
    '#required' => TRUE,
  );
  $form['branch_name'] = array(
    '#title' => t('URL label'),
    '#type' => 'textfield',
    '#default_value' => $branch->branch_name,
    '#required' => TRUE,
  );
  $form['title'] = array(
    '#title' => t('Page label'),
    '#type' => 'textfield',
    '#default_value' => $branch->title,
    '#required' => TRUE,
  );
  $form['status'] = array(
    '#title' => t('Show this branch in tabs'),
    '#type' => 'checkbox',
    '#default_value' => $branch->status,
  );

  $form['data'] = array('#tree' => TRUE);
  $function = 'api_branch_edit_form_' . $branch->type;
  $function($form, $branch);

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save branch'),
  );
  if (!is_null($branch_id)) {
    $form[] = array(
      '#value' => l(t('Delete'), 'admin/settings/api/branches/'. $branch->branch_id .'/delete'),
    );
  }

  return $form;
}

function api_branch_edit_form_files(&$form, $branch) {
  $form['data']['directories'] = array(
    '#title' => t('Directories'),
    '#type' => 'textarea',
    '#default_value' => $branch->directories,
    '#rows' => 3,
    '#description' => t('Absolute paths to index, one per line.'),
    '#required' => TRUE,
    '#element_validate' => array('api_check_directories'),
  );
  $form['data']['excluded_directories'] = array(
    '#title' => t('Excluded directories'),
    '#type' => 'textarea',
    '#default_value' => $branch->excluded_directories,
    '#rows' => 3,
    '#description' => t('Absolute paths to exclude from the index, one per line.'),
    '#element_validate' => array('api_check_directories'),
  );
}

function api_branch_edit_form_php(&$form, $branch) {
  $form[] = array('#value' => '<p>' . t('Links to external documentation, such as the PHP manual.') . '</p>');

  $form['data']['summary'] = array(
    '#type' => 'textfield',
    '#title' => t('Function list'),
    '#default_value' => $branch->summary,
    '#required' => TRUE,
    '#description' => t('The URL of the PHP function summary document.'),
  );
  $form['data']['path'] = array(
    '#type' => 'textfield',
    '#title' => t('Function URL'),
    '#default_value' => $branch->path,
    '#required' => TRUE,
    '#description' => t('The URL format used to build the link to php functions.  Use the variable <em>!function</em> in place of the function name.'),
  );

  // PHP manual reference will never be visible in the UI.
  $form['project']['#type'] = 'value';
  $form['project']['#value'] = 'php';
  $form['title']['#type'] = 'value';
  $form['title']['#value'] = 'php';
  $form['status']['#type'] = 'value';
  $form['status']['#value'] = FALSE;
}

function api_branch_edit_form_validate($form, &$form_state) {
  // Check for bad characters in branch names.
  if (preg_match('/[^A-Za-z0-9-_.]/', $form_state['values']['branch_name'])) {
    form_set_error('branch_name', t("Only letters, numbers, '.', '-' and '_' are allowed in the URL label."));
  }

  // Check for duplicate branch names.
  if (db_result(db_query("SELECT 1 FROM {api_branch} WHERE branch_id <> %d AND project = '%s' AND branch_name = '%s'", $form['#branch']->branch_id, $form_state['values']['project'], $form_state['values']['branch_name']))) {
    form_set_error('branch_name', t('%project and %branch_name is already used by another branch.', array('%project' => $form_state['values']['project'], '%branch_name' => $form_state['values']['branch_name'])));
  }
}

function api_check_directories($element, &$form_state) {
  // Check for valid directories.
  foreach (explode("\n", $element['#value']) as $directory) {
    $directory = trim($directory);
    if (!empty($directory)) {
      if (!is_dir($directory)) {
        form_set_error($element['#name'], t('%directory is not a directory.', array('%directory' => $directory)));
      }
      elseif (!is_readable($directory)) {
        form_set_error($element['#name'], t('%directory is not readable by PHP.', array('%directory' => $directory)));
      }
    }
  }
  form_set_value($element, api_clean_directory_list($element['#value']), $form_state);
}

function api_branch_edit_form_submit($form, &$form_state) {
  $branch = $form['#branch'];
  $branch->project = $form_state['values']['project'];
  $branch->branch_name = $form_state['values']['branch_name'];
  $branch->title = $form_state['values']['title'];
  $branch->status = $form_state['values']['status'];
  $branch->data = $form_state['values']['data'];

  api_save_branch($branch);

  drupal_set_message(t('Saved %branch_name.', array('%branch_name' => $branch->branch_name)));
  $form_state['redirect'] = 'admin/settings/api';
}

/**
 * Trim whitespace, trailing directory separators, and remove blank lines.
 */
function api_clean_directory_list($list) {
  $array = explode("\n", $list);
  foreach ($array as $key => $directory) {
    $array[$key] = rtrim(trim($directory), DIRECTORY_SEPARATOR);
    if (empty($array[$key])) {
      unset($array[$key]);
    }
  }
  return implode("\n", $array);
}

function api_branch_delete_form($form_state, $branch_id) {
  $branches = api_get_branches();
  return confirm_form(array('#branch' => $branches[$branch_id]), t('Are you sure you want to delete %branch_name?', array('%branch_name' => $branches[$branch_id]->branch_name)), 'admin/settings/api', NULL, t('Delete'));
}

function api_branch_delete_form_submit($form, &$form_state) {
  $result = db_query("SELECT did FROM {api_documentation} WHERE branch_id = %d", $form['#branch']->branch_id);
  $dids = array();
  while($did = db_fetch_object($result)) {
    $dids[] = $did->did;
  }
  if (count($dids) > 0) {
    $placeholders = db_placeholders($dids);
    db_query("DELETE FROM {api_file} WHERE did IN (". $placeholders .")", $dids);
    db_query("DELETE FROM {api_function} WHERE did IN (". $placeholders .")", $dids);
  }
  db_query("DELETE FROM {api_reference_storage} WHERE branch_id = %d", $form['#branch']->branch_id);
  db_query("DELETE FROM {api_documentation} WHERE branch_id = %d", $form['#branch']->branch_id);
  db_query("DELETE FROM {api_branch} WHERE branch_id = '%s'", $form['#branch']->branch_id);

  if (variable_get('api_default_branch', NULL) == $form['#branch']->branch_id) {
    variable_set('api_default_branch', NULL);
  }

  menu_rebuild();
  $form_state['redirect'] = 'admin/settings/api';
}

function api_comments_settings_form(&$form_state) {
  $type = new stdClass();
  $type->type = 'api';
  $form = array(
    '#node_type' => $type,
    'identity' => array(
      'type' => array(
        '#type' => 'value',
        '#value' => 'api',
      ),
    ),
  );
  comment_form_alter($form, $form_state, 'node_type_form');
  unset($form['comment']['#type']);
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save comment settings'),
  );

  return $form;
}

function api_comments_settings_form_submit($form, &$form_state) {
  foreach ($form_state['values'] as $key => $value) {
    if (strpos($key, 'comment') === 0) {
      variable_set($key . '_api', $value);
    }
  }
  drupal_set_message(t('Saved API comment settings.'));
}
