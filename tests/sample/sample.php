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
define('SAMPLE_CONSTANT');

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
 * Function that has classes for parameter and return value.
 *
 * @param SubSample $parameter
 *   This parameter should link to the class.
 *
 * @return SampleInterface
 *   This return value should link to the interface.
 */
function sample_class_function($parameter) {
}

/**
 * @} end samples
 */

/**
 * For testing duplicate function name linking.
 */
function duplicate_function() {
}

/**
 * For testing duplicate constant linking.
 */
define('DUPLICATE_CONSTANT');
