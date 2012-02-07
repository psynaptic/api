<?php

/**
 * @file
 * A sample file to make as a new project.
 */

/**
 * A second sample function.
 *
 * This function links to the first sample function, with both see and link,
 * to verify that links work across projects. The see link is at the bottom.
 * Here is the link:
 * @link samples Link to the samples group @endlink
 *
 * Here we use the sample_function(), $sample_global, and SAMPLE_CONSTANT in
 * the text. These should all turn into links. So should a reference to the
 * sample.php file.
 *
 * @see sample_function()
 */
function second_sample_function() {
  // Use global variable, constant, and function from the sample project in the
  // function body and verify that they turn into links.
  global $sample_global;

  $sample_global = SAMPLE_CONSTANT;

  $foo = sample_function('a', 'b');
}
