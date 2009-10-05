<?php
// $Id$

/**
 * @file
 * The PHP documentation parser that generates content for api.module.
 */

/**
 * Parse out function definitions from the PHP manual.
 */
function api_parse_php_manual($location) {
  $response = drupal_http_request($location);
  if ($response->code == 200) {
    $function_matches = array();
    preg_match_all('!^[a-zA-Z0-9_]+ ([a-zA-Z0-9_]+)\(.*\n.*$!m', $response->data, $function_matches, PREG_SET_ORDER);
    foreach ($function_matches as $function_match) {
      $docblock = array(
        'object_name' => $function_match[1],
        'branch_name' => 'php',
        'object_type' => 'function',
        'file_name' => $location,
        'title' => $function_match[1],
        'summary' => api_documentation_summary($function_match[0]),
        'documentation' => $function_match[0],
      );
      api_save_documentation(array($docblock));
    }
  }
}

function api_parse_file($callback, $file_path, $branch_name, $file_name) {
  $docblock = array(
    'object_name' => $file_name,
    'branch_name' => $branch_name,
    'object_type' => 'file',
    'file_name' => $file_name,
    'title' => strpos($file_name, '/') ? substr($file_name, strrpos($file_name, '/') + 1) : $file_name,
    'documentation' => '',
    'version' => '',
    'modified' => filemtime($file_path),
    'source' => str_replace(array("\r\n", "\r"), array("\n", "\n"), file_get_contents($file_path)),
  );

  $match = array();
  if (preg_match('!\$'.'Id: .*?,v (.*?) (.*?) (.*?) (.*?) Exp \$!', $docblock['source'], $match)) {
    $docblock['version'] = $match[1] .' (checked in on '. $match[2] .' at '. $match[3] .' by '. $match[4] .')';
  }

  $callback($docblock);
}

/**
 * Read in the file at the given path and parse it as if it consisted entirely of
 * documentation.
 */
function api_parse_text_file($docblock) {
  $docblock['documentation'] = api_format_documentation($docblock['source'], $docblock['branch_name']);
  $docblock['code'] = api_format_php($docblock['source']);

  api_save_documentation(array($docblock));
}

/**
 * Read in the file at the given path and parse it as an HTML file.
 */
function api_parse_html_file($docblock) {
  $docblock['code'] = '<pre>' . htmlspecialchars($docblock['source']) . '</pre>';
  $title_match = array();
  if (preg_match('!<title>(.*)</title>!is', $docblock['source'], $title_match)) {
    $docblock['title'] = trim($title_match[1]);
  }
  $documentation_match = array();
  if (preg_match('!<body>(.*?</h1>)?(.*)</body>!is', $docblock['source'], $documentation_match)) {
    $docblock['documentation'] = $documentation_match[2];
  }

  api_save_documentation(array($docblock));
}

/**
 * Read in the file at the given path and parse its documentation.
 */
function api_parse_php_file($docblock) {
  $docblock['code'] = api_format_php($docblock['source']);
  $docblocks = array($docblock);

  $docblock_matches = array();
  preg_match_all('!/\*\*(.*?)\*/!s', $docblocks[0]['source'], $docblock_matches, PREG_SET_ORDER | PREG_OFFSET_CAPTURE);
  foreach ($docblock_matches as $docblock_match) {
    $docblock = array(
      'object_name' => '',
      'object_type' => '',
      'file_name' => $docblocks[0]['file_name'],
      'title' => '',
      'documentation' => '',
      'content' => str_replace(array("\n *", "\n "), array("\n", "\n"), $docblock_match[1][0]),
      'start' => $docblock_match[0][1],
      'length' => strlen($docblock_match[0][0]),
    );
    $docblock['code_start'] = $docblock['start'] + $docblock['length'] + 1;

    // Determine what kind of documentation block this is.
    if (substr($docblocks[0]['source'], $docblock['code_start'], 8) === 'function') {
      $docblock['object_type'] = 'function';
      $function_matches = array();
      preg_match('!^function (&?([a-zA-Z0-9_]+)\(.*?)\s*\{!', substr($docblocks[0]['source'], $docblock['code_start']), $function_matches);
      $docblock['object_name'] = $function_matches[2];
      $docblock['title'] = $function_matches[2];
      $docblock['signature'] = $function_matches[1];

      // We rely on the Drupal coding convention that functions are closed in column 1.
      $docblock['code_end'] = strpos($docblocks[0]['source'], "\n}", $docblock['code_start']) + 2;
    }
    elseif (substr($docblocks[0]['source'], $docblock['code_start'], 6) === 'define') {
      $docblock['object_type'] = 'constant';
      $constant_matches = array();
      preg_match('!^define\([\'"]([a-zA-Z0-9_]+)[\'"]!', substr($docblocks[0]['source'], $docblock['code_start']), $constant_matches);
      $docblock['object_name'] = $constant_matches[1];
      $docblock['title'] = $constant_matches[1];

      $docblock['code_end'] = strpos($docblocks[0]['source'], ';', $docblock['code_start']) + 1;
    }
    elseif (substr($docblocks[0]['source'], $docblock['code_start'], 6) === 'global') {
      $docblock['object_type'] = 'global';
      $global_matches = array();
      preg_match('!^global (\$[a-zA-Z0-9_]+)!', substr($docblocks[0]['source'], $docblock['code_start']), $global_matches);

      $docblock['object_name'] = substr($global_matches[1], 1);
      $docblock['title'] = $global_matches[1];

      $docblock['code_end'] = strpos($docblocks[0]['source'], ';', $docblock['code_start']) + 1;
    }

    $docblocks[] = $docblock;
  }

  // Find undocumented functions.
  $function_matches = array();
  preg_match_all('%(?<!\*/\n)^function (&?([a-zA-Z0-9_]+)\(.*?)\s*\{%sm', $docblocks[0]['source'], $function_matches, PREG_SET_ORDER | PREG_OFFSET_CAPTURE);
  foreach ($function_matches as $function_match) {
    $docblock = array(
      'object_name' => $function_match[2][0],
      'object_type' => 'function',
      'file_name' => $docblocks[0]['file_name'],
      'title' => $function_match[2][0],
      'documentation' => '',
      'signature' => $function_match[1][0],
      'parameters' => '',
      'return_value' => '',
      'code_start' => $function_match[0][1],
    );
    $docblock['code_end'] = strpos($docblocks[0]['source'], "\n}", $docblock['code_start']) + 2;

    $docblocks[] = $docblock;
  }

  // Find undocumented constants.
  $constant_matches = array();
  preg_match_all('%(?<!\*/\n)^define\([\'"]([a-zA-Z0-9_]+)[\'"]%sm', $docblocks[0]['source'], $constant_matches, PREG_SET_ORDER | PREG_OFFSET_CAPTURE);
  foreach ($constant_matches as $constant_match) {
    $docblock = array(
      'object_name' => $constant_match[1][0],
      'object_type' => 'constant',
      'file_name' => $docblocks[0]['file_name'],
      'title' => $constant_match[1][0],
      'documentation' => '',
      'code_start' => $constant_match[0][1],
    );
    $docblock['code_end'] = strpos($docblocks[0]['source'], ';', $docblock['code_start']) + 1;

    $docblocks[] = $docblock;
  }

  // Find undocumented globals.
  $global_matches = array();
  preg_match_all('%(?<!\*/\n)^global (\$[a-zA-Z0-9_]+)%sm', $docblocks[0]['source'], $global_matches, PREG_SET_ORDER | PREG_OFFSET_CAPTURE);
  foreach ($global_matches as $global_match) {
    $docblock = array(
      'object_name' => $global_match[1][0],
      'object_type' => 'global',
      'file_name' => $docblocks[0]['file_name'],
      'title' => $global_match[1][0],
      'documentation' => '',
      'code_start' => $global_match[0][1],
    );
    $docblock['code_end'] = strpos($docblocks[0]['source'], ';', $docblock['code_start']) + 1;

    $docblocks[] = $docblock;
  }

  api_save_documentation($docblocks);
}

/**
 * Find functions called in a formatted block of code.
 */
function api_parse_function_calls($code) {
  $function_call_matches = array();
  $function_calls = array();
  preg_match_all('!<span class="php-function-or-constant">([a-zA-Z0-9_]+)</span>\(!', $code, $function_call_matches, PREG_SET_ORDER);
  array_shift($function_call_matches); // Remove the first match, the function declaration itself.
  foreach ($function_call_matches as $function_call_match) {
    $function_calls[$function_call_match[1]] = $function_call_match[1];
  }

  return $function_calls;
}

/**
 * Save a documentation block into the database.
 *
 * @param $docblocks
 *   An array containing information about the documentation block.
 * @return
 *   The documentation ID of the inserted/updated construct.
 */
function api_save_documentation($docblocks) {
  $old_dids = array();
  $nested_groups = array();
  $result = db_query("SELECT did FROM {api_documentation} WHERE branch_name = '%s' AND file_name = '%s'", $docblocks[0]['branch_name'], $docblocks[0]['file_name']);
  while ($object = db_fetch_object($result)) {
    $old_dids[] = $object->did;
  }

  $dids = array();
  foreach ($docblocks as $docblock) {
    if (preg_match('/' . API_RE_TAG_START . 'mainpage/', $docblock['content'])) {
      $mainpage_matches = array();
      preg_match('/' . API_RE_TAG_START . 'mainpage (.*?)\n/', $docblock['content'], $mainpage_matches);
      $docblock['title'] = $mainpage_matches[1];
      $docblock['content'] = preg_replace('/' . API_RE_TAG_START . 'mainpage.*?\n/', '', $docblock['content']);
      $docblock['object_type'] = 'mainpage';
      $docblock['object_name'] = $docblocks[0]['branch_name'];
    }
    elseif (preg_match('/' . API_RE_TAG_START . 'file/', $docblock['content'])) {
      $docblocks[0]['content'] = str_replace('@file', '', $docblock['content']);
      $docblocks[0]['documentation'] = api_format_documentation($docblocks[0]['content'], $docblocks[0]['branch_name']);
    }
    elseif (preg_match('/' . API_RE_TAG_START . 'defgroup/', $docblock['content'])) {
      $group_matches = array();
      if (preg_match('/' . API_RE_TAG_START . 'defgroup ([a-zA-Z0-9_.-]+) +(.*?)\n/', $docblock['content'], $group_matches)) {
        $docblock['object_name'] = $group_matches[1];
        $docblock['title'] = $group_matches[2];
        $docblock['content'] = preg_replace('/' . API_RE_TAG_START . 'defgroup.*?\n/', '', $docblock['content']);
        $docblock['object_type'] = 'group';
      }
      else {
        watchdog('api', 'Malformed @defgroup in %file at line %line.', array('%file' => $docblocks[0]['file_path'], '%line' => $docblock['start_line']), WATCHDOG_NOTICE);
      }
    }

    if ($docblock['object_type'] === '') {
      continue;
    }

    if (!empty($docblock['content'])) {
      // Find parameter definitions.
      $param_match = array();
      $offset = 0;
      $docblock['parameters'] = '';
      while (preg_match('/' . API_RE_TAG_START . 'param(.*?)(?=\n' . API_RE_TAG_START . '|\n\n|$)/s', substr($docblock['content'], $offset), $param_match, PREG_OFFSET_CAPTURE)) {
        $docblock['content'] = str_replace($param_match[0][0], '', $docblock['content']);
        $docblock['parameters'] .= "\n\n". $param_match[1][0];
        $offset = $param_match[0][1];
      }
      $docblock['parameters'] = api_format_documentation($docblock['parameters'], $docblocks[0]['branch_name']);

      // Find return value definitions.
      $return_matches = array();
      $docblock['return_value'] = '';
      preg_match_all('/' . API_RE_TAG_START . 'return(.*?)(\n' . API_RE_TAG_START . '|\n\n|$)/s', $docblock['content'], $return_matches, PREG_SET_ORDER);
      foreach ($return_matches as $return_match) {
        $docblock['content'] = str_replace($return_match[0], '', $docblock['content']);
        $docblock['return_value'] .= "\n\n". $return_match[1];
      }
      $docblock['return_value'] = api_format_documentation($docblock['return_value'], $docblocks[0]['branch_name']);

      // Determine group membership.
      $group_matches = array();
      preg_match_all('/' . API_RE_TAG_START . '(ingroup|addtogroup) ([a-zA-Z0-9_]+)/', $docblock['content'], $group_matches);
      $docblock['groups'] = $group_matches[2];
      $docblock['content'] = preg_replace('/' . API_RE_TAG_START . 'ingroup.*?\n/', '', $docblock['content']);

      // Handle nested function groups.
      foreach ($nested_groups as $group_id) {
        if (!empty($group_id)) {
          $docblock['groups'][] = $group_id;
        }
      }
      if (preg_match('/' . API_RE_TAG_START . '{/', $docblock['content'])) {
        if ($docblock['object_type'] === 'group') {
          array_push($nested_groups, $docblock['object_name']);
        }
        else {
          $group_matches = array();
          if (preg_match('/' . API_RE_TAG_START . '(ingroup|addtogroup) ([a-zA-Z0-9_]+)/', $docblock['content'], $group_matches)) {
            array_push($nested_groups, $group_matches[2]);
          }
          else {
            array_push($nested_groups, '');
          }
        }
      }
      if (preg_match('/' . API_RE_TAG_START . '}/', $docblock['content'])) {
        array_pop($nested_groups);
      }

      $docblock['documentation'] = api_format_documentation($docblock['content'], $docblocks[0]['branch_name']);
    }
    if (!isset($docblock['summary'])) {
      $docblock['summary'] = api_documentation_summary($docblock['documentation']);
    }
    if (!isset($docblock['code'])) {
      if (isset($docblock['code_end'])) {
        $docblock['start_line'] = substr_count(substr($docblocks[0]['source'], 0, $docblock['code_start']), "\n");
        $docblock['code'] = api_format_php("<?php\n". substr($docblocks[0]['source'], $docblock['code_start'], $docblock['code_end'] - $docblock['code_start']) ."\n?>");
      }
      else {
        $docblock['start_line'] = 0;
        $docblock['code'] = '';
      }
    }
    $did = db_result(db_query("SELECT did FROM {api_documentation} WHERE object_name = '%s' AND branch_name = '%s' AND object_type = '%s'", $docblock['object_name'], $docblocks[0]['branch_name'], $docblock['object_type'], $docblock['file_name']));
    if ($did > 0) {
      db_query("UPDATE {api_documentation} SET title = '%s', file_name = '%s', summary = '%s', documentation = '%s', code = '%s' WHERE did = %d", $docblock['title'], $docblock['file_name'], $docblock['summary'], $docblock['documentation'], $docblock['code'], $did);
    }
    else {
      $node = new stdClass();
      $node->type = 'api';
      $node->uid = 0;
      if (module_exists('comment')) {
        $node->comment = COMMENT_NODE_READ_WRITE;
      }
      node_save($node);
      $did = $node->nid;
      db_query("INSERT INTO {api_documentation} (did, object_name, branch_name, object_type, title, file_name, summary, documentation, code) VALUES (%d, '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s')", $did, $docblock['object_name'], $docblocks[0]['branch_name'], $docblock['object_type'], $docblock['title'], $docblock['file_name'], $docblock['summary'], $docblock['documentation'], $docblock['code']);
    }

    switch ($docblock['object_type']) {
      case 'function':
        db_query('DELETE FROM {api_function} WHERE did = %d', $did);
        db_query("INSERT INTO {api_function} (did, signature, start_line, parameters, return_value) VALUES (%d, '%s', %d, '%s', '%s')", $did, $docblock['signature'], $docblock['start_line'], $docblock['parameters'], $docblock['return_value']);

        if (!empty($docblock['code'])) {
          db_query("DELETE FROM {api_reference_storage} WHERE branch_name = '%s' AND object_type = 'function' AND from_did = %d", $docblocks[0]['branch_name'], $did);
          foreach (api_parse_function_calls($docblock['code']) as $function_name) {
            api_reference($docblocks[0]['branch_name'], 'function', $function_name, $did);
          }
        }
        break;

      case 'file':
        db_query('DELETE FROM {api_file} WHERE did = %d', $did);
        db_query("INSERT INTO {api_file} (did, modified, version) VALUES (%d, %d, '%s')", $did, $docblock['modified'], $docblock['version']);
        break;
    }

    if (isset($docblock['groups'])) {
      db_query("DELETE FROM {api_reference_storage} WHERE branch_name = '%s' AND object_type = 'group' AND from_did = %d", $docblocks[0]['branch_name'], $did);
      foreach ($docblock['groups'] as $group_name) {
        api_reference($docblocks[0]['branch_name'], 'group', $group_name, $did);
      }
    }

    $dids[] = $did;
  }

  $old_dids = array_diff($old_dids, $dids);
  if (count($old_dids) > 0) {
    $old_dids = implode(',', $old_dids);
    db_query('DELETE FROM {api_documentation} WHERE did IN (%s)', $old_dids);
    db_query('DELETE FROM {api_file} WHERE did IN (%s)', $old_dids);
    db_query('DELETE FROM {api_function} WHERE did IN (%s)', $old_dids);
    db_query('DELETE FROM {api_reference_storage} WHERE from_did IN (%s) OR to_did IN (%s)', $old_dids, $old_dids);
  }

  api_schedule_shutdown();
}

/**
 * Format a documentation block as HTML.
 */
function api_format_documentation($documentation, $branch_name) {
  // Don't do processing on empty text (so we don't end up with empty paragraphs).
  if (empty($documentation)) {
    return '';
  }

  $documentation = check_plain($documentation);

  // @link full URLs.
  $documentation = preg_replace('/' . API_RE_TAG_START . 'link ((http:\/\/|https:\/\/|ftp:\/\/|mailto:|smb:\/\/|afp:\/\/|file:\/\/|gopher:\/\/|news:\/\/|ssl:\/\/|sslv2:\/\/|sslv3:\/\/|tls:\/\/|tcp:\/\/|udp:\/\/)([a-zA-Z0-9@:%_+*~#?&=.,\/;-]*[a-zA-Z0-9@:%_+*~#&=\/;-])) (.*?) ' . API_RE_TAG_START . 'endlink/', '<a href="$1">$4</a>', $documentation);
  // Site URLs.
  $documentation = preg_replace('/' . API_RE_TAG_START . 'link \/([a-zA-Z0-9_\/-]+) (.*?) ' . API_RE_TAG_START . 'endlink/', str_replace('%24', '$', l('$2', '$1')), $documentation);

  // Process the @see tag
  $documentation = preg_replace('/\n' . API_RE_TAG_START . 'see (.*[^.])\.?/', '<h3>See also</h3><p>$1</p>', $documentation);

  // Replace left over curly braces
  $documentation = preg_replace('/' . API_RE_TAG_START . '[{}]/', '', $documentation);

  // Process the @code @endcode tags.
  $documentation = preg_replace_callback('/' . API_RE_TAG_START . 'code(.+?)' . API_RE_TAG_START . 'endcode/s', 'api_format_embedded_php', $documentation);

  // Format lists
  $documentation = api_format_documentation_lists($documentation);

  // Convert newlines into paragraphs.
  $documentation = api_autop($documentation);

  return $documentation;
}

/**
 * Like _filter_autop(), but does not add <br /> tags.
 */
function api_autop($text) {
  // All block level tags
  $block = '(?:table|thead|tfoot|caption|colgroup|tbody|tr|td|th|div|dl|dd|dt|ul|ol|li|pre|select|form|blockquote|address|p|h[1-6]|hr)';

  // Split at <pre>, <script>, <style> and </pre>, </script>, </style> tags.
  // We don't apply any processing to the contents of these tags to avoid messing
  // up code. We look for matched pairs and allow basic nesting. For example:
  // "processed <pre> ignored <script> ignored </script> ignored </pre> processed"
  $chunks = preg_split('@(</?(?:pre|script|style|object)[^>]*>)@i', $text, -1, PREG_SPLIT_DELIM_CAPTURE);
  // Note: PHP ensures the array consists of alternating delimiters and literals
  // and begins and ends with a literal (inserting NULL as required).
  $ignore = FALSE;
  $ignoretag = '';
  $output = '';
  foreach ($chunks as $i => $chunk) {
    if ($i % 2) {
      // Opening or closing tag?
      $open = ($chunk[1] != '/');
      list($tag) = split('[ >]', substr($chunk, 2 - $open), 2);
      if (!$ignore) {
        if ($open) {
          $ignore = TRUE;
          $ignoretag = $tag;
        }
      }
      // Only allow a matching tag to close it.
      elseif (!$open && $ignoretag == $tag) {
        $ignore = FALSE;
        $ignoretag = '';
      }
    }
    elseif (!$ignore) {
      $chunk = preg_replace('|\n*$|', '', $chunk) ."\n\n"; // just to make things a little easier, pad the end
      $chunk = preg_replace('|<br />\s*<br />|', "\n\n", $chunk);
      $chunk = preg_replace('!(<'. $block .'[^>]*>)!', "\n$1", $chunk); // Space things out a little
      $chunk = preg_replace('!(</'. $block .'>)!', "$1\n\n", $chunk); // Space things out a little
      $chunk = preg_replace("/\n\n+/", "\n\n", $chunk); // take care of duplicates
      $chunk = preg_replace('/\n?(.+?)(?:\n\s*\n|\z)/s', "<p>$1</p>\n", $chunk); // make paragraphs, including one at the end
      $chunk = preg_replace('|<p>\s*</p>\n|', '', $chunk); // under certain strange conditions it could create a P of entirely whitespace
      $chunk = preg_replace("|<p>(<li.+?)</p>|", "$1", $chunk); // problem with nested lists
      $chunk = preg_replace('|<p><blockquote([^>]*)>|i', "<blockquote$1><p>", $chunk);
      $chunk = str_replace('</blockquote></p>', '</p></blockquote>', $chunk);
      $chunk = preg_replace('!<p>\s*(</?'. $block .'[^>]*>)!', "$1", $chunk);
      $chunk = preg_replace('!(</?'. $block .'[^>]*>)\s*</p>!', "$1", $chunk);
      $chunk = preg_replace('/&([^#])(?![A-Za-z0-9]{1,8};)/', '&amp;$1', $chunk);
    }
    $output .= $chunk;
  }
  return $output;
}

/**
 * Regexp replace callback for code tags.
 */
function api_format_embedded_php($matches) {
  return "\n\n". api_format_php("<?php\n". decode_entities($matches[1]) ."\n?>") ."\n\n";
}

/**
 * Parse a block of text for lists that use hyphens or asterisks as bullets, and
 * format them as proper HTML lists.
 */
function api_format_documentation_lists($documentation) {
  $lines = explode("\n", $documentation);
  $output = '';
  $bullet_indents = array(-1);

  foreach ($lines as $line) {
    $matches = array();
    preg_match('!^( *)([*-] )?(.*)$!', $line, $matches);
    $indent = strlen($matches[1]);
    $bullet_exists = $matches[2];

    if ($indent < $bullet_indents[0]) {
      // First close off any lists that have completed.
      while ($indent < $bullet_indents[0]) {
        array_shift($bullet_indents);
        $output .= '</li></ul>';
      }
    }

    if ($indent == $bullet_indents[0]) {
      if ($bullet_exists) {
        // A new bullet at the same indent means a new list item.
        $output .= '</li><li>';
      }
      else {
        // If the indent is the same, but there is no bullet, that also
        // signifies the end of the list.
        array_shift($bullet_indents);
        $output .= '</li></ul>';
      }
    }

    if ($indent > $bullet_indents[0] && $bullet_exists) {
      // A new list at a lower level.
      array_unshift($bullet_indents, $indent);
      $output .= '<ul><li>';
    }

    $output .= $matches[3] ."\n";
  }

  // Clean up any unclosed lists.
  array_pop($bullet_indents);
  foreach ($bullet_indents as $indent) {
    $output .= '</li></ul>';
  }

  return $output;
}

/**
 * Retrieve a summary from a documentation block.
 */
function api_documentation_summary($documentation) {
  $pos = strpos($documentation, '</p>');
  if ($pos !== FALSE) {
    $documentation = substr($documentation, 0, $pos);
  }
  $documentation = trim(strip_tags($documentation));

  if (strlen($documentation) > 255) {
    return substr($documentation, 0, strrpos(substr($documentation, 0, 252), ' ')) . '…';
  }
  else {
    return $documentation;
  }
}

/**
 * Colorize and format a PHP script.
 */
function api_format_php($code) {
  $output = '';

  if (!defined('T_ML_COMMENT')) {
    define('T_ML_COMMENT', T_COMMENT);
  }
  if (!defined('T_DOC_COMMENT')) {
    define('T_DOC_COMMENT', T_COMMENT);
  }

  $tokens = token_get_all($code);

  $in_string = FALSE;

  foreach ($tokens as $token) {
    if ($in_string) {
      if ($token == '"') {
        $output .= '"</span>';
        $in_string = FALSE;
      }
      else {
        $output .= is_array($token) ? htmlspecialchars($token[1]) : htmlspecialchars($token);
      }
      continue;
    }
    elseif ($token == '"') {
      $output .= '<span class="php-string">"';
      $in_string = TRUE;
      continue;
    }

    if (is_array($token)) {
      $type = $token[0];
      $value = htmlspecialchars($token[1]);

      switch ($type) {
        case T_OPEN_TAG:
        case T_CLOSE_TAG:
          $output .= '<span class="php-boundry">'. $value .'</span>';
          break;

        case T_COMMENT:
        case T_ML_COMMENT:
        case T_DOC_COMMENT:
          $output .= '<span class="php-comment">'. $value .'</span>';
          break;

        case T_VARIABLE:
          $output .= '<span class="php-variable">'. $value .'</span>';
          break;

        case T_CONSTANT_ENCAPSED_STRING:
        case T_INLINE_HTML:
          $output .= '<span class="php-string">'. $value .'</span>';
          break;

        case T_STRING:
          $output .= '<span class="php-function-or-constant">'. $value .'</span>';
          break;

        case T_LNUMBER:
        case T_DNUMBER:
          $output .= '<span class="php-constant">'. $value .'</span>';
          break;

        case T_ARRAY_CAST: case T_ARRAY: case T_AS: case T_BOOL_CAST:
        case T_BREAK: case T_CASE: case T_CLASS: case T_CONST:
        case T_CONTINUE: case T_DECLARE: case T_DEFAULT: case T_DO:
        case T_DOUBLE_CAST: case T_ECHO: case T_ELSE: case T_ELSEIF:
        case T_EMPTY: case T_ENDDECLARE: case T_ENDFOR: case T_ENDFOREACH:
        case T_ENDIF: case T_ENDSWITCH: case T_ENDWHILE: case T_EVAL:
        case T_EXIT: case T_EXTENDS: case T_FOR: case T_FOREACH:
        case T_FUNCTION: case T_GLOBAL: case T_IF: case T_INCLUDE_ONCE:
        case T_INCLUDE: case T_INT_CAST: case T_ISSET: case T_LIST:
        case T_NEW: case T_OBJECT_CAST: case T_PRINT:
        case T_REQUIRE_ONCE: case T_REQUIRE: case T_RETURN: case T_STATIC:
        case T_STRING_CAST: case T_SWITCH: case T_UNSET_CAST: case T_UNSET:
        case T_USE: case T_VAR: case T_WHILE:
          $output .= '<span class="php-keyword">'. $value .'</span>';
          break;

        default:
          $output .= $value;
      }
    }
    else {
      $output .= $token;
    }
  }

  // Manage whitespace:
  return '<pre class="php"><code>'. trim($output) .'</code></pre>';
}

/**
 * Since we may parse a file containing a reference before we have parsed the
 * file containing the referenced object, we keep track of the references
 * using a scratch table and save the references to the database table after the
 * referenced object has been parsed.
 */
function api_reference($branch_name, $to_type, $to_name, $from_did) {
  static $is_php_function = array();

  if ($to_type == 'function' && !isset($is_php_function[$to_name])) {
    $is_php_function[$to_name] = (db_result(db_query("SELECT COUNT(*) FROM {api_documentation} WHERE object_name = '%s' AND branch_name = 'php'", $to_name)) == 1);
  }

  if ($to_type != 'function' || !$is_php_function[$to_name]) {
    db_query("INSERT INTO {api_reference_storage} (object_name, branch_name, object_type, from_did) VALUES ('%s', '%s', '%s', %d)", $to_name, $branch_name, $to_type, $from_did);
  }
}

function api_schedule_shutdown() {
  static $scheduled = FALSE;

  if (!$scheduled) {
    register_shutdown_function('api_shutdown');
    $scheduled = TRUE;
  }
}

/**
 * Update the references that were collected, update JSON object list, clear
 * cache.
 */
function api_shutdown() {
  // Figure out all the dids of the object/branch/types.
  db_query('UPDATE {api_reference_storage} r INNER JOIN {api_documentation} d ON r.object_name = d.object_name AND r.branch_name = d.branch_name AND r.object_type = d.object_type SET r.to_did = d.did');

  $directory = file_directory_path();
  if (is_dir($directory) && is_writable($directory) && (variable_get('file_downloads', FILE_DOWNLOADS_PUBLIC) == FILE_DOWNLOADS_PUBLIC)) {
    $path = file_create_path('api');
    file_check_directory($path, FILE_CREATE_DIRECTORY);
    $date = gmdate('U');
    foreach (api_get_branches() as $branch_name => $branch) {
      $new_json = api_autocomplete($branch_name, FALSE);

      $old_file_path = variable_get('api_autocomplete_path_' . $branch_name, FALSE);
      if ($old_file_path !== FALSE) {
        if (md5($new_json) === md5(file_get_contents($old_file_path))) {
          continue; // No changes, no file write.
        }
        // Delete in the future, help avoid race conditions.
        job_queue_add('file_delete', t('Remove expired API JSON, %path.'), array('%path' => $old_file_path));
      }

      $file_name = $path . '/api-' . $branch_name . '-' . $date . '.json';
      file_save_data($new_json, $file_name, FILE_EXISTS_REPLACE);
      variable_set('api_autocomplete_path_' . $branch_name, $file_name);
    }
  }

  cache_clear_all();
}

function api_update_all_branches() {
  foreach (api_get_branches() as $branch) {
    api_update_branch($branch);
  }
}

function api_update_branch($branch) {
  static $parse_functions = array(
    'php' => 'api_parse_php_file',
    'module' => 'api_parse_php_file',
    'inc' => 'api_parse_php_file',
    'install' => 'api_parse_php_file',
    'engine' => 'api_parse_php_file',
    'theme' => 'api_parse_php_file',
    'profile' => 'api_parse_php_file',

    'txt' => 'api_parse_text_file',

    'htm' => 'api_parse_html_file',
    'html' => 'api_parse_html_file',
  );

  // List all documented files for the branch.
  $files = array();
  $result = db_query("SELECT f.did, f.modified, d.object_name FROM {api_documentation} d INNER JOIN {api_file} f ON d.did = f.did WHERE d.branch_name = '%s' AND d.object_type = 'file'", $branch->branch_name);
  while ($file = db_fetch_object($result)) {
    $files[$file->object_name] = $file;
  }

  foreach (api_scan_directories($branch->directories, $branch->excluded_directories) as $path => $file_name) {
    preg_match('!\.([a-z]*)$!', $file_name, $matches);
    if (isset($matches[1]) && isset($parse_functions[$matches[1]])) {
      if (isset($files[$file_name])) {
        $parse = (filemtime($path) > $files[$file_name]->modified);
        unset($files[$file_name]); // All remaining files will be removed.
      }
      else { // New file.
        $parse = TRUE;
      }
      if ($parse) {
        job_queue_add('api_parse_file', t('API parse %branch %file'), array($parse_functions[$matches[1]], $path, '%branch' => $branch->branch_name, '%file' => $file_name), drupal_get_path('module', 'api') .'/parser.inc', TRUE);
      }
    }
  }

  // Remove outdated files.
  foreach (array_keys($files) as $file_name) {
    watchdog('api', 'Removing %file.', array('%file' => $file_name));
    $result = db_query("SELECT ad.did FROM {api_documentation} ad WHERE ad.file_name = '%s' AND ad.branch_name = '%s'", $file_name, $branch->branch_name);
    while ($doc = db_fetch_object($result)) {
      db_query("DELETE FROM {api_documentation} WHERE did = %d", $doc->did);
      db_query("DELETE FROM {api_file} WHERE did = %d", $doc->did);
      db_query("DELETE FROM {api_function} WHERE did = %d", $doc->did);
      db_query("DELETE FROM {api_reference_storage} WHERE from_did = %d OR to_did = %d", $doc->did, $doc->did);
    }
    api_schedule_shutdown();
  }
}

/**
 * Find all the files in the directories specified for a branch.
 */
function api_scan_directories($directories, $excluded_directories) {
  $directory_array = explode("\n", $directories);
  $excluded_array = explode("\n", $excluded_directories);

  if (count($directory_array) > 1) {
    $directories_components = array();
    foreach ($directory_array as $directory) {
      $directory_components = array();
      $parts = explode(DIRECTORY_SEPARATOR, $directory);
      foreach ($parts as $part) {
        if (strlen($part)) {
          array_unshift($directory_components, reset($directory_components) .'/'. $part);
        }
      }
      $directories_components[] = $directory_components;
    }

    $common_ancestor_components = call_user_func_array('array_intersect', $directories_components);
    $common_ancestor = reset($common_ancestor_components);
  }
  else {
    $common_ancestor = $directories;
  }

  $source_files = array();
  foreach ($directory_array as $directory) {
    $files = file_scan_directory($directory, '.*');
    foreach ($files as $path => $file) {
      if (strpos($path, '/.') !== FALSE) {
        continue;
      }
      $excluded = FALSE;
      // If the file is in an excluded path, ignore it
      foreach ($excluded_array as $excluded_path) {
        if (!empty($excluded_path) && (strpos($path, $excluded_path) === 0)) {
          $excluded = TRUE;
        }
      }
      if (!$excluded) {
        $file_name = substr($path, strlen($common_ancestor) + 1);
        $source_files[$path] = $file_name;
      }
    }
  }
  return $source_files;
}
