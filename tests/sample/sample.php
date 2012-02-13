<?php

/**
 * @file
 * A sample file.
 */

/**
 * @defgroup samples Samples
 *
 * A sample group.
 *
 * @{
 */

/**
 * A sample global.
 */
global $sample_global;

/**
 * A sample constant.
 */
define('SAMPLE_CONSTANT', 7);

/**
 * A sample function.
 *
 * @see duplicate_function()
 *
 * Use for sample-related purposes.
 *
 * This is a sample list:
 * - One item.
 * - Another item.
 *   - A sub-item. This one goes for multiple lines, just to make
 *     sure that that works. It should. And here's a colon: just to
 *     make sure that isn't wonky.
 *   - Another sub-item.
 * - A third item.
 * This is not part of the list.
 *
 * This list uses our key format:
 * - key1: The usual format, no quotes.
 * - 'key2': Sometimes we have quotes.
 * - "key3 multiple": Sometimes double quotes and multiple words.
 * - The following item should not have strong formatting.
 * - http://example.com
 *
 * @param $parameter
 *   A generic parameter.
 * @param $complex_parameter
 *   Information about the $complex_parameter parameter. Example:
 *   @code
 *     $complex_parameter = 3;
 *   @endcode
 *
 *   A second paragraph about the $complex_parameter parameter.
 *
 *   @link http://php.net this is a link for the parameter @endlink
 *
 * @return
 *   Something about the return value.
 *
 *   A second paragraph about the return value.
 *
 * @see SAMPLE_CONSTANT
 */
function sample_function($parameter, $complex_parameter) {
}

/**
 * @} end samples
 */

/**
 * Function that has classes for parameter and return value.
 *
 * @param SubSample $parameter
 *   This parameter should link to the class.
 *
 * @return SampleInterface
 *   This return value should link to the interface.
 *
 * @ingroup samples
 */
function sample_class_function($parameter) {
}

/**
 * For testing duplicate function name linking.
 *
 * Also, here is some test documentation for multiple links in one paragraph.
 * This comes from the Simplenews project, modified to make links to groups
 * that we have here.
 *
 * @link samples Subscribers @endlink subscribe to @link class_samples
 * newsletters (categories) @endlink. That connection is called
 * a @link samples subscription @endlink. Nodes of enabled content types
 * are @link class_samples newsletter issues @endlink. These are then sent to
 * the subscribers of the newsletter the issue is attached to.
 */
function duplicate_function() {
}

/**
 * For testing duplicate constant linking.
 */
define('DUPLICATE_CONSTANT', 12);

/**
 * Respond to sample updates.
 *
 * This hook is for testing hook linking.
 */
function hook_sample_name() {
}

/**
 * Alter samples.
 *
 * This hook is for testing alter hook linking.
 */
function hook_another_sample_alter() {
}

/**
 * Returns HTML for a sample.
 *
 * This theme function is for testing linking in theme().
 *
 * @param $variables
 *   An associative array containing:
 *   - foo: The foo object that is being formatted.
 *   - show_bar: TRUE to show the bar component, FALSE to omit it.
 */
function theme_sample_one($variables) {
}

/**
 * Returns HTML for another sample.
 *
 * This theme function is for testing linking in theme(). It should not be
 * linked, because of the sample-two.tpl.php file, which has higher priority.
 *
 * @param $variables
 *   An associative array containing:
 *   - foo: The foo object that is being formatted.
 *   - show_bar: TRUE to show the bar component, FALSE to omit it.
 */
function theme_sample_two($variables) {
}

/**
 * Returns HTML for yet another sample.
 *
 * This theme function is for testing linking in theme().
 *
 * @param $variables
 *   An associative array containing:
 *   - foo: The foo object that is being formatted.
 *   - show_bar: TRUE to show the bar component, FALSE to omit it.
 */
function theme_sample_four($variables) {
}

/**
 * Does something interesting, to test in-code linking.
 */
function sample_in_code_links() {
  // Should link to function.
  $foo = sample_function();
  // Should link to theme function.
  $bar = theme('sample_one', $foo);
  // Should link to theme template, not function, though both exist.
  $baz = theme('sample_two', $foo);
  // Should link to theme template.
  $boo = theme('sample_three');
  // Should link to the sample_four theme function.
  $bop = theme('sample_four__option', $foo);

  // Should link to hook.
  $x = module_invoke_all('sample_name', $foo, $baz);
  $stuff = '';
  // Should link to hook.
  foreach (module_implements('sample_name') as $module) {
    // Should link to hook. Note that the variable name has to be $module for
    // this link to work.
    module_invoke($module, 'sample_name', $baz);
  }

  // Should link to alter hook.
  $xx = drupal_alter('another_sample', $foo);

  // Should link to search for this function.
  $z = duplicate_function();

  // Should link to class.
  $j = new SubSample();

  // Should link to constant.
  $k = SAMPLE_CONSTANT;
  // Should link to search for this constant.
  $l = DUPLICATE_CONSTANT;

  $menu = array(
    'title' => 'A title goes here.',
    // Should link to sample_function.
    'page callback' => 'sample_function',
  );

  // Functions that don't exist, so should not be linked, but
  // should still be visible.
  $a = nonexistent_function($a, $b);
  $b = module_invoke_all('nonexistent_hook', $foo);
  $c = theme('nonexistent_theme_hook', $foo);
  foreach (module_implements('nonexistent_hook') as $module) {
    module_invoke($module, 'nonexistent_hook', $foo);
  }
  $d = drupal_alter('nonexistent_alter_name', $foo);
}

/**
 * Implements hook_sample_name().
 *
 * This is used for testing the "N functions implement hook()" link on function
 * pages.
 */
function foo_sample_name() {
}
