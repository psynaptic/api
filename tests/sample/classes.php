<?php

/**
 * @file Object-oriented tests.
 */

/**
 * Sample class.
 */
class Sample implements SampleInterface {
  /**
   * A class constant.
   */
  const constant = 'constant value';

  /**
   * A property.
   */
  private $property = 'variable value';

  /**
   * Metasyntatic member function.
   */
  public function foo() {
  }
}

/**
 * Sample interface.
 */
interface SampleInterface {
  /**
   * Implement this API.
   */
  public function foo();
}

/**
 * Subclass.
 *
 * @see Sample::foo() should be link
 */
class SubSample extends Sample implements SampleInterfaceTwo {
  /**
   * Metasyntatic member function.
   */
  public function bar() {
  }
}

/**
 * Sample interface.
 */
interface SampleInterfaceTwo {
  /**
   * Implement this API.
   */
  public function bar();
}
